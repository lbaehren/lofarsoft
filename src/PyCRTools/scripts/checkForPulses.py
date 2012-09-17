"""
Check a given set of datafiles (for one event) for pulses in timeseries.
======================

Created by Arthur Corstanje, Sep 2012
"""

from pycrtools import hArray
import pycrtools as cr
import numpy as np
import matplotlib.pyplot as plt
import sys
# use filefilter as parameter
#filefilter = '/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS00*.h5'

if len(sys.argv) != 2:
    print 'Use checkForPulses <filefilter>'
    exit()
else:
    filefilter = sys.argv[1]

#filefilter = '/Volumes/WDdata/bigdata/L62972_D20120801T100626.???Z_CS00[2356]_R000_tbb.h5'
#filefilter = '~/test/L63248_D20120809T195349.417Z_CS003_R000_tbb.h5'
blocksize = 65536
nblocks = 12 # or from f["DATA_LENGTH"], see below
nsigma = 5
minpulselen = 7

# get corresponding file list, sorted by station...
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
#nblocks = max(100, nblocks)
nblocks -= 1 # hack
timeseries_data = f.empty("TIMESERIES_DATA")

pulseLocationsPerAntenna = np.zeros( (nofchannels, 2048) )
maximaPerAntenna = np.zeros( (nofchannels, 2048) )
npulsesPerAntenna = np.zeros(nofchannels)

# Arrays for writing the # antennas in the strongest pulse, for each data block
antennasPerBlock = np.zeros(nblocks)
locationStrongestPulse = np.zeros(nblocks)
strongestMaximum = np.zeros(nblocks)
plotted = False
for i in range(nblocks):
    print 'Block: %d of %d' % (i, nblocks)
#    f["BLOCK"] = i
    f.getTimeseriesData(timeseries_data, block = i)

    # get all sigmas
    sigma = cr.hArray( timeseries_data[...].stddev() ) # avoid problems with Vectors
#    timeseries_data[..., 32000:32007] = 500.0 + np.random.rand() * 10.0 # artificial peak
#    timeseries_data[..., 32001] = -500.0
#    timeseries_data[..., 32002] = +500.0
#    timeseries_data[..., 21000:21007] = 300.0 + np.random.rand() * 20.0
    timeseries_data[...] /= sigma[...] # dividing out sigma to get it in SNR-units
    print "searching per antenna"
    # Look for >= 7-sigma pulses
    pulse = cr.trerun("LocatePulseTrain","separate",timeseries_data, nsigma = nsigma, minpulselen = minpulselen, prepulselen = 400, search_per_antenna=True) 
    peaksPerAntenna = cr.hArray(pulse.npeaks_list).toNumpy()

    indexlist = pulse.indexlist.toNumpy()
    if pulse.npeaks > 0: 
        maxima = pulse.maxima.toNumpy()
        # needed maximaStrongestPulse, or can do as well with 'maxima'? Timeseries_data_cut can contain more crossings of the threshold than pulse detections...
        maximaStrongestPulse = cr.hArray( pulse.timeseries_data_cut[...].max() ).toNumpy()
        # remove possible NaN's, convert to zero
        NaNlocations = np.isnan(maximaStrongestPulse)
        maximaStrongestPulse[NaNlocations] = 0
        antennasAboveThreshold = len( np.where(maximaStrongestPulse > pulse.nsigma)[0] )
        print 'Block %d: start pos %d, time = %3.3f ms, # antennas for strongest pulse = %d' % (i, pulse.start, 1e3 * (pulse.start + i*blocksize) / 200.0e6, pulse.npeaks)
        antennasPerBlock[i] = pulse.npeaks # antennasAboveThreshold
        locationStrongestPulse[i] = pulse.start + i * blocksize
        strongestMaximum[i] = maximaStrongestPulse.max()
        # test plot
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
    
plt.xlim(xmin = 0, xmax = nblocks * blocksize)

for i in range(nblocks):
    x = locationStrongestPulse[i] 
    y = strongestMaximum[i] * 1.03
    
    plt.annotate(str(int(antennasPerBlock[i])), (x, y), horizontalalignment = 'center')

#for i in range(nofchannels):
#    pulseLocationsPerAntenna[i, npulsesPerAntenna:npulsesPerAntenna+peaksPerAntenna] = indexlist[i, npulsesPerAntenna:npulsesPerAntenna+peaksPerAntenna, 0]


