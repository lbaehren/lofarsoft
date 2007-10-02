import sys
sys.path.append('')

import bdsm

chain = [bdsm.Op_loadFITS(), bdsm.Op_bstat(), bdsm.Op_thresholds(), 
         bdsm.Op_islands()] #, bdsm.Op_gausfit()]

img = bdsm.execute(bdsm.fits_chain,
#                   {'fits_name': 'cpp.fits'})
				   {'fits_name': 'dummy.fits'})
#                   {'fits_name': 'simul3.fits'})
#                   {'fits_name': 'WN65341H.fits', 'beam': (.015, .015, 0)})

sys.exit()


from pylab import *
import numpy as N
import scipy.optimize as opt
from test_fcn import *

def find_peak(arr):
    idx = arr.argmax(axis=None)
    return (arr.flat[idx], N.unravel_index(idx, arr.shape))

isl = img.islands[0]
#isl.opts.beam = (2, 1, 90)
peak, idx = find_peak(isl.masked)

print 'Beam used: ', isl.opts.beam
print 'bbox: ', isl.bbox

o = bdsm.gausfit.gaus_fcn(isl)
o.add_gaussian(peak, idx)
par = o.pars
o.fit(final=0, verbose=2)

o2=gfcn(isl)
o2.add_gaussian(peak, idx)
o2.fit(final=1, verbose=1)

sys.exit()

img_data = img.islands[0].img.data

def g2d(x1, x2, params):
    if len(params) != 6:
        raise "Wrong number of parameters"
    
    amp, x1o, x2o, sx1, sx2, theta = params
    theta = math.radians(theta)
    x1 -= x1o; x2 -= x2o
    f1 = ( x1*math.cos(theta) + x2*math.sin(theta))/sx1
    f2 = (-x1*math.sin(theta) + x2*math.cos(theta))/sx2
    return amp * N.exp(-0.5*(f1**2 + f2**2))

def fcn(x):
    val = N.fromfunction(g2d, img_data.shape, params=x)
    N.subtract(img_data, val, val)
    #N.multiply(val, val, val)
    return val.flatten()

def fcn2(x):
    t = fcn(x)
    return (t*t).sum()

#par = [4.7, 6, 4, 1.7, 1.7, 90]
#par = [4.76530313492, 6, 4, 1.6986434618357242, 1.6986434618357242, 90.0]
res = opt.leastsq(fcn, par, full_output=1)#, xtol=1e-3, ftol=1e-3)
print "================"
print "Res: ", res[0]
print "err: ", res[3], res[4]
print "chisq: ", fcn2(res[0])
print "nfev: ", res[2]['nfev']


from test_fcn2 import *
f = gfcn()
f.set_image(img_data)
f.set_params(par)
f.fit(final=1, verbose=1)
