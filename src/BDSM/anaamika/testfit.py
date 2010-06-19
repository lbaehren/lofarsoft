
import numpy as N
from scipy.optimize import leastsq
import matplotlib
from matplotlib import pylab as pl


def func(c, x):
    import numpy as N
    y=N.sqrt(c[0]*c[0]+c[1]*c[1]/(x*x))
    return y

res =lambda p, x, y, err: (y-func(p,x))/err

x=N.arange(20)*0.1 + 0.1

preal=[2.5, 0.4]
y=N.sqrt(preal[0]*preal[0]+preal[1]*preal[1]/(x*x))
y=y+N.random.random(len(y))*0.3
err=N.ones(len(x))

p0=[1.8, 0.83]
p, flag=leastsq(res, p0, args=(x, y, err))

pl.figure(1)
pl.show(1)
pl.clf()
pl.plot(x,y,'ro-')
pl.plot(x,N.sqrt(p[0]*p[0]+p[1]*p[1]/(x*x)),'b-')


