#! /usr/bin/env python
#
# A test script to plot wavefront curvature using the pycrtools.plotfootprint task.
# Arthur Corstanje, 2011 

from pycrtools import *
import numpy as np
from pycrtools import srcfind

deg2rad = 2*np.pi / 360.0

event1 = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110716T094509.665Z/'
event2 = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110714T174749.986Z/'
polarization = 0

# run task to get arrival times for all antennas in the event
#fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, pol=polari) # no parameters set...
fptask1 = trerun("plotfootprint", "1", colormap = 'jet', filefilter = event1, pol=polarization) # no parameters set...
fptask2 = trerun("plotfootprint", "2", colormap = 'jet', filefilter = event2, pol=polarization) # no parameters set...

times1 = fptask1.arrivaltime.toNumpy()
times2 = fptask2.arrivaltime.toNumpy()
times1 *= 1.0e-9 # in seconds
times2 *= 1.0e-9

times = 0.5 * (times2 - times1) # !! Magic...

times -= times.mean()

positions = fptask1.positions.toNumpy()
positions = positions.flatten()

# get the best-fitting plane wave through all the measured time delays
# which are already corrected for cable delays if event processed with Cabledelays.pic database file present
bestfitDirection = srcfind.directionForHorizontalArray(positions, times) 

# get the calculated delays according to this plane wave
bestfitDelays = srcfind.timeDelaysFromDirection(positions, bestfitDirection)

# is it necessary / correct to subtract mean times and mean calculated times?
#times -= times.mean()
bestfitDelays -= bestfitDelays.mean()

residu = times - bestfitDelays

# remove outside 3-sigma residues
sigma = residu.std()
for i in range(len(residu)):
    if abs(residu[i]) > 3 * sigma: # mean = 0, subtracted.
        print 'removing outlier %d, value %f' % (i, residu[i])
        residu[i] = 0.0 # can be done in a oneliner or with masked arrays or with hArrays... (lazy)

residu *= 1.0e9

residu[np.where(abs(residu) > 10.0)] = 0.0 # hack

residues = hArray(residu)
fptask = trerun("plotfootprint", "3", colormap = 'jet', filefilter = event1, arrivaltime=residues, pol=polarization) 
#fptask = trerun("plotfootprint", "2", colormap = 'jet', filefilter = event, arrivaltime=residues, pol=1)

#plt.figure()
# Make lateral distribution of curvature delays. 
# ! check if the ant-id indices match with those from plotfootprint!
#ldftask = trerun("ldf", "0", topdir = '/Users/acorstanje/triggering/CR/results_withcaltables', events='VHECR_LORA-20110716T094509.665Z', signals0 = residues, Draw3D = False, logplot=False)
#plt.axis(xmin=0, xmax=300, ymin=-30, ymax=30)



