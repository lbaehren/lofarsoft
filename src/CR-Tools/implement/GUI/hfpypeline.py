execfile("hftools.py")

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

datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/example.event")
#DataReader Parameters
Antenna=1; Blocksize=1024; Stride=0; Shift=0

#Define data vectors to work with
fftdata = ComplexVec(); powerspectrum = FloatVec()
fftdata.resize(Blocksize,0.0); 
powerspectrum.resize(Blocksize,0.0)

for Block in range(1,3): 
    hReadFile(fftdata,datareader_ptr,"CalFFT",Antenna,Blocksize,Block,Stride,Shift)
    fftdata.SpectralPower(powerspectrum) 

hCloseFile(datareader_ptr)
#execfile("hfplot.py")
#gr=mglGraph(mglGraphZB,800,600) # Create mgl Graphobject
#qw=hplot(gr,powerspectrum) # Plot it into simple Qt Widget
