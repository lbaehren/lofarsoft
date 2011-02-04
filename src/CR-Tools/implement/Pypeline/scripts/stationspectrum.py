#! /usr/bin/env python

#cd /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/tmp
#execfile("../stationspectrum.py")

#import pdb
#pdb.set_trace()

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
tmpfilename="testseti_"
tmpfileext=".dat"
filename= LOFARSOFT+"/data/lofar/CS302C-B0T6:01:48.h5"
filename= LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
lofarmode="LBA100MHZ"
quality_database_filename="qualitydatabase"
doplot=False
plotsubspectrum=151# Which part of the spectrum to plot
maxblocksflagged=2
#delta_nu=120 # Hz 
delta_nu=200 #6 frequency resolution in Hz
blocklen=2**10 #15The size of a block being read in
stride_n=2# if >0 then then one block is actually stride*blocklen and data is stored on disk during processing to save memory (slower!)
maxnantennas=4# Maximum number of antennas to sum over
maxchunks=1 # the maximum number of chunks to integrate over
#------------------------------------------------------------------------
#datafile=crfile(filename)
#antennas=datafile["selectedAntennas"]

stride=2**stride_n
spectrum_file=tmpfilename+"spec"+tmpfileext

datafile=crfile(filename)
header=datafile.hdr
freqs=datafile["Frequency"]

antennas=hArray(range(min(datafile["nofAntennas"],maxnantennas)))
antennaIDs=ashArray(hArray(datafile["AntennaIDs"])[antennas])

quality=[]
antennacharacteristics={}

full_blocklen=blocklen*stride
start_frequency=freqs[0]; end_frequency=freqs[-1]
delta_t=datafile["sampleInterval"]
fullsize_estimated=1./delta_nu/delta_t
nblocks=2**int(round(log(fullsize_estimated/full_blocklen,2)))
nbands=(stride+1)/2

fullsize=nblocks*full_blocklen
blocklen_section=blocklen/stride
nsubblocks=stride*nblocks
nblocks_section=nblocks/stride
subspeclen=blocklen*nblocks
subsubspeclen=min(blocklen*nblocks,2**16)
nsubsubspectra=subspeclen/2/subsubspeclen #factor 2 since we only look at first half
speclen=fullsize/2+1
dostride=(stride>1)
nchunks=min(datafile.filesize/fullsize,maxchunks)
nspectraadded=0
nspectraflagged=0

delta_frequency=(end_frequency-start_frequency)/(speclen-1.0)
delta_band=(end_frequency-start_frequency)/stride*2

parameters={
"lofarmode":{"doc":"Which LOFAR mode was used (HBA/LBA+UpperFrequency)"},
"start_frequency":{"doc":"Start frequency of spectrum","unit":"Hz"},
"end_frequency":{"doc":"End frequency of spectrum","unit":"Hz"},
"delta_frequency":{"doc":"Frequency resolution of spectrum","unit":"Hz"},
"delta_band":{"doc":"Frequency width of one section/band of spectrum","unit":"Hz"},
"full_blocklen":{"doc":"Full block length","unit":"Samples"},
"stride":{"doc":"Subblocks within blocks"},
"nblocks":{"doc":"Number of blocks"},
"nbands":{"doc":"Number of sections/bands in spectrum"},
"subspeclen":{"doc":"Size of one section/band of the final spectrum"},
"subspecwidth":{"doc":"Width of subspectrum after 1st FFT","val":subsubspeclen*delta_frequency/1000.,"unit":"kHz"},
"spectrum_file":{"doc":"Filename of file containing spectrum"},
"nsamples_data":{"doc":"Number of samples in raw antenna file","val":float(fullsize)/10**6,"unit":"MSamples"},
"size_data":{"doc":"Number of samples in raw antenna file","val":float(fullsize)/1024/1024*16,"unit":"MBytes"},
"nantennas":{"doc":"The number of antennas averaged","val":len(antennas)},
"nchunks_max":{"doc":"Maximum number of spectral chunks to average","val":float(datafile.filesize)/fullsize},
"nchunks":{"doc":"Number of spectral chunks that are averaged"}
}

