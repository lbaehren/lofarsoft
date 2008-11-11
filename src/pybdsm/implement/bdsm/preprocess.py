"""Module preprocess

Calculates some basic statics of the image and sets up processing
parameters for PyBDSM.
"""
import numpy as N
import _cbdsm
from image import *

### Insert attributes into Image class
Image.raw_mean = Float(doc="Unclipped image mean")
Image.raw_rms  = Float(doc="Unclipped image rms")
Image.clipped_mean = Float(doc="Clipped image mean")
Image.clipped_rms  = Float(doc="Clipped image rms")


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
