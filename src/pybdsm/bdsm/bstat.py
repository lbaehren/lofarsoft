import math
import numpy as N
import numpy.core.ma as M
from Image import Op

class Op_bstat(Op):
    def stat(self, arr):
        N = arr.count()
        return arr.mean(), arr.std() * math.sqrt(N/(N-1))

    def clip_stat(self, arr, mean, sigma, kappa=3):
        nmean = nsigma = None; cnt = 0
        while sigma != nsigma and cnt < 100:
            cnt += 1
            if nsigma:
                sigma = nsigma
                mean  = nmean
            mask = (abs(arr - mean) > kappa*sigma).filled(fill_value = True)
            narr = M.array(arr.data, mask = mask, copy = False)
            nmean, nsigma = self.stat(narr)

        if sigma != nsigma:
            raise RuntimeError("Sigma-clipped RMS calculation does not converge")
        return nmean, nsigma

    def __call__(self, img):
        data = img.img
        opts = img.opts

        mean, rms   = self.stat(data)
        cmean, crms = self.clip_stat(data, mean, rms)

        opts.mean  = cmean
        opts.rms   = crms

        print "mean: ", mean, "rms: ", rms, \
            "\ncmean: ", cmean, "crms: ", crms
        return img
    
class Op_thresholds(Op):
    def __call__(self, img):
        opts = img.opts
            ### for simplicity assume hard thresholds
        opts.thresh_pix = 5  # sigma's for hard threshold
        opts.isl_clip    = 3  # sigma's for island boundary
        opts.isl_minsize = 4  # minimal island size

        opts.isl_thresh = opts.mean + opts.rms*opts.isl_clip
        
        return img
