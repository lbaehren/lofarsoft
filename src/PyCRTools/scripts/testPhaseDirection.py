from pycrtools import *
from pycrtools import srcfind as sf
import numpy as np

twopi = 2 * np.pi
deg2rad = twopi / 360.0
az = 134.0 * deg2rad
el = 41.0 * deg2rad
freq = 15.0e6

pos = metadata.getAntennaPositions("CS002", "LBA_OUTER")
pos = pos.ravel()
times = sf.timeDelaysFromDirection(pos, (az, el))

phases = (twopi * freq) * times

noise = np.random.random(len(phases)) * 2.0 - 1.0
phases += noise # phase noise uniform in [-0.3, 0.3]

az = az + 1.0*deg2rad
el = el
phaseError = sf.phaseError(az, el, pos, phases, freq)
mse = sf.mse(az, el, pos, times)

print phaseError
print mse

# make a crude image of the phase-error as function of (az, el)
azsteps = 360.0 / 1
elsteps = 90.0 / 1

elevations = np.arange(elsteps) * (90.0 / elsteps)
azimuths = np.arange(azsteps) * (360.0 / azsteps)

imarray = np.zeros((elsteps, azsteps))
elstep = 0
minimum = 1.0e9
for el in elevations:
    print elstep
    azstep = 0
    for az in azimuths:
        badness = sf.phaseError(az*deg2rad, el*deg2rad, pos, phases, freq)
        if badness < minimum:
            minimum = badness
            minpos = (az, el)
        imarray[elsteps - 1 - elstep, azstep] = badness # hack to get elevations shown bottom to top as 0.0 - 90.0
        azstep += 1
    elstep += 1

print 'Best fit: (az, el) = ', minpos
print 'Phase error = ', minimum
plt.imshow(imarray, cmap=plt.cm.hot,extent=(0, 360.0, 0.0, 90.0))
plt.colorbar()