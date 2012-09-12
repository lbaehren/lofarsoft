"""
Check a given set of datafiles (for one event) for pulses in timeseries.
======================

Created by Arthur Corstanje, Sep 2012
"""

from pycrtools import hArray
import pycrtools as cr
#from pycrtools.tasks.shortcuts import *
#from pycrtools import metadata as md
import numpy as np
import matplotlib.pyplot as plt
#from datetime import datetime
#from pycrtools import xmldict


# use filefilter as parameter
filefilter = '/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS00*.h5'
blocksize = 65536
# get corresponding file list
filelist = cr.listFiles(filefilter)
superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007"]
if len(filelist) == 1:
    filelist = filelist[0]
else: # sort files on station ID
    sortedlist = []
    for station in superterpStations:
        thisStationsFile = [filename for filename in filelist if station in filename]
        if len(thisStationsFile) > 0:
            sortedlist.append(thisStationsFile[0])
    filelist = sortedlist
print '[checkForPulses] Processing files: '
print filelist

# Open file, get number of blocks, process timeseries data...

f = cr.open(filelist, blocksize = blocksize)
nofchannels = len(f["SELECTED_DIPOLES"])
print '# channels = %d' % nofchannels

nblocks = min(f["DATA_LENGTH"]) / blocksize # MAXIMUM_READ_LENGTH should be implemented... this may not work
nblocks -= 2 # hack
timeseries_data = f.empty("TIMESERIES_DATA")

pulseLocationsPerAntenna = np.zeros( (nofchannels, 2048) )
maximaPerAntenna = np.zeros( (nofchannels, 2048) )
npulsesPerAntenna = np.zeros(nofchannels)
plotted = False
for i in range(nblocks):
    print 'Block: %d of %d' % (i, nblocks)
#    f["BLOCK"] = i
    f.getTimeseriesData(timeseries_data, block = i)

    # get all sigmas
    sigma = cr.hArray( timeseries_data[...].stddev() ) # avoid problems with Vectors
    timeseries_data[..., 32000:32007] = 500.0 + np.random.rand() * 10.0 # artificial peak
    timeseries_data[..., 32001] = -500.0
    timeseries_data[..., 32002] = +500.0
    timeseries_data[..., 21000:21007] = 300.0 + np.random.rand() * 20.0
    timeseries_data[...] /= sigma[...] # if dividing out sigma to get it in SNR-units
    print "searching per antenna"
    # Look for >= 7-sigma pulses
    pulse = cr.trerun("LocatePulseTrain","separate",timeseries_data, nsigma = 7, minpulselen = 7, prepulselen = 400, search_per_antenna=True) 
    peaksPerAntenna = cr.hArray(pulse.npeaks_list).toNumpy()

    indexlist = pulse.indexlist.toNumpy()
    if pulse.maxima: 
        maxima = pulse.maxima.toNumpy()
        if maxima.max() > 15 and peaksPerAntenna.sum() > 20 and not plotted:
            y = pulse.timeseries_data_cut.toNumpy()
            for k in range(nofchannels):
                plt.plot(y[k])
            plotted = True
        for ch in range(nofchannels):
            for j in range(peaksPerAntenna[ch]):
                pulseLocationsPerAntenna[ch, npulsesPerAntenna[ch] + j] = indexlist[ch, j, 0] + i * blocksize
                maximaPerAntenna[ch, npulsesPerAntenna[ch] + j] = maxima[ch, j]
        npulsesPerAntenna += peaksPerAntenna

plt.figure()
for i in range(nofchannels):
    select_index = np.where(maximaPerAntenna[i] > 0.01)
    x = pulseLocationsPerAntenna[i][select_index]
    y = maximaPerAntenna[i][select_index]
    plt.plot(x, y, 'o')
    

#for i in range(nofchannels):
#    pulseLocationsPerAntenna[i, npulsesPerAntenna:npulsesPerAntenna+peaksPerAntenna] = indexlist[i, npulsesPerAntenna:npulsesPerAntenna+peaksPerAntenna, 0]


