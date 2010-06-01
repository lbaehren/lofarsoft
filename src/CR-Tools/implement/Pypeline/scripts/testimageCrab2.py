#! /usr/bin/env python

from pycrtools import *

def p_(var):
    if (type(var)==list): map(lambda x:p_(x),var)
    else: print " ",var,"=>",eval(var)

#from pycrtools import *

filename_sun=LOFARSOFT+"/data/lopes/example.event"
filename_biglofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"
filename_crab="/Volumes/MyBook/data/TBB/Crab/pulse3/CS302C-B5T6:01:48.h5"
TBBboards=[5]
TBBboard=0
figno=0

plt.clf()
#------------------------------------------------------------------------

print "t=",time.clock(),"s -","Reading in data and setting parameters"
file=crfile(filename_crab)
file["blocksize"]=1024 #2**12
nblocks1=100
nblocks2=10
NyquistZone=2
size=180
pixel_sep=3.
center=[178.9,27.7,1]
center=[178.9,30.0,1]
center=[178.9,28.0,1]
center=[195,33,1]
center=[180,90,1]
FarField=True

size2=size/2
azrange=(center[0]-size2,center[0]+size2+1,pixel_sep)
elrange=(center[1]-size2,center[1]+size2+1,pixel_sep)
azrange=(0.,360.,8.)
elrange=(0.,90.,2.)
n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixels=n_az*n_el

file_time=file["Time"].setUnit("\\mu","s")
file_frequencies=file["Frequency"]
file_efield=file["Fx"].setPar("xvalues",file_time)

file_fft=file["emptyFFT"].setPar("xvalues",file_frequencies)

file_fft[...].fftcasa(file_efield[...],NyquistZone)

print "t=",time.clock(),"s -","Reading in Calibration Data"
#antenna_positions=file.getCalData("Position")
antennaset="HBA"
stationname="CS302"
antennafilename=LOFARSOFT+"/data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
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
    ant.extend([float(str.split()[0]),float(str.split()[1]),float(str.split()[2])])  #Y polarization

#antenna_positions=hArray(ant,[int(nrantennas),int(nrdir)])
antenna_positions=hArray(ant[TBBboards[TBBboard]*16:TBBboards[TBBboard]*16+file["nofSelectedAntennas"]-1],[16,3])



#cal_delays=file.getCalData("Delay")
delays=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"]])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,file["fftLength"]],name="Beamformed FFT")
power=hArray(float,dimensions=beamformed_fft,name="Spectral Power",xvalues=file_frequencies,fill=0.0)

phase_center=hArray(antenna_positions[0].vec())
antenna_positions -= phase_center

azel=hArray(float,dimensions=[n_pixels,3])
cartesian=azel.new()

#------------------------------------------------------------------------

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
hGeometricDelays(delays,antenna_positions,cartesian,FarField)
#delays += cal_delays

print "t=",time.clock(),"s -","Calculating phases"
#phases=hArray(float,dimensions=sun_fft,name="Phases",xvalues=sun_frequencies)
phases.delaytophase(file_frequencies,delays)
#hGeometricPhases(phases,sun_frequencies,antenna_positions,cartesian,True)


print "t=",time.clock(),"s -","Calculating weights"
weights.phasetocomplex(phases)
#hGeometricWeights(weights,file_frequencies,antenna_positions,cartesian,FarField)

for block in range(nblocks1):
    print "t=",time.clock(),"s -","Reading in Block #",block
    file["block"]=block
    file_efield.read(file,"Fx")
    file_fft[...].fftcasa(file_efield[...],NyquistZone)
#
    print "t=",time.clock(),"s -","Applying weights"
    shifted_fft.copy(file_fft)
    shifted_fft *= weights
#
    print "t=",time.clock(),"s -","Beamforming"
    for n in range(n_pixels):
        beamformed_fft[n] = shifted_fft[n,0]
        shifted_fft[n,1:,...].addto(beamformed_fft[n])
#
    print "t=",time.clock(),"s -","Adding up spectral power"
    power.spectralpower(beamformed_fft)

print "t=",time.clock(),"s -","Binning and Normalizing"
intpower=np.array(power[...].sum())
maxval=intpower.max()
intpower /= maxval
intpower.resize([n_az,n_el])

print "t=",time.clock(),"s -","Plotting"
plt.clf()
#plt.subplot(1,2,1)
plt.imshow(intpower,cmap=plt.cm.hot,extent=(azrange[0],azrange[1],elrange[0],elrange[1]))
#print intpower
print "t=",time.clock(),"s -","Done Method 1"

intpower2=np.array(power.vec())
intpower2.resize([n_az,n_el,513])


for i in range(513): 
 max=intpower2[:,:,i].max()
 intpower2[:,:,i]/=max

import pyfits
hdu = pyfits.PrimaryHDU(intpower2)
hdulist = pyfits.HDUList([hdu])
hdulist.writeto("/Volumes/MyBook/data/TBB/Crab/pulse3/image15.fits")

