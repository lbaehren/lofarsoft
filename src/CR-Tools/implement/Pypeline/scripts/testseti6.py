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
fullsize=256*128
nblocks=128
stride=4
#------------------------------------------------------------------------
full_blocklen=fullsize/nblocks # block length of data when read in
blocklen=full_blocklen/stride
blocklen_section=blocklen/stride
nsubblocks=stride*nblocks
nblocks_section=nblocks/stride
subspeclen=blocklen*nblocks


t0=time.clock(); print "Setting up."
#Open file
datafile=crfile(filename)
antennas=list(datafile["antennas"])
nAntennas=datafile["nofSelectedAntennas"]
antenna=0 # Select which antenna to read in
cdata=hArray(complex,[nblocks,blocklen]) # creating input and work array
cdataT=hArray(complex,[blocklen,nblocks]) # creating output array with transposed axis

tmpspecT=hArray(complex,[stride,nblocks_section,blocklen]) 
#tmpspec=hArray(complex,[nblocks_section,stride,blocklen]) 
tmpspec=hArray(complex,[nblocks_section,full_blocklen]) 

specT=hArray(complex,[full_blocklen,nblocks_section]) 
specT2=hArray(complex,[stride,blocklen,nblocks_section]) 
spec=hArray(complex,[blocklen,nblocks]) 
allspec=hArray(complex,[stride,subspeclen]) 

bigfft=hArray(complex,[fullsize]) 
print "Time:",time.clock()-t0,"s for set-up."

t0=time.clock(); print "Reading in data and doing a full FFT."
datafile["blocksize"]=fullsize #Setting initial block size
bigfft.read(datafile,"Fx",0,antenna)
print "Time:",time.clock()-t0,"s for reading."
bigfft.fftw(bigfft)
print "Time:",time.clock()-t0,"s for reading + FFT."

t0=time.clock(); print "Reading in data and doing a double FFT."
datafile["blocksize"]=blocklen #Setting initial block size
ofiles=[]
for offset in range(stride):
    print "Pass #",offset
    blocks=range(offset,nsubblocks,stride)
    cdata[...].read(datafile,"Fx",blocks,antenna)
    print "Time:",time.clock()-t0,"s for reading."
    cdataT.doublefft1(cdata,fullsize,nblocks,blocklen,offset)
    print "Time:",time.clock()-t0,"s for reading + FFT."
    ofile=tmpfilename+str(offset)+"a"+tmpfileext
    ofiles+=[ofile]
    cdata.writedump(ofile)  # output of doublefft1 is in cdata ...
    
#Now sort the different blocks together (which requires a transpose over passes/strides)
ofiles2=[]
for offset in range(stride):
    tmpspecT[...].readdump(Vector(ofiles),Vector(int,stride,fill=offset))
    #This transpose it to make sure the blocks are properly interleaved
    hTranspose(tmpspec,tmpspecT,stride,nblocks_section)
    specT.doublefft2(tmpspec,nblocks_section,full_blocklen)
    ofile=tmpfilename+str(offset)+"b"+tmpfileext
    specT.writedump(ofile)
    ofiles2+=[ofile]

ofiles3=[]
for offset in range(stride):
    specT2[...].readdump(Vector(ofiles2),Vector(int,stride,fill=offset))
    #This transpose it to make sure the blocks are properly interleaved
    hTranspose(spec,specT2,stride,blocklen)
    ofile=tmpfilename+"spec_"+str(offset)+tmpfileext
    spec.writedump(ofile)
    ofiles3+=[ofile]

for offset in range(stride):
    allspec[...].readdump(Vector(ofiles3),Vector(int,stride,fill=offset))

print "Time:",time.clock()-t0,"s for resorting, 2nd FFT, and 3rd transpose."

allspec.abs()
bigfft.abs()
allspec.plot(logplot="y")
bigfft.plot(logplot="y",clf=False)
print "Mean difference between full and blockwise FFT:", allspec.meandiffsquaredsum(bigfft)
