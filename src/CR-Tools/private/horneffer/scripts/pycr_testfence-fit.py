#! /usr/bin/env python

import os
import pycr as cr
import numpy as np
import matplotlib.pyplot as plt
import matching as match
import srcfind as sfind

rad2deg = 180./np.pi
#------------------------------------------------------------------------
"""
(++) Parameters
---------------
Set some parameters "by hand", such as:
 - filename
 - station name
 - azRange
 - elRange
 - distsRange (not supported yet)
 - fixedDist
 - FarField

"""
LOFARSOFT=os.environ["LOFARSOFT"]
#filename=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5"
#-------
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/trigger-dumps-2010-07-07-cs003--23'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/trigger-dumps-2010-07-07-cs005--38'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/trigger-dumps-2010-07-07-cs006--35'
#-------
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/trigger-dumps-2010-07-07-cs003--25'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/trigger-dumps-2010-07-07-cs005--40'
filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/trigger-dumps-2010-07-07-cs006--37'
#-------
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/trigger-dumps-2010-07-07-cs006--69'
#-------
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs003.dat'
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat'
triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs006.dat'
#-------
#stationname="RS205"
#stationname="CS003"
#stationname="CS005"
stationname="CS006"
antennaset="LBA_OUTER"
phase_center_in=[0., 0., 0.]
def_pointing=[172.+90., 63.,  10000.]
def_pointing=[276.43960316,    62.61127495,  6767.67527943]
FarField=False
blocksize = 512
#blocksize = 132096
AntennaSelection='odd'
#------------------------------------------------------------------------
"""
  Open the file, set the parameters in the DataReader, and get the data
"""
dr = cr.open(filename,'LOFAR_TBB')

"""

(++) Coordinates
---------------

For doing actual beamforming we need to know the antenna
positions.

"""

dr.blocksize = blocksize
dr.station = stationname
dr.antennaset = antennaset
antenna_positions = dr["antenna_position"]
samplefreq = dr["samplefrequency"]
ants = dr["nofantennas"]
blocksize = dr["blocksize"]
fftlength = dr["fftlength"]

"""

(++) Trigger message-matching
---------------

For doing actual beamforming we need to know the antenna
positions.

"""
ddate =dr["date"] + dr["sample_number"][0]/200e6
(mIDs, mdDates, mTdiffs, mTriggerDates, mSampleNums) = match.matchTriggerfileToTime((ddate+0.00033024),triggerMessageFile)

match_positions = np.reshape(antenna_positions[mIDs],(len(mIDs)*3))

print 'Brute Force direction search with', len(mIDs), 'antennas:'
(bfaz, bfel, bftime) = sfind.directionBruteForceSearch(match_positions, mTdiffs)
print (bfaz, bfel, bftime)
print 'mse:', sfind.mse(bfaz, bfel, match_positions, mTdiffs)

print 'Linear Fit direction search with', len(mIDs), 'antennas:'
(lfaz, lfel) = sfind.directionForHorizontalArray(match_positions, mTdiffs)
print (lfaz, lfel)
print 'mse:', sfind.mse(lfaz, lfel, match_positions, mTdiffs)

"""
  Open the file, set the parameters in the DataReader, and get the data
"""
toffset = np.mean((mTriggerDates-dr["date"])+(mSampleNums-dr["sample_number"][0])/200e6)
dr.block = int((toffset*samplefreq)/blocksize)
print "Set block-number to:", dr["block"]


cr_time = np.empty(blocksize)
dr.read("Time",cr_time)

cr_freqs = np.empty(fftlength)
dr.read("Frequency",cr_freqs)

cr_efield = np.empty((ants, blocksize))
dr.read("Fx",cr_efield)

"""

As a next step we create an empty vector to hold the Fourierspectrum
of the data and the the fourier transforms

"""
cr_fft = np.empty( (ants, fftlength), dtype=complex )
for ant in range(ants):
    status =  cr.forwardFFTW(cr_fft[ant], cr_efield[ant])


"""

(++) Beamforming
---------------

First set up the variables we are going to use

"""
azel = np.zeros( (3) );
cartesian = np.zeros( (3) );
delays =  np.zeros( (ants) );
weights = np.empty( (ants, fftlength), dtype=complex )
shifted_fft = np.empty( (ants, fftlength), dtype=complex )
shifted_efield = np.empty( (ants, blocksize))
beamformed_fft = np.empty( (fftlength), dtype=complex )
beamformed_efield =  np.zeros( (blocksize) )
beamformed_efield_smoothed= np.zeros( (blocksize) )

"""
Then do the shifting and backwards-fft
"""
azel = np.array(def_pointing)

cr.azElRadius2Cartesian(cartesian, azel, True)
cr.geometricDelays(delays, antenna_positions, cartesian, FarField)
cr.complexWeights(weights, delays, cr_freqs)

shifted_fft = cr_fft*weights

for ant in range(ants):
    status =  cr.backwardFFTW(shifted_efield[ant], shifted_fft[ant])

