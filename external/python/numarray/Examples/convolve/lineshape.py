# Copyright (C) 2002 Jochen Küpper <jochen@jochen-kuepper.de>


"""Example usage for Convolve.Lineshape"""

__author__  = "Jochen Küpper <jochen@jochen-kuepper.de>"
__date__    = "$Date: 2002/08/23 03:52:22 $"[7:-11]
__version__ = "$Revision: 1.1 $"[11:-2]


import os, sys
import numarray as num
import random as ran

import Convolve
import Convolve.Lineshape as ls


def main(*args):
    resolution = 0.1
    fwhm = 0.5
    # get some (random) data, i.e. a stick-spectrum
    x = num.arange(-50, 50+resolution, resolution)
    y = num.zeros(x.shape, num.Float)
    for i in range(10):
        y[ran.randint(0, len(y)-1)] = ran.random()
    # create lineshape objects
    g = ls.GaussProfile(num.arange(-10*fwhm, 10*fwhm+resolution, resolution), 1.0)
    l = ls.LorentzProfile(num.arange(-10*fwhm, 10*fwhm+resolution, resolution), 1.0)
    v = ls.VoigtProfile(num.arange(-10*fwhm, 10*fwhm+resolution, resolution), (0.6, 0.6))
    # convolute data with profile
    res_g = Convolve.convolve(y, g(), Convolve.SAME)
    res_l = Convolve.convolve(y, l(), Convolve.SAME)
    res_v = Convolve.convolve(y, v(), Convolve.SAME)
    for i in zip(x, y, res_g, res_l, res_v):
        print "%12.6f %12.6f %12.6f %12.6f %12.6f" % i


if __name__ == '__main__':
    main(*sys.argv[1:])

