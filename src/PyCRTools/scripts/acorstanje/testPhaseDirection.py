from pycrtools import *
from pycrtools import srcfind as sf
import numpy as np

twopi = 2 * np.pi
deg2rad = twopi / 360.0
# take a test direction and frequency, add some phase noise, make image of phase error from all directions.
az = 97.0 * deg2rad
el = 54.0 * deg2rad
freq = 30.0e6

print 'Frequency is %f MHz' % (freq / 1.0e6)
print 'Simulated incoming direction is az = %f, el = %f' % (az, el)

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
noiseAmplitude = 1.0 # radians
noise = np.random.random(len(phases)) * 2 * noiseAmplitude - noiseAmplitude
phases += noise # phase noise uniform in [ - noiseAmplitude, noiseAmplitude]

# do direction fit by brute force search
# showImage = True makes a crude image of the phase-error as function of (az, el)

(fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(pos, phases, freq, showImage=True, verbose=True)

print 'Best fit: az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
print 'Phase error: %f' % minPhaseError

