execfile("hftools.py")
import os
from math import *

def demo(descr,code,code2=""):
    print descr
    if code2 != "":
        print code,"=",eval(code), ";", code2," = ",eval(code2)
    else:
        print code,"=",eval(code)
n=IntVec()
v1=FloatVec()
v1=FloatVec()
v1.extend([1,2,3,4,0,5])
v2=FloatVec()
v2.resize(5)
n.resize(7)
print v1
hNegate(v1)
print v1
hAbs(v1)
print v1
hNegate(v1)
print v1
hAbs(v1,v2)
print v2
v2+=v2
print v2
v2+=3
print v2

v3=FloatVec()
v3.resize(10)
v2.mul(3.5,v3)
print v2,v3

print "\n------------------------------------------------------------------------"
demo("Vector:","v1")
demo("Vector:","v2")
demo("Resize a vector:","v2.resize(8)","v2")
demo("Resize a vector and fill with non-zero values:","v2.resize(10,-1)","v2")
v3=v2.new()
print "Make a new vector of same size and type as original one:"
print "v3=v2.new();v3=",v3
demo("Fill a vector with values","v3.fill(-2)","v3")
demo("Adding scalar to vector:","v1+3")
demo("Adding two vectors:","v1+v2")
demo("Vector:","v1")
v1+=v2
print "Adding a vector in place:"
print "v1+=v2; v1=",v1
demo("Mean:","v1.mean()")
demo("Median:","v1.median()")
demo("Summing:","v1.sum()")
demo("Standard Deviation:","v1.stddev()")
#------------------------------------------------------------------------
#Now use Dataeader
#------------------------------------------------------------------------
LOFARSOFT=os.environ["LOFARSOFT"]
filename=LOFARSOFT+"/data/lopes/example.event"
#filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"

datareader_ptr=hOpenFile(filename)
Filesize=64*1024

#DataReader Parameters
nAntennas=8
Antenna=0; Blocksize=Filesize/2; Stride=0; Shift=0; 
nBlocks=Filesize/Blocksize
FFTSize=Blocksize/2+1

#Define data vectors to work with
fftdata = ComplexVec()

powerspectrum = FloatVec()
powerspectrum.resize(FFTSize,0.0)

print "Summing up Blocks: ",
for Block in range(nBlocks): 
    print Block,",",
    hReadFile(fftdata,datareader_ptr,"CalFFT",Antenna,Blocksize,Block,Stride,Shift)
    fftdata.SpectralPower(powerspectrum) 
print " - Done."

rawdata = IntVec()
where=IntVec()
where.resize(Blocksize)
nsigma=4
probability=funcGaussian(nsigma,1,0)
npeaksexpected=probability*Blocksize
npeakserror=sqrt(npeaksexpected)


print "Quality checking of Blocks: nsigma=",nsigma, "peaks expected=",npeaksexpected
for Antenna in range(nAntennas):
    for Block in range(nBlocks): 
        print "Antenna={0:2d},".format(Antenna),"Block={0:3d}:".format(Block),
        hReadFile(rawdata,datareader_ptr,"Fx",Antenna,Blocksize,Block,Stride,Shift)
        datamean = rawdata.mean()
        datarms = rawdata.stddev(datamean)
        datanpeaks=rawdata.findgreaterthanabs(int(round(nsigma*datarms)),where)
        nonGaussianity=(datanpeaks-npeaksexpected)/npeakserror
        print "mean={0:4.2f},".format(datamean),"rms={0:5.1f},".format(datarms),"npeaks={0:5d},".format(datanpeaks),"nonGaussianity={0:5.2f}".format(nonGaussianity)
print "Done."
hCloseFile(datareader_ptr)


#execfile("hfplot.py")
#gr=mglGraph(mglGraphZB,800,600) # Create mgl Graphobject
#qw=hplot(gr,powerspectrum) # Plot it into simple Qt Widget