def parameters_print(pars):
    print "\n#Parameters:\n#-----------------------------------------------------------------------"
    for key, value in pars.items():
        ks=value.keys()
        s=""
        if "doc"  in ks: s+=value["doc"] + ": "
        if "val"  in ks: s+=key+" = "+str(value["val"])
        if "unit" in ks: s+=" "+value["unit"]
        print s
    print "#-----------------------------------------------------------------------\n"

def parameters_eval(pars):
    for key,value in pars.items():
        if not "val" in value.keys(): pars[key]["val"]=eval(key)

def parameters_mergeheader(pars,hdr):
    for key,value in pars.items():
        hdr[key]=value["val"]

parameters_eval(parameters)
parameters_print(parameters)
parameters_mergeheader(parameters,header)

header["spectrum_pars"]=parameters

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

subfrequencies=hArray(float,[subspeclen],name="Frequency",units=("M","Hz"),header=header)
frequencies=hArray(float,[subspeclen/2],name="Frequency",units=("M","Hz"),header=header)
subsubfrequencies=hArray(frequencies.vec(),[nsubsubspectra,subsubspeclen],name="Frequency",units=("M","Hz"),header=header)

cdata=hArray(complex,[nblocks,blocklen],name="cdata",header=header) # creating input and work array
cdataT=hArray(complex,[blocklen,nblocks],name="cdataT",header=header) # creating output array with transposed axis

#Note, that all the following arrays have the same memory als cdata and cdataT
tmpspecT=hArray(cdataT.vec(),[stride,nblocks_section,blocklen],header=header) 
tmpspec=hArray(cdata.vec(),[nblocks_section,full_blocklen],header=header) 

specT=hArray(cdataT.vec(),[full_blocklen,nblocks_section],header=header) 
specT2=hArray(cdataT.vec(),[stride,blocklen,nblocks_section],header=header) 
spec=hArray(cdata.vec(),[blocklen,nblocks],header=header)

writeheader=True # used to make sure the header is written the first time
#power=hArray(float,[subspeclen/2],name="Spectral Power",xvalues=frequencies,par=[("logplot","y")])
power=hArray(float,[subspeclen],name="Spectral Power",xvalues=frequencies,par=[("logplot","y")],header=header)

subspec=hArray(cdata.vec(),[subspeclen],name="FFT",xvalues=subfrequencies,par=[("logplot","y")],header=header) 
print "Time:",time.clock()-t0,"s for set-up."

t0=time.clock(); print "Reading in data and doing a double FFT."
datafile["blocksize"]=blocklen #Setting initial block size

t0=time.clock();
initialround=True
for antenna in antennas:
    rms=0; mean=0; npeaks=0
    datafile["selectedAntennas"]=[antenna]
    antennaID=antennaIDs[antenna]
    print "#Antenna =",antenna,"( ID=",antennaID,")"
    for nchunk in range(nchunks):
