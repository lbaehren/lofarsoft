import numpy as N
import scipy.ndimage as nd
from image import Op

class Op_islands(Op):
    """Detect islands of emission in the image

    Adds to the image attribute 'islands' with the list of detected islands
    """
    def __call__(self, img):
        data_mask = img.img_mask
        parameters = (img.mean, img.rms, img.opts.isl_clip,
                      img.opts.isl_peak_clip, img.opts.isl_min_size)

        img.islands = self.ndimage_alg(img, data_mask, *parameters)

        print "Islands found: " + str(len(img.islands))
        return img

    def ndimage_alg(self, img, data_mask, mean, rms, thresh1, thresh2, minsize):
        """Island detection algorithm using image manipulation routines 
        from scipy.ndimage.measurements package.
        """
        data = img.img

        ### islands detection
        mask = (data - mean)/thresh1 > rms
        if data_mask is not False:
            mask[data_mask] = False
        labels, count = nd.label(mask, nd.generate_binary_structure(2,2))
        slices = nd.find_objects(labels)

        ### filter out noise and convert to 'real' islands
        res = []
        for idx, s in enumerate(slices):
            idx += 1            # indices are counted from 1
            if (labels[s] == idx).sum() >= minsize and \
                    (nd.maximum(data[s], labels[s], idx) - mean[s].mean())/thresh2 > rms[s].mean():
                res.append(Island(img, labels, s, idx))

        return res



class Island:
    """This class represents an island.

    Most usefull attributes:
      bbox      : bounding box of the island (in the image coordinates)
                  (slice(x0min, x0max), slice(x1min, x1max))
      origin    : coordinates of the lower-left corner of the island
      img       : image of the islands
      isl_mask  : mask, where all but 'active' pixels are flagged
      noisy_mask: mask including active pixels and surrounding noise.
                  i.e. accidental active pixel or neighbour islands are masked out
      shape     : shape of the island
      size      : number of active pixels in the island
      opts      : reference to image's options
      mean      : average mean for island
      rms       : average rms for island
    """
    def __init__(self, img, labels, bbox, idx):
        bbox = self._expand_bbox(bbox, img.img.shape)
        origin = (bbox[0].start, bbox[1].start)
        data = img.img[bbox]

        ### create (inverted) masks
        isl_mask = (labels[bbox] == idx)
        noise_mask = (labels[bbox] == 0)
        N.logical_or(noise_mask, isl_mask, noise_mask)

        isl_size = isl_mask.sum()

        ### invert masks
        N.logical_not(isl_mask, isl_mask)
        N.logical_not(noise_mask, noise_mask)
        if img.opts.masked:
            noise_mask[img.img_mask[bbox]] = True

        self.bbox = bbox
        self.origin = origin
        self.img  = data
        self.isl_mask = isl_mask
        self.noisy_mask = noise_mask
        self.shape = data.shape
        self.size = isl_size
        self.opts = img.opts
        self.mean = img.mean[bbox].mean()
        self.rms  = img.rms[bbox].mean()

    def _expand_bbox(self, bbox, shape):
        return (slice(max(0, bbox[0].start - 1), min(shape[0], bbox[0].stop + 1)),
                slice(max(0, bbox[1].start - 1), min(shape[1], bbox[1].stop + 1)))
