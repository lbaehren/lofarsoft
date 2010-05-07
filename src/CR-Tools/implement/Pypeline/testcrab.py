#
#alias pycr='/sw/bin/python2.6 -i /Users/STV/usg2/src/CR-Tools/implement/Pypeline/pycrinit.py'
#. $LOFARSOFT/devel_common/scripts/init.sh

crabdir="/Volumes/MyBook/data/TBB/Crab/"
directory_pulse4=crabdir+"pulse4/"
stationname="RS503"
blocksize=2**16
data_directory=directory_pulse4

filenames=os.listdir(data_directory)

filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"


#antennapositions

antennaset="HBA"
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


#Initialization

#filename=data_directory+filenames[0]
filename=filenames[0]
TBBboard=0
cfile=crfile(data_directory+filename)

cfile["blocksize"]=blocksize

nsegments=50
nblocks=cfile.filesize/blocksize/nsegments

c_time=cfile["emptyTime"]
c_time.read(cfile,"Time")
c_time.setUnit("mu","")
c_frequency=cfile["emptyFrequency"]
c_frequency.read(cfile,"Frequency")
c_frequency+=100*10**6.
c_efield=cfile["emptyFx"]
c_efield.par.xvalues=c_time
c_fft=cfile["emptyFFT"]
c_fft.par.xvalues=c_frequency
c_fft.par.logplot="y"

c_dynspec=hArray(float,[nblocks,cfile.fftLength],fill=0,name="Dynamic Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
c_complex_dynspec=hArray(complex,[nblocks,cfile.fftLength],fill=0.+j0,name="Dynamic Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
spectrum=hArray(float,[cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
#complexspectrum=hArray(complex,[1,cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])
#spectrum=hArray(float,[1,cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])

#calculatin phases
FarField=True
n_pixels=1
azel=hArray([218.476,54.4378,1],dimensions=[n_pixels,3])
cartesian=azel.new()
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
delays=hArray(float,dimensions=[n_pixels,cfile["nofSelectedAntennas"]])
phases=hArray(float,dimensions=[n_pixels,cfile["nofSelectedAntennas"],cfile["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,cfile["nofSelectedAntennas"],cfile["fftLength"]],name="Complex Weights")


#used_antenna_positions=hArray(float,dimensions=[cfile["nofSelectedAntennas"]*2])
#fill with the used antenna positions


#hGeometricDelays(delays,used_antenna_positions,cartesian,FarField)
hGeometricDelays(delays,antenna_positions,cartesian,FarField)
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
c_frequency.setUnit("M","")

print "Make Numpy array"
#c_complex_dynspec.writedump("dynspec.dat")

#c_complex_dynspec.abs()
#c_dynspec.real(c_complex_dynspec)
c_dynspec.spectralpower(c_complex_dynspec)
c_dynspec.log()
ds=np.array(c_dynspec.vec())
ds.resize([nblocks,cfile.fftLength])
print "plot Numpy array"
plt.imshow(ds,cmap=plt.cm.hot,aspect='auto')





