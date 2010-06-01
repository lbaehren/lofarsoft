#! /usr/bin/env python

from pycrtools import *

filename_lofar_big=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"

filename= filename_lofar_big
filename= filename_lofar_onesecond

datafile=crfile(filename)
antennas=list(datafile["antennas"])
datafile.set("selectedAntennas",[0]).set("blocksize",min(2**22,2**22))
#datafile.set("blocksize",min(2**16,2**22))
nBlocks=min(datafile["filesize"]/datafile["blocksize"],2)

print time.asctime()
print "File     : ", datafile.filename
print "Filesize : ", datafile["filesize"]/10**6, "MSample"
print "Blocksize: ", datafile["blocksize"]/10**6, "MSample"
print "nBlocks  : ", nBlocks
print "Antennas : ", datafile["nofSelectedAntennas"]
print "Frequency resolution: ", int(datafile["increment"][1]),"Hz"
t0=time.clock()
frequency=datafile["Frequency"]
fft=datafile["emptyFFT"]
#fx=datafile["emptyFx"]

avspectrum=hArray(float,dimensions=fft,name="average spectrum")
print "t=",time.clock()-t0,"s -","Created vectors"
for ant in antennas[0:1]:
    print "\nAntenna = ",ant
    print "----------------"
    datafile["selectedAntennas"]=[ant]
    for block in range(nBlocks):
        print "Block = ",block," Time=",time.asctime()
#    fx.read(datafile.set("Block",block),"Fx").none()
#    print "t=",time.clock()-t0,"s -","Read in Fx"
#    fx.fft(fft,1)
        fft.read(datafile.set("Block",block),"FFT").none()
        print "t=",time.clock()-t0,"s -","Finished calculating FFT"
        fft.spectralpower(avspectrum)
        print "t=",time.clock()-t0,"s -","Finished calculating average spectrum"

dumpfile="seti-avspectrum.dmp"
print "Dumping spectrum to file",dumpfile 
avspectrum.writedump(dumpfile)
#avspectrum.readdump(dumpfile)

fblocklen=2**11
nfBlocks=datafile["fftLength"]/fblocklen

print "Frequency block length for plotting : ", fblocklen
print "Number of frequency blocks  : ", nfBlocks

print time.asctime()

freq=hArray(frequency.vec(),[datafile["fftLength"]/fblocklen,fblocklen],units=("","Hz"))
freq.setUnit("k","Hz")
spec=hArray(avspectrum.vec(),[datafile["fftLength"]/fblocklen,fblocklen],xvalues=freq)
spec[nfBlocks/2].plot(logplot="y",xvalues=freq[nfBlocks/2],xlabel="Frequency",ylabel="Spectral Power",title="SETI Spectrum from TBB")
"""
plt.savefig("setispec.pdf")
for i in range(nfBlocks):
    print "Plotting Spectrum ",i
    spec[i].plot(logplot="y",xvalues=freq[i],xlabel="Frequency",ylabel="Spectral Power",title="SETI Spectrum from TBB ("+str(i)+")")
    plt.savefig("setispec"+str(i)+".pdf")
"""
print time.asctime()


