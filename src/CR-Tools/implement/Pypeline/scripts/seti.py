#! /usr/bin/env python

#cd /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/tmp
#execfile("../stationspectrum.py")


"""
Test of a SETI application by making a high-resolution spectrum of a long data set, using a double FFT.

This file tests a double FFT all at once in memory.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.

# but use from module import as much as possible to make sure the user
# has an easy life, after all it is the user who needs to be happy in
# the end.

from pycrtools import *
import time

execfile("../qualitycheck.py")
plt.ioff()
plt.clf()

#------------------------------------------------------------------------
#Input values
#------------------------------------------------------------------------
tmpfilename="testseti3_"
tmpfileext=".dat"
filename= LOFARSOFT+"/data/lofar/CS302C-B0T6:01:48.h5"
filename= LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
lofarmode="LBA100MHZ"
quality_database_filename="qualitydatabase"
doplot=True
plotsubspectrum=151# Which part of the spectrum to plot
maxblocksflagged=2
#delta_nu=120 # Hz 
delta_nu=6 # Hz 
blocklen=2**15 #The size of a block being read in
stride=1 # if >1 then then one block is actually stride*blocklen and data is stored on disk during processing to save memory (slower!)
maxchunks=0# read from file
#maxchunks=2# read only first chunk of data needed to get required resolution
#maxchunks=100# sum over multiple chunks of data to get an average spectrum
maxnantennas=100# Maximum number of antennas to sum over
#------------------------------------------------------------------------
filename= LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"

datafile=crfile(filename)
antennas=datafile["selectedAntennas"]

datafile=crfile(filename)
f=datafile["Frequency"]

antennas=hArray(range(min(datafile["nofAntennas"],maxnantennas)))
antennaIDs=hArray(datafile["AntennaIDs"])[antennas]

quality=[]
antennacharacteristics={}

full_blocklen=blocklen*stride
start_frequency=f[0]; end_frequency=f[-1]
delta_t=datafile["sampleInterval"]
fullsize_estimated=1./delta_nu/delta_t
nblocks=2**int(round(log(fullsize_estimated/full_blocklen,2)))

fullsize=nblocks*full_blocklen
blocklen_section=blocklen/stride
nsubblocks=stride*nblocks
nblocks_section=nblocks/stride
subspeclen=blocklen*nblocks
subsubspeclen=min(blocklen*nblocks,2**16)
nsubsubspectra=subspeclen/2/subsubspeclen #factor 2 since we only look at first half
speclen=fullsize/2+1
dobig=(fullsize<=4194304)
dobig=False
dostride=(stride>1)
nchunks=min(datafile.filesize/fullsize,maxchunks)
nspectraadded=0
nspectraflagged=0

delta_frequency=(end_frequency-start_frequency)/(speclen-1.0)
delta_band=(end_frequency-start_frequency)/stride*2

maxgap=int(ceil(1000/delta_frequency))

print "Frequency Resolution:",delta_frequency,"Hz"
print "Full block length:",full_blocklen,"samples split into", stride,"subblocks."
print "Number of blocks:", nblocks
print "Width of subspectrum:",subsubspeclen*delta_frequency/1000,"kHz"
print "Maximum gap between peaks: ",maxgap*delta_frequency," Hz ( ",maxgap," channels )"

if fullsize>10**6:
    print "Full size:",fullsize/10**6,"MSamples"
    print "Full size:",fullsize/1024/1024*16,"MBytes"
else:
    print "Full size:",fullsize/10**3,"kSamples"
    print "Full size:",fullsize/1024*16,"kBytes"
print "Number of chunks to average:",1.0*datafile.filesize/fullsize," ( taking ",nchunks,")"

t0=time.clock(); print "Setting up."
#Open file

#Delete arrays first, in case they were already existing, to make sure
#enough memory is available when re-running the script
tmpspecT=None
tmpspec=None
specT=None
specT2=None
spec=None
power=None
subpower=None
subspec=None
subfrequencies=None
frequencies=None
subsubfrequencies=None
bigfrequencies=None
bigfft=None
cdata=None
cdataT=None

subfrequencies=hArray(float,[subspeclen],name="Frequency",units=("M","Hz"))
frequencies=hArray(float,[subspeclen/2],name="Frequency",units=("M","Hz"))
subsubfrequencies=hArray(frequencies.vec(),[nsubsubspectra,subsubspeclen],name="Frequency",units=("M","Hz"))

cdata=hArray(complex,[nblocks,blocklen]) # creating input and work array
cdataT=hArray(complex,[blocklen,nblocks]) # creating output array with transposed axis

#Note, that all the following arrays have the same memory als cdata and cdataT
tmpspecT=hArray(cdataT.vec(),[stride,nblocks_section,blocklen]) 
tmpspec=hArray(cdata.vec(),[nblocks_section,full_blocklen]) 

specT=hArray(cdataT.vec(),[full_blocklen,nblocks_section]) 
specT2=hArray(cdataT.vec(),[stride,blocklen,nblocks_section]) 
spec=hArray(cdata.vec(),[blocklen,nblocks])

power=hArray(float,[subspeclen/2],name="Power of spectrum",xvalues=frequencies,par=[("logplot","y")])
subpower=hArray(power.vec(),[nsubsubspectra,subsubspeclen],name="Power of spectrum",xvalues=frequencies,par=[("logplot","y")])
    
subspec=hArray(cdata.vec(),[subspeclen],name="FFT",xvalues=subfrequencies,par=[("logplot","y")]) 
print "Time:",time.clock()-t0,"s for set-up."

def findpeaks(self,subpower,threshold=6):
    datamean=subpower[...].meaninverse()
    datathreshold = subpower[...].stddevbelow(datamean)
    datathreshold *= threshold
    datathreshold += datamean
    maxgapvec=Vector(int,len(datamean),fill=maxgap)
    minlength=Vector(int,len(datamean),fill=1)
    npeaks=self[...].findsequencegreaterthan(subpower[...],datathreshold,maxgapvec,minlength)
    return (npeaks,datathreshold,datamean)
    
def rp(offset,sub=-1,clf=True,markpeaks=False):
    """Basic plotting of a part of the specturm"""
    global subspeclen,start_frequency,delta_frequency,delta_band,ofiles3,nsubsubspectra,subsubfrequencies
    if (sub==-1) & (subspeclen>524288):
        print "Spectrum too large, plotting subspectrum #0"
        sub=0
    if dobig:
        bigfft[offset*subspeclen:(offset+1)*subspeclen].plot(xvalues=bigfrequencies[offset*subspeclen:(offset+1)*subspeclen],clf=True)
        clf=False
    if dostride: power.readdump(ofiles3[offset])
    frequencies.fillrange((start_frequency+offset*delta_band)/10**6,delta_frequency/10**6)
    if sub>=0:
        sub=min(sub,nsubsubspectra-1)
        if markpeaks:
            plotconst(datathreshold[sub],subsubfrequencies[sub]).plot(clf=clf,color="green")
            subpower[sub].plot(xvalues=subsubfrequencies[sub],highlight=datapeaks[sub],nhighlight=npeaks[sub],color="blue",clf=False)
        else:
            subpower[sub].plot(xvalues=subsubfrequencies[sub],clf=clf)
    else:
        power.plot(clf=clf)

t0=time.clock(); print "Reading in data and doing a double FFT."
datafile["blocksize"]=blocklen #Setting initial block size

t0=time.clock(); 
for antenna in antennas:
    rms=0; mean=0; npeaks=0
    datafile["selectedAntennas"]=[antenna]
    antennaID=antennaIDs[antenna]
    print "#Antenna =",antenna,"( ID=",antennaID,")"
    for nchunk in range(nchunks):
        print "#  Chunk ",nchunk,"/",nchunks-1,". Reading in data and doing a double FFT."
        ofiles=[]; ofiles2=[]; ofiles3=[]
        for offset in range(stride):
            print "#    Pass ",offset,"/",stride-1,"Starting block=",offset+nchunk*nsubblocks
            blocks=range(offset+nchunk*nsubblocks,(nchunk+1)*nsubblocks,stride)
            
            quality.append(CRQualityCheckAntenna(cdata,datafile=datafile,normalize=True,blockoffset=offset+nchunk*nsubblocks,observatorymode=lofarmode))
            CRDatabaseWrite(quality_database_filename+".txt",quality[-1])
            mean+=quality[-1]["mean"]
            rms+=quality[-1]["rms"]
            npeaks+=quality[-1]["npeaks"]
            dataok=(quality[-1]["nblocksflagged"]<=maxblocksflagged)
            if not dataok:
                print "##Data flagged - not taking it."
                break
            #            print "Time:",time.clock()-t0,"s for reading."
            cdataT.doublefft1(cdata,fullsize,nblocks,blocklen,offset)
            #            print "Time:",time.clock()-t0,"s for 1st FFT."
            if dostride:
                ofile=tmpfilename+str(offset)+"a"+tmpfileext
                ofiles+=[ofile]
                cdata.writedump(ofile)  # output of doublefft1 is in cdata ...
        #Now sort the different blocks together (which requires a transpose over passes/strides)
        #       print "Time:",time.clock()-t0,"s for 1st FFT now doing 2nd FFT."
        if dataok:
            for offset in range(stride):
                if dostride:
                    print "#    Offset",offset
                    tmpspecT[...].readdump(Vector(ofiles),Vector(int,stride,fill=offset))
                    #This transpose it to make sure the blocks are properly interleaved
                    hTranspose(tmpspec,tmpspecT,stride,nblocks_section)
                specT.doublefft2(tmpspec,nblocks_section,full_blocklen)
                if dostride:
                    ofile=tmpfilename+str(offset)+"b"+tmpfileext
                    specT.writedump(ofile)
                    ofiles2+=[ofile]
    #        print "Time:",time.clock()-t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
            for offset in range(stride):
                ofile=tmpfilename+"spec_"+str(offset)+tmpfileext
                if dostride:
                    print "#    Offset",offset
                    specT2[...].readdump(Vector(ofiles2),Vector(int,stride,fill=offset))
                     #This transpose it to make sure the blocks are properly interleaved
                    hTranspose(spec,specT2,stride,blocklen)
                    if (nchunk>0): power.readdump(ofile)
                    else: power.fill(0.0)
                    power.spectralpower(spec)
                    nspectraadded+=1
                    ofiles3+=[ofile]
                else:
                    power.spectralpower(specT)
                    nspectraadded+=1
                    if nspectraadded>1:
                        power *= (nspectraadded-1.0)/nspectraadded                        
                power.writedump(ofile)
            print "#  Time:",time.clock()-t0,"s for processing this chunk. Number of spectra added =",nspectraadded
        else: #dataok
            nspectraflagged+=1
            print "#  Time:",time.clock()-t0,"s for reading and ignoring this chunk.  Number of spectra flagged =",nspectraflagged
    print "#  End of all chunks (Antenna =",antenna,") -------------------------------"
    if nchunks>0:
        mean/=nchunks
        rms/=nchunks
        antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":quality[-nchunks:]}
        print "#Antenna characteristics ",antennaID," =",{"mean":mean,"rms":rms,"npeaks":npeaks}
        f=open(quality_database_filename+".py","a")
        f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(antennacharacteristics[antennaID])+"\n")
        f.close()
        if doplot:
            rp(0,plotsubspectrum,markpeaks=False,clf=False)
            plt.draw()
print "##End of all Antennas - nspectraadded=",nspectraadded,"nspectraflagged=",nspectraflagged

def pa(sub=0,nstart=0):
    for i in range(nstart,nsubsubspectra):
        print i," - n peaks =",npeaks[i],", mean = ",datamean[i]
        rp(sub,i,markpeaks=True)
        s=raw_input("Press enter (q to quit): ")
        if s=="q": break

#----------------------

if maxchunks==0:
    print "maxchunks==0 - > Reading spectrum from file."  
    power.readdump("spectrum.dat")
    antennacharacteristics={}
    filename="qualitydatabase_16.py"
    execfile(filename)
    a_mean=map(lambda(k):antennacharacteristics[k]["mean"],antennacharacteristics.keys())
    a_rms=map(lambda(k):antennacharacteristics[k]["rms"],antennacharacteristics.keys())
    a_npeaks=map(lambda(k):antennacharacteristics[k]["npeaks"],antennacharacteristics.keys())
    #plt.clf(); plt.plot(a_rms,a_npeaks,'bo'); plt.xlabel("RMS [ADC]"); plt.ylabel("Npeaks")
    #plt.clf(); plt.plot(a_rms,a_mean,'bo'); plt.xlabel("RMS [ADC]"); plt.ylabel("Mean [ADC]")
    
plt.ion()
datapeaks=hArray(int,[nsubsubspectra,subsubspeclen/16],name="Slicelist of Peaks")
(npeaks,datathreshold,datamean)=findpeaks(datapeaks,subpower)
rp(0,151,markpeaks=True)

#rp(0,nsubsubspectra/2,markpeaks=True)

#cdata=hArray(float,[nblocks,blocklen]) # creating input and work array
#cdata.read(datafile,"Fx")
#cdata[0].plot()

"""
"""
#pa 0,141
