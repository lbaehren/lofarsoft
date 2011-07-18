#!/usr/bin/env python
#
#  testbigcrfile.py
#  CRPipeline
#
#  Created by Arthur Corstanje on 4/27/11.
#  Copyright (c) 2011 __MyCompanyName__. All rights reserved.

from pycrtools import *
import pycrtools as cr 
from pycrtools import pulsefit as pf
from pycrtools import beamformer as bf
import numpy as np

def incoherentadd(crf, length = -1, blockno = -1, doPlot = True):
    if length == -1:
        len = crf["DATA_LENGTH"][0] / 2
    else:
        len = length
    print crf["DATA_LENGTH"]
    nofAntennas = crf["NOF_DIPOLE_DATASETS"]
    crf["SELECTED_DIPOLES"] = [crf["DIPOLE_NAMES"][0]]
    if length == -1:
        crf["BLOCKSIZE"] = len
    else:
        crf["BLOCKSIZE"] = length
    if blockno == -1:
        crf["BLOCK"] = 1
    else:
        crf["BLOCK"] = blockno
    result = crf["EMPTY_TIMESERIES_DATA"]
    for i in range(0, 96, 2): # nofAntennas
        print 'Doing antenna %d' % i
        crf["SELECTED_DIPOLES"] = [crf["DIPOLE_NAMES"][i]]
        a = crf["TIMESERIES_DATA"]
        result.add(a)
    #crf["SELECTED_DIPOLES"] = crf["DIPOLE_NAMES"] # put selection back to all
    if doPlot:
        toplot = result.toNumpy()
        print toplot[0].size
#        import pdb; pdb.set_trace()
#        print len(result)
        x = np.arange(toplot[0].size, dtype=float)
        x *= 5.0e-6 # ms
        timeoffset = crf["SAMPLE_NUMBER"][0] * 5.0e-6 + blockno * len * 5.0e-6
        x += timeoffset
        print x[1]
        print x.size
        print toplot.size
        plt.plot(x, toplot[0])
        plt.ylabel('Voltage (ADC units)')
        plt.xlabel('Time (ms)')
#        result.plot()
        raw_input("Plotted incoherent addition of all antennas.")
    return result
    
filename = '/Users/acorstanje/triggering/LORA/17mei/LORAdump--3-121.h5'
#filename = '/data/acorstanje/LORA/LORAdump--5-123.h5'
blocksize = 65536 
blockNo = 0
crfile = cr.open(filename)

#incoherentadd(crfile, length=65536, blockno=blockNo)

samplefreq = 200.0e6 # must be
dataLength = crfile["DATA_LENGTH"][0]
print 'Data length is %d' % dataLength # once again assume all the same; move to Datacheck?
print 'Block size is now: %d' % blocksize
nblocks = int(dataLength) / int(blocksize)
print 'So there are: %d blocks' % nblocks
nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
crfile["BLOCKSIZE"] = blocksize # workaround, needed for correct settings in Beamformer
     
#blockNo = int((triggerFitResult["avgToffset"] * samplefreq) / blocksize)
#print "fullPulseFit: set block-number to:", blockNo
#  crfile.set("block", blockNo)

cr_time = hArray(float, dimensions=[blocksize])

cr_efield = crfile["EMPTY_TIMESERIES_DATA"]

crfile.getTimeseriesData(cr_efield, blockNo) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...

# temp: plot to see if pulse is in this block...
#  efield = cr_efield.toNumpy()
#  plt.clf()
#  plt.plot(efield.T)
#  raw_input("--- Plotted pulse, check if pulse is in block - press Enter to continue...")

cr_fft = crfile["EMPTY_FFT_DATA"]
#  import pdb; pdb.set_trace()
#cr_fft = hArray(complex, dimensions = [nofAntennas, blocksize/2 + 1])

print 'Number of antennas = %d' % cr_fft.shape()[0]
print 'Block length fft = %d' % cr_fft.shape()[1]

crfile["ANTENNA_SET"] = "LBA_OUTER" # HACK !!
antenna_positions = crfile["RELATIVEANTENNA_POSITIONS"]
cr_fft[...].fftw(cr_efield[...])
for i in range(cr_fft.shape()[0]):
    cr_fft[i, 0] = 0.0 # kill DC offsets

