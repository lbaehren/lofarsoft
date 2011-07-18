#! /usr/bin/env python

"""
########### Dynamic spectrum pipeline ###########

-Clancy James, c.james@astro.ru.nl

This file will eventually contain a pipeline designed to generate
dynamic-spectrum data, that is, data in the time-frequency domain.

Currently it contains only the functionality from an earlier version
written by S. ter Veen, 'pipeline_start.py'.

All it does now is take data, run it through a filter, and produce
frequency-domain outputs. It also has the option of producing a
cumulative spectrum.


"""

############## loading packages ###############
import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import pycrtools as cr
import metadata as md
import IO

########### parameters needed to run  ###########

"""
These parameters will eventually be loaded from a parset file.
For now they are hard-coded, and need to be edited individually.
"""

# directory data is stored in - here I assume in $LOFARDATA
datadir="/Volumes/My Book/LOFAR_DATA/"

# names of individual files
filenames=["CS302C-B0T15:40:08.h5"]

# the selection of antennas to use. The only implementation so far is
# all of them (equivalent to 'None'), or a particular subset
antennaSelection = None
#antennaSelection = [0,1,4]

# number of raw voltages to Fourier transform
blocksize=1024

# sums power in the time and freq domains
timeIntegration = 5
freqIntegration = 1

print 'blocksize is ', blocksize

# position within the data to begin reading
startblock=0

# number of blocks to operate on
nblocks=1000

# types of filter to use on the data
filtertype='PPF'
#filtertype=None
#filtertype='Hanning'

# defines how calibration will proceed (just a true/false for now)
calMethod=True

# antennaSet
antennaSet='LBA_OUTER'

# option to produce an average spectrum
makeAvgSpectrum=True


############# program begins running ################


######## modifies inputs according to what is currently possible #######

if calMethod:
    print 'No calibration currently available - turning off'
    calMethod=False


########## checks files and opens them for reading ###########
for i in range(len(filenames)):
    filenames[i]=datadir+filenames[i]
    if not os.path.isfile(filenames[i]):
        raise IOError('File ', filenames[i], ' can not be found.')

crfile=IO.open(filenames,blocksize,antennaSelection)
actualSelection=crfile.get('selectedAntennas')
if antennaSelection:
    if not actualSelection==antennaSelection:
        raise IOError('Requested antennas could not be selected')
        antennaSelection=actualSelection
else:
    antennaSelection=actualSelection

######## setting up arrays ##########

nSelectedAntennas=len(antennaSelection)
fxdata=cr.hArray(float,[nSelectedAntennas, blocksize])
fftSize=blocksize/2+1
freqData=cr.hArray(complex, [nSelectedAntennas, fftSize])

# setting up cumulative and temporary arrays for avg spec
if makeAvgSpectrum:
    avg_spec = cr.hArray(float,[nSelectedAntennas, fftSize],0.)
    tempSpec = cr.hArray(complex,[nSelectedAntennas, fftSize])

# determining
freqDimSize = int(fftSize/freqIntegration)
timeDimSize = int(nblocks/timeIntegration)

# reduces the number of blocks
nblocks = timeDimSize * timeIntegration

# frequency dimensions increased - ones on edges will be ignored
if freqDimSize * freqIntegration < fftSize:
    freqDimSize += 1

# array to hold the dynamic spectrum
dynamicSpectrum = cr.hArray(float, [nSelectedAntennas,timeDimSize,freqDimSize])

########## sets up the filtering #########

if filtertype == 'PPF' and not blocksize==1024:
    print 'Blocksize must be 1024 for the ppf.'
    print 'Turning ppf to Hanning filter'
    filtertype = 'Hanning'

if filtertype == 'PPF':
    import ppf
    ppf=ppf.PPF()
    ppf.weights.div(65536.)
elif filtertype == 'Hanning':
    thefilter=cr.hArray(float,[blocksize])
    thefilter.gethanningfilter()
elif filtertype:
    print 'No known filter specified - setting to None.'
    filtertype = None


############# loops through data ###############

if filtertype =='PPF':
    # fills the internal buffer of the filter
    for block in range(startblock, startblock+15):
        crfile.getTimeseriesData(fxdata,block)
        ppf.add(fxdata)
    startblock += 15

timeCounter=0
timeIndex=0

for block in range(startblock, startblock+nblocks):
    crfile.getTimeseriesData(fxdata,block)

    if timeCounter==timeIntegration:
        timeCounter=0
        timeIndex += 1

    if filtertype == 'PPF':
        ppf.add(fxdata)
    elif filtertype == 'Hanning':
        fxdata[...].mul(thefilter)

    # calculating the power
    freqData[...].fftw(fxdata[...])
    cr.hAbs(freqData[...])
#    freqData.mul(freqData)

    dynamicSpectrum[...,timeIndex].muladd(freqData[...], freqData[...])

    timeCounter += 1


dynamicSpectrum.div(float(timeIntegration))
