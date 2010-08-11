"""Module islands.

Defines operation Op_islands which does island detection.
Current implementation uses scipy.ndimage operations for island detection.
While it's implemented to work for images of arbitrary dimensionality, 
the bug in the current version of scipy (0.6) often causes crashes 
(or just wrong results) for 3D inputs.

If this (scipy.ndimage.label) isn't fixed by the time we need 3D source 
extraction, one will have to adopt my old pixel-runs algorithm for 3D data. 
Check out islands.py rev. 1362 from repository for it.
"""

import numpy as N
import scipy.ndimage as nd
from image import *
import mylogger
import pyfits
import output_fbdsm_files as opf

nisl = Int(doc="Total number of islands detected")

class Op_islands(Op):
    """Detect islands of emission in the image

    All detected islands are stored in the list img.islands,
    where each individual island is represented as an instance 
    of class Island.

    Prerequisites: module rmsimage should be run first.
    """
    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Islands   ")
        opts = img.opts

        img.islands = self.ndimage_alg(img, opts)

        img.nisl = len(img.islands)

        mylog.info('%s %i' % ("Number of islands found : ", len(img.islands)))

        for i, isl in enumerate(img.islands):
            isl.island_id = i

        if opts.output_fbdsm: opf.write_fbdsm_islands(img)

        return img

    def ndimage_alg(self, img, opts):
        """Island detection using scipy.ndimage

        Use scipy.ndimage.label to detect islands of emission in the image.
        Island is defined as group of tightly connected (8-connectivity 
        for 2D images) pixels with emission.

        The following cuts are applied:
         - pixel is considered to have emission if it is 'thresh_isl' times 
           higher than RMS.
         - Island should have at least 'minsize' active pixels
         - There should be at lease 1 pixel in the island which is 'thresh_pix'
           times higher than noise (peak clip).

        Parameters:
        image, mask: arrays with image data and mask
        mean, rms: arrays with mean & rms maps
        thresh_isl: threshold for 'active pixels'
        thresh_pix: threshold for peak
        minsize: minimal acceptable island size

        Function returns a list of Island objects.
        """
        ### islands detection

        image = img.ch0 
        mask = img.mask
        rms = img.rms
        mean = img.mean
        thresh_isl = opts.thresh_isl
        thresh_pix = opts.thresh_pix
        minsize = opts.minpix_isl
        clipped_mean = img.clipped_mean
        saverank = opts.savefits_rankim
        hdr = img.header

                        # act_pixels is true if significant emission
        act_pixels = (image-mean)/thresh_isl >= rms
        if isinstance(mask, N.ndarray):
            act_pixels[mask] = False

                        # dimension of image
        rank = len(image.shape)
                        # generates matrix for connectivity, in this case, 8-conn
        connectivity = nd.generate_binary_structure(rank, rank)
                        # labels = matrix with value = (initial) island number
        labels, count = nd.label(act_pixels, connectivity)
                        # slices has limits of bounding box of each such island
        slices = nd.find_objects(labels)

        ### apply cuts on island size and peak value
        pyrank = N.zeros(image.shape)
        res = []
        for idx, s in enumerate(slices):
            idx += 1 # nd.labels indices are counted from 1
                        # number of pixels inside bounding box which are in island
            isl_size = (labels[s] == idx).sum()
            isl_peak = nd.maximum(image[s], labels[s], idx)
            isl_maxposn = tuple(N.array(N.unravel_index(N.argmax(image[s]), image[s].shape))+\
                          N.array((s[0].start, s[1].start)))
            if (isl_size >= minsize) and (isl_peak - mean[isl_maxposn])/thresh_pix > rms[isl_maxposn]:
              isl = Island(image, mask, mean, rms, labels, s, idx)
              pyrank[isl.bbox] += N.invert(isl.mask_active)*idx / idx
              res.append(isl)

        if saverank: pyfits.writeto(img.imagename + '.pyrank.fits', N.transpose(pyrank), clobber=True)

        return res


from image import *

class Island(object):
    """Instances of this class represent islands of emission in the image.

    It's primary use is a container for all kinds of data describing island.
    """
    bbox        = List(Instance(slice(0), or_none=False),
                       doc = "Bounding box of the island")
    origin      = List(Float(), doc="Coordinates of lower-left corner")
    image       = NArray(doc="Sub-image of the island")
    mask_active = NArray(doc="Mask for just active pixels")
    mask_noisy  = NArray(doc="Mask for active pixels and surrounding noise")
    shape       = List(Int(), doc="Shape of the island")
    size_active = Int(doc="Number of active pixels in the island")
    mean        = Float(doc="Average mean value")
    rms         = Float(doc="Average rms")
    island_id   = Int(doc="Island id, starting from 0")
    resid_rms   = Float(doc="Rms of residual image of island")
    resid_mean  = Float(doc="Mean of residual image of island")

    def __init__(self, img, mask, mean, rms, labels, bbox, idx):
        """Create Island instance.

        Parameters:
        img, mask, mean, rms: arrays describing image
        labels: labels array from scipy.ndimage
        bbox: slices
        """
        TCInit(self)
        
        ### we make bbox slightly bigger
        bbox = self.__expand_bbox(bbox, img.shape)
        origin = [b.start for b in bbox]   # easier in case ndim > 2
        data = img[bbox]

        ### create (inverted) masks
        isl_mask = (labels[bbox] == idx)
        noise_mask = (labels[bbox] == 0)
        N.logical_or(noise_mask, isl_mask, noise_mask)

        isl_size = isl_mask.sum()

        ### invert masks
        N.logical_not(isl_mask, isl_mask)
        N.logical_not(noise_mask, noise_mask)
        if isinstance(mask, N.ndarray):
            noise_mask[mask[bbox]] = True

        ### finish initialization
        self.bbox = bbox
        self.origin = origin
        self.image = data
        self.mask_active = isl_mask
        self.mask_noisy = noise_mask
        self.shape = data.shape
        self.size_active = isl_size
        bbox_rms_im = rms[bbox]
        in_bbox_and_unmasked = N.where(~N.isnan(bbox_rms_im))
        self.rms  = bbox_rms_im[in_bbox_and_unmasked].mean()
        bbox_mean_im = mean[bbox]
        in_bbox_and_unmasked = N.where(~N.isnan(bbox_mean_im))
        self.mean  = bbox_mean_im[in_bbox_and_unmasked].mean()

    ### do map etc in case of ndim image
    def __expand_bbox(self, bbox, shape):
        """Expand bbox of the image by 1 pixel"""
        def __expand(bbox, shape):
            return slice(max(0, bbox.start - 1), min(shape, bbox.stop + 1))
        return map(__expand, bbox, shape) 


### Insert attribute for island list into Image class
Image.islands = List(tInstance(Island), doc="List of islands")


