#! /usr/bin/env python

#execfile(PYP+"/scripts/testfft.py")
#execfile('/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testfft.py')

from pycrtools import *

#Bechmnark test to compare the FFTs in the current CASA implementation
#against a direct FFTW implementation
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"

filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
datafile=crfile(filename)
N=2**18
#N=2**16
datafile["blocksize"]=N
fx=datafile["Fx"]
fx2=datafile["emptyFx"]
fft=datafile["emptyFFT"]

fx1=hArray(complex,dimensions=fx)
fft1=hArray(complex,dimensions=fx)

LOOPS=100

# Set up input/output arrays
y = hArray(complex, N/2+1)

# Create plan
t0=time.clock()
p = FFTWPlanManyDftR2c(N, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)
for i in range(LOOPS):
    hFFTWExecutePlan(y, fx, p)
    pass
t3=time.clock()-t0
print "t=",t3,"s -","FFTW with plan"

t0=time.clock()
for i in range(LOOPS):
    fft.fftcasa(fx,1)
    pass
t2=time.clock()-t0
print "t=",t2,"s -","CASA"

t0=time.clock()
for i in range(LOOPS):
    fft.fftw(fx)
    pass
t1=time.clock()-t0
print "t=",t1,"s -","FFTW"

print "t2/t1=",t2/t1


#fft2=fft.new()
#fft2.fftw(fx)
#print "fft2",list(fft2.vec())
#hNyquistSwap(fft2,2)
#print "fft2,swap",list(fft2.vec())
#fft.fftcasa(fx,2)
#print "fft",list(fft.vec())
