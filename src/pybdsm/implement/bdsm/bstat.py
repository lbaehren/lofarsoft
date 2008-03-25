import numpy as N
import scipy.ndimage as nd
import _cbdsm
from image import Op

class Op_bstat(Op):
    """Calculate image statistics
    """
    def __call__(self, img):
        opts = img.opts
        mask = img.img_mask

        mean, rms, cmean, crms = _cbdsm.bstat(img.img, mask, opts.rms_clip)
        img.cmean = cmean
        img.crms  = crms

        img.mean = N.zeros(img.img.shape, dtype=N.float32)
        img.rms  = N.zeros(img.img.shape, dtype=N.float32)

        if opts.noise_map:
            axes, mean_map, rms_map = self.bstat_map(img.img, mask, opts.rms_clip,
                                                     opts.bstat_box)
            ax = map(self.remap_axis, img.img.shape, axes)
            ax = N.meshgrid(*ax[-1::-1])
            nd.map_coordinates(mean_map, ax[-1::-1], order=opts.spline_rank, output=img.mean)
            nd.map_coordinates(rms_map,  ax[-1::-1], order=opts.spline_rank, output=img.rms)

            img.mmean = mean_map
            img.mrms  = rms_map
            img.maxes = axes
        else:
            img.mean[:,:] = cmean
            img.rms[:,:]  = crms

        print "mean: ", cmean, "rms: ", crms
        return img

    def bstat_map(self, arr, mask=False, kappa=3, box=None):
        """Calculate map of the mean/rms values
        """
        if box is None:
            box = (50, 25)

        if mask is not False:
            raise NotImplementedError, "non-trivial masks are not supported yet"

        BS, SS = box
        imgshape = N.array(arr.shape)
        boxcount = 1 + (imgshape - BS)/SS
        bounds   = N.asarray((boxcount-1)*SS + BS < imgshape, dtype=int)
        mapshape = 2 + boxcount + bounds

        mean_map = N.zeros(mapshape, dtype=float)
        rms_map  = N.zeros(mapshape, dtype=float)
        axes     = map( (lambda len: N.zeros(len, dtype=float)), mapshape)

        ### step 1: internal area of the image
        for i in range(boxcount[0]):
            for j in range(boxcount[1]):
                mean, rms, cmean, crms = _cbdsm.bstat(arr[i*SS:i*SS+BS, j*SS:j*SS+BS], mask, kappa)
                mean_map[i+1, j+1], rms_map[i+1, j+1] = cmean, crms

        ### step 2: borders of the image
        if bounds[0]:
            for j in range(boxcount[1]):
                mean, rms, cmean, crms = _cbdsm.bstat(arr[-BS:, j*SS:j*SS+BS], mask, kappa)
                mean_map[-2, j+1], rms_map[-2, j+1] = cmean, crms
        if bounds[1]:
            for i in range(boxcount[0]):
                mean, rms, cmean, crms = _cbdsm.bstat(arr[i*SS:i*SS+BS, -BS:], mask, kappa)
                mean_map[i+1, -2], rms_map[i+1, -2] = cmean, crms
        if bounds.all():
                mean, rms, cmean, crms = _cbdsm.bstat(arr[-BS:, -BS:], mask, kappa)
                mean_map[-2,-2], rms_map[-2,-2] = cmean, crms

        ### step 3: correct(extrapolate) borders of the image
        def correct_borders(map):
            map[0, :] = map[1, :]
            map[:, 0] = map[:, 1]
            map[-1, :] = map[-2, :]
            map[:, -1] = map[:, -2]

            map[0,0] = (map[1,0] + map[0, 1])/2.
            map[-1,0] = (map[-2, 0] + map[-1, 1])/2.
            map[0, -1] = (map[0, -2] + map[1, -1])/2.
            map[-1,-1] = (map[-2, -1] + map[-1, -2])/2.

        correct_borders(mean_map)
        correct_borders(rms_map)

        ### step 4: fill in coordinate axes
        for i in range(2):
            axes[i][1:boxcount[i]+1] = N.arange(boxcount[i])*SS + BS/2. - .5
            if bounds[i]:
                axes[i][-2] = imgshape[i] - BS/2. - .5
            axes[i][-1] = imgshape[i] - 1

        return axes, mean_map, rms_map

    def remap_axis(self, size, arr):
        """Recalculate axis coordinates as produced by bstat_map.
        Calculates the coordinates of each image pixel in the mapped array.
        """
        from math import floor, ceil
        res = N.zeros(size, dtype=float)

        for i in range(len(arr) - 1):
            i1 = arr[i]
            i2 = arr[i+1]
            t = N.arange(ceil(i1), floor(i2)+1, dtype=float)
            res[ceil(i1):floor(i2)+1] = i + (t-i1)/(i2-i1)

        return res



class Op_thresholds(Op):
    """Compute thresholds
    """
    def __call__(self, img):
        return img
