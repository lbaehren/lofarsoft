"""Module rmsimage.

Defines operation Op_rmsimage which calculates noise and
rms maps. Depending on the value of parameter rms_noise_map
the generated map will contain either real map or a constant
value.

The current implementation will handle both 2D and 3D images,
where for 3D case it will calculate maps for each plane.

Masked images aren't hadled properly yet.
"""

import numpy as N
import scipy.ndimage as nd
import _cbdsm
from image import Op, Image, NArray

### insert into Image tc-variables for mean & rms maps
Image.mean = NArray(doc="Mean map")
Image.rms  = NArray(doc="RMS map")


class Op_rmsimage(Op):
    """Calculate rms & noise maps

    Prerequisites: Module preprocess should be run first.
    """
    def __call__(self, img):
        opts = img.opts
        data = img.image
        mask = img.mask

        map_opts = (opts.rms_clip, opts.rms_box, opts.spline_rank)

        mean = N.zeros(data.shape, dtype=N.float32)
        rms  = N.zeros(data.shape, dtype=N.float32)

        if opts.rms_noise_map: ## calculate maps
            if len(data.shape) == 2:   ## 2d case
                self.map_2d(data, mean, rms, mask, *map_opts)
            elif len(data.shape) == 3: ## 3d case
                if not isinstance(mask, N.ndarray):
                    mask = N.zeros(data.shape[0], dtype=bool)
                for i in range(data.shape[0]):
                    ## iterate each plane
                    self.map_2d(data[i], mean[i], rms[i], mask[i], *map_opts)
            else:
                raise RuntimeError("Can't hadnle array of this shape")
        else:     ## no map - just fill in constant value
            mean[:] = img.clipped_mean
            rms[:]  = img.clipped_rms

        img.mean = mean
        img.rms  = rms

        return img

    def map_2d(self, arr, out_mean, out_rms, mask=False,
               kappa=3, box=None, interp=1):
        """Calculate mean&rms maps and store them into provided arrays

        Parameters:
        arr: 2D array with data
        out_mean, out_rms: 2D arrays where to store calculated maps
        mask: mask
        kappa: clipping value for rms/mean calculations
        box: tuple of (box_size, box_step) for calculating map
        interp: order of interpolating spline used to interpolate 
                calculated map
        """
        axes, mean_map, rms_map = self.rms_mean_map(arr, mask, kappa, box)
        ax = map(self.remap_axis, arr.shape, axes)
        ax = N.meshgrid(*ax[-1::-1])
        nd.map_coordinates(mean_map, ax[-1::-1], order=interp, output=out_mean)
        nd.map_coordinates(rms_map,  ax[-1::-1], order=interp, output=out_rms)

    def rms_mean_map(self, arr, mask=False, kappa=3, box=None):
        """Calculate map of the mean/rms values

        Parameters:
        arr:  2D array with data
        mask: mask
        kappa: clipping for calculationg rms/mean within each box
        box: box parameters (box_size, box_step)

        Returns:
        axes: list of 2 arrays with coordinates of boxes alongside each axis
        mean_map: map of mean values
        rms_map: map of rms values

        Description:
        This function calculates clipped mean and rms maps for the array.
        The algorithm is a moving-window algorithm, where mean&rms are 
        calculated within a window of a size (box_size * box_size), and the
        window is stepped withing the image by steps of box_steps.

        Special care is taken for the borders of the image -- outer borders
        (where box doesn't fit properly) are given one extra round with a box
        applied to the border of the image. Additionally outer values are 
        extrapolated to cover whole image size, to simplify further processing.

        See also routine 'remap_axes' for 'inverting' axes array

        Example:
        for an input image of 100x100 pixels calling rms_mean_map with default
        box parameters (50, 25) will result in the following:

        axes = [array([  0. ,  24.5,  49.5,  74.5,  99. ]),
                array([  0. ,  24.5,  49.5,  74.5,  99. ])]

        mean_map = <5x5 array>
        rms_map  = <5x5 array>
 
        rms_map[1,1] is calculated for  arr[0:50, 0:50]
        rms_map[2,1] is calculated for  arr[25:75, 0:50]
        ...etc...
        rms_map[0,0] is extrapolated as .5*(rms_map[0,1] + rms_map[1,0])
        rms_map[0,1] is extrapolated as rms_map[1,1]
        """
        if box is None:
            box = (50, 25)

        if isinstance(mask, N.ndarray):
            raise NotImplementedError, "non-trivial masks are not supported yet"

        ## some math first. 
        ##   boxcount is number of boxes alongsize each axis
        ##   bounds is non-zero for axes which have extra pixels beyond last box
        BS, SS = box
        imgshape = N.array(arr.shape)
        boxcount = 1 + (imgshape - BS)/SS
        bounds   = N.asarray((boxcount-1)*SS + BS < imgshape, dtype=int)
        mapshape = 2 + boxcount + bounds

        ## arrays for calculated data
        mean_map = N.zeros(mapshape, dtype=float)
        rms_map  = N.zeros(mapshape, dtype=float)
        axes     = [N.zeros(len, dtype=float) for len in mapshape]

        ## pull out bstat routine from _cbdsm
        bstat = _cbdsm.bstat

        ### step 1: internal area of the image
        for i in range(boxcount[0]):
            for j in range(boxcount[1]):
                m, r, cm, cr = bstat(arr[i*SS:i*SS+BS, j*SS:j*SS+BS], mask, kappa)
                mean_map[i+1, j+1], rms_map[i+1, j+1] = cm, cr

        ### step 2: borders of the image
        if bounds[0]:
            for j in range(boxcount[1]):
                m, r, cm, cr = bstat(arr[-BS:, j*SS:j*SS+BS], mask, kappa)
                mean_map[-2, j+1], rms_map[-2, j+1] = cm, cr
        if bounds[1]:
            for i in range(boxcount[0]):
                m, r, cm, cr = bstat(arr[i*SS:i*SS+BS, -BS:], mask, kappa)
                mean_map[i+1, -2], rms_map[i+1, -2] = cm, cr
        if bounds.all():
                m, r, cm, cr = bstat(arr[-BS:, -BS:], mask, kappa)
                mean_map[-2,-2], rms_map[-2,-2] = cm, cr

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
        """Invert axis mapping done by rms_mean_map

        rms_mean_map 'compresses' axes by returning short arrays with
        coordinades of the boxes. This routine 'inverts' this compression
        by calculating coordinates of each pixel of the original array
        within compressed array.

        Parameters:
        size: size of the original (and resulting) array
        arr : 'compressed' axis array from rms_mean_map

        Example:
        the following 'compressed' axis (see example in rms_mean_map):

           ax = array([  0. ,  24.5,  49.5,  74.5,  99. ])

        will be remapped as:

        print remap_axis(100, ax)
        [ 0.          0.04081633  0.08163265  0.12244898 ....
           ...............................................
          3.91836735  3.95918367  4.        ]

        which means that pixel 0 in the original image corresponds to pixels
        0 in the rms/mean_map array (which is 5x5 array).
        pixel 1 of the original image has coordinate of 0.04081633 in the
        compressed image (e.g. it has no exact counterpart, and it's value
        should be obtained by interpolation)
        """
        from math import floor, ceil
        res = N.zeros(size, dtype=float)

        for i in range(len(arr) - 1):
            i1 = arr[i]
            i2 = arr[i+1]
            t = N.arange(ceil(i1), floor(i2)+1, dtype=float)
            res[ceil(i1):floor(i2)+1] = i + (t-i1)/(i2-i1)

        return res
