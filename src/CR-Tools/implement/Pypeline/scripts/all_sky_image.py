#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits


# (start) configuration
# Data information
# Data should show the Crab Nebula and a giant pulse in limited unknown bandwidth
# The position of the Crab Nebula should be (pulse3) Az,El 206.229,57.0392
#                                           (pulse1) Az,El 183.831, 59.0699
#					                        (pulse5) Az,El 224.908, 52.6475
# Data can be obtained from: 
# mkdir $LOFAR_DATA_DIR/Crab
# scp -r <username>@helios:/mnt/lofar/CS1_tbb/CrabFirstTrigger/pulse1 $LOFAR_DATA_DIR/Crab/
# scp -r <username>@helios:/mnt/lofar/CS1_tbb/CrabFirstTrigger/pulse3 $LOFAR_DATA_DIR/Crab/
# scp -r <username>@helios:/mnt/lofar/CS1_tbb/CrabFirstTrigger/pulse5 $LOFAR_DATA_DIR/Crab/

# Get environment variables

# Files will be saved in your data directory with the name:
# We may want to change this to an automatic name.
SummedFitsFilename="lightning_16_48_7.fits"
SeparatedFitsFilename="imageCrab_100blocks.fits"

# Get data files
LOFARSOFT = os.environ["LOFARSOFT"].rstrip('/')+'/'
LOFAR_DATA_DIR = os.environ['LOFAR_DATA_DIR'].rstrip('/')+'/'
data_directory1 = LOFAR_DATA_DIR+"Crab/pulse1/"
data_directory3 = LOFAR_DATA_DIR+"Crab/pulse3/"
data_directory5 = LOFAR_DATA_DIR+"Crab/pulse5/"
data_directory_lightning = "/mnt/lofar/CS1_tbb/Fri26-06-2009/"
filenames1=["CS302C-B0T5:00:39.h5","CS302C-B1T5:00:39.h5","CS302C-B2T5:00:39.h5","CS302C-B3T5:00:39.h5","CS302C-B4T5:00:39.h5","CS302C-B5T5:00:39.h5"]
filenames3=["CS302C-B0T6:01:48.h5","CS302C-B1T6:01:48.h5","CS302C-B2T6:01:48.h5","CS302C-B3T6:01:48.h5","CS302C-B4T6:01:48.h5","CS302C-B5T6:01:48.h5"]
filenames5=['CS302C-B0T6:50:02.h5', 'CS302C-B1T6:50:02.h5', 'CS302C-B2T6:50:02.h5', 'CS302C-B3T6:50:02.h5', 'CS302C-B4T6:50:02.h5', 'CS302C-B5T6:50:02.h5']
filenames1_ear2=["CS302C-B3T5:00:39.h5","CS302C-B4T5:00:39.h5","CS302C-B5T5:00:39.h5"]
filenames_lightning=["CS302C-B0T15:55:31.h5","CS302C-B1T15:55:31.h5","CS302C-B2T15:55:31.h5","CS302C-B4T15:55:31.h5","CS302C-B5T15:55:31.h5"]
filenames_lightning2=["CS302C-B0T17:15:14.h5","CS302C-B1T17:15:14.h5","CS302C-B2T17:15:14.h5","CS302C-B4T17:15:14.h5","CS302C-B5T17:15:14.h5"]
filenames_lightning4=["CS302C-B0T19:50:25.h5","CS302C-B1T19:50:25.h5","CS302C-B2T19:50:25.h5","CS302C-B4T19:50:25.h5","CS302C-B5T19:50:25.h5"]
#filenames_lightning3=["CS302C-B0T17:07:22.h5","CS302C-B1T17:07:22.h5","CS302C-B2T17:07:22.h5","CS302C-B4T17:07:22.h5","CS302C-B5T17:07:22.h5"
data_directory_lightninglocal = "/Users/STV/Astro/data/lightning/"
filenames_lightninglocal = filenames=["lightning_16_48.h5"]
data_directory=data_directory_lightninglocal
filenames=filenames_lightninglocal
#TBBboards=[0,1,2,3,4,5]
nrfiles=1
donotskipcalculation=True

