#! /usr/bin/env python
#
# Copyright (c) Arthur Corstanje, Sep 2010
#
# This implements and tests a size-N FFT, with N = M K,
# by splitting up the FFT in a radix-M (or radix-K) splitting step.
# When N is too large to fit in memory, like when we'll take 256 * 10^6 Samples from an entire TBB buffer,
# the calculation can be done by doing 2 M FFTs of size-M, plus N complex multiplications (phase factors). 
# In this case we'd have M = 16384 which is relatively small.
# Generalization to N = M K is easy (as long as N factorizes).
#
# So the procedure is:
#
# make a MxK matrix-shape out of the length-N signal
# do FFT on all the columns in the matrix (or: transpose first, then operate on rows)
# multiply element-wise by a set of phase factors (see below)
# do FFT on all the rows 
# result is in the columns of this FFT.
#
# Therefore:
# transpose the result (columns to rows)
# unravel to length-N output
#
# We've anticipated that one 'row' can be kept in memory;
# doing the two transpositions is a significant remaining problem (not addressed here).
# Maybe it can be reduced to just one transpose, that would be much better.

import numpy as np
import matplotlib.pyplot as plt
pi = np.pi; cos = np.cos; sin = np.sin; sqrt = np.sqrt

bigFFTblocksize = 2048        # small enough to be done in memory (for this test)
w0 = 2 * pi / bigFFTblocksize # fundamental frequency of the 'big' signal
rowBlocksize = 16                
columnBlocksize = 128         # = bigFFTblocksize / rowBlocksize

#fftsize = blocksize           # / 2 + 1 for real input... first do all of it

t = np.arange(0, bigFFTblocksize)
#x = 1 + sin((32+5) * w0 * t) 
x = cos((100 + 20 * sin(w0*t)) * w0 * t) # The FFT loves frequency-modulated signals... 
bigFFT = np.fft.fft(x)                   # done in one step, this is what we'll do below in two steps

x = x.reshape(columnBlocksize, rowBlocksize) # put into M x K matrix with M = size of row, K = size of column. And, # rows = N / columnBlocksize and vice versa.

phaseFactorMatrix = (2*pi/bigFFTblocksize)*np.ones(bigFFTblocksize)
phaseFactorMatrix = phaseFactorMatrix.reshape(columnBlocksize, rowBlocksize)
for i in range(columnBlocksize):
    for k in range(rowBlocksize):
        phaseFactorMatrix[i, k] *= -(i*k) 

# We'll multiply the result of the FFT-by-columns by Phi_ik = exp( - j 2pi/N (i*k))
# Explanation follows elsewhere...

phaseFactorMatrix = np.exp(phaseFactorMatrix * 1j) # complex exponential

# in a large FFT, operations over columns mean you have to transpose first, then operate over rows... Not done here.
fftOverColumns = np.fft.fft(x, axis=0)
    
fftOverColumns *= phaseFactorMatrix

fftOverRows = np.fft.fft(fftOverColumns, axis=1)

testFFT=fftOverRows.transpose()
testFFT=testFFT.reshape(bigFFTblocksize)

plt.plot(t, np.abs(bigFFT))
plt.plot(t, np.abs(testFFT))
print 'The mean squared difference between one-step and two-step FFTs is: %e' % np.linalg.norm(bigFFT - testFFT)
print 'while the norm of the FFT is: %e' % np.linalg.norm(bigFFT)

plt.show()


# x = np.cos(5 * w0 * t)
# y = np.vstack((x, x, x))
# np.fft.fft(y, axis=1)

#for i in range(10):
#    print fftOverColumns[i, 1]
#print ' '
#for i in range(32,42):
#    print bigFFT[i]