ant_indices = range(0, nofAntennas, 2)
maxHeight = 0

#for i in range(1):
    # (50, 12) for block 80
    # (51, 12) for block 79
az = 62.0 #float(i) * 360.0 / 60
el = 90.0 - 26.3
start_position = [az, el, 1.0]
print 'Using start position az = %f, el = %f' % (az, el)
thisBF = bf.Beamformer(crfile, cr_fft) # initialize object. Isn't this an ugly memory leak?

for block in range(nblocks):
    crfile.getTimeseriesData(cr_efield, block) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...
    cr_fft[...].fftw(cr_efield[...])
    for i in range(cr_fft.shape()[0]):
        cr_fft[i, 0] = 0.0 # kill DC offsets
    tiedArrayBeam = thisBF.getTiedArrayBeam(start_position, cr_fft, antenna_positions, ant_indices, FarField=False)
    length = len(tiedArrayBeam)
    beam = tiedArrayBeam.toNumpy()
    start = block * blocksize
    
    # Now count the pulses; taken from the pycrtools-tutorial
    nsigma = 5 # parameter
    datamean = tiedArrayBeam.mean()
    thresholdHigh = tiedArrayBeam.stddev(datamean)
    thresholdHigh *= nsigma
    thresholdLow = thresholdHigh * (-1.0)
    thresholdLow += datamean
    thresholdHigh += datamean
    maxgap = Vector(int, len(datamean), fill=20) # parameter...
    minlength = Vector(int, len(datamean), fill=1) # idem

    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    pulses = hArray(int, dimensions=[100, 2], name = 'Location of the pulses')
    npulses = pulses[...].findsequenceoutside(tiedArrayBeam, thresholdLow, thresholdHigh, maxgap, minlength)
        
    beammax = tiedArrayBeam.max()[0]
    beammin = tiedArrayBeam.min()[0]
    if abs(beammin) > beammax:
        beammax = abs(beammin)
    
    toplot = beam
    #plt.clf()
    x = np.arange(toplot.size, dtype=float)
    x *= 5.0e-6 # ms
    timeoffset = crfile["SAMPLE_NUMBER"][0] * 5.0e-6 + block * blocksize * 5.0e-6
    x += timeoffset
    #plt.plot(x, toplot)
    print 'Done block %d, time start = %f, mean = %f, stddev = %f, threshold = %f, max = %f' % (block, timeoffset, datamean[0], thresholdHigh[0] / nsigma, thresholdHigh[0], beammax)
#    print npulses
    if beammax > thresholdHigh[0]:
        plt.clf()
        plt.plot(x, toplot)
        raw_input('plotted this beam')

        
    
#    (fitDataEven, optBeamEven) = pf.simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices,  FarField=False,blocksize=blocksize, method='smoothedAbs', doPlot=True)
    
#    height = - fitDataEven[1]
#    if height > maxHeight:
#        toplot = optBeamEven.toNumpy()
#        print toplot[0].size
#        import pdb; pdb.set_trace()
#        print len(result)
#        x = np.arange(toplot.size, dtype=float)
#        x *= 5.0e-6 # ms
#        timeoffset = crfile["SAMPLE_NUMBER"][0] * 5.0e-6 + blockNo * blocksize * 5.0e-6
#        x += timeoffset
#        print x[1]
#        print x.size
#        print toplot.size
#        plt.plot(x, toplot)
#        plt.ylabel('Voltage (ADC units)')
#        plt.xlabel('Time (ms)')

       # optBeamEven.plot()
        #raw_input("Opt beam.")
     
# HACK
#  start_position = [255.0, 25.0]
#    for entry in flaggedList:
#        if entry in ant_indices:
#            ant_indices.remove(entry) # or [ant_indices.remove(x) for x in flaggedList if x in ant_indices] ...
#import pdb; pdb.set_trace()
#(fitDataEven, optBeamEven) = simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices, cr_freqs, FarField=FarField,blocksize=blocksize, method=method, doPlot=doPlot)