# Set calibration variables
stationname="CS302" #This value should also be able to be obtained from the files, but then we have to open one first.
antennaset="LBA_INNER" #Some files don't have the Antennaset set correctly in the hdf5 files yet.
antennafilename=LOFARSOFT+"data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
NyquistZone=1    #Some files don't have the Nyquistzone set correctly in the hdf5 files yet
caltablefilename=LOFARSOFT+"src/CR-Tools/data/calibrationCS302.dat" #(Not avaiable for LBA)
calibrationON = False

# Set what part of the data you want to make an image from
blocksize=2**10
#startblock=99597
#startblock=65000# for 17:15
#startblock=3725*2**6
startblock=200260 
nblocks=1
#selected_frequencies=range(268,292,1)
#selected_frequencies=range(357,358,1)
#selected_frequencies=[80,110,155,230,450,451]
#selected_frequencies=[50,51,49,48]
selected_frequencies=range(50,450,1)
FarField=False
Xpol=False
Ypol=False


# Set what part of the sky you want to make an image from
# This should be replaced by the new grid code
azrange=(0.,365.,5)
elrange=(0.,95.,5)
#Cas A 342 24
#if filenames==filenames1:
#    azrange=(183.831-30,183.831+30,3)
#    elrange=(59.0699-30,59.0699+30,3)
#if filenames==filenames3:
#    azrange=(206.229-30,206.229+33,3)
#    elrange=(57.0392-30,57.0392+33,3)
#if filenames==filenames5:
#    azrange=(224.908-30,224.908+33,3)
#    elrange=(52.6475-30,52.6475+33,3)


# (end) configuration

pixels=np.load('pixels.npy')
world=np.load('world.npy')

# Obtain the antenna positions from a calibration file
# This should be changed to obtain the positions from the hdf5
# This is limited to a single station set in the configuration variables
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
# At the moment only variables for CS302 HBA avaiable.
# Maybe can also add them for RS503 
# Add a check if the file correct file is avaiable?

# These where obtained by correlation on the source for a dataset from an earlier time
# De gegeven getallen zijn p0 en p1, waarbij fase =  p0+p1*freq. Om p1 om te zetten in delays moet je het nog delen door (2*pi)
# Gebruikswijze in oude code
# double phase=p0[rcu_ids[i](antenna)/2]+frequencies(ifreq)*p1[rcu_ids[i](antenna)/2];
# pahse=p0[rcuID]+freq*p1[rcuID]
# matCal(ifreq,antenna)=DComplex(cos(-calibrationfactor*phase),sin(-calibrationfactor*phase));
# dr[i]->setFFT2calFFT(matCal);

# NOT YET AVAILABLE FOR LBA

caltablefile=open(caltablefilename,'r')
caltablefile.seek(0)
str=''
caltable2 = []
for i in range(48):
    str=caltablefile.readline()
    caltable2.extend([str.split()[0],str.split()[1]])
    caltable2.extend([str.split()[0],str.split()[1]])

caltable=hArray(caltable2,[96,2])

# (end) get METAdata


# (start) Imaging

# Initialisation
# read in the files


files=[crfile(data_directory+filenames[0])]
for i in range(1,nrfiles):
    files.append(crfile(data_directory+filenames[i]))


AntennasInFile=[]
totAntennas=0
shifts=[]
for i in range(0,nrfiles):
    AntennasInFile.append(files[i]["nofSelectedAntennas"])
    totAntennas+=AntennasInFile[i]
    files[i]["blocksize"]=blocksize
    shifts.append(files[i]["SAMPLE_NUMBER"])
    files[i]["NyquistZone"]=NyquistZone

# search for maximum in shifts
shiftmax=0
for i in range(0,nrfiles):
    localmax=shifts[i].max()
    if localmax>shiftmax:
        shiftmax=localmax
    

# multiple number uses SAMPLE_NUMBER and TIME
for i in range(0,nrfiles):
    shifts2=[]
    for j in range(0,AntennasInFile[i]):
        shifts2.append(shiftmax-shifts[i][j])
    files[i]["ShiftVector"]=shifts2

print "Shifts: ",shifts2

rcuIDs=hArray(int,totAntennas)
curAntenna=0
for i in range(0,nrfiles):
    rcuIDstemp=files[i]["AntennaIDs"]
    for j in range(AntennasInFile[i]):
        rcuIDs[curAntenna]=rcuIDstemp[j]%1000
        curAntenna+=1

print "rcuIds: ",rcuIDs

used_antenna_pos=hArray(float,[totAntennas,3])
for i in range(totAntennas):
    used_antenna_pos[i]=antenna_positions[rcuIDs[i].val()]


