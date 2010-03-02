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
filename=LOFARSOFT+"/data/lopes/test.event"
#filename=LOFARSOFT+"/data/lopes/example.event"
#filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"

datareader_ptr=hOpenFile(filename)
Filesize=64*1024

#DataReader Parameters
nAntennas=30
Observatory="LOPES"
ObservationTime=0
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

nsigma=4

qualityfields=("mean","rms","nonGaussianity")
qualitylimits=((-15,15),(10,40),(-5,5))

def CheckParameterConformance(fields,limits,data):
    noncompliance=[]
    for i in range(len(fields)):
        if (data[i]<limits[i][0]) | (data[i]>limits[i][1]) : noncompliance.append(fields[i])
    return noncompliance


rawdata=IntVec()
where=IntVec()
where.resize(Blocksize)
probability=funcGaussian(nsigma,1,0)
npeaksexpected=probability*Blocksize
npeakserror=sqrt(npeaksexpected)
qualityflaglist=[]
print "Quality checking of Blocks: nsigma=",nsigma, "peaks expected=",npeaksexpected
for Antenna in range(nAntennas):
    for Block in range(nBlocks):
        print "Antenna={0:2d},".format(Antenna),"Block={0:3d}:".format(Block),
        hReadFile(rawdata,datareader_ptr,"Fx",Antenna,Blocksize,Block,Stride,Shift)
        datamean = rawdata.mean()
        datarms = rawdata.stddev(datamean)
        datanpeaks=rawdata.findgreaterthanabs(int(round(nsigma*datarms)),where)
        dataNonGaussianity=(datanpeaks-npeaksexpected)/npeakserror
        qualitydata=(datamean,datarms,dataNonGaussianity)
        print "mean={0:4.2f},".format(datamean),"rms={0:5.1f},".format(datarms),"npeaks={0:5d},".format(datanpeaks),"nonGaussianity={0:5.2f}".format(dataNonGaussianity),
        noncompliancelist=CheckParameterConformance(qualityfields,qualitylimits,qualitydata)
        if noncompliancelist:
            qualityflaglist.append((Antenna,Block,noncompliancelist))
            print noncompliancelist,"!!"
        else: print ""
print "Done."
print "Quality Flaglist:",qualityflaglist
hCloseFile(datareader_ptr)

#------------------------------------------------------------------------
print "\nNow testing calculation of geometric antenna delays"
antennapositions=[(-300,100,0.0),(0,0,1)]
skypositions=[(0,0,100),(1000,1000,3000),(-4000,3300,1000)]
print "Antennas=",antennapositions
print "Skypositions=",skypositions

ants=FloatVec()
ants.extendflat(antennapositions)
sky=FloatVec()
sky.extendflat(skypositions)
delays=FloatVec()
delays.resize(len(ants)*len(sky)/9)
hGeometricDelays(ants,sky,delays,False)

print "Delays=",delays

#execfile("hfplot.py")
#gr=mglGraph(mglGraphZB,800,600) # Create mgl Graphobject
#qw=hplot(gr,powerspectrum) # Plot it into simple Qt Widget


# ____________________________________________________________ Gain calibration

print "Performing Gain calibration..."

gainConversionFactor = 10.0
rawdata = FloatVec()
rawdata.resize(1024,1.0)
hADC2Voltage(rawdata, gainConversionFactor)


# ______________________________________________________________ Hanning filter

print "Creating Hanning filter..."

hanningFilter0 = FloatVec()
hanningFilter0.resize(size(rawdata))
hGetHanningFilter(hanningFilter0) # Basic hanning filter

hanningFilter1 = FloatVec()
hanningFilter1.resize(size(rawdata))
hGetHanningFilter(hanningFilter1,0.5) # Hanning filter with height parameter

hanningFilter2 = FloatVec()
hanningFilter2.resize(size(rawdata))
hGetHanningFilter(hanningFilter2,0.5, 256) # Hanning filter with height and width parameters

hanningFilter3 = FloatVec()
hanningFilter.resize(size(rawdata))
hGetHanningFilter(hanningFilter3,0.75) # Hanning filter with height, width and slope parameters

print "Applying Hanning filter to data..."

# Apply a basic Hanning window on the data
# hApplyHanningFilter(rawdata)

# Apply a previously defined filter on the data
hApplyFilter(rawdata, hanningFilter2)
