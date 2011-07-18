#! /usr/bin/env python
#
# Copyright (c) Arthur Corstanje, Sep 2010
#
# NO -> We use some GPL copyright for all our software ...
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


#HF: Note: (num)python has rows first, then columns ...?
#HF: x = np.arange(0, 10)
#HF: y=x.reshape(2,5)
#HF: -> array([[0, 1, 2, 3, 4],
#HF:           [5, 6, 7, 8, 9]])
#HF: y.sum(axis=0) -> array([ 5,  7,  9, 11, 13])
#HF: y.sum(axis=1) -> array([10, 35])

import numpy as np
import matplotlib.pyplot as plt
pi = np.pi; cos = np.cos; sin = np.sin; sqrt = np.sqrt

rowBlocksize = 128
columnBlocksize = 1024  # = bigFFTblocksize / rowBlocksize
bigFFTblocksize = rowBlocksize*columnBlocksize        # small enough to be done in memory (for this test)
w0 = 2 * pi / bigFFTblocksize # fundamental frequency of the 'big' signal

#fftsize = blocksize           # / 2 + 1 for real input... first do all of it

#numpy legacy code
t = np.arange(0, bigFFTblocksize)
x = cos((100 + 20 * sin(w0*t)) * w0 * t) # The FFT loves frequency-modulated signals...
x = x.reshape(columnBlocksize, rowBlocksize) # put into M x K matrix with M = size of row, K = size of column. And, # rows = N / columnBlocksize and vice versa.

#Now proceed with hftools implementation
a=hArray(complex,dimensions=x.shape,fill=hArray(x))
b=hArray(complex,dimensions=[bigFFTblocksize],fill=a)
bT=hArray(complex,dimensions=[bigFFTblocksize],fill=-1)
bigFFT=hArray(complex,dimensions=x.shape)
print "Do FFT in one step!"
bigFFT.fftw(a)   # done in one step, this is what we'll do below in two steps
print "Done. Do in two steps."

#First transpose of data and do ffts over rows
aT=hArray(a).transpose()
aT[...].fftw(aT[...])


# We'll multiply the result of the FFT-by-columns by Phi_ik = exp( - j 2pi/N (i*k))
aT.doublefftphasemul(bigFFTblocksize,columnBlocksize,rowBlocksize,0)

#Do the second transpose and fft again
a.transpose(aT)
a[...].fftw(a[...]) # should be the same as fftOverRows

#Final (3rd) transpose contains result
aT.transpose(a)
print "Done."
print "Do double fft in one c++ function:"
bT.doublefft(b,bigFFTblocksize,columnBlocksize,rowBlocksize,0)

#And plot the absolute values
print "Plot Values."

aT.abs()
aT.log()
bT.abs()
bT.log()
bigFFT.abs()
bigFFT.log()

aT.reshape([bigFFTblocksize])
bigFFT.reshape([bigFFTblocksize])

aT.plot()
bigFFT.plot(clf=False)
bT.plot(clf=False)
print "Done plotting."

print "Difference in abs log between full and double FFT:",abs(bigFFT.diffsquaredsum(aT)[0])

#plt.show()