print "Antenna positions: ",used_antenna_pos
# .... image part



# Calculate number of pixels
n_az=90#int((azrange[1]-azrange[0])/azrange[2])
n_el=90#int((elrange[1]-elrange[0])/elrange[2])
world.resize(n_az,n_el,2)
pixels.resize(n_az,n_el,2)
n_el=90
n_az=90
n_pixels=n_az*n_el

# Initialize arrays that are used.
file_time=files[0]["Time"].setUnit("\\mu","s")
file_frequencies=files[0]["Frequency"][selected_frequencies]
file_allfrequencies=files[0]["Frequency"]
file_efield=hArray(float,[totAntennas,files[0]["blocksize"]]).setPar("xvalues",file_time)
file_efieldtemp=[]
for i in range(nrfiles):
    file_efieldtemp.append(files[i]["emptyFx"].setPar("xvalues",file_time))
file_fft=hArray(complex,[totAntennas,files[0]["fftLength"]]).setPar("xvalues",file_allfrequencies)
selected_fft=hArray(complex,[totAntennas,len(file_frequencies)])
summed_fft=hArray(float,[totAntennas,len(file_frequencies)])
phasecorr=hArray(float,[totAntennas,files[0]["fftLength"]])
hanning=hArray(float,files[0]["blocksize"])
hanning.gethanningfilter()


#Calculate calibration phase corrections
phasecorr.copy(file_frequencies)
for i in range(totAntennas):
    phasecorr[i].mul(float(caltable[rcuIDs[i].val(),1].val()))
    phasecorr[i].add(float(caltable[rcuIDs[i].val(),0].val()))
    phasecorr[i].negate()

phasecorrweights=hArray(complex,[totAntennas,files[0]["fftLength"]])
phasecorrweights.phasetocomplex(phasecorr)

# Initialize geographical delay arrays
delays=hArray(float,dimensions=[n_pixels,totAntennas])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,totAntennas,len(file_frequencies)],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,totAntennas,len(file_frequencies)],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,len(file_frequencies)],name="Beamformed FFT")
power=hArray(float,dimensions=beamformed_fft,name="Spectral Power",xvalues=file_frequencies,fill=0.0)



# Calculate grid.
# Maybe should have a common phase_center to substract from the antennas is different files are processed
# in parallel or if a non-local coordinate system is used.
# phase_center=hArray(antenna_positions[0].vec())
# antenna_positions -= phase_center

azel=hArray(float,dimensions=[n_pixels,3])
cartesian=azel.new()

print "t=",time.clock(),"s -","Creating pximap GRID"
n=0
for el in range(n_el):
    for az in range(n_az):
        azel[n,0]=world[az,el,0]*180./np.pi
        azel[n,1]=world[az,el,1]*180./np.pi
        azel[n,2]=1
        n+=1


print "t=",time.clock(),"s -","Calculating coordinates and delays"
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
hGeometricDelays(delays,used_antenna_pos,cartesian,FarField)

phases.delaytophase(file_frequencies,delays)
weights.phasetocomplex(phases)



#shut down history tracking for the arrays as it's a big "memory leak"
for i in range(nrfiles):
    file_efieldtemp[i].setHistory(False)

    
file_efield.setHistory(False)
file_fft.setHistory(False)
shifted_fft.setHistory(False)
beamformed_fft.setHistory(False)
power.setHistory(False)
selected_fft.setHistory(False)


