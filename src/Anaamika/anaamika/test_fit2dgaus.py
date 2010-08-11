
import numpy as N
from scipy.optimize import leastsq
import pylab as pl
import scipy
from math import log10

def gaussian(c, x, y):
    from math import cos, sin, pi
    rad = 180.0/pi

    cs = cos(c[5]/rad)
    sn = sin(c[5]/rad)
    f1 = ((x-c[1])*cs+(y-c[2])*sn)/c[3]
    f2 = ((y-c[2])*cs-(x-c[1])*sn)/c[4]
    val = c[0]*N.exp(-0.5*(f1*f1+f2*f2))

    return val

#def gaussian(p, x2, y2):
#    """Returns a gaussian function with the given parameters"""
#    height, center_x, center_y, width = p
#    #return lambda x,y: height*N.exp(
#    return height*N.exp(-(((center_x-x2)/width)**2+((center_y-y2)/width)**2)/2)

def fitgaussian(data, p, x2, y2):

    params = p
    print 'initial params = ',params
    errorfunction = lambda p: N.ravel(gaussian(p, x2, y2) - data)
    p, success = leastsq(errorfunction, params)
    return p


x2, y2 = scipy.mgrid[0:10:0.1, 0:10:0.1]
p = [1.0, 2.5, 7.25, 2.23, 3.2, 30.0]
print 'real params = ',p

snr = N.power(10.0, N.arange(log10(0.1), log10(100.0), 0.1))
noise = p[0]/snr

param_list=[]
pl.figure()
for i in range(len(snr)):
    z2 = gaussian(p, x2, y2)+ N.random.normal(loc=0.0, scale = noise[i], size=x2.shape)

    rndm = N.random.random(6)*0.5+0.7
    p0 = p*rndm
    
    params = fitgaussian(z2, p0, x2, y2)
    params[5] = params[5] % 360
    param_list.append(params)
    z2_fit = gaussian(params, x2, y2)

    print 'SNR = ', snr[i], '; Fitted params = ',params

    pl.subplot(6,6,i+1)
    pl.imshow(z2-z2_fit,interpolation='nearest')
    pl.colorbar()
    pl.title('SNR='+str(N.int(N.round(snr[i]*10))/10.0))
    

param_list = N.array(param_list)
mn = N.min(snr)
mx = N.max(snr)
lim1 = [mn, mx]

pl.figure()
pl.suptitle('SNR (independent gaussian noise) vs fitted parameters')
titles = ['Amplitude', 'x-centre', 'y-centre', 'sigma-x', 'sigma-y', 'P.A.']
for i in range(6):
    pl.subplot(3,2,i+1)
    pl.title(titles[i])
    pl.semilogx(snr, param_list[:,i],'r')
    pl.semilogx(lim1, [p[i], p[i]],'b')
    




