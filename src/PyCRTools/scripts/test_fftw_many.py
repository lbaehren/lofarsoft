#! /usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr

A = 5
N = 1024
Nf = N/2+1
howmany = 10

# Create input and output arrays
x_in = cr.hArray(A*np.sin(np.linspace(0,100,N*howmany)*100)+np.random.rand(N*howmany))
x_out = cr.hArray(A*np.sin(np.linspace(0,100,N*howmany)*100)+np.random.rand(N*howmany))

y = cr.hArray(complex, Nf*howmany)

# Create a plan
p = cr.FFTWPlanManyDftR2c(N, howmany, 1, N, 1, Nf, cr.fftw_flags.ESTIMATE)

# Execute plan
cr.hFFTWExecutePlan(y, x_in, p)

# Compute inverse FFT
p_inv = cr.FFTWPlanManyDftC2r(N, howmany, 1, Nf, 1, N, cr.fftw_flags.ESTIMATE)

cr.hFFTWExecutePlan(x_out, y, p_inv)

plt.subplot(3,1,1)
plt.plot(x_in.toNumpy())
plt.subplot(3,1,2)
plt.plot(np.abs(y.toNumpy())/Nf)
plt.subplot(3,1,3)
plt.plot(x_out.toNumpy())
plt.show()

