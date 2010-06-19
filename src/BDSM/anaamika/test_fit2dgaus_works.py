
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
print '#########################################'
print '#####  End of 1d ########################'
print '#########################################'

####################################################################################
##  2d 
####################################################################################

def gaussian(p, x2, y2):
    """Returns a gaussian function with the given parameters"""
    height, center_x, center_y, width = p
    #return lambda x,y: height*N.exp(
    return height*N.exp(-(((center_x-x2)/width)**2+((center_y-y2)/width)**2)/2)

def moments(data):
    """Returns (height, x, y, width_x, width_y)
    the gaussian parameters of a 2D distribution by calculating its
    moments """
    from math import sqrt
    total = data.sum()
    X, Y = N.indices(data.shape)
    x = (X*data).sum()/total
    y = (Y*data).sum()/total
    col = data[:, int(y)]
    width_x = sqrt(abs((N.arange(col.size)-y)**2*col).sum()/col.sum())
    row = data[int(x), :]
    width_y = sqrt(abs((N.arange(row.size)-x)**2*row).sum()/row.sum())
    height = data.max()
    return height, x, y, (width_x+width_y)/2.0

def fitgaussian(data, p, x2, y2):
    """Returns (height, x, y, width_x, width_y)
    the gaussian parameters of a 2D distribution found by a fit"""
    params = moments(data)
    params = p
    print 'initial params = ',params
    errorfunction = lambda p: N.ravel(gaussian(p, x2, y2) -
                                 data)
    p, success = leastsq(errorfunction, params)
    return p


#x2,y2 = scipy.mgrid[0.1:10.1:0.1, 0.1:10.1:0.1]
#x2,y2 = scipy.mgrid[0:100:1, 0:100:1]
#sh = x2.shape
#
#p2_real = N.array([4.5, 32.0, 56.0, 13.0])
#z2 = func_gaus2d_sym(p2_real, N.ravel(x2), N.ravel(y2))
#
#p2_0 = N.array([3.5, 5.5, 4.6, 3.0])
#p2_0 = p2_real*0.9
#p2, flag = leastsq(func_gaus2d_sym_res, p2_0, args=(N.ravel(x2), N.ravel(y2), z2))
#
#z2_fit = func_gaus2d_sym(p2, N.ravel(x2), N.ravel(y2))
#
#print p2_real
#print p2_0
#print p2
#
#z2_fit = z2_fit.reshape(sh)
#z2 = z2.reshape(sh)
#
#print '\n--------------------------------------------\n'

x2, y2 = scipy.mgrid[0:10:0.1, 0:10:0.1]
p = [3.2, 4.5, 5.25, 3.23]

z2 = gaussian(p, x2, y2)+ N.random.random(x2.shape)*0.02

print 'real params = ',p
params = fitgaussian(z2, list(N.array(p)*0.8), x2, y2)
z2_fit = gaussian(params, x2, y2)

print 'fitted params = ',params


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
pl.show()

#pl.figure()
#pl.plot(N.ravel(z2), '.r')
#pl.plot(func_gaus2d_sym(p2_real, N.ravel(x2), N.ravel(y2)), '-r')
#pl.plot(func_gaus2d_sym(p2_0, N.ravel(x2), N.ravel(y2)), '-c')
#pl.plot(func_gaus2d_sym(p2, N.ravel(x2), N.ravel(y2)), '-b')


