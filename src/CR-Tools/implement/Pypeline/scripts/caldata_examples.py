#! /usr/bin/env python 


######## A Modified Pipeline Start Script by Clancy ##########

# this is intended to show off the module 'caldata'


import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import pycrtools as cr
from IOpkg import calData as cal
import time as time

reload(cal)


LOFARSOFT=os.environ["LOFARSOFT"]
datadir=LOFARSOFT+'/data/lofar/trigger-2010-04-15/'
filename=["triggered-pulse-2010-04-15-RS205.h5"]

filename[0]=datadir+filename[0]

#########

# initiates a calibration data object with default values
print 'Initiating class with default values only'
caldata = cal.calData(filename, "LBA_OUTER")
# normally we perform a calibration, but this data is crap
print 'Default for the fft is ', caldata.fftMethod
print 'Default for the filter is: ', caldata.filterType
print 'Default block size is: ', caldata.blockSize
print 'Calibrate? ', caldata.calMethod

print 'We have not set any antenna selection: ', caldata.antennaSelection
print 'so do not have any o/p-related quantities: '
print '   nAntennas = ', caldata.nAntennas
print '   op array = ', caldata.timeData
print '   op array = ', caldata.freqData
print '\nand hence can not get data back: '

data=caldata.getFFTData()
print ' the data is: ', data

print '\n\n But when we DO select the antennas: '
selection=range(20,38)
caldata.setAntennaSelection(selection)

caldata.setFrequencyRange(3e7,5e7)

print 'We DO have these quantities!'
print '   nAntennas = ', caldata.nAntennas
print '   op array = ', caldata.timeData
print '   op array = ', caldata.freqData
print '\nand CAN get frequency-domain data back! '
print caldata.getFFTData()
print 'from block: ', caldata.currentBlock, 'and again:'
print caldata.getFFTData()
print 'from ', caldata.currentBlock
print '\n\n'

print 'we can also ask to get back various quantities ourselves'
freqData=caldata.getFFTData()
timeData=caldata.timeData
allfrequencies=caldata.allFreqData
print 'which can be plotted in the usual way'
print freqData, '\n', timeData, '\n', allfrequencies
print '\n\n\n'

print '\nIf we want to plot, we can ask caldata what frequencies it is returning'
print 'frequencies in Hz are: ', caldata.frequencies

print 'It will also give us the antenna positions'
print 'Antenna positions are: ', caldata.getAntennaPositions()

print '\n\n If we want to use our own arrays which will ',\
'not be over-written for future calls, we can get the right dimensions:'
freqdata=cr.hArray(complex,caldata.getOPDim())
allFreqData=cr.hArray(complex, caldata.getAllFreqDim())
timeData=cr.hArray(float, caldata.getTimeDim())
print 'and just send these arrays to the get routine: '
freqData=caldata.getFFTData(allFreqData=allFreqData, timeData=timeData)
print 'and these will be used, while the internal arrays will remain unaltered'

