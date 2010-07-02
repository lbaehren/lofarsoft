#! /usr/bin/env python

# import used libraries
import bfdata
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import time

# name of file we want to analyse
data_directory='/Users/STV/Astro/data/pulsars/128chTest/'
save_spectrum_directory='/Users/STV/Astro/data/seti/'
filename='SB0.stokes'
spectrum_filename_prefix='moon_spectrum_'

# open the file
file=open(data_directory+filename,'rb')



# specify the parameters of the data format
# these come from the observation settings / parset file
nrstations=1
nrsamples=768
nrchannels=32
nrpol=2

#Select station that is analyzed
station=0

#select channel that is analyzed
channel=5

#select polarization
pol=0

# Assuming full timeresolution data, calculate how much blocks are needed for a 1 Hz resolution
nrblocksperfft=8

# Specify which subband the file is from
subbandnr=321


# For how many timestamps should the data be analyzed
time_axis_length=3

# Determine spectrum filename

# Initialize used array in the right dimensions and datatype
pre_fft_data=np.array(np.zeros(nrsamples*nrblocksperfft),dtype=complex)
spectrum=np.array(np.zeros((time_axis_length,nrsamples*nrblocksperfft)),dtype=float)

# check if all the parameters are specified well
(seqnr1,data)=bfdata.get_block_rawvoltagedata_old(file,nrstations,nrchannels,nrsamples,nrpol)
(seqnr2,data)=bfdata.get_block_rawvoltagedata_old(file,nrstations,nrchannels,nrsamples,nrpol)
print "Checking if data parameters are specified correctly..."
assert seqnr1+1==seqnr2
# run over all channels
for channel in range(nrchannels):
    file.seek(0)
    spectrum_filename=spectrum_filename_prefix+'station_'+str(station)+'_SB_'+str(subbandnr)+'_channel_'+str(channel)+'_pol_'+str(pol)+'.fits'
    t0=time.time()
# Make an fft of each datablock
    for timestep in range(time_axis_length):
        for counter in range(nrblocksperfft):
            # read in a block of data
            (seqnr,data)=bfdata.get_block_rawvoltagedata_old(file,nrstations,nrchannels,nrsamples,nrpol)
            print "Analyzing block",seqnr," estimated time left: ",(time.time()-t0)/(timestep+1)*time_axis_length
            # The data had 5 dimensions: stations, channels, samples, polarisation, (real,imaginary)
            # Add the real part to the complex array
            pre_fft_data[counter*nrsamples:(counter+1)*nrsamples].real.fill(data[station,channel,0:nrsamples,pol,0])
            # Add the imaginary part to the complex array
            pre_fft_data[counter*nrsamples:(counter+1)*nrsamples].imag.fill(data[station,channel,0:nrsamples,pol,1])
        
        # Take an fft
        fft_data=np.fft.fft(pre_fft_data)
        # Add this to the spectrum
        spectrum[timestep]=abs(fft_data)

# To show the spectrum uncomment the next options
#plt.clf()
#plt.imshow(spectrum,cmap=plt.cm.hot,aspect='auto')

#Save as a fits file

# Make pyfits objects
    hdu = pyfits.PrimaryHDU(spectrum)
    hdulist = pyfits.HDUList([hdu])
    prihdr = hdulist[0].header

# Calculate the variables to put in the header record
    startfreq=2e8/1024*subbandnr
    freqdelta=2e8/1024/nrchannels/nrblocksperfft/nrsamples
    timedelta=5e-9*1024*nrchannels*nrblocksperfft*nrsamples

# Write the header information 
    prihdr.update('CTYPE1','FREQ')  #This means "unkown longitude" with CARtesian projection, the 
#                                   #casaviewer asumes this the be J2000 because it probably can't do AZEL
    prihdr.update('CRVAL1',startfreq)  #value of the axis at the reference point "CRPIX"
    prihdr.update('CDELT1',freqdelta)  #increment from the reference pixel to the next pixel
    prihdr.update('CROTA1',0.)          #rotation, just leave it at 0.
    prihdr.update('CRPIX1',0.5)          #pixel position at which the axis has the "CRVAL" value
    prihdr.update('CUNIT1','Hz')       #the unit in which "CRVAL" and "CDELT" are given

#prihdr.update('CTYPE2','??LT-STG')  #This means "unkown latitude" with CARtesian projection, see above
#prihdr.update('CRVAL2',90.) 
#prihdr.update('CDELT2',2.566666603088)
#prihdr.update('CROTA2',0.)
#prihdr.update('CRPIX2',45.5)
#prihdr.update('CUNIT2','deg')

    prihdr.update('CTYPE2','TIME')       #This means "linear time coordinate"
    prihdr.update('CRVAL2',0.)
    prihdr.update('CDELT2',timedelta)
    prihdr.update('CROTA2',0.)
    prihdr.update('CRPIX2',0.5)
    prihdr.update('CUNIT2','s')

# Write the data to the fits file
    hdulist.writeto(save_spectrum_directory+spectrum_filename)
