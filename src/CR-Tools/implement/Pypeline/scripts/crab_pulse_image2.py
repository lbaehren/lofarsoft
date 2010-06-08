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
filenames=filenames1
#TBBboards=[0,1,2,3,4,5]
nrfiles=3
antennaset="HBA"
antennafilename=LOFARSOFT+"data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
blocksize=2**10
nblocks=300
startblock=5155
NyquistZone=2
caltablefilename=LOFARSOFT+"src/CR-Tools/data/calibrationCS302.dat" #(Not avaiable for LBA)
#selected_frequencies=range(268,292,1)
#selected_frequencies=range(357,358,1)
selected_frequencies=[80,110,155,230]
FarField=True
Xpol=True
Ypol=False
azrange=(0.,365.,5)
elrange=(0.,95.,5)
#Cas A 342 24
#if filenames==filenames1:
#    azrange=(110.,262.5,2.5)
#    elrange=(50.,76.5,1.5)
#if filenames==filenames3:
#    azrange=(205,232.5,2.5)
#    elrange=(45,77.5,2.5)
azrange=(183.831-30,183.831+30,3)
elrange=(59.0699-30,59.0699+30,3)

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


n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixels=n_az*n_el



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
phasecorr.copy(file_frequencies)
hanning=hArray(float,files[0]["blocksize"])
hanning.gethanningfilter()
#hanning.fill(1)

for i in range(totAntennas):
    phasecorr[i].mul(float(caltable[rcuIDs[i].val(),1].val()))
    phasecorr[i].add(float(caltable[rcuIDs[i].val(),0].val()))
    phasecorr[i].negate()

phasecorrweights=hArray(complex,[totAntennas,files[0]["fftLength"]])
phasecorrweights.phasetocomplex(phasecorr)
#THESE ARE NOT APPLIED YET


delays=hArray(float,dimensions=[n_pixels,totAntennas])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,totAntennas,len(file_frequencies)],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,totAntennas,len(file_frequencies)],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,len(file_frequencies)],name="Beamformed FFT")
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


#shut down history tracking for the arrays as it's a big "memory leak"
for i in range(nrfiles):
    file_efieldtemp[i].setHistory(False)

    
file_efield.setHistory(False)
file_fft.setHistory(False)
shifted_fft.setHistory(False)
beamformed_fft.setHistory(False)
power.setHistory(False)
selected_fft.setHistory(False)


t0=time.clock()
#file["block"]=startblock
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
    #file_efield[...].plot()
    print "t=",time.clock()-t0,"s -","Applying hanning filter",block
    file_efield*=hanning
    print "t=",time.clock()-t0,"s -","Performing fft",block
    file_fft[...].fftcasa(file_efield[...],NyquistZone)
    file_fft.mul(phasecorrweights)
    print "t=",time.clock()-t0,"s -","Copying to grid",block
    #file_fft[...].plot()
    #copies the fft to all positions in shifted_fft
    for i in range(totAntennas):
        selected_fft[i].copy(file_fft[i,selected_frequencies])
    summed_fft.spectralpower(selected_fft)
    shifted_fft.copy(selected_fft)
    print "t=",time.clock()-t0,"s -","Applying weights ",block
    shifted_fft[...].mul(weights[...])
    print "t=",time.clock()-t0,"s -","Beamforming ",block
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

#weight the frequency channels of power
for j in range(len(selected_frequencies)):
    sum2=0
    for k in range(totAntennas):
        sum2+=summed_fft[k,j].val()
    power[...,j].div(sum2)



intpower=np.array(power[...].sum())
#maxval=intpower.max()
#intpower /= maxval
intpower.resize([n_el,n_az])

print "t=",time.clock(),"s -","Plotting"
plt.clf()
#plt.subplot(1,2,1)
plt.imshow(intpower,cmap=plt.cm.hot,extent=(azrange[0],azrange[1],elrange[1],elrange[0]))

#np.save(LOFAR_DATA_DIR+"/Crab/pulse3/datadumpCal2,intpower)



freqpower=np.array(power.vec())
freqpower.resize([n_el,n_az,len(selected_frequencies)])
#for i in range(1,5):
#    plt.subplot(2,2,i)
#    plt.imshow(freqpower[:,:,i-1],cmap=plt.cm.hot,extent=(azrange[0],azrange[1],elrange[1],elrange[0]))



hdu = pyfits.PrimaryHDU(intpower)
hdulist = pyfits.HDUList([hdu])
prihdr = hdulist[0].header
(0-azrange[0])/azrange[2]

prihdr.update('CTYPE1','??LN-CAR')  #This means "unkown longitude" with CARtesian projection, the 
#                                   #casaviewer asumes this the be J2000 because it probably can't do AZEL
prihdr.update('CRVAL1',azrange[0])  #value of the axis at the reference point "CRPIX"
prihdr.update('CDELT1',azrange[2])  #increment from the reference pixel to the next pixel
prihdr.update('CROTA1',0.)          #rotation, just leave it at 0.
prihdr.update('CRPIX1',0.5)          #pixel position at which the axis has the "CRVAL" value
prihdr.update('CUNIT1','deg')       #the unit in which "CRVAL" and "CDELT" are given

prihdr.update('CTYPE2','??LT-CAR')  #This means "unkown latitude" with CARtesian projection, see above
prihdr.update('CRVAL2',0.) 
prihdr.update('CDELT2',elrange[2])
prihdr.update('CROTA2',0.)
prihdr.update('CRPIX2',(0.5-elrange[0])/elrange[2])
prihdr.update('CUNIT2','deg')

#prihdr.update('CTYPE3','TIME')       #This means "linear time coordinate"
#prihdr.update('CRVAL3',0.)
#prihdr.update('CDELT3',5e-9)
#prihdr.update('CROTA3',0.)
#prihdr.update('CRPIX3',1.)
#prihdr.update('CUNIT3','s')

hdulist.writeto(data_directory+"/imageCrabInt15.fits")

#For images where frequency or time are also a dimension, the array has to be rearranged


hdu2 = pyfits.PrimaryHDU(freqpower)
hdulist2 = pyfits.HDUList([hdu2])
hdulist2.writeto(data_directory+"/imageCrab15.fits")



# RFI mitigation and baseline calculation

# Beamforming section



#for block in range(200000,200300):
#    file["block"]=block
#    file_efield.read(file,"Fx").none()
#    print block," ",file_efield[0].max()

