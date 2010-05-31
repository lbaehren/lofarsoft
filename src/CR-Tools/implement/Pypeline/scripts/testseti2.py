filename_lofar_big=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"


#------------------------------------------------------------------------
#Input values
#------------------------------------------------------------------------
filename= filename_lofar_onesecond
filename= filename_lofar_big
nAntennas=1
deltanu_input=100 #Hz
#------------------------------------------------------------------------

datafile=crfile(filename)
antennas=list(datafile["antennas"])
datafile["selectedAntennas"]=[0]

block_size_1=2**int(ceil(log(sqrt(datafile["filesize"]),2)))
#block_size_1=2**(int(ceil(log(sqrt(datafile["filesize"]),2)))+4)
#block_size_1=int(sqrt(datafile["filesize"])/2)*2
datafile["blocksize"]=block_size_1 #Setting initial block size
deltanu_1=datafile["increment"][1] #Retrieving frequency interval of first step spectrum
block_size_2=min(2*int(deltanu_1/deltanu_input/2),datafile.filesize/block_size_1)
nBlocks_1=block_size_2 #Number of 1st level blocks needed to reach reqired frequency resolution
nLoops=datafile["filesize"]/(block_size_1*nBlocks_1) # How often to repeat step 1 to read in the entire file

nChannels_1=block_size_1/2+1 # Number of frequency channels in coarse spectrum (after step 1)
nChannels_2=block_size_2
nChannels=nChannels_1*nChannels_2
blocksize_full = (nChannels - 1)*2

nuMax=deltanu_1*(nChannels_1-1)
deltanu=nuMax/(nChannels-1)

print time.asctime()
print "File       : ", datafile.filename
print "Filesize   : ", datafile["filesize"]/10**6, "MSample"
print "Blocksize 1: ", block_size_1
print "Blocksize 2: ", block_size_2
print "nBlocks 1  : ", nBlocks_1
print "nChannels_1:", nChannels_1
print "nChannels_2:", nChannels_2
print "nLoops     : ", nLoops
print "Antennas   : ", datafile["nofSelectedAntennas"]
print "Frequency resolution 1: ", int(datafile["increment"][1]/10**3),"kHz"
print "Frequency resolution 2: ", deltanu,"Hz"
print "Total Number of channels:", nChannels
print "Full blocksize needed :", blocksize_full


#Do the first step of splitting the data into smaller frequency chunks

t0=time.clock()
fft1=datafile["emptyFFT"]
frequency=hArray(float,[nChannels_1,nBlocks_1],name="Frequency",units=("","Hz"))
subbands=hArray(complex,dimensions=frequency,fill=0.,xvalues=frequency)
fft_subbands=hArray(complex,dimensions=frequency,fill=0.,xvalues=frequency)
frequency.fillrange(0,deltanu)
frequency.setUnit("k","")
fx=datafile["emptyFx"]
hanning1=fx.new()
hanning1.gethanningfilter()
hanning2=hArray(float,dimensions=[1,nChannels_2])
hanning2.gethanningfilter()
power=hArray(float,dimensions=subbands,xvalues=frequency,name="Spectral Power",fill=0.)
power.par.logplot="y"

block=0
#nLoops=1
print "t=",time.clock()-t0,"s -","Created vectors"
for loop in range(nLoops):
    for ant in antennas[0:nAntennas]:
        print "\nAntenna = ",ant
        print "----------------"
        datafile["selectedAntennas"]=[ant]
#        nBlocks_1=2
        for b in range(nBlocks_1):
            block=loop*nBlocks_1+b
            if ((block/100)*100==block):
                print "Loop=",loop,"b=",b,"Block = ",block," Time=",time.asctime()
            fx.read(datafile.set("Block",block),"Fx").none() # Read data into fx array
            fx *= hanning1
            fft1.fftw(fx) # Make FFT and store in array fft
            subbands.redistribute(fft1,b,nBlocks_1) # i.e. the size of the last dimension of subbands
        subbands *= hanning2
        fft_subbands[...].fftw(subbands[...])
        fft_subbands.spectralpower(power) ##Note here the order is changed, output is in second argument ...

print "t=",time.clock()-t0,"s -","Done with calculation."

#plt.subplot(1,2,1)
#power.plot(title="Split FFT",clf=False)

t0=time.clock()
print "Now doing all at once for comparison ...."

datafile["blocksize"]=blocksize_full
fx_full=datafile["emptyFx"]
fft_full=datafile["emptyFFT"]
frequency_full=datafile["Frequency"]
frequency_full.setUnit("k","")

power_full=hArray(float,dimensions=fft_full,xvalues=frequency_full,name="Spectral Power")
power_full.par.logplot="y"

hanning3=hArray(float,dimensions=fx_full)
hanning3.gethanningfilter()

for ant in antennas[0:nAntennas]:
    datafile["selectedAntennas"]=[ant]
    print "t=",time.clock()-t0,"s -","Antenna =",ant
    for loop in range(nLoops):
        print "t=",time.clock()-t0,"s -","Loop =",loop
        fx_full.read(datafile.set("block",loop),"Fx").none()
        fx_full *= hanning3
        fft_full.fftw(fx_full)
        fft_full.spectralpower(power_full)
print "t=",time.clock()-t0,"s -","Done with calculation."

power_full2=hArray(power_full,dimensions=power,xvalues=frequency)
#plt.subplot(1,2,2)
#power_full.plot(title="Full FFT at once",clf=False)
power.plot(xvalues=frequency);power_full.plot(xvalues=frequency,clf=False)

#File       :  /Users/falcke/LOFAR/usg/data/lofar/rw_20080701_162002_0109.h5
#Filesize   :  2 MSample
#Blocksize 1:  2048
#Blocksize 2:  976
#
#block_size_1=2**int(ceil(log(sqrt(datafile["filesize"]),2)))
##power_full /= 8.59
#i=607;o=1;power[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1]);power_full[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1],clf=False)
#59260*1000/deltanu/nBlocks_1
#i=901;o=2;power[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1]);power_full[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1],clf=False)
#i=193;o=1;power[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1]);power_full[i-o:i+o+1].plot(xvalues=frequency[i-o:i+o+1],clf=False)
#power.plot(xvalues=frequency);power_full.plot(xvalues=frequency,clf=False)
