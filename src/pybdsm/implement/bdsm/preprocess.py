"""Module preprocess

Calculates some basic statics of the image and sets up processing
parameters for PyBDSM.
"""
import numpy as N
import _cbdsm
from image import Op

class Op_preprocess(Op):
    """Preprocessing -- calculate some basic statistics and set 
    processing parameters"""

    def __call__(self, img):
        arr = img.image
        mask = img.mask

        mean, rms, cmean, crms = _cbdsm.bstat(arr, mask, img.opts.rms_clip)

        img.raw_mean    = mean
        img.raw_rms     = rms
        img.clipped_mean= cmean
        img.clipped_rms = crms

        return img
