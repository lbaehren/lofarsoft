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
from image import Op

class Op_islands(Op):
    """Detect islands of emission in the image

    All detected islands are stored in the list img.islands,
    where each individual island is represented as an instance 
    of class Island.

    Prerequisites: module rmsimage should be run first.
    """
    def __call__(self, img):
        opts = img.opts

        img.islands = self.ndimage_alg(img.image, img.mask,
                                       img.mean, img.rms,
                                       opts.isl_clip,
                                       opts.isl_peak_clip,
                                       opts.isl_min_size)

        print "Islands found: " + str(len(img.islands))
        return img

    def ndimage_alg(self, img, mask, mean, rms, thresh1, thresh2, minsize):
        """Island detection using scipy.ndimage

        Use scipy.ndimage.label to detect islands of emission in the image.
        Island is defined as group of tightly connected (8-connectivity 
        for 2D images) pixels with emission.

        The following cuts are applied:
         - pixel is considered to have emission if it is 'thresh1' times 
           higher than RMS.
         - Island should have at least 'minsize' active pixels
         - There should be at lease 1 pixel in the island which is 'thresh2'
           times higher than noise (peak clip).

        Parameters:
        img, mask: arrays with image data and mask
        mean, rms: arrays with mean & rms maps
        thresh1: threshold for 'active pixels'
        thresh2: threshold for peak
        minsize: minimal acceptable island size

        Function returns a list of Island objects.
        """
        ### islands detection
        act_pixels = (img-mean)/thresh1 > rms
        if isinstance(mask, N.ndarray):
            act_pixels[mask] = False

        rank = len(img.shape)
        connectivity = nd.generate_binary_structure(rank, rank)
        labels, count = nd.label(act_pixels, connectivity)
        slices = nd.find_objects(labels)

        ### apply cuts on island size and peak value
        res = []
        for idx, s in enumerate(slices):
            idx += 1 # nd.labels indices are counted from 1
            isl_size = (labels[s] == idx).sum()
            isl_peak = nd.maximum(img[s], labels[s], idx)

            if (isl_size >= minsize) and \
                    (isl_peak - mean[s].mean())/thresh2 > rms[s].mean():
                res.append(Island(img, mask, mean, rms, labels, s, idx))

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
        origin = [b.start for b in bbox]
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
        self.mean = mean[bbox].mean()
        self.rms  = rms[bbox].mean()

    def __expand_bbox(self, bbox, shape):
        """Expand bbox of the image by 1 pixel"""
        def __expand(bbox, shape):
            return slice(max(0, bbox.start - 1), min(shape, bbox.stop + 1))

        return map(__expand, bbox, shape) 


### Insert attribute for island list into Image class
Image.islands = List(tInstance(Island), doc="List of islands")
