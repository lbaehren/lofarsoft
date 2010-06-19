
import numpy as N
from scipy.optimize import leastsq
import matplotlib
from matplotlib import pylab as pl
import scipy


def func_gaus2d_sym(c, x, y):
    z=c[0]*N.exp(-0.5*((x-c[1])*(x-c[1])+(y-c[2])*(y-c[2]))/c[3]/c[3])
    return z

res =lambda p, x, y, z, err: (z-func_gaus2d_sym(p,x,y))/err

preal=[5.5, 4.5, 5.2, 2.3]

x,y=scipy.mgrid[0.1:10.1:0.1, 0.1:10.1:0.1]
err=N.ones(x.shape)*preal[0]/10.*0.

z=preal[0]*N.exp(-0.5*((x-preal[1])*(x-preal[1])+(y-preal[2])*(y-preal[2]))/preal[3]/preal[3])
z=z+N.random.random(y.shape)*preal[0]/50.0

p0=[3.0, 5.0,4.5, 3.0]

p, flag=leastsq(res, p0, args=(N.ravel(x), N.ravel(y), N.ravel(z), N.ravel(err)))
zfit=p[0]*N.exp(-0.5*((x-p[1])*(x-p[1])+(y-p[2])*(y-p[2]))/p[3]/p[3])

pl.figure()
pl.subplot(221)
pl.imshow(z,interpolation='nearest')
pl.colorbar()
pl.subplot(222)
pl.imshow(zfit,interpolation='nearest')
pl.colorbar()
pl.subplot(223)
pl.imshow(z-zfit,interpolation='nearest')
pl.colorbar()