q1 = np.logical_and(antenna_positions[:,0]>=0., antenna_positions[:,1]>=0.)
q2 = np.logical_and(antenna_positions[:,0]<0., antenna_positions[:,1]>=0.)
q3 = np.logical_and(antenna_positions[:,0]<0., antenna_positions[:,1]<0.)
q4 = np.logical_and(antenna_positions[:,0]>=0., antenna_positions[:,1]<0.)

if (AntennaSelection == "even"):
    ant_indices = range(0,ants,2)
elif (AntennaSelection == "odd"):
    ant_indices = range(1,ants,2)
else:
    ant_indices = range(0,ants,1)

"""
Display
---------------
Now, that we have some beam-formed data, we can (should) display it.
"""

plt.clf()
for n in ant_indices:
    plt.plot(cr_time*1e6,cr_efield[n])
raw_input("Plotted raw timeseries - press Enter to continue...")

plt.clf()
for n in ant_indices:
    plt.plot(cr_time*1e6,shifted_efield[n])
raw_input("Plotted shifted timeseries - press Enter to continue...")

window = np.array([1.,1.,1.,1.,1.])
window /= np.sum(window)
plt.clf()
for n in ant_indices:
    plt.plot(cr_time*1e6,np.convolve(np.abs(shifted_efield[n]),window,'same'))
raw_input("Plotted shifted abs-timeseries - press Enter to continue...")


quadrant1_beam =  np.zeros( (blocksize) )
quadrant2_beam =  np.zeros( (blocksize) )
quadrant3_beam =  np.zeros( (blocksize) )
quadrant4_beam =  np.zeros( (blocksize) )
quad1ind =  np.array(ant_indices)[(q1[ant_indices])]
quad2ind =  np.array(ant_indices)[(q2[ant_indices])]
quad3ind =  np.array(ant_indices)[(q3[ant_indices])]
quad4ind =  np.array(ant_indices)[(q4[ant_indices])]
for n in quad1ind:
    quadrant1_beam += shifted_efield[n]
for n in quad2ind:
    quadrant2_beam += shifted_efield[n]
for n in quad3ind:
    quadrant3_beam += shifted_efield[n]
for n in quad4ind:
    quadrant4_beam += shifted_efield[n]

quadrant1_beam /= len(quad1ind)
quadrant2_beam /= len(quad2ind)
quadrant3_beam /= len(quad3ind)
quadrant4_beam /= len(quad4ind)

plt.clf()
plt.plot(cr_time*1e6,quadrant1_beam)
plt.plot(cr_time*1e6,quadrant2_beam)
plt.plot(cr_time*1e6,quadrant3_beam)
plt.plot(cr_time*1e6,quadrant4_beam)
raw_input("Plotted beamformed timeseries - press Enter to continue...")

plt.clf()
plt.plot(cr_time*1e6,np.convolve(np.abs(quadrant1_beam),window,'same')) 
plt.plot(cr_time*1e6,np.convolve(np.abs(quadrant2_beam),window,'same'))
plt.plot(cr_time*1e6,np.convolve(np.abs(quadrant3_beam),window,'same'))
plt.plot(cr_time*1e6,np.convolve(np.abs(quadrant4_beam),window,'same'))
raw_input("Plotted smoothed beamformed timeseries - press Enter to continue...")

"""

(++) Position fitting
---------------

Now we can define the "badness of fit" function and fit the position

"""

def beamform_function(azel_in):
    if ( azel_in[0] > 360. or azel_in[0] < 0. or azel_in[1] > 90. or azel_in[1] < 0.):
        erg = 0.
    else:
        azel[0] = azel_in[0]
        azel[1] = azel_in[1]
        if (FarField):
            azel[2] = 1.
        else:
            azel[2] = azel_in[2]
        cr.azElRadius2Cartesian(cartesian, azel, True)
        cr.geometricDelays(delays, antenna_positions, cartesian, FarField)
        cr.complexWeights(weights, delays, cr_freqs)        
        shifted_fft = cr_fft*weights        
        beamformed_fft = shifted_fft[ant_indices[0]]
        for n in ant_indices[1:]:
            beamformed_fft += shifted_fft[n]
        cr.backwardFFTW(beamformed_efield, beamformed_fft);         
        erg = - np.max(np.convolve(np.abs(beamformed_efield),window,'same'))
    print "beamform_function azel:", azel_in, " result:", erg
    return erg

from scipy.optimize import fmin

ant_indices = range(0,96,2)
start_pointing = [(bfaz*rad2deg), (bfel*rad2deg), 30000.]
if (FarField):
    evenopt = fmin(beamform_function, start_pointing[0:2], xtol=1e-2, ftol=1e-4)
else:
    evenopt = fmin(beamform_function, start_pointing, xtol=1e-2, ftol=1e-4)

ant_indices = range(1,96,2)
start_pointing = [(bfaz*rad2deg), (bfel*rad2deg), 30000.]
if (FarField):
    oddopt = fmin(beamform_function, start_pointing[0:2], xtol=1e-2, ftol=1e-4)
else:
    oddopt = fmin(beamform_function, start_pointing, xtol=1e-2, ftol=1e-4)


print 'Fitting results:'
print '    Even Antennas:', evenopt
print '     Odd Antennas:', oddopt











