#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr

eventdir = '/Users/acorstanje/triggering/CR/results_john/VHECR_LORA-20110714T174749.986Z'

datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

for datadir in datadirs:
    if not os.path.isfile(os.path.join(datadir,"results.py")):
        continue
    resfile=open(os.path.join(datadir,"results.py"))
    
    res = {}
    execfile(os.path.join(datadir,"results.py"),res)
    res = res["results"]

    trms = np.array(res["timeseries_raw_rms"])
    plt.plot(trms)
    raw_input('...')
    trms *= trms
    avg = np.mean(trms)
    spread = np.std(trms)
    print datadir
#    print "RMS power in timeseries_rms (= the data in the block with the pulse, from 0:pulse.start)"
#    print "mean = %f" % avg
#    print "sdev = %f" % spread
    print "relative error = %f" % (spread / avg)
    outliers = np.where(abs(trms - avg) > 3 * spread)
    outlierCount = len(outliers[0])
    if outlierCount > 0:
        print 'There are %d outliers above 3 sigma' % outlierCount
        ok_index = np.where(abs(trms - avg) < 3 * spread)
        corrected = trms[ok_index]
        newavg = np.mean(corrected)
        newspread = np.std(corrected)
        print '--> Corrected relative error = %f' % (newspread / newavg)
#    print ' '


# AC: determine power in calcbaseline_galactic, for 'clean channels' between 30 and 85 MHz.
# then divide it out, and divide by average spectrum abs.power
        
#        numin_i = calcbaseline_galactic.numin_i
#        numax_i = calcbaseline_galactic.numax_i
#        baselinepower = hArray(calcbaseline_galactic.baseline[..., numin_i:numax_i].sum())
#        avgspectrumpower = hArray(averagespectrum_good_antennas[..., numin_i:numax_i].sum())
#        avgspectrum = hArray(copy=averagespectrum_good_antennas)
#        avgspectrum[..., applybaseline.dirty_channels] = 0.0 # heeft bijna geen invloed op timeseries_rms
#        avgspectrumpower = hArray(avgspectrum[..., numin_i:numax_i].sum())

#        tomul = hArray(float, dimensions = averagespectrum_good_antennas, fill=0.0)
#        todiv = hArray(copy=tomul)
        
#        tomul[...] = avgspectrumpower[...]
#        todiv[...] = baselinepower[...]
#        fft_data.div(tomul)
#        fft_data.div(todiv)
#        fft_data *= blocksize * blocksize
