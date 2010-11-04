#! /usr/bin/env python 

import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import pycrtools as hf
import metadata as md
import IO as IO


#Configuration
# we need to have a more sensible way of getting to the file/filenames themselves
LOFARSOFT=os.environ["LOFARSOFT"]

######### Now we define what data we want to work on and get it ##############

datadir=LOFARSOFT+'/data/lofar/trigger-2010-04-15/'
filenames=["triggered-pulse-2010-04-15-RS205.h5"]

for i in range(len(filenames)):
    filenames[i]=datadir+filenames[i]

#start block and nrblocks to process
# if ppf operates on 16 blocks at once,
# then really we only have 4 useful blocks
# here before edge effects make us go insane
# we need to mke this more logical!

startblock=7
nblocks=100
blocksize=1024

######## Now we define what we want to do with the data ###########

# Filter type for the FFT: Hanning or PPF
#filtertype="PPF"
filtertype="Hanning"
if filtertype is "PPF":
    #Check if PPF is possible
    assert blocksize == 1024
    import PPF
    myppf=PPF.PPF()

########## Now we again look at subsets of the data ###########

#Antenna set
antennaset="LBA_OUTER"

#antenna selection (on number or by IDs)
#Examples:
#selection=[142000001]#,142000005,142002018]
#selection=[0,5,8]
selection=[8]
#selection=None

####### Now we read in the actual data and perform checks to ensure ########
####### that we will be able to do to it what we want to do with it ########

#Initialisation of arrays
crfile=IO.open(filenames,blocksize,selection)
fxdata=crfile["emptyFx"]
fftdata=crfile["emptyFFT"]
crfile.setAntennaset(antennaset)

# Apply calibration shift for clockdelays
#crfile.applyCalibrationShift()

# Gets Calibration object for weight and gain calibration
#MyCal=crfile.getCalibrator()

nyquistZone=crfile["nyquistZone"]
sin_freq=20.
#sweights0=np.linspace(0.,1023., 1024)
#sweights=hf.hArray(sweights0)
#sweights.mul(2.*hf.pi/sin_freq)
#sweights.sin()

# Obtain antenna positions. Last value True=hArray, False=np.array
#AntPos=md.get("RelativeAntennaPositions",crfile["antennaIDs"],crfile.antennaset,True)
# this should be much more efficient. We can use fftw in the advanced format to create
# one long fft over the whole block. At the very least do this for a bunch of stuff
for block in range(startblock,startblock+nblocks):

    crfile.readdata(fxdata,block)
    
#    fxdata.mul(sweights)
    #Quality check, to be made

    # Apply filter before the FFT
    if filtertype is "Hanning":
        fxdata[...].applyhanningfilter()
    elif filtertype is "PPF":
        fxdata.div(32768*2)
        myppf.add(fxdata)
    fftdata[...].fftcasa(fxdata[...],nyquistZone)
    # Apply calibration for station clock, antenna cables and station calibration (now only for LBA_OUTER, blocksize = 1024 )
    #MyCal.applyCalibration(fftdata)
    
#plot data:

fftdata.abs()
fftdata.log()
fftdata[...].plot(clf=False)
