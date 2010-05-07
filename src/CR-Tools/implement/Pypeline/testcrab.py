#
#alias pycr='/sw/bin/python2.6 -i /Users/STV/usg2/src/CR-Tools/implement/Pypeline/pycrinit.py'
#. $LOFARSOFT/devel_common/scripts/init.sh

crabdir="/Volumes/MyBook/data/TBB/Crab/"
directory_pulse4=crabdir+"pulse4/"
blocksize=2**16
data_directory=directory_pulse4

filenames=os.listdir(data_directory)

filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"
filename=data_directory+filenames[0]
cfile=crfile(filename)

#Initialization
cfile["blocksize"]=blocksize

nsegments=50
nblocks=cfile.filesize/blocksize/nsegments

c_time=cfile["emptyTime"]
c_time.read(cfile,"Time")
c_time.setUnit("mu","")
c_frequency=cfile["emptyFrequency"]
c_frequency.read(cfile,"Frequency")
c_frequency.setUnit("M","")
c_frequency+=100
c_efield=cfile["emptyFx"]
c_efield.par.xvalues=c_time
c_fft=cfile["emptyFFT"]
c_fft.par.xvalues=c_frequency
c_fft.par.logplot="y"

c_dynspec=hArray(float,[nblocks,cfile.fftLength],fill=0,name="Dynamic Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])

spectrum=hArray(float,[cfile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=c_frequency,par=[("logplot","y")])

#Reading in one File

t0=time.clock()
for block in range(nblocks):
    t1=time.clock()-t0
    print "block=",block,"t=",t1,"s"
    cfile["Block"]=block
    c_efield.read(cfile,"Fx").none()
    c_fft[...].fftw(c_efield[...])
    c_fft[...].nyquistswap(2)
    spectrum.fill(0)
    for i in range(cfile.nofAntennas):
        spectrum.spectralpower(c_fft[i])
    c_dynspec[block].add(spectrum)
print "Make Numpy array"
#c_dynspec.writedump("dynspec.dat")

c_dynspec.log()

ds=np.array(c_dynspec.vec())
ds.resize([nblocks,cfile.fftLength])
print "plot Numpy array"
plt.imshow(ds,cmap=plt.cm.hot,aspect='auto')
