# Test script to get relative arrival times of pulses from the octocopter data.
# Started by Arthur Corstanje, Oct 2012.

import pycrtools as cr
import numpy as np
import matplotlib.pyplot as plt

#filefilter = '/Volumes/WDdata/octocopter/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'
filefilter = '/Users/acorstanje/triggering/CR/octocopter_test/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'

filelist = cr.listFiles(filefilter)
if len(filelist) == 1:
    filelist = filelist[0]
f = cr.open(filelist, blocksize = 16384) # big blocks, then cut out pulses

assert f["ANTENNA_SET"] == 'LBA_OUTER'

# cheap pulse finder; get pulse time from crossing level ADC = 500
timeseries = f["TIMESERIES_DATA"]
threshold = 500
firstPulsePosition = timeseries[...].findgreaterthan(threshold) # returns first position where y > 500
firstPulsePosition = cr.hArray(firstPulsePosition).toNumpy()

noPulseCount = len(np.where(firstPulsePosition < 0)[0])
validPulses = firstPulsePosition[np.where(firstPulsePosition > 0)]
first = min(validPulses)
last = max(validPulses)
avg = int(np.average(validPulses))
print 'Pulse position first / last / avg: %d, %d, %d' % (first, last, avg)

nofChannels = f["NOF_SELECTED_DATASETS"]

# Cut out a block around the pulse, store in new hArray
cutoutLength = 1024
start = avg - cutoutLength / 2
end = avg + cutoutLength / 2
if (end < last) or (start > first):
    raise ValueError('Blocksize too small for all pulses to fit in it!')

timeseries_cut_to_pulse = cr.hArray(float, [nofChannels, cutoutLength])
timeseries_cut_to_pulse[...].copy(timeseries[..., start:end])

# may want to subtract the pulse offsets here; requires bookkeeping.
#        timeseries_data_cut_to_pulse_offsets=hArray(int,ndipoles,fill=(maxima_power.maxx+0.5-timeseries_data_cut_pulse_width/2))
#        timeseries_data_cut_to_pulse[...].copy(pulse.timeseries_data_cut[..., timeseries_data_cut_to_pulse_offsets:timeseries_data_cut_to_pulse_offsets+timeseries_data_cut_pulse_width])


# now cross correlate all channels in full_timeseries, get relative times
crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", timeseries_cut_to_pulse, oversamplefactor=16)

y = crosscorr.crosscorr_data.toNumpy()
plt.plot(y[0]) # will have maximum at 16 * blocksize / 2 (8192)
plt.figure()
plt.plot(y[2]) # will have a shifted maximum


#And determine the relative offsets between them
sample_interval = 5.0e-9
maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=5, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = 0)

#print startTimes
maxima_cc.lags *= 1e9
print maxima_cc.lags

# plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

plt.figure()
cr.hArray(maxima_cc.lags).plot()

"""
sdev = crosscorr.crosscorr_data[...].stddev()
maxima = crosscorr.crosscorr_data[...].max() # need to look at positive maximum only!

ksigma = hArray(maxima / sdev)
k = ksigma.toNumpy()

x = np.where(k < 4.5) # bad indices
print 'Number of BAD channels: ', len(x[0])
if len(x[0]) > 0:
    y = hArray(maxima_cc.lags).toNumpy()
    yy = y[x]
    plt.scatter(x, yy)
    plt.title('Delays from cross correlations\nBlue dots represent flagged delays (no clear maximum)')

plt.figure()
hArray(ksigma).plot()
plt.title('Strength (in sigmas) of the crosscorrelation maximum')
"""