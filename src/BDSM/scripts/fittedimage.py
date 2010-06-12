"""Module fittedimage.

It calculates model image from the list of gaussians.
"""

import numpy as N
from image import *

### Insert attribute into Image class for model image
Image.fitted_image = NArray(doc="Model image calculated from " \
                                "extracted gaussians")

class Op_fittedimage(Op):
    """Creates an image from the fitted gaussians.

    The resulting model image is stored in the 
    fitted_image attribute.

    Prerequisites: module gausfit should be run first.
    """
    def __call__(self, img):
        shape = img.image.shape
        thresh= img.opts.fittedimage_clip

        fimg = N.zeros(shape, dtype=float)

        for g in img.gaussians():
            C1, C2 = g.center_pix
            isl = img.islands[g.island_idx]
            b = self.find_bbox(thresh*isl.rms, g)

            bbox = N.s_[max(0, int(C1-b)):min(shape[0], int(C1+b+1)),
                        max(0, int(C2-b)):min(shape[1], int(C2+b+1))]

            fimg[bbox] += self.gaussian_fcn(g=g, *N.mgrid[bbox])

        img.fitted_image = fimg
        return img

    def find_bbox(self, thresh, g):
        """Calculate bounding box for gaussian.

        This function calculates size of the box for evaluating
        gaussian, so that value of gaussian is smaller than threshold
        outside of the box.

        Parameters:
        thres: threshold
        g: Gaussian object
        """
        from math import ceil, sqrt, log
        A = g.peak_flux
        S = g.shape_pix[0]
        return ceil(S*sqrt(-2*log(thresh/A)))

    def gaussian_fcn(self, x1, x2, g):
        """Evaluate Gaussian on the given grid.

        Parameters:
        x1, x2: grid (as produced by numpy.mgrid f.e.)
        g: Gaussian object
        """
        from math import radians, sin, cos
        A = g.peak_flux
        C1, C2 = g.center_pix
        S1, S2, Th = g.shape_pix

        th = radians(Th)
        cs = cos(th)
        sn = sin(th)

        f1 = ((x1-C1)*cs + (x2-C2)*sn)/S1
        f2 = (-(x1-C1)*sn + (x2-C2)*cs)/S2

        return A*N.exp(-(f1*f1 + f2*f2)/2)
