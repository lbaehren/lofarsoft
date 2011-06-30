
import numpy as N
from scipy.optimize import leastsq
import pylab as pl
import scipy

def func_1d_res(c, x, z, err):
    res = (z - func_1d(c, x))/err
    return res

def func_1d(c, x):
    import math
    y = c[0]*N.sin(2*math.pi*c[1]*x+c[2])*N.exp(-x/c[3])
    return y

def func_gaus2d_sym_res(c, x, y, z):
    res=(z - func_gaus2d_sym(c,x,y))
    return z

def func_gaus2d_sym(c, x, y):
    val=c[0]*N.exp(-0.5*((x-c[1])*(x-c[1])+(y-c[2])*(y-c[2]))/c[3]/c[3])
    return val


x1 = N.arange(0.0,0.1,0.002)
n = len(x1)
p1_real = N.array([10.0, 1.0/3e-2, 3.14159/6.0, 0.02])
z1 = func_1d(p1_real, x1)
z1 = z1 + N.random.random(n)*0.1
err = N.ones(n)*0.1

p1_0 = N.array([8.0, 1.0/2.3e-2, 3.14159/3.0, 0.01])
p1, flag = leastsq(func_1d_res, p1_0, args=(x1, z1, err))

print p1_real
print p1_0
print p1
pl.figure()
pl.plot(x1, z1, 'or')
pl.plot(x1, func_1d(p1_0, x1), '-c')
pl.plot(x1, func_1d(p1_real, x1), '-r')
pl.plot(x1, func_1d(p1, x1), '-b')

def gaussian(height, center_x, center_y, width):
    """Returns a gaussian function with the given parameters"""
    width = float(width)
    return lambda x,y: height*N.exp(
                -(((center_x-x)/width)**2+((center_y-y)/width)**2)/2)


x2,y2 = scipy.mgrid[0.1:10.1:0.1, 0.1:10.1:0.1]
sh = x2.shape

p2_real = N.array([4.5, 3.25, 5.65, 4.353])
z2 = func_gaus2d_sym(p2_real, N.ravel(x2), N.ravel(y2))

p2_0 = N.array([3.5, 5.5, 4.6, 3.0])
p2_0 = p2_real*0.9
p2, flag = leastsq(func_gaus2d_sym_res, p2_0, args=(N.ravel(x2), N.ravel(y2), z2))

z2_fit = func_gaus2d_sym(p2, N.ravel(x2), N.ravel(y2))

print p2_real
print p2_0
print p2

z2_fit = z2_fit.reshape(sh)
z2 = z2.reshape(sh)

print '--------'

indexes = scipy.mgrid[0.1:10.1:0.1, 0.1:10.1:0.1]
errorfunction = lambda p: N.ravel(gaussian(*p2_0)(*indexes) - z2)
p_new, success = leastsq(errorfunction, p2_0)

print p_new


pl.figure()
pl.subplot(221)
pl.imshow(z2,interpolation='nearest')
pl.colorbar()
pl.subplot(222)
pl.imshow(z2_fit,interpolation='nearest')
pl.colorbar()
pl.subplot(223)
pl.imshow(z2-z2_fit,interpolation='nearest')
pl.colorbar()

pl.figure()
pl.plot(N.ravel(z2), '.r')
pl.plot(func_gaus2d_sym(p2_real, N.ravel(x2), N.ravel(y2)), '-r')
pl.plot(func_gaus2d_sym(p2_0, N.ravel(x2), N.ravel(y2)), '-c')
pl.plot(func_gaus2d_sym(p2, N.ravel(x2), N.ravel(y2)), '-b')


