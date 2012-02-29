#! /usr/bin/env python
#
# A test script to plot wavefront curvature using the pycrtools.plotfootprint task.
# Arthur Corstanje, 2011 

from pycrtools import *
import numpy as np
from scipy.optimize import fmin # the downhill-simplex search method
from pycrtools import srcfind

def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2] # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = srcfind.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse
    

deg2rad = 2*np.pi / 360.0

event = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110716T094509.665Z/'
#event = '/Users/acorstanje/triggering/CR/results_3jan2012/VHECR_LORA-20120103T003111.187Z/'

polarization = 0

# run task to get arrival times for all antennas in the event
#fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, pol=polari) # no parameters set...
fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, pol=polarization) # no parameters set...

times = fptask.arrivaltime.toNumpy()
plotsizes = hArray(dimensions = fptask.arrivaltime, fill=50.0)
fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, power = plotsizes, pol=polarization)  # want to see all dots the same size, not proportional to power in this case.

times *= 1.0e-9 # in seconds
times -= times.mean()

positions = fptask.positions.toNumpy()
positions2D = positions
positions = positions.flatten()

# get the best-fitting plane wave through all the measured time delays
# which are already corrected for cable delays if event processed with Cabledelays metadata present
bestfitDirection = srcfind.directionForHorizontalArray(positions, times) 

print 'Az, el = %f, %f' % (bestfitDirection[0] / deg2rad, bestfitDirection[1] / deg2rad)

# Use this as a starting point for a brute-force search
rangesize = deg2rad * 5.0 # -10..+10 degrees
step = 5.0 * deg2rad 
Rsteps = 20
Rstart = 500.0
az = bestfitDirection[0]
el = bestfitDirection[1]
azrange = np.arange(az - rangesize, az + rangesize, step)
elrange = np.arange(max(0.0, el - rangesize), min(el + rangesize, np.pi / 2), step)

startPosition = (az, el, 1.0) # 1.0 means R = 1000.0 ...
print 'Doing simplex search for minimum MSE...'
optimum = fmin(mseMinimizer, startPosition, (positions, times, 0, 3), xtol=1e-5, ftol=1e-5, full_output=1)
#raw_input('Done simplex search.')

simplexDirection = optimum[0]
simplexMSE = optimum[1]
(simplexAz, simplexEl, simplexR) = simplexDirection
simplexR = 1000.0 / simplexR    

print azrange
print elrange

minMSE = 1.0e10
for az in azrange:
    for el in elrange:
        R = Rstart
        for i in range(Rsteps):
            R *= 1.2
            thisMSE = srcfind.mseWithDistance(az, el, R, positions, times, allowOutlierCount = 3)
            if thisMSE < minMSE:
                minMSE = thisMSE
                bestDirection = (az, el, R)
                print 'New MSE: %f; best direction = (%f, %f, %f)' % (thisMSE, az / deg2rad, el / deg2rad, R)

print '-----'
print 'Brute force search: '
(bruteAz, bruteEl, bruteR) = bestDirection
print 'Best MSE: %f; best direction = (%f, %f, %f)' % (minMSE, bruteAz / deg2rad, bruteEl / deg2rad, bruteR)
msePlanar = srcfind.mseWithDistance(bruteAz, bruteEl, 1.0e6, positions, times, allowOutlierCount = 3)
print 'Best MSE for R = 10^6 (approx. planar): %f' % msePlanar
print '-----'
print 'Simplex search: '
print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
print '-----'
# get the calculated delays according to this plane wave
bestfitDelays = srcfind.timeDelaysFromDirectionAndDistance(positions, bestDirection)
bestfitDelaysPlanar = srcfind.timeDelaysFromDirection(positions, bestDirection)
# is it necessary / correct to subtract mean times and mean calculated times?
times -= times.mean()
bestfitDelays -= bestfitDelays.mean()
bestfitDelaysPlanar -= bestfitDelaysPlanar.mean()

residu = times - bestfitDelays
residu *= 1.0e9
residuPlanar = times - bestfitDelaysPlanar
residuPlanar *= 1.0e9
#print residu
residu[np.where(abs(residu) > 30.0)] = np.float('nan') # hack
residuPlanar[np.where(abs(residuPlanar) > 30.0)] = np.float('nan') # don't show outliers

#residues = hArray(residu)
#residuesPlanar = hArray
#residues = hArray(residuForAllAntennas)
#residues *= 1.0e9

plotsizes = hArray(dimensions=[len(residu)], fill=50.0)
# plot residu = data times - bestfit times
fptask = trerun("plotfootprint", "1", colormap = 'jet', filefilter = event, arrivaltime=hArray(residu), power = plotsizes, pol=polarization) 
# plot residuPlanar = data times - bestfit planewave times
fptask = trerun("plotfootprint", "2", colormap = 'jet', filefilter = event, arrivaltime=hArray(residuPlanar), power = plotsizes, pol=polarization) 
# plot residu - residuPlanar; show curvature from the best-fitted function
fptask = trerun("plotfootprint", "3", colormap = 'jet', filefilter = event, arrivaltime=hArray(1.0e9*bestfitDelays - 1.0e9*bestfitDelaysPlanar), power = plotsizes, pol=polarization)

#plt.figure()
# Make lateral distribution of curvature delays. 
# ! check if the ant-id indices match with those from plotfootprint!
#ldftask = trerun("ldf", "0", topdir = '/Users/acorstanje/triggering/CR/results_withcaltables', events='VHECR_LORA-20110716T094509.665Z', signals0 = residues, Draw3D = False, logplot=False)
#plt.axis(xmin=0, xmax=300, ymin=-30, ymax=30)


"""
# remove outside 3-sigma residues
sigma = residu.std()
#for i in range(len(residu)):
#    if abs(residu[i]) > 3 * sigma: # mean = 0, subtracted.
#        residu[i] = np.float('nan') # can be done in a oneliner or with masked arrays or with hArrays... (lazy)

residu *= 1.0e9
oldresidu = residu
# try new direction finding by putting NaNs in times array at these positions...
newPositions2D = positions2D[np.where(abs(residu) < 15.0)]
newPositions = newPositions2D.flatten()
newTimes = times[np.where(abs(residu) < 15.0)]

# doesnt work! times[np.where(abs(residu) > 20.0)] = np.float('nan')
bestfitDirection = srcfind.directionForHorizontalArray(newPositions, newTimes) 
bestfitDelays = srcfind.timeDelaysFromDirection(newPositions, bestfitDirection)
bestfitDelays -= bestfitDelays.mean()

residu = newTimes - bestfitDelays # new

residuForAllAntennas = []
newindex = 0
for i in range(len(oldresidu)):
    if abs(oldresidu[i]) < 15.0:
        thisValue = residu[newindex] 
        newindex += 1
    else:
        thisValue = float('nan')
    residuForAllAntennas.extend([thisValue]) # the new value
"""

