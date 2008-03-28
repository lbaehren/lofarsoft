import numpy as N
from image import Op

class Op_fittedimage(Op):
    """Creates an image from the fitted gaussians
    """
    def __call__(self, img):
        shape = img.img.shape
        gimg = N.zeros(shape, dtype=float)
        thresh = img.opts.fittedimage_clip

        for isl in img.islands:
            for p in isl.gaul:
                b = self.boxsize(thresh*isl.rms, p)
                bbox = N.s_[max(0, int(p[1]-b)):min(shape[0], int(p[1]+b+1)),
                            max(0, int(p[2]-b)):min(shape[1], int(p[2]+b+1))]

                gimg[bbox] += self.gaussian_fcn(params=p, *N.mgrid[bbox])

        img.fitted_img = gimg
        return img

    def boxsize(self, thresh, params):
        from math import ceil, sqrt, log
        A, x1c, x2c, sx1, sx2, th = params
        return ceil(sx1*sqrt(-2*log(thresh/A)))

    def gaussian_fcn(self, x1, x2, params):
        from math import radians, sin, cos
        A, x1c, x2c, sx1, sx2, th = params
        th = radians(th)
        cs = cos(th)
        sn = sin(th)
        f1 = ((x1-x1c)*cs + (x2-x2c)*sn)/sx1
        f2 = (-(x1-x1c)*sn + (x2-x2c)*cs)/sx2
        return A*N.exp(-(f1*f1 + f2*f2)/2)
