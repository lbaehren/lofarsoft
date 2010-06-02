#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits

# (start) configuration
# Data information
# Lightning data can be obtained from (it is 3 GB per file)
# make sure you have a LOFAR_DATA_DIR variable of the directory you want to store the data
# mkdir $LOFAR_DATA_DIR
# scp <username>@helios /mnt/lofar/lightning/lightning* $LOFAR_DATA_DIR/lightning/
# A peak can be found at:   File 16_48 , block 200260 , position Az=100, El=0-20
#                           File 17_23 , block 225950 , position Az=44, El=37
#                           File 17_46 , didn't test yet


# Get environment variables
LOFARSOFT = os.environ["LOFARSOFT"].rstrip('/')+'/'
LOFAR_DATA_DIR = os.environ['LOFAR_DATA_DIR'].rstrip('/')+'/'
data_directory = LOFAR_DATA_DIR+"/lightning/" 
stationname="CS302"
#filenames=["lightning_16_48.h5"]
filenames=["lightning_17_23.h5"]
nrfiles=len(filenames)
#channelIDs=[142000002,142000003,142002022,142005042,142008066,142011094]
antennaset="LBA_INNER"
antennafilename=LOFARSOFT+"data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
blocksize=2**10
nblocks=1 # no loop yet for more blocks
#startblock=200260 #for 16_48
startblock=225950 #for 17_23 (+1)
#startblock=1000

# (end) configuration


# (start) get METAdata

# Get antenna positions
# Task: Read antenna positions from a config file for the specified station and antennaset
# Returns a 96*3 hArray antenna_positions
# To select them for each board use: (antenna_positions[TBBboards[TBBboard]*16:TBBboards[TBBboard]*16+nofSelectedAntennas-1]
# where the current TBBboard and nofSelectedAntennas (=file["nofSelectedAntennas"]) still has to be specified
# In the future the antenna positions should be selected from the current datafile

antfile = open(antennafilename,'r') 
antfile.seek(0)
str = ''
while antennaset not in str:
    str = antfile.readline()

str = antfile.readline()
str = antfile.readline()
nrantennas = str.split()[0]
nrdir = str.split()[4]
ant = []
for i in range(0,int(nrantennas)):
    str=antfile.readline()
    ant.extend([float(str.split()[0]),float(str.split()[1]),float(str.split()[2])])  #X polarization
    ant.extend([float(str.split()[3]),float(str.split()[4]),float(str.split()[5])])  #Y polarization


antenna_positions=hArray(ant,[2*int(nrantennas),int(nrdir)])

# Get gain calibration coefficients

# These where obtained by correlation on the source for a dataset from an earlier time
# De gegeven getallen zijn p0 en p1, waarbij fase =  p0+p1*freq. Om p1 om te zetten in delays moet je het nog delen door (2*pi)
# Gebruikswijze in oude code
# double phase=p0[rcu_ids[i](antenna)/2]+frequencies(ifreq)*p1[rcu_ids[i](antenna)/2];
# matCal(ifreq,antenna)=DComplex(cos(-calibrationfactor*phase),sin(-calibrationfactor*phase));
# dr[i]->setFFT2calFFT(matCal);



# (end) get METAdata


# (start) Imaging

# Initialisation
# read in the files

file=crfile(data_directory+filenames[0])


totAntennas=file["nofSelectedAntennas"]
AntennasInFile=file["nofSelectedAntennas"]

file["blocksize"]=blocksize
shifts=file["SAMPLE_NUMBER"]
shifts-=shifts[-1]
shifts.negate()
#multiple number uses SAMPLE_NUMBER and TIME
shifts2=[]
for i in range(6):
    shifts2.append(shifts[i])

file["ShiftVector"]=shifts2

rcuIDs=file["AntennaIDs"]
for i in range(len(rcuIDs)):
    rcuIDs[i]=rcuIDs[i]%1000


used_antenna_pos=hArray(float,[totAntennas,3])
for i in range(totAntennas):
    used_antenna_pos[i]=antenna_positions[rcuIDs[i]]


# .... image part

NyquistZone=1
FarField=True
azrange=(0.,360.,8.)
elrange=(0.,90.,2.)
n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixels=n_az*n_el

file_time=file["Time"].setUnit("\\mu","s")
file_frequencies=file["Frequency"]
file_efield=file["Fx"].setPar("xvalues",file_time)
file_fft=file["emptyFFT"].setPar("xvalues",file_frequencies)


delays=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"]])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,file["fftLength"]],name="Beamformed FFT")
power=hArray(float,dimensions=beamformed_fft,name="Spectral Power",xvalues=file_frequencies,fill=0.0)

#phase_center=hArray(antenna_positions[0].vec())
#antenna_positions -= phase_center

azel=hArray(float,dimensions=[n_pixels,3])
cartesian=azel.new()

print "t=",time.clock(),"s -","Creating pximap GRID"
n=0
for el in np.arange (*elrange):
    for az in np.arange(*azrange):
        azel[n,0]=az
        azel[n,1]=el
        azel[n,2]= 1
        n+=1


print "t=",time.clock(),"s -","Calculating coordinates and delays"
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
hGeometricDelays(delays,used_antenna_pos,cartesian,FarField)

phases.delaytophase(file_frequencies,delays)
weights.phasetocomplex(phases)

# At the moment we don't know how much data there is for each file.


t0=time.clock()

#file["block"]=startblock
for block in range(startblock,startblock+nblocks):
    file["block"]=block
    file_efield.read(file,"Fx").none()
    #file_efield[...].plot()
    file_fft[...].fftcasa(file_efield[...],NyquistZone)
    #file_fft[...].plot()
    #copies the fft to all positions in shifted_fft
    shifted_fft.copy(file_fft)
    shifted_fft[...].mul(weights[...])
    for n in range(n_pixels):
        beamformed_fft[n] = shifted_fft[n,0]
        shifted_fft[n,1:,...].addto(beamformed_fft[n])
    power.spectralpower(beamformed_fft)
    print "t=",time.clock()-t0,"s -","Processed block",block


print "t=",time.clock(),"s -","Binning and Normalizing"
intpower=np.array(power[...].sum())
#maxval=intpower.max()
#intpower /= maxval
intpower.resize([n_az,n_el])

print "t=",time.clock(),"s -","Plotting"
plt.clf()
#plt.subplot(1,2,1)
plt.imshow(intpower,cmap=plt.cm.hot,extent=(azrange[0],azrange[1],elrange[1],elrange[0]))

#import pyfits
#hdu = pyfits.PrimaryHDU(intpower)
#hdulist = pyfits.HDUList([hdu])
#hdulist.writeto(data_directory+"/image15.fits")


# RFI mitigation and baseline calculation

# Beamforming section



#for block in range(200000,200300):
#    file["block"]=block
#    file_efield.read(file,"Fx").none()
#    print block," ",file_efield[0].max()
