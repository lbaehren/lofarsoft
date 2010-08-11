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
#triggerMassageFile=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205-triggerdump.dat"
#-------
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/trigger-dumps-2010-07-07-cs003--23'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/trigger-dumps-2010-07-07-cs005--38'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/trigger-dumps-2010-07-07-cs006--35'
#-------
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/trigger-dumps-2010-07-07-cs003--25'
#filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/trigger-dumps-2010-07-07-cs005--40'
filename='/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/trigger-dumps-2010-07-07-cs006--37'
#-------
#triggerMassageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs003.dat'
#triggerMassageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat'
triggerMassageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs006.dat'
#stationname="RS205"
#stationname="CS003"
#stationname="CS005"
stationname="CS006"
antennaset="LBA_OUTER"
azRange=[0., 360., 1.]
elRange=[0., 90., 1.]
distsRange=[10., 50., 100., 500., 1000.]
fixedDist=200.
FarField=True

#------------------------------------------------------------------------
"""
  Open the file, set the parameters in the DataReader, and get the data
"""
dr = cr.open(filename,'LOFAR_TBB')

dr.blocksize = 512
dr.block = 128

ants = dr["nofantennas"]
blocksize = dr["blocksize"]
fftlength = dr["fftlength"]

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

(++) Coordinates
---------------

For doing actual beamforming we need to know the antenna
positions.

"""

dr.station = stationname
dr.antennaset = antennaset
antenna_positions = dr["antenna_position"]



"""

(++) Trigger message-matching
---------------

For doing actual beamforming we need to know the antenna
positions.

"""
ddate =dr["date"] + dr["sample_number"][0]/200e6
(mIDs, mdDates, mTdiffs, mscratch, mscratch) = match.matchTriggerfileToTime((ddate+0.00033024),triggerMassageFile)

match_positions = antenna_positions[mIDs]

match_positions = np.reshape(antenna_positions[mIDs],(len(mIDs)*3))

print 'Brute Force direction search:'
(bfaz, bfel, bftime) = sfind.directionBruteForceSearch(match_positions, mTdiffs)
print (bfaz, bfel, bftime)
print 'mse:', sfind.mse(bfaz, bfel, match_positions, mTdiffs)

print 'Linear Fit direction search:'
(lfaz, lfel) = sfind.directionForHorizontalArray(match_positions, mTdiffs)
print (lfaz, lfel)
print 'mse:', sfind.mse(lfaz, lfel, match_positions, mTdiffs)

"""

(++) Imaging
---------------

Now let's start setting up the image:
"""
n_az=int((azRange[1]-azRange[0])/azRange[2])
n_el=int((elRange[1]-elRange[0])/elRange[2])

n_pixels = n_az * n_el;
pixarray = np.zeros( (n_el,n_az) ) 
pixarray_time = np.zeros( ((250-100),n_el,n_az ) ) 

azel = np.zeros( (3) );
cartesian = np.zeros( (3) );
delays =  np.zeros( (ants) );
weights = np.empty( (ants, fftlength), dtype=complex )
shifted_fft = np.empty( (ants, fftlength), dtype=complex )
beamformed_fft = np.empty( (fftlength), dtype=complex )
beamformed_efield =  np.zeros( (blocksize) )
beamformed_efield_smoothed= np.zeros( (blocksize) )

el_values = np.arange((elRange[1]-elRange[2]), (elRange[0]-elRange[2]), -elRange[2])
az_values = np.arange(azRange[0], azRange[1], azRange[2])

ant_indices =  range(1,96,2)

for el_ind in range(n_el):
    print "processing elevation",el_ind,"out of",n_el
    azel[1]=el_values[el_ind]
    elrad = el_values[el_ind]/rad2deg
    for az_ind in range(n_az):
        azrad = az_values[az_ind]/rad2deg
        pixarray[el_ind,az_ind] = -sfind.mse(azrad, elrad, match_positions, mTdiffs) 

plt.imshow(pixarray,cmap=plt.cm.hot,extent=((azRange[0]-azRange[2]/2),(azRange[1]-azRange[2]/2),(elRange[0]-elRange[2]/2),(elRange[1]-elRange[2]/2)))
plt.xlabel("Azimuth [deg]")
plt.ylabel("Elevation [deg]")
plt.show()
