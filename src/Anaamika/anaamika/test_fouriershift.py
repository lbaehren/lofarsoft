from pylab import *
import numpy as N
import scipy
from scipy import ndimage
from scipy import fft

snr=5

x=N.arange(50)
l0=N.zeros(50)

oc = 0.33
for i in range(50):
   l0[i] = exp(-0.5*(i-25-oc)*(i-25-oc)/4.3/4.3)

noi = N.random.normal(0., 1./snr, 50)
l0 += noi
    

f1=ndimage.fourier_shift(scipy.fft(l0),7.0)
f2=ndimage.fourier_shift(scipy.fft(l0),7.7)

y1=scipy.ifft(f1).real
y2=scipy.ifft(f2).real

x0=N.arange(0,50,0.1)
y0=N.arange(0,50,0.1)
for i in range(500):
   y0[i] = exp(-0.5*(x0[i]-25-oc)*(x0[i]-25-oc)/4.3/4.3)


plot(x, l0, '-ro', x-7.0, y1, 'b+-', x-7.7, y2, 'g+-')