# Process blocks of data
t0=time.clock()
if donotskipcalculation:
    for block in range(startblock,startblock+nblocks):
        antennastart=0
        antennaend=0
        print "t=",time.clock()-t0,"s -","Reading data ",block
        for i in range(nrfiles):
            antennastart=antennaend
            antennaend=antennastart+AntennasInFile[i]
            if block==startblock:
                print "selecting antennas",antennastart," ",antennaend
            files[i]["block"]=block
            file_efieldtemp[i].read(files[i],"Fx").none()
            file_efield[antennastart:antennaend].copy(file_efieldtemp[i])
        print "t=",time.clock()-t0,"s -","Applying hanning filter",block
        file_efield*=hanning
        print "t=",time.clock()-t0,"s -","Performing fft",block
        file_fft[...].fftcasa(file_efield[...],NyquistZone)
        if calibrationON:
            file_fft.mul(phasecorrweights)
        print "t=",time.clock()-t0,"s -","Copying to grid",block
        #copies the fft to all positions in shifted_fft
        for i in range(totAntennas):
            selected_fft[i].copy(file_fft[i,selected_frequencies])
        summed_fft.spectralpower(selected_fft)
        shifted_fft.copy(selected_fft)
        print "t=",time.clock()-t0,"s -","Applying weights ",block
        shifted_fft[...].mul(weights[...])
        print "t=",time.clock()-t0,"s -","Beamforming ",block
        # Process both polarizations separately. Actually this should check if the RCU numbers is odd or even for HBAs
        # And check this for LBA as well. At the moment it only works for continuous datasets.
        for n in range(n_pixels):
            if Xpol:
                beamformed_fft[n] = shifted_fft[n,0]
                for ant in range(2,totAntennas,2):
                    shifted_fft[n,ant,...].addto(beamformed_fft[n,...])
                power.spectralpower(beamformed_fft)
            if Ypol:
                beamformed_fft[n] = shifted_fft[n,1]
                for ant in range(3,totAntennas,2):
                    shifted_fft[n,ant,...].addto(beamformed_fft[n,...])
                power.spectralpower(beamformed_fft)
        print "t=",time.clock()-t0,"s -","Processed block",block

print "t=",time.clock(),"s -","Binning and Normalizing"

# weight the frequency channels in power
# This should be replaced by a proper gain calibration beforehand.
#for j in range(len(selected_frequencies)):
#    sum2=0
#    for k in range(totAntennas):
#        sum2+=summed_fft[k,j].val()
#    power[...,j].div(sum2)



intpower=np.array(power[...].sum())
intpower.resize([n_el,n_az])
#intpower.swapaxes(0,1)
print "t=",time.clock(),"s -","Plotting"
plt.clf()
#plt.subplot(1,2,1)
plt.imshow(intpower,cmap=plt.cm.hot)

#np.save(LOFAR_DATA_DIR+"/Crab/pulse3/datadumpCal2,intpower)



#freqpower=np.array(power.vec())
#freqpower.resize([n_el,n_az,len(selected_frequencies)])
#for i in range(1,5):
#    plt.subplot(2,2,i)
#    plt.imshow(freqpower[:,:,i-1],cmap=plt.cm.hot,extent=(azrange[0],azrange[1],elrange[1],elrange[0]))

npazel=np.array(azel.vec())
npazel.resize([n_el,n_az,3])
npazel[intpower==intpower.max()]


hdu = pyfits.PrimaryHDU(intpower)
hdulist = pyfits.HDUList([hdu])
prihdr = hdulist[0].header
(0-azrange[0])/azrange[2]

#increment=2.566666603088*np.pi/180.

prihdr.update('CTYPE1','??LN-STG')  #This means "unkown longitude" with CARtesian projection, the 
#                                   #casaviewer asumes this the be J2000 because it probably can't do AZEL
prihdr.update('CRVAL1',180.)  #value of the axis at the reference point "CRPIX"
prihdr.update('CDELT1',2.566666603088)  #increment from the reference pixel to the next pixel
prihdr.update('CROTA1',0.)          #rotation, just leave it at 0.
prihdr.update('CRPIX1',45.5)          #pixel position at which the axis has the "CRVAL" value
prihdr.update('CUNIT1','deg')       #the unit in which "CRVAL" and "CDELT" are given

prihdr.update('CTYPE2','??LT-STG')  #This means "unkown latitude" with CARtesian projection, see above
prihdr.update('CRVAL2',90.) 
prihdr.update('CDELT2',2.566666603088)
prihdr.update('CROTA2',0.)
prihdr.update('CRPIX2',45.5)
prihdr.update('CUNIT2','deg')

#prihdr.update('CTYPE3','TIME')       #This means "linear time coordinate"
#prihdr.update('CRVAL3',0.)
#prihdr.update('CDELT3',5e-9)
#prihdr.update('CROTA3',0.)
#prihdr.update('CRPIX3',1.)
#prihdr.update('CUNIT3','s')

hdulist.writeto(data_directory+SummedFitsFilename)

# For images where frequency or time are also a dimension, the array has to be rearranged
# Still have to do this part
# 

#hdu2 = pyfits.PrimaryHDU(freqpower)
#hdulist2 = pyfits.HDUList([hdu2])
#hdulist2.writeto(data_directory+SeparatedFitsFilename)


