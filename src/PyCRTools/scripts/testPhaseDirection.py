from pycrtools import *
from pycrtools import srcfind as sf
import numpy as np

twopi = 2 * np.pi
deg2rad = twopi / 360.0
az = 97.0 * deg2rad
el = 54.0 * deg2rad
freq = 30.0e6

print 'Frequency is %f MHz' % (freq / 1.0e6)
print 'Simulated position is az = %f, el = %f' % (az, el)

pos = metadata.getAntennaPositions("CS002", "LBA_OUTER")
pos = pos.ravel()
times = sf.timeDelaysFromDirection(pos, (az, el))

phases = (twopi * freq) * times

# test phase-error function versus MSE of times
# at a small offset in az/el. Values should match closely for small offsets.
az = az + 1.0*deg2rad
el = el
phaseError = sf.phaseError(az, el, pos, phases, freq)
mse = sf.mse(az, el, pos, times)

print 'Phase error at small offset = %f' % phaseError
print 'MSE based on arrival times  = %f' % mse

# add phase noise and see how the direction fitting deals with this.
noiseAmplitude = 1.5 # radians
noise = np.random.random(len(phases)) * 2 * noiseAmplitude - noiseAmplitude
phases += noise # phase noise uniform in [ - noiseAmplitude, noiseAmplitude]

# do direction fit by brute force search
(fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(pos, phases, freq, showImage=True, verbose=True)

print 'Best fit: az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
print 'Phase error: %f' % minPhaseError

# make a crude image of the phase-error as function of (az, el)
#azsteps = 360.0 / 1
#elsteps = 90.0 / 1

#elevations = np.arange(elsteps) * (90.0 / elsteps)
#azimuths = np.arange(azsteps) * (360.0 / azsteps)

#imarray = np.zeros((elsteps, azsteps))
#elstep = 0
#minimum = 1.0e9
#for el in elevations:
#    print elstep
#    azstep = 0
#    for az in azimuths:
#        badness = sf.phaseError(az*deg2rad, el*deg2rad, pos, phases, freq)
#        if badness < minimum:
#            minimum = badness
#            minpos = (az, el)
#        imarray[elsteps - 1 - elstep, azstep] = badness # hack to get elevations shown bottom to top as 0.0 - 90.0
#        azstep += 1
#    elstep += 1

#print 'Best fit: (az, el) = ', minpos
#print 'Phase error = ', minimum
#plt.imshow(imarray, cmap=plt.cm.hot,extent=(0, 360.0, 0.0, 90.0))
#plt.colorbar()