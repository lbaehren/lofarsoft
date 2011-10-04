#! /usr/bin/env python
#
# A test script to plot wavefront curvature using the pycrtools.plotfootprint task.
# Arthur Corstanje, 2011 

from pycrtools import *
import numpy as np
from pycrtools import srcfind

deg2rad = 2*np.pi / 360.0

event = '/Users/acorstanje/triggering/CR/results_john/VHECR_LORA-20110714T174749.986Z'

fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event) # no parameters set...

times = fptask.arrivaltime.toNumpy()
times *= 1.0e-9 # in seconds
times -= times.mean()

positions = fptask.positions.toNumpy()
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
        residu[i] = 0.0 # can be done in a oneliner or with masked arrays or with hArrays... (lazy)

residu *= 1.0e9
residues = hArray(residu)
fptask = trerun("plotfootprint", "1", colormap = 'jet', filefilter = event, arrivaltime=residues) # no parameters set...
plt.figure()
# Make lateral distribution of curvature delays. Will not be correct because of non-symmetry around the axis (inclination!)
# ! check if the ant-id indices match with those from plotfootprint!
ldftask = trerun("ldf", "0", topdir = '/Users/acorstanje/triggering/CR/results_john', events='VHECR_LORA-20110714T174749.986Z', signals0 = residues, Draw3D = False, logplot=False)
plt.axis(xmin=0, xmax=300, ymin=-30, ymax=30)
