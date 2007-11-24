import numpy as N
from image import Op

class Op_bstat(Op):
    """Calculate image statistics
    """
    def __call__(self, img):
        import _cbdsm
        opts = img.opts

        mask = (False if img.img_mask is img.nomask else img.img_mask)
        mean, rms, cmean, crms = _cbdsm.bstat(img.img, mask, opts.rms_clip)

        opts.mean  = cmean
        opts.rms   = crms

        print "mean: ", mean, "rms: ", rms, \
            "\ncmean: ", cmean, "crms: ", crms
        return img


class Op_thresholds(Op):
    """Compute thresholds
    """
    def __call__(self, img):
        opts = img.opts

        opts.thresh_isl = opts.mean + opts.rms*opts.isl_clip
        opts.thresh_isl2= opts.mean + opts.rms*opts.isl_peak_clip

        return img
