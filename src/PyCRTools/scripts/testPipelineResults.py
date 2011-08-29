#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np

datadir = '/Users/acorstanje/triggering/CR/results4/lora-event-0-station/pol0/2'
res = {}
execfile(os.path.join(datadir,"results.py"),res)
res = res["results"]

trms = res["timeseries_rms"]
avg = np.mean(trms)
spread = np.std(trms)

print "RMS power in timeseries_rms (= the data in the block with the pulse, from 0:pulse.start)"
print "mean = %f" % avg
print "sdev = %f" % spread
print "relative error = %f" % (spread / avg)



