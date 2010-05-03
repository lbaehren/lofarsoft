#

#Bechmnark test to compare the FFTs in the current CASA implementation
#against a direct FFTW implementation
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"

filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
datafile=crfile(filename)
datafile["blocksize"]=2**18
fx=datafile["Fx"]
fx2=datafile["emptyFx"]
fft=datafile["emptyFFT"]

fx1=hArray(complex,dimensions=fx)
fft1=hArray(complex,dimensions=fx)

LOOPS=100

t0=time.clock()
for i in range(LOOPS):
    fft.fftw(fx)
t1=time.clock()-t0
print "t=",t1,"s -","FFTW"
t0=time.clock()
for i in range(LOOPS):
    fx.fft(fft,1)
t2=time.clock()-t0
print "t=",t2,"s -","CASA"
print "t2/t1=",t2/t1

#fft2=fft.new()
#fft2.fftw(fx)
#print "fft2",list(fft2.vec())
#hNyquistSwap(fft2,2)
#print "fft2,swap",list(fft2.vec())
#fx.fft(fft,2)
#print "fft",list(fft.vec())