#        pdb.set_trace()
        print "#  Chunk ",nchunk,"/",nchunks-1,". Reading in data and doing a double FFT."
        ofiles=[]; ofiles2=[]; ofiles3=[]
        for offset in range(stride):
            print "#    Pass ",offset,"/",stride-1,"Starting block=",offset+nchunk*nsubblocks
            blocks=range(offset+nchunk*nsubblocks,(nchunk+1)*nsubblocks,stride)            
            cdata[...].read(datafile,"Fx",blocks)
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
                cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...
        #Now sort the different blocks together (which requires a transpose over passes/strides)
        print "Time:",time.clock()-t0,"s for 1st FFT now doing 2nd FFT."
        if dataok:
            nspectraadded+=1
            for offset in range(stride):
                if dostride:
                    print "#    Offset",offset
                    tmpspecT[...].readfilebinary(Vector(ofiles),Vector(int,stride,fill=offset)*(nblocks_section*blocklen))
                    #This transpose it to make sure the blocks are properly interleaved
                    hTranspose(tmpspec,tmpspecT,stride,nblocks_section)
                specT.doublefft2(tmpspec,nblocks_section,full_blocklen)
                if dostride:
                    ofile=tmpfilename+str(offset)+"b"+tmpfileext
                    specT.writefilebinary(ofile)
                    ofiles2+=[ofile]
            print "Time:",time.clock()-t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
            for offset in range(nbands):
                if (nspectraadded==1): # first chunk
                    power.fill(0.0)
                else: #2nd or higher chunk, so read data in and add new spectrum to it
                    power.readfilebinary(spectrum_file,subspeclen*offset)
                    power*= (nspectraadded-1.0)/(nspectraadded)                        
                if dostride:
                    print "#    Offset",offset
                    specT2[...].readfilebinary(Vector(ofiles2),Vector(int,stride,fill=offset)*(blocklen*nblocks_section))
                    hTranspose(spec,specT2,stride,blocklen) # Make sure the blocks are properly interleaved
                    if nspectraadded>1: spec/=float(nspectraadded)   
                    power.spectralpower(spec)
                else: # no striding, data is all fully in memory
                    if nspectraadded>1: specT/=float(nspectraadded)   
                    power.spectralpower(specT)
                if stride==1: power[0:subspeclen/2].write(spectrum_file,nblocks=nbands,block=offset,writeheader=writeheader)
                else: power.write(spectrum_file,nblocks=nbands,block=offset,writeheader=writeheader)
                writeheader=False
            print "#  Time:",time.clock()-t0,"s for processing this chunk. Number of spectra added =",nspectraadded
        else: #data not ok
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
print "Total time used:",time.clock()-t0,"s."
print "To read back the spectrum type: sp=hArrayRead('"+spectrum_file+"')"
#----------------------


#Now update the header file again ....
parameters["nspectraadded"]={"doc":"Total number of spectra co-added","val":nspectraadded}
parameters["nspectraflagged"]={"doc":"Total number of spectra co-added","val":nspectraflagged}
parameters["antennacharacteristics"]={"doc":"Characteristic numbers (rms,mean,npeaks, etc.) for antenna data obtained from QualityCheck","val":antennacharacteristics}
parameters_mergeheader(parameters,power.par.hdr)
if stride==1: power.writeheader(spectrum_file,nblocks=nbands,dim=[subspeclen/2])
else: power.writeheader(spectrum_file,nblocks=nbands)

def make_frequencies(spectrum,offset=-1,frequencies=None,setxvalues=True):
    hdr=spectrum.par.hdr
    if offset<0:
        mult=hdr["nbands"]
        offset=0
    else: mult=1;
    l=hdr["subspeclen"]
    if hdr["stride"]==1: l/=2
    if frequencies==None:
        frequencies=hArray(float,[l*mult],name="Frequency",units=("M","Hz"),header=hdr)
    frequencies.fillrange((hdr["start_frequency"]+offset*hdr["delta_band"])/10**6,hdr["delta_frequency"]/10**6)
    if setxvalues: spectrum.par.xvalues=frequencies
    return frequencies

sp=hArrayRead('testseti_spec.dat');sp.vec()[0]=2000;sp.plot()
make_frequencies(sp)
mean=sp.vec()[0:30000].mean()
rms=sp.vec()[0:30000].stddev(mean)
noise=rms/mean
print "rms,mean,noise = ",(rms,mean,noise)

"""
sp=hArrayRead('testseti_spec.dat')
sp[0,0]=sp[0,1]
make_frequencies(sp)
sp.plot()

sp[0,:1000].mean()
Out[36]: Vector(float, 1, [94.0171097408])

In [37]: sp[0,:100].mean()
Out[37]: Vector(float, 1, [168.183441975])


sp=hArrayRead('testseti_spec.dat',0)
sp[0]=sp[1]
make_frequencies(sp,0)
sp.plot()

sp2=hArrayRead('testseti_spec.dat',1)
make_frequencies(sp2,1)
sp2.plot(clf=False)
"""

#plt.ion()
#datapeaks=hArray(int,[nsubsubspectra,subsubspeclen/16],name="Slicelist of Peaks")
#(npeaks,datathreshold,datamean)=findpeaks(datapeaks,subpower)
#rp(0,151,markpeaks=True)

##rp(0,nsubsubspectra/2,markpeaks=True)

##cdata=hArray(float,[nblocks,blocklen]) # creating input and work array
##cdata.read(datafile,"Fx")
##cdata[0].plot()

"""
"""
#pa 0,141
