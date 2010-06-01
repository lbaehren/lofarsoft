#! /usr/bin/env python

#This file does beamforming in a specified direction and then makes a dynamic spectrum in that direction
#Resolution: Full resolution (no integration yet)
#Number of files: only 1 at the moment
#Different time offset per antenna: Not yet supported   (can use $LOFARSOFT/data/lofar/trigger-2010-02-11/ or /mnt/lofar/CS1_tbb/CrabFirstTrigger/pulse3/ )
#Calibration: Not yet supported
#Number of directions: 1
#Selection of part of the antennas in the file: Not yet supported, only works with full boards at the moment

from pycrtools import *

tStart=time.clock()
n_pixels=1

#pulse3
#crabdir="/Volumes/MyBook/data/TBB/Crab/"
#crabdir="/mnt/lofar/CS1_tbb/CrabFirstTrigger/"
#directory_pulse3=crabdir+"pulse3/"
#stationname="CS302"
#data_directory=directory_pulse3
#filenames=["CS302C-B0T6:01:48.h5","CS302C-B1T6:01:48.h5","CS302C-B2T6:01:48.h5","CS302C-B3T6:01:48.h5","CS302C-B4T6:01:48.h5","CS302C-B5T6:01:48.h5"]
#TBBboards=[0,1,2,3,4,5]
#azel=hArray([206.229,57.0392,1],dimensions=[n_pixels,3]) #pulse3
#blocksize=2**16

#pulse4
#crabdir="/Volumes/MyBook/data/TBB/Crab/"
#stationname="RS503"
#directory_pulse4=crabdir+"pulse4/"
#data_directory=directory_pulse4
#filenames=["RS503C-B0T6:21:27.h5","RS503C-B1T6:43:10.h5","RS503C-B3T6:21:49.h5","RS503C-B4T6:21:49.h5","RS503C-B5T6:21:49.h5"]
#TBBboards=[0,1,3,4,5]
#azel=hArray([218.476,54.4378,1],dimensions=[n_pixels,3]) #pulse4
#blocksize=2**16

#testdata
#LOFARSOFT="/Users/STV/usg2"
directory_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/"
stationname="RS208"
data_directory=directory_lofar
filenames=["triggered-pulse-2010-02-11-TBB1.h5","triggered-pulse-2010-02-11-TBB2.h5"]
TBBboards=[1,2]
azel=hArray([218.476,54.4378,1],dimensions=[n_pixels,3])
blocksize=2**10

#filenames=os.listdir(data_directory)
#blocksize=2**16






filename_lofar="triggered-pulse-2010-02-11-TBB1.h5"


#pulse3: Az, El: 206.229 57.0392

#src/CR-Tools/data/calibrationCS302.dat
#	double phase=p0[rcu_ids[i](antenna)/2]+frequencies(ifreq)*p1[rcu_ids[i](antenna)/2];
#					//cout << phase << " ";
#matCal(ifreq,antenna)=DComplex(cos(-calibrationfactor*phase),sin(-calibrationfactor*phase));

#antennapositions

antennaset="LBA_INNER"
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


antenna_positions=hArray(ant,[int(nrantennas),int(nrdir)])
#maybe this selection can be done later and at the moment it only works if a full TBB board is dumped
#ant_pos_tbb0=hArray(ant[0:15],[16,3])
#ant_pos_tbb1=hArray(ant[16:31],[16,3])
#ant_pos_tbb2=hArray(ant[32:47],[16,3])
#ant_pos_tbb3=hArray(ant[48:63],[16,3])
#ant_pos_tbb4=hArray(ant[64:79],[16,3])
#ant_pos_tbb5=hArray(ant[80:95],[16,3])


#Initialization

#filename=data_directory+filenames[0]
#filename=filenames[0]
#filecounter=0
#for filename in filenames:
filename=filenames[0]

cfile=crfile(data_directory+filename)

cfile["blocksize"]=blocksize

