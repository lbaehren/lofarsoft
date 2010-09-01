#! /usr/bin/env python 

import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import pycrtools as hf
import metadata as md
import IO as IO


#Configuration
datadir="/Users/STV/Astro/data/TBB/Crab/pulse1/"
filenames=["CS302C-B0T5:00:39.h5","CS302C-B1T5:00:39.h5","CS302C-B2T5:00:39.h5","CS302C-B3T5:00:39.h5","CS302C-B4T5:00:39.h5","CS302C-B5T5:00:39.h5"]
for i in range(len(filenames)):
    filenames[i]=datadir+filenames[i]

#start block and nrblocks to process
startblock=30
nblocks=1
blocksize=1024*1024

# Filter type for the FFT: Hanning or PPF
filtertype="Hanning"
if filtertype is "PPF":
    #Check if PPF is possible
    assert blocksize == 1024
    import PPF
    myppf=PPF.PPF()

#Antenna set
antennaset="HBA"

#antenna selection (on number or by IDs)
#Examples:
selection=[142000001]#,142000005,142002018]
#selection=[0,5,8]
#selection=None

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

# Obtain antenna positions. Last value True=hArray, False=np.array
#AntPos=md.get("RelativeAntennaPositions",crfile["antennaIDs"],crfile.antennaset,True)
for block in range(startblock,startblock+nblocks):

    crfile.readdata(fxdata,block)
    
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
