#! /usr/bin/env python

"""
Test of a SETI application by making a high-resolution spectrum of a long data set, using a double FFT.

This file tests a double FFT all at once in memory.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
from pycrtools import *
import time

#------------------------------------------------------------------------
#Input values
#------------------------------------------------------------------------
tmpfilename="testseti_"
tmpfileext=".dat"
filename= LOFARSOFT+"/data/lofar/CS302C-B0T6:01:48.h5"
filename= LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
nblocks=4096
#nblocks=256
stretch=2
stride=1
maxchunks=0
#------------------------------------------------------------------------
datafile=crfile(filename)

fullsize=nblocks*nblocks*stretch
full_blocklen=fullsize/nblocks # block length of data when read in
blocklen=full_blocklen/stride
blocklen_section=blocklen/stride
nsubblocks=stride*nblocks
nblocks_section=nblocks/stride
subspeclen=blocklen*nblocks
subsubspeclen=min(blocklen*nblocks,2**16)
nsubsubspectra=subspeclen/2/subsubspeclen #factor 2 since we only look at first half
speclen=fullsize/2+1
dobig=(fullsize<=4194304)
dostride=(stride>1)
nchunks=datafile.filesize/fullsize

#start_frequency=10**8;end_frequency=2*10**8
start_frequency=0; end_frequency=10**8
delta_frequency=(end_frequency-start_frequency)/(speclen-1.0)
delta_band=(end_frequency-start_frequency)/stride*2
subfrequencies=hArray(float,[subspeclen],name="Frequency",units=("M","Hz"))
frequencies=hArray(float,[subspeclen/2],name="Frequency",units=("M","Hz"))
subfrequencies=hArray(frequencies.vec(),[nsubsubspectra,subsubspeclen],name="Frequency",units=("M","Hz"))


print "Frequency Resolution:",delta_frequency,"Hz"
if fullsize>10**6:
    print "Full size:",fullsize/10**6,"MSamples"
    print "Full size:",fullsize/1024/1024*16,"MBytes"
else:
    print "Full size:",fullsize/10**3,"kSamples"
    print "Full size:",fullsize/1024*16,"kBytes"

t0=time.clock(); print "Setting up."
#Open file
antennas=list(datafile["antennas"])
nAntennas=datafile["nofSelectedAntennas"]
antenna=0 # Select which antenna to read in

cdata=hArray(complex,[nblocks,blocklen]) # creating input and work array
cdataT=hArray(complex,[blocklen,nblocks]) # creating output array with transposed axis

#Note, that all the following arrays have the same memory als cdata and cdataT
tmpspecT=hArray(cdataT.vec(),[stride,nblocks_section,blocklen]) 
tmpspec=hArray(cdata.vec(),[nblocks_section,full_blocklen]) 

specT=hArray(cdataT.vec(),[full_blocklen,nblocks_section]) 
specT2=hArray(cdataT.vec(),[stride,blocklen,nblocks_section]) 
spec=hArray(cdata.vec(),[blocklen,nblocks])

power=hArray(float,[subspeclen/2],name="Power of spectrum",xvalues=frequencies,par=[("logplot","y")])
subpower=hArray(power.vec(),[nsubsubspectra,subsubspeclen],name="Power of spectrum",xvalues=frequencies,par=[("logplot","y")])
    
subspec=hArray(cdata.vec(),[subspeclen],name="FFT",xvalues=subfrequencies,par=[("logplot","y")]) 
print "Time:",time.clock()-t0,"s for set-up."

if dobig:
    bigfrequencies=hArray(float,[fullsize])
    bigfrequencies.fillrange(start_frequency/10**6,delta_frequency/10**6)
    bigfrequencies.setUnit("M","Hz")
    bigfft=hArray(complex,[fullsize],name="Big FFT",xvalues=bigfrequencies,par=[("logplot","y")])
    t0=time.clock(); print "Reading in data and doing a full FFT."
    datafile["blocksize"]=fullsize #Setting initial block size
    bigfft.read(datafile,"Fx",0,antenna)
    print "Time:",time.clock()-t0,"s for reading."
    bigfft.fftw(bigfft)
    print "Time:",time.clock()-t0,"s for reading + FFT."
    bigfft.abs()
else:
    print "Not doing big fft - memory demands too high."

t0=time.clock(); print "Reading in data and doing a double FFT."
datafile["blocksize"]=blocklen #Setting initial block size
for nchunk in range(min(nchunks,maxchunks)):
    t0=time.clock(); print "Starting with chunk #",nchunk,". Reading in data and doing a double FFT."
    ofiles=[]; ofiles2=[]; ofiles3=[]
    for offset in range(stride):
        print "Pass #",offset
        blocks=range(offset+nchunk*nsubblocks,(nchunk+1)*nsubblocks,stride)
        cdata[...].read(datafile,"Fx",blocks,antenna)
        print "Time:",time.clock()-t0,"s for reading."
        cdataT.doublefft1(cdata,fullsize,nblocks,blocklen,offset)
        print "Time:",time.clock()-t0,"s for 1st FFT."
        if dostride:
            ofile=tmpfilename+str(offset)+"a"+tmpfileext
            ofiles+=[ofile]
            cdata.writedump(ofile)  # output of doublefft1 is in cdata ...
    #Now sort the different blocks together (which requires a transpose over passes/strides)
    print "Time:",time.clock()-t0,"s for 1st FFT now doing 2nd FFT."
    for offset in range(stride):
        if dostride:
            print "Pass #",offset
            tmpspecT[...].readdump(Vector(ofiles),Vector(int,stride,fill=offset))
            #This transpose it to make sure the blocks are properly interleaved
            hTranspose(tmpspec,tmpspecT,stride,nblocks_section)
        specT.doublefft2(tmpspec,nblocks_section,full_blocklen)
        if dostride:
            ofile=tmpfilename+str(offset)+"b"+tmpfileext
            specT.writedump(ofile)
            ofiles2+=[ofile]
    print "Time:",time.clock()-t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
    for offset in range(stride):
        ofile=tmpfilename+"spec_"+str(offset)+tmpfileext
        if dostride:
            print "Pass #",offset
            specT2[...].readdump(Vector(ofiles2),Vector(int,stride,fill=offset))
             #This transpose it to make sure the blocks are properly interleaved
            hTranspose(spec,specT2,stride,blocklen)
            if (nchunk>0): power.readdump(ofile)
            else: power.fill(0.0)
            power.spectralpower(spec)
            ofiles3+=[ofile]
        else:
            power.spectralpower(specT)
        power.writedump(ofile)
    print "Time:",time.clock()-t0,"s for resorting, 2nd FFT, and 3rd transpose."
print "End of all chunks."

datapeaks=hArray(int,[nsubsubspectra,subsubspeclen/16],name="Idexlist of Peaks")

def findpeaks(subpower,threshold=7):
    datamean=hArray(subpower[...].meaninverse())
    datathreshold = hArray(subpower[...].stddevbelow(datamean.vec()))
    datathreshold *= threshold
    npeaks=datapeaks[...].findgreaterthan(subpower[...],datathreshold.vec())
    return npeaks
    
def rp(offset,sub=-1,clf=True):
    """Basic plotting of a part of the specturm"""
    if (sub==-1) & (subspeclen>524288):
        print "Spectrum too large, plotting subspectrum #0"
        sub=0
    if dobig:
        bigfft[offset*subspeclen:(offset+1)*subspeclen].plot(xvalues=bigfrequencies[offset*subspeclen:(offset+1)*subspeclen],clf=True)
        clf=False
    if dostride: power.readdump(ofiles3[offset])
    frequencies.fillrange((start_frequency+offset*delta_band)/10**6,delta_frequency/10**6)
    if sub>=0:
        sub=min(sub,nsubsubspectra-1)
        subpower[sub].plot(xvalues=subfrequencies[sub],clf=clf)
    else:
        power.plot(clf=clf)

if maxchunks==0:
    print "maxchunks==0 - > Reading spectrum from file."  
    power.readdump("spectrum.dat")
#for i in range(stride): rp(i)


#rp 0,151
#npeaks=findpeaks(subpower)
