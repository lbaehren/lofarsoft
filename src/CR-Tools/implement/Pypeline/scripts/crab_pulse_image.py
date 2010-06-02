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

# Get environment variables
LOFARSOFT = os.environ["LOFARSOFT"].rstrip('/')+'/'
LOFAR_DATA_DIR = os.environ['LOFAR_DATA_DIR'].rstrip('/')+'/'
data_directory3 = LOFAR_DATA_DIR+"Crab/pulse3/"
data_directory1 = LOFAR_DATA_DIR+"Crab/pulse1/"
stationname="CS302"
filenames3=["CS302C-B0T6:01:48.h5","CS302C-B1T6:01:48.h5","CS302C-B2T6:01:48.h5","CS302C-B3T6:01:48.h5","CS302C-B4T6:01:48.h5","CS302C-B5T6:01:48.h5"]
filenames1=["CS302C-B0T5:00:39.h5","CS302C-B1T5:00:39.h5","CS302C-B2T5:00:39.h5","CS302C-B3T5:00:39.h5","CS302C-B4T5:00:39.h5","CS302C-B5T5:00:39.h5"]
filenames1_ear1=["CS302C-B0T5:00:39.h5","CS302C-B1T5:00:39.h5","CS302C-B2T5:00:39.h5"]
data_directory=data_directory1
filenames=filenames1_ear1
#TBBboards=[0,1,2,3,4,5]
nrfiles=3
antennaset="HBA"
antennafilename=LOFARSOFT+"data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
blocksize=2**10
nblocks=10
startblock=1000
NyquistZone=2
caltablefilename=LOFARSOFT+"src/CR-Tools/data/calibrationCS302.dat"
#muting channel 0:100 and 270:513 in the code now. Should make these variables
# (end) configuration


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
# pahse=p0[rcuID]+freq*p1[rcuID]
# matCal(ifreq,antenna)=DComplex(cos(-calibrationfactor*phase),sin(-calibrationfactor*phase));
# dr[i]->setFFT2calFFT(matCal);

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
    
#totAntennas=file["nofSelectedAntennas"]
#AntennasInFile=file["nofSelectedAntennas"]

#search for maximum in shifts
shiftmax=0
for i in range(0,nrfiles):
    localmax=shifts[i].max()
    if localmax>shiftmax:
        shiftmax=localmax
    

#multiple number uses SAMPLE_NUMBER and TIME
for i in range(0,nrfiles):
    shifts2=[]
    for j in range(0,AntennasInFile[i]):
        shifts2.append(shiftmax-shifts[i][j])
    files[i]["ShiftVector"]=shifts2

rcuIDs=hArray(int,totAntennas)
curAntenna=0
for i in range(0,nrfiles):
    rcuIDstemp=files[i]["AntennaIDs"]
    for j in range(AntennasInFile[i]):
        rcuIDs[curAntenna]=rcuIDstemp[j]%1000
        curAntenna+=1


used_antenna_pos=hArray(float,[totAntennas,3])
for i in range(totAntennas):
    used_antenna_pos[i]=antenna_positions[rcuIDs[i].val()]


# .... image part

FarField=True
azrange=(0.,368.,8)
elrange=(0.,94.,4)
n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixels=n_az*n_el



file_time=files[0]["Time"].setUnit("\\mu","s")
file_frequencies=files[0]["Frequency"]
file_efield=hArray(float,[totAntennas,files[0]["blocksize"]]).setPar("xvalues",file_time)
file_efieldtemp=[]
for i in range(nrfiles):
    file_efieldtemp.append(files[i]["emptyFx"].setPar("xvalues",file_time))
file_fft=hArray(complex,[totAntennas,files[0]["fftLength"]]).setPar("xvalues",file_frequencies)

phasecorr=hArray(float,[totAntennas,files[0]["fftLength"]])
phasecorr.copy(file_frequencies)

for i in range(totAntennas):
    phasecorr[i].mul(float(caltable[i,1].val()))
    phasecorr[i].add(float(caltable[i,0].val()))
    phasecorr[i].negate()

phasecorrweights=hArray(complex,[totAntennas,files[0]["fftLength"]])
phasecorrweights.phasetocomplex(phasecorr)
#THESE ARE NOT APPLIED YET


delays=hArray(float,dimensions=[n_pixels,totAntennas])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,totAntennas,files[0]["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,totAntennas,files[0]["fftLength"]],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,files[0]["fftLength"]],name="Beamformed FFT")
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
    antennastart=0
    antennaend=0
    for i in range(nrfiles):
        antennastart=antennaend
        antennaend=antennastart+AntennasInFile[i]
        if block==startblock:
            print "selecting antennas",antennastart," ",antennaend
        files[i]["block"]=block
        file_efieldtemp[i].read(files[i],"Fx").none()
        file_efield[antennastart:antennaend].copy(file_efieldtemp[i])

    #file_efield[...].plot()
    file_fft[...].fftcasa(file_efield[...],NyquistZone)
    file_fft.mul(phasecorrweights)
    for i in range(totAntennas):
        file_fft[i,0:100]=0
        file_fft[i,270:513]=0

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

#np.save(LOFAR_DATA_DIR+"/Crab/pulse3/datadumpCal2,intpower)


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

