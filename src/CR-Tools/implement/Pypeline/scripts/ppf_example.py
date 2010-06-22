#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import matplotlib.pyplot as plt

antnr=0
file=crfile(filename_lofar)
file["blocksize"]=1024

# Initialize arrays and values
weights=hArray(float,[16,1024])
weights.readdump(LOFARSOFT+'/src/CR-tools/data/ppfWeights16384.dat')

if weights[0,0].val==0.0
    print "Reading in new data"
    # Reading of weights failed
    f=open('/home/veen/usg/src/CR-Tools/data/Coeffs16384Kaiser-quant.dat')
    weights.setDim([16*1024])
    f.seek(0)
    for i in range(0,16*1024):
        weights[i]=float(f.readline())
    weights.setDim([16,1024])
    weights.writedump(LOFARSOFT+'/src/CR-tools/data/ppfWeights16384.dat')

    
buffer=hArray(float,[16,1024])
input=hArray(float,1024)
avg_spectrum=hArray(float,513)
avg_spectrum_ppf=hArray(float,513)
fftdata=hArray(complex,513)
startrow=15

# 
for block in range(0,516):
    # Set the block to be read
    file["block"]=block
    
    # Read in a block of data
    input.copy(file["Fx"][antnr])
    #input.read(file,"Fx")

    # Copy the data to the buffer
    buffer[startrow].copy(input)
    
    # For comparison make an fft of the data without a filter
    fftdata.fftw(input)
    
    # Add this to the average spectrum. The PPF needs 15 blocks to initiate,
    # so we skip the first 15 blocks
    if block > 15:
        avg_spectrum.spectralpower(fftdata)
    
    # Empty the input array as it's also out output.
    input.fill(0)
    
    # Multiply the weights times the buffer. This is a 16x1024 operation in time order
    for row in range(0,16):
        #print "rows: ",row," ",(row+startrow)%16
        input.muladd(weights[row],buffer[(row+startrow)%16])
    
    # to maintain the time order, our buffer row decreases
    startrow=startrow-1
    #print "startrow=",startrow
    
    # If the startrow becomes negative start at the end of the buffer again.
    startrow=startrow%16
        
    # Take an fft of the filtered data
    fftdata.fftw(input)
    
    # Normalize the data
    #fftdata/=16
    
    # Add the data to the average spectrum
    if block > 15:
        avg_spectrum_ppf.spectralpower(fftdata)

# Take the log of both spectra
avg_spectrum.log()
avg_spectrum_ppf.log()

# Convert the spectra to numpy arrays to plot them
avg_spec=np.array(avg_spectrum.vec())
avg_spec_ppf=np.array(avg_spectrum_ppf.vec())

# Calculate the frequency axis for plotting
freq=np.array(file["frequencyValues"].vec())
freq/=1e6

# Normalize the two spectra
avg_spec+=(avg_spec_ppf[3]-avg_spec[3])

# Plot the data
plt.clf()
plt.plot(freq,avg_spec,freq,avg_spec_ppf,label="Average spectrum over 500 blocks, no filter")

