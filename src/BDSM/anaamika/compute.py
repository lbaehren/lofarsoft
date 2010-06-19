""" This has extra routines """

import numpy as N
import scipy

def moment(x,mask=None):
    """ Calculates first 3 moments of numpy array x. Only those values of x 
      for which mask is False are used, if mask is given. """
    if mask == None:
        mask=N.zeros(x.shape, dtype=bool)
    m1=N.zeros(1)
    m2=N.zeros(x.ndim) 
    m3=N.zeros(x.ndim)
    for i, val in N.ndenumerate(x):
        if not mask[i]:
            m1 += val
	    m2 += val*N.array(i)
	    m3 += val*N.array(i)*N.array(i)
    m2 /= m1
    m3 = N.sqrt(m3/m1-m2*m2)
    return m1, m2, m3
        
def fit_mask_1d(x, y, sig, mask, funct, order=0):
    """ calls scipy.optimise.leastsq for a 1d function with a mask."""
    import functions as func
    from scipy.optimize import leastsq

    #print mask.shape, x.shape, y.shape, sig.shape
    ind=N.where(mask==False)
    n=sum(mask)
    xfit=x[ind]; yfit=y[ind]; sigfit=sig[ind]

    if funct == func.poly:
       p0=N.array([0]*(order+1))
       p0[1]=(yfit[0]-yfit[-1])/(xfit[0]-xfit[-1])
       p0[0]=yfit[0]-p0[1]*xfit[0]
    if funct == func.wenss_fit:
       p0=N.array([y[N.argmax(x)]] + [1.])
    res=lambda p, x, y, err: (y-funct(p, x))/sig
    (p, flag)=leastsq(res, p0, args=(x, y, sig))

    return p

def dist_2pt(p1, p2):
    """ Calculated distance between two points given as tuples p1 and p2. """
    from math import sqrt
    dx=p1[0]-p2[0]
    dy=p1[1]-p2[1]
    dist=sqrt(dx*dx + dy*dy)

    return dist

def std(y):
    """ Returns unbiased standard deviation. """
    from math import sqrt

    l=len(y)
    s=N.std(y)
    return s*sqrt(float(l)/(l-1))



def imageshift(image, shift):
    """ Shifts a 2d-image by the tuple (shift). Positive shift is to the right and upwards. 
    This is done by fourier shifting. """
    from scipy import ndimage

    shape=image.shape

    f1=scipy.fft(image, shape[0], axis=0)
    f2=scipy.fft(f1, shape[1], axis=1)

    s=ndimage.fourier_shift(f2,shift, axis=0)

    y1=scipy.ifft(s, shape[1], axis=1)
    y2=scipy.ifft(y1, shape[0], axis=0)

    return y2.real