nsegments=1
#nsegments=50
nblocks=cfile.filesize/blocksize/nsegments

c_time=cfile["emptyTime"]
c_time.read(cfile,"Time")
#c_time.setUnit("mu","")
c_frequency=cfile["emptyFrequency"]
c_frequency.read(cfile,"Frequency")
c_frequency+=100*10**6.
c_efield=cfile["emptyFx"]
c_efield.par.xvalues=c_time
c_fft=cfile["emptyFFT"]
c_fft.par.xvalues=c_frequency
c_fft.par.logplot="y"

c_dynspec=hArray(float,[nblocks,cfile.fftLength],fill=0,name="Dynamic Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
c_complex_dynspec=hArray(complex,[nblocks,cfile.fftLength],fill=0.+0.*1j,name="Dynamic Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
spectrum=hArray(float,[cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
#complexspectrum=hArray(complex,[1,cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
#spectrum=hArray(float,[1,cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])


 
#Initialization phases
#part of this needs to change to inside the loop if the nofSelectedAntennas per file changes
FarField=True


#azel=hArray([206.229,57.0392,1],dimensions=[n_pixels,3]) #this part moved up, for different directions of different files
cartesian=azel.new()
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
delays=hArray(float,dimensions=[n_pixels,cfile["nofSelectedAntennas"]])
phases=hArray(float,dimensions=[n_pixels,cfile["nofSelectedAntennas"],cfile["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,cfile["nofSelectedAntennas"],cfile["fftLength"]],name="Complex Weights")

filecounter=0
for filename in filenames:
#
 cfile=crfile(data_directory+filename)
#
 cfile["blocksize"]=blocksize
 TBBboard=TBBboards[filecounter] 
 #used_antenna_positions=hArray(float,dimensions=[cfile["nofSelectedAntennas"]*2])
 #fill with the used antenna positions
 
 
 #hGeometricDelays(delays,used_antenna_positions,cartesian,FarField)
 
 
 #change this to have the values of this file
 used_antenna_positions=hArray(antenna_positions[TBBboards[TBBboard]*16:TBBboards[TBBboard]*16+cfile["nofSelectedAntennas"]-1],dimensions=[cfile["nofSelectedAntennas"],int(nrdir)])
 hGeometricDelays(delays,used_antenna_positions,cartesian,FarField)
 #delays += cal_delays
 phases.delaytophase(c_frequency,delays)
 weights.phasetocomplex(phases)
 
 
#Reading in one File
 
 t0=time.clock()
 for block in range(nblocks):
  t1=time.clock()-t0
  print "block=",block,"t=",t1,"s"
  cfile["Block"]=block
  c_efield.read(cfile,"Fx").none()
  c_fft[...].fftw(c_efield[...])
  c_fft[...].nyquistswap(2)
  c_fft *= weights
  c_fft[...].addto(c_complex_dynspec[[block],...])
#    complexspectrum.fill(0)
#    for i in range(cfile.nofAntennas):
#    spectrum[...].spectralpower(c_fft[...])
#    c_fft[...].addto(complexspectrum[...])

#Now we can set the unit in the frequency array. Before this had to
#match the inverse units of the delays (1/second=Hz).
#c_frequency.setUnit("M","")

print "Make Numpy array"
#c_complex_dynspec.writedump(data_directory+"dynspec2.dat")

#c_complex_dynspec.abs()
#c_dynspec.real(c_complex_dynspec)
c_dynspec.spectralpower(c_complex_dynspec)
c_dynspec.log()
ds=np.array(c_dynspec.vec())
ds.resize([nblocks,cfile.fftLength])
print "plot Numpy array"
plt.imshow(ds,cmap=plt.cm.hot,aspect='auto')


tEnd=time.clock()
tTotal=tEnd-tStart
print "Run time ",tTotal,"s"
#import pyfits
#hdu = pyfits.PrimaryHDU(ds)
#hdulist = pyfits.HDUList([hdu])
#hdulist.writeto(data_directory+'spectrum2.fits')




  
 



