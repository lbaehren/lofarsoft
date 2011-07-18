#! /usr/bin/env python

import time
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr

N = 64#16*1024

# Create input and output arrays
x0 = cr.hArray(np.sin(np.linspace(0,100,N)*100)+np.random.rand(N))
x1 = cr.hArray(np.sin(np.linspace(0,100,N)*100)+np.random.rand(N))
x2 = cr.hArray(np.sin(np.linspace(0,100,N)*100)+np.random.rand(N))
x3 = cr.hArray(np.sin(np.linspace(0,100,N)*100)+np.random.rand(N))
#y0 = cr.hArray(complex, N/2+1)
y1 = cr.hArray(complex, N/2+1)
y2 = cr.hArray(complex, N/2+1)

# Create a plan
p = cr.FFTWPlanManyDftR2c(N, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

# Execute plan
tstart = time.time()
for i in range(1000):
    cr.hFFTWExecutePlan(y1, x0, p)
tend = time.time()

print "fftw", tend-tstart, "s"

tstart = time.time()
for i in range(1000):
    y2.fftcasa(x0, 1)
tend = time.time()

print "fftcasa", tend-tstart, "s"

# Compare results

print "FFTW", y1
print "FFT CASA", y2

plt.figure()
plt.subplot(2,1,1)
plt.plot(np.abs(y1.toNumpy()))
plt.subplot(2,1,2)
plt.plot(np.abs(y2.toNumpy()))

# Compute inverse FFT
pinv = cr.FFTWPlanManyDftC2r(N, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
cr.hFFTWExecutePlan(x1, y1, pinv)
cr.hFFTWExecutePlan(x3, y2, pinv)

x2.invfftcasa(y2, 1)

print x0
print x1
print x2

plt.figure()
plt.subplot(4,1,1)
plt.plot(x0.toNumpy())
plt.subplot(4,1,2)
plt.plot(x1.toNumpy()/N)
plt.subplot(4,1,3)
plt.plot(x2.toNumpy())
plt.subplot(4,1,4)
plt.plot(x3.toNumpy()/N)
plt.show()

## Compute complex to complex FFT
#p1 = cr.FFTWPlanManyDft(N/2+1, 1, 1, 1, 1, 1, cr.fftw_sign.FORWARD, cr.fftw_flags.ESTIMATE)
#cr.hFFTWExecutePlan(y2, y1, p1)

