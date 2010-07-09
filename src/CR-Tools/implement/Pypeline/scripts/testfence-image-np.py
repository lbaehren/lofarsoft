import pycrtools #import this first, so that pycr overwrites the DatxaReader
import pycr as cr
import ttl
import numpy as np
#import scipy as sp
import matplotlib.pyplot as plt
#import pyfits
import os
import metadata

def p_(var):
    if (type(var)==list): map(lambda x:p_(x),var)
    else: print " ",var,"=>",eval(var)

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
filename=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5"
stationname="RS205"
antennaset="LBA_OUTER"
azRange=[180., 360., 1.]
elRange=[0., 60., 1.]
distsRange=[10., 50., 100., 500., 1000.]
fixedDist=200.
FarField=True

#------------------------------------------------------------------------
"""
  Open the file, set the parameters in the DataReader, and get the data
"""
dr = cr.crFileOpen(filename)

dr.crFileSetParameter("Blocksize", 512)
dr.crFileSetParameter("Block", 128)

ants = dr.crFileGetParameter("nofantennas")
blocksize = dr.crFileGetParameter("Blocksize")
fftlength = dr.crFileGetParameter("fftlength")

cr_time = np.empty( (blocksize) )
dr.read("Time",cr_time)

cr_freqs = np.empty( (fftlength) )
dr.read("Frequency",cr_freqs)

cr_efield = np.empty( (ants, blocksize) )
dr.read("Fx",cr_efield)

"""

As a next step we create an empty vector to hold the Fourierspectrum
of the data and the the fourier transforms

"""
cr_fft = np.empty( (ants, fftlength), dtype=complex )
for ant in range(ants):
    status =  ttl.forwardFFTW(cr_fft[ant], cr_efield[ant])

"""

(++) Coordinates
---------------

For doing actual beamforming we need to know the antenna
positions. For this we will use the method getCalData:

"""

antenna_positions = metadata.getRelativeAntennaPositions(stationname, antennaset);


"""

(++) Imaging
---------------

Now let's start setting up the image:
"""
if FarField != True:
    print "Near-field imaging not supported yet! Switching to fixed distance."
    #FarField = True
else:
    fixedDist = 1.

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

azel[2]= fixedDist
for el_ind in range(n_el):
    print "processing elevation",el_ind,"out of",n_el
    azel[1]=el_values[el_ind]
    for az_ind in range(n_az):
        azel[0]=az_values[az_ind]
        
        ttl.azElRadius2Cartesian(cartesian, azel, True)
        ttl.geometricDelays(delays, antenna_positions, cartesian, FarField)
        ttl.complexWeights(weights, delays, cr_freqs)
        
        shifted_fft = cr_fft*weights
        beamformed_fft = shifted_fft[ant_indices[0]]
        for n in ant_indices[1:]:
            beamformed_fft += shifted_fft[n]

        ttl.backwardFFTW(beamformed_efield, beamformed_fft); 
        pixarray[el_ind,az_ind] = np.sum(np.abs(beamformed_efield))

"""
        beamformed_efield.abs()
        pixarray[el_ind,az_ind] = beamformed_efield.max()[0]/cr["blocksize"]
        hRunningAverage(beamformed_efield_smoothed, beamformed_efield, 5, hWEIGHTS.GAUSSIAN)
        for tindex in range( (250-100) ):
            pixarray_time[tindex,(n_el-el_ind-1),az_ind] = beamformed_efield_smoothed[100:250].vec()[tindex]/cr["blocksize"]
"""
#pixarray[0,0]=np.max(pixarray)
plt.imshow(pixarray,cmap=plt.cm.hot,extent=((azRange[0]-azRange[2]/2),(azRange[1]-azRange[2]/2),(elRange[0]-elRange[2]/2),(elRange[1]-elRange[2]/2)))
plt.xlabel("Azimuth [deg]")
plt.ylabel("Elevation [deg]")


"""
hdu = pyfits.PrimaryHDU(pixarray_time)
hdulist = pyfits.HDUList([hdu])
prihdr = hdulist[0].header
prihdr.update('CTYPE1','??LN-CAR')  #This means "unkown longitude" with CARtesian projection, the 
#                                   #casaviewer asumes this the be J2000 because it probably can't do AZEL
prihdr.update('CRVAL1',azRange[0])  #value of the axis at the reference point "CRPIX"
prihdr.update('CDELT1',azRange[2])  #increment from the reference pixel to the next pixel
prihdr.update('CROTA1',0.)          #rotation, just leave it at 0.
prihdr.update('CRPIX1',1.)          #pixel position at which the axis has the "CRVAL" value
prihdr.update('CUNIT1','deg')       #the unit in which "CRVAL" and "CDELT" are given

prihdr.update('CTYPE2','??LT-CAR')  #This means "unkown latitude" with CARtesian projection, see above
prihdr.update('CRVAL2',elRange[0]) 
prihdr.update('CDELT2',elRange[2])
prihdr.update('CROTA2',0.)
prihdr.update('CRPIX2',1.)
prihdr.update('CUNIT2','deg')

prihdr.update('CTYPE3','TIME')       #This means "linear time coordinate"
prihdr.update('CRVAL3',0.)
prihdr.update('CDELT3',5e-9)
prihdr.update('CROTA3',0.)
prihdr.update('CRPIX3',1.)
prihdr.update('CUNIT3','s')

hdulist.writeto("testfence-image.fits")

"""
