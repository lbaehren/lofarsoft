#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np
import pycrtools as cr

eventdir = '/Users/acorstanje/triggering/CR/results4/lora-event-0-station'

datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

for datadir in datadirs:
    if not os.path.isfile(os.path.join(datadir,"results.py")):
        continue
    resfile=open(os.path.join(datadir,"results.py"))
    
    res = {}
    execfile(os.path.join(datadir,"results.py"),res)
    res = res["results"]

    trms = np.array(res["timeseries_rms"])
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


