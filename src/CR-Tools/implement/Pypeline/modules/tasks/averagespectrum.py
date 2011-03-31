"""Spectrum documentation.

sp=hArrayRead('tmpspec.dat'); make_frequencies(sp)
t=task(); r=t(sp)

Old:
current_task=False
import tasks

import tasks.averagespectrum as avspec
av=avspec.averagespectrum(maxnantennas=2)


import tasks.averagespectrum
tt=avspec.ttest(y=2)

import tasks.averagespectrum as avspec
ws=avspec.WorkSpace(maxnantennas=2)
ws.stride=2
del ws.stride
ws

ws=tasks.WorkSpace(**args)
"""

#import pdb; pdb.set_trace()

from pycrtools import *
from pycrtools.tasks.shortcuts import *
import pycrtools.tasks as tasks
import pycrtools.qualitycheck as qualitycheck
import time
from pycrtools import IO

#Defining the workspace parameters

def make_frequencies(spectrum,offset=-1,frequencies=None,setxvalues=True):
    """Calculates the frequencies for the calculated spectrum (outdated)
    """
    hdr=spectrum.par.hdr
    if offset<0 and hdr.has_key("nbands"):
        mult=hdr["nbands"]
    else:
        mult=1
    if offset<0: offset=0
    if hdr.has_key("subspeclen"):
        l=hdr["subspeclen"]
        if hdr["stride"]==1: l/=2
    else:
        l=len(spectrum)
    if frequencies==None:
        frequencies=hArray(float,[l*mult],name="Frequency",units=("M","Hz"),header=hdr)
    frequencies.fillrange((hdr["start_frequency"]+offset*hdr["delta_band"])/10**6,hdr["delta_frequency"]/10**6)
    if setxvalues:
        spectrum.par.xvalues=frequencies
    return frequencies

#def averagespectrum_calcfrequencies(self):
#    """Calculates the frequencies for the calculated spectrum.
#    """


def averagespectrum_getfile(ws):
    """
    To produce an error message in case the file does not exist
    """
    if ws.file_start_number < len(ws.filenames):
        return crfile(ws.filenames[ws.file_start_number])
    else:
        print "ERROR: File "+ws.filefilter+" not found!"
        return None

"""
*default*  contains a default value or a function that will be assigned
when the parameter is accessed the first time and no value has been
explicitly set. The function has the form "lambda ws: functionbody", where ws is the worspace itself, so that one can access other
parameters. E.g.: default:lambda ws: ws.par1+1 so that the default
value is one larger than he value in par1 in the workspace.

*doc* a documentation string describing the parameter

*unit* the unit of the value (for informational purposes only)

*export* (True) if False do not export the parameter with
 ws.parameters() or print the parameter

*workarray* (False) If True then this is a workarray which contains
 large amount of memory and is listed separately and not written to
 file.
"""

class WorkSpace(tasks.WorkSpace(taskname="AverageSpectrum")):
    parameters = {

        "filefilter":p_("$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                        "Unix style filter (i.e., with *,~, $VARIABLE, etc.), to describe all the files to be processed."),

        "file_start_number":{default:0,
                    doc:"Integer number pointing to the first file in the 'filenames' list with which to start. Can be changed to restart a calculation."},

        "datafile":{default:averagespectrum_getfile,export:False,doc:"Data file object pointing to raw data."},

        "doplot":{default:False,doc:"Plot current spectrum while processing."},

        "plotlen":{default:2**12,doc:"How many channels +/- the center value to plot during the calculation (to allow progress checking)."},

        "plotskip":{default:1,doc:"Plot only every 'plotskip'-th spectrum, skip the rest (should not be smaller than 1)."},

        "lofarmode":{default:"LBA_OUTER",
                     doc:"Which LOFAR mode was used (HBA/LBA_OUTER/LBA_INNER)"},

        "stride_n":p_(0,"if >0 then divide the FFT processing in n=2**stride_n blocks. This is slower but uses less memory."),

        "doublefft":{doc:"if True split the FFT into two, thereby saving memory.", default:False},

        "delta_nu":{default:1000,doc:"Frequency resolution",unit:"Hz"},

        "blocklen":{default:lambda ws:2**int(round(log(ws.fullsize_estimated,2))/2),doc:"The size of a block being read in."},

        "maxnantennas":{default:1,doc:"Maximum number of antennas per file to sum over (also used to allocate some vector sizes)."},

        "maxnchunks":{default:1,doc:"Maximum number of chunks of raw data to integrate spectrum over."},

        "maxblocksflagged":{default:2,doc:"Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."},

        "stride":{default:lambda ws: 2**ws.stride_n if ws.doublefft else 1,
                  doc:"If stride>1 divide the FFT processing in n=stride blocks."},

        "tmpfileext":{default:".pcr",
                      doc:"Extension of filename for temporary data files (e.g., used if stride>1.)"},

        "tmpfilename":{default:"tmp",
                       doc:"Root filename for temporary data files."},

        "filenames":{default:lambda ws:listFiles(ws.filefilter),
                    doc:"List of filenames of data file to read raw data from."},

        "spectrum_file":{default:lambda ws:(ws.filenames[0] if len(ws.filenames)>0 else "unknown")+".spec"+ws.tmpfileext,
                         doc:"Filename to store the final spectrum."},

        "qualitycheck":{default:True,doc:"Perform basic qualitychecking of raw data and flagging of bad data sets."},

        "quality_db_filename":{default:"qualitydatabase",
                               doc:"Root filename of log file containing the derived antenna quality values (uses .py and .txt extension)."},

        "quality":{default:[],doc:"A list containing quality check information about every large chunk of data that was read in. Use Task.qplot(Entry#,flaggedblock=nn) to plot blocks in question.",export:False,output:True},

        "antennacharacteristics":{default:{},doc:"A dict with antenna IDs as key, containing quality information about every antenna.",export:False,output:True},

        "mean_antenna":{default:lambda self: hArray(float,[self.maxnantennas], name="Mean per Antenna"),doc:"Mean value of time series per antenna.",output:True},

        "rms_antenna":{default: lambda self: hArray(float,[self.maxnantennas], name="RMS per Antenna"),doc:"Rms value of time series per antenna.",output:True},

        "npeaks_antenna":{default: lambda self: hArray(float,[self.maxnantennas], name="Number of Peaks per Antenna"),doc:"Number of peaks of time series per antenna.",output:True},

        "mean":{default:0,doc:"Mean of mean time series values of all antennas.",output:True},

        "mean_rms":{default:0,doc:"RMS of mean of mean time series values of all antennas.",output:True},

        "npeaks":{default:0,doc:"Mean of number of peaks all antennas.",output:True},

        "npeaks_rms":{default:0,doc:"RMS of npeaks over all antennas.",output:True},

        "rms":{default:0,doc:"Mean of rms time series values of all antennas.",output:True},

        "rms_rms":{default:0,doc:"RMS of rms of mean time series values of all antennas.",output:True},

        "homogeneity_factor":{default:0,doc:"=1-(rms_rms/rms+ npeaks_rms/npeaks)/2 - this describes the homogeneity of the data processed. A homogeneity_factor=1 means that all antenna data were identical, a low factor should make one wonder if something went wrong.",output:True},

        "spikeexcess":dict(default=20,doc="Set maximum allowed ratio of detected over expected peaks per block to this level (1 is roughly what one expects from Gaussian noise)."),
        "rmsfactor":dict(default=2,doc="Factor by which the RMS is allowed to change within one chunk of time series data before it is flagged."),

        "meanfactor":dict(default=3,doc="Factor by which the mean is allowed to change within one chunk of time series data before it is flagged."),

        "randomize_peaks":{default:True,doc:"Replace all peaks in time series data which are 'rmsfactor' above or below the mean with some random number in the same range."},

        "peak_rmsfactor":dict(default=5,doc="At how many sigmas above the mean will a peak be randomized."),

        "start_frequency":{default:lambda ws:ws.freqs[0],
                           doc:"Start frequency of spectrum",unit:"Hz"},

        "end_frequency":{default:lambda ws:ws.freqs[-1],
                         doc:"End frequency of spectrum",unit:"Hz"},

        "delta_frequency":p_(lambda ws:(ws.end_frequency-ws.start_frequency)/(ws.speclen-1.0),"Separation of two subsequent channels in final spectrum"),

        "delta_band":{default:lambda ws:(ws.end_frequency-ws.start_frequency)/ws.stride*2,
                      doc:"Frequency width of one section/band of spectrum",unit:"Hz"},

        "full_blocklen":{default:lambda ws:ws.blocklen*ws.stride,
                         doc:"Full block length",unit:"Samples"},

        "fullsize":p_(lambda ws:ws.nblocks*ws.full_blocklen,"The full length of the raw time series data used for one spectral chunk."),

        "nblocks":{default:lambda ws:2**int(round(log(ws.fullsize_estimated/ws.full_blocklen,2))),
                   doc:"Number of blocks of lenght blocklen the time series data set is split in. The blocks are also used for quality checking."},

        "nbands":{default:lambda ws:(ws.stride+1)/2,
                  doc:"Number of bands in spectrum which are separately written to disk, if stride>1. This is one half of stride, since only the lower half of the spectrum is written to disk."},

        "subspeclen":{default:lambda ws:ws.blocklen*ws.nblocks,
                      doc:"Size of one section/band of the final spectrum"},

        "nsamples_data":{default:lambda ws:float(ws.fullsize)/10**6,
                         doc:"Number of samples in raw antenna file",unit:"MSamples"},

        "size_data":{default:lambda ws:float(ws.fullsize)/1024/1024*16,
                     doc:"Number of samples in raw antenna file",unit:"MBytes"},

        "nantennas":{default:lambda ws:len(ws.antennas),
                     doc:"The actual number of antennas available for calculation in the file (<maxnantennas)."},

        "nantennas_start":{default:0,
                     doc:"Start with the nth antenna in each file (see also natennas_stride). Can be used for selecting odd/even antennas."},

        "antenna_list":{default:{},
                     doc:"List of antenna indices used as input from each filename.",output:True},

        "nantennas_stride":{default:1,
                     doc:"Take only every nth antenna from antennas list (see also natennas_start). Use 2 to select odd/even."},

        "nchunks_max":{default:lambda ws:float(ws.datafile.filesize)/ws.fullsize,
                       doc:"Maximum number of spectral chunks to average"},

        "nchunks":{default:lambda ws:min(ws.datafile.filesize/ws.fullsize,ws.maxnchunks),
                   doc:"Number of spectral chunks that are averaged"},

        "nspectraadded":p_(lambda ws:0,"Number of spectra added at the end.",output=True),

        "nspectraflagged":p_(lambda ws:0,"Number of spectra flagged and not used.",output=True),

        "antennas":p_(lambda ws:hArray(range(min(ws.datafile["nofAntennas"],ws.maxnantennas))),"Antennas from which to select initially for the current file."),

        "antennas_used":p_(lambda ws:set(),"A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",output=True),

        "antennaIDs":p_(lambda ws:ashArray(hArray(ws.datafile["AntennaIDs"])[ws.antennas]),"Antenna IDs to be selected from for current file."),

        "nantennas_total":p_(0,"Total number of antennas that were processed (flagged or not) in this run.",output=True),


        "delta_t":p_(lambda ws:ws.datafile["sampleInterval"],"Sample length in raw data set.","s"),
        
        "fullsize_estimated":p_(lambda ws:min(1./ws.delta_nu/ws.delta_t,ws.datafile.filesize)),
        "blocklen_section":p_(lambda ws:ws.blocklen/ws.stride),
        "nsubblocks":p_(lambda ws:ws.stride*ws.nblocks),
        "nblocks_section":p_(lambda ws:ws.nblocks/ws.stride),
        "speclen":p_(lambda ws:ws.fullsize/2+1),
        "header":p_(lambda ws:ws.datafile.hdr,"Header of datafile",export=False),
        "freqs":p_(lambda ws:ws.datafile["Frequency"],export=False),


#Now define all the work arrays used internally

        "frequencies":{workarray:True,
                       doc:"Frequency axis for final power spectrum.",default:lambda ws:
                       hArray(float,[ws.subspeclen/2 if ws.stride==1 else ws.subspeclen],name="Frequency",units=("M","Hz"),header=ws.header)},

        "power":{workarray:True,
                 doc:"Resulting average power spectrum (or part thereof if stride>1)",default:lambda ws:
                 hArray(float,[ws.subspeclen/2 if ws.stride==1 else ws.subspeclen],name="Spectral Power",par=dict(logplot="y"),header=ws.header,xvalues=ws.frequencies)},

#                hArray(float,[ws.subspeclen],name="Spectral Power",xvalues=ws.frequencies,par=[("logplot","y")],header=ws.header)},
#Need to cerate frequencies, taking current subspec (stride) into account

        "cdata":{workarray:True,
                 doc:"main input and work array",default:lambda ws:
                 hArray(complex,[ws.nblocks,ws.blocklen],name="cdata",header=ws.header)},

        "cdataT":{workarray:True,
                  doc:"Secondary input and work array",default:lambda ws:
                  hArray(complex,[ws.blocklen,ws.nblocks],name="cdataT",header=ws.header)},

#Note, that all the following arrays have the same memory als cdata and cdataT

        "tmpspecT":{workarray:True,
                    doc:"Wrapper array for cdataT",default:lambda ws:
                    hArray(ws.cdataT.vec(),[ws.stride,ws.nblocks_section,ws.blocklen],header=ws.header)},

        "tmpspec":{workarray:True,
                   doc:"Wrapper array for cdata",default:lambda ws:
                   hArray(ws.cdata.vec(),[ws.nblocks_section,ws.full_blocklen],header=ws.header)},

        "specT":{workarray:True,
                 doc:"Wrapper array for cdataT",default:lambda ws:
                 hArray(ws.cdataT.vec(),[ws.full_blocklen,ws.nblocks_section],header=ws.header)},

        "specT2":{workarray:True,
                  doc:"Wrapper array for cdataT",default:lambda ws:
                  hArray(ws.cdataT.vec(),[ws.stride,ws.blocklen,ws.nblocks_section],header=ws.header)},

        "spec":{workarray:True,
                doc:"Wrapper array for cdata",default:lambda ws:
                hArray(ws.cdata.vec(),[ws.blocklen,ws.nblocks],header=ws.header)}
}

#    files = [f for f in files if test.search(f)]

class AverageSpectrum(tasks.Task):
    """

    The function will calculate an average spectrum from a list of
    files and a series of antennas (all averaged into one
    spectrum).

    The task will do a basic quality check of each time series data
    set and only average good data.

    For very large FFTs, and to save memory, one can use a 'doublefft'
    which means that one big FFT is split into two steps of smaller
    FFts. This allows almost arbitrary resolution.

    The desired frequency resolution is provided with the parameter
    delta_nu, but this will be rounded off to the nearest value using
    channel numbers of a power of 2.

    The resulting spectrum is stored in the array Task.power (only
    complete for stride=1) and written to disk as an hArray with
    parameters stored in the header dict (use
    getHeader('AverageSpectrum') to retrieve this.)

    This spectrum can be read back and a baseline can be fitted with
    FitBaseline.

    To avoid the spectrum being influenced by spikes in the time
    series, those spikes can be replaced by random numbers, before the
    FFT is taken (see 'randomize_peaks').

    The quality information (RMS, MEAN, flagged blocks per antennas)
    is stored in a data 'quality database' in text and python form and
    is also available as Task.quality. (See also:
    Task.antennacharacteristics, Task.mean, Task.mean_rms, Task.rms,
    Task.rms_rms, Task.npeaks, Task.npeaks_rms,
    Task.homogeneity_factor - the latter is printend and is a useful
    hint if something is wrong)

    Flagged blocks can be easily inspeced using the task method
    Task.qplot (qplot for quality plot).

    If you see an outputline like this,

      # Start antenna = 92 (ID= 17011092) - 4 passes:
      184 - Mean=  3.98, RMS=  6.35, Npeaks=  211, Nexpected=256.00 (Npeaks/Nexpected=  0.82), nsigma=  2.80, limits=( -2.80,   2.80)
      185 - Mean=  3.97, RMS=  6.39, Npeaks=  200, Nexpected=256.00 (Npeaks/Nexpected=  0.78), nsigma=  2.80, limits=( -2.80,   2.80)
      186 - Mean=  3.98, RMS=  6.40, Npeaks=  219, Nexpected=256.00 (Npeaks/Nexpected=  0.86), nsigma=  2.80, limits=( -2.80,   2.80)
      - Block   514: mean=  0.25, rel. rms=   2.6, npeaks=   16, spikyness=  15.00, spikeexcess= 16.00   ['rms', 'spikeexcess']

    this will tell you that Antenna 17011092 was worked on (the 92nd
    antenna in the data file) and the 186th chunk (block 514)
    contained some suspicious data (too many spikes). If you want to
    inspect this, you can call

    Task.qplot(186)

    This will plot the chunk and highlight the first flagged block
    (#514) in that chunk.

    Task.qplot(186,1)

    would highlight the second flagged block (which does not exist here).

    If the chunks are too long to be entirely plotted, use

    Task.qplot(186,all=False).

    """
    WorkSpace = WorkSpace

    def init(self):
        """
        Initialize the task
        """

        # Start initialization
        #Nothing to do
        return

    def run(self):
        """Run the program.
        """
        #crfile=IO.open(filenames)

        #print "Time:",time.clock()-self.ws.t0,"s for set-up."

        self.quality=[]
        self.antennas_used=set()
        self.antennacharacteristics={}
        self.spectrum_file_bin=os.path.join(self.spectrum_file,"data.bin")
        self.dostride=(self.stride>1)
        self.nspectraflagged=0
        self.nspectraadded=0
        self.count=0
        self.nofAntennas=0
        self.nantennas_total=0
        self.power.getHeader("increment")[1]=self.delta_frequency
        self.updateHeader(self.power,["nofAntennas","nspectraadded","filenames","antennas_used"],fftLength="speclen",blocksize="fullsize",filename="spectrum_file")

        self.t0=time.clock() #; print "Reading in data and doing a double FFT."

        clearfile=True
        dataok=True
        initialround=True

        if self.doplot:
            plt.ioff()

        npass = self.nchunks*self.stride
        original_file_start_number=self.file_start_number

        for fname in self.filenames[self.file_start_number:]:
            print "# Start File",str(self.file_start_number)+":",fname
            self.ws.update(workarrays=False) # since the file_start_number was changed, make an update to get the correct file
            if self.stride==1:
                self.datafile["blocksize"]=self.blocklen*self.nblocks #Setting initial block size
            else:
                self.datafile["blocksize"]=self.blocklen #Setting initial block size
            self.antenna_list[fname]=range(self.nantennas_start,self.nantennas, self.nantennas_stride)
            for iantenna in self.antenna_list[fname]:
                antenna=self.antennas[iantenna]
                rms=0; mean=0; npeaks=0
                self.datafile["selectedAntennas"]=[antenna]
                antennaID=self.antennaIDs[iantenna]
                print "# Start antenna =",antenna,"(ID=",str(antennaID)+") -",npass,"passes:"
                for nchunk in range(self.nchunks):
                    #if self.nchunks>1: sys.stdout.write("*")
                    #print "#  Chunk ",nchunk,"/",self.nchunks-1,". Reading in data and doing a double FFT."
                    ofiles=[]; ofiles2=[]; ofiles3=[];
                    for offset in range(self.stride):
                        #if self.stride>1: sys.stdout.write(".")
                        #print "#    Pass ",offset,"/",self.stride-1,"Starting block=",offset+nchunk*self.nsubblocks
                        if self.stride==1:
                            self.cdata.read(self.datafile,"Fx",nchunk)
                        else:
                            blocks=range(offset+nchunk*self.nsubblocks,(nchunk+1)*self.nsubblocks,self.stride)
                            self.cdata[...].read(self.datafile,"Fx",blocks)
                        if self.qualitycheck:
                            self.count=len(self.quality)
                            self.quality.append(qualitycheck.CRQualityCheckAntenna(self.cdata,datafile=self.datafile,normalize=False,blockoffset=offset+nchunk*self.nsubblocks,observatorymode=self.lofarmode,spikeexcess=self.spikeexcess,spikyness=100000,rmsfactor=self.rmsfactor,meanfactor=self.meanfactor,count=self.count))
                            if not self.quality_db_filename=="":
                                qualitycheck.CRDatabaseWrite(self.quality_db_filename+".txt",self.quality[self.count])
                            mean+=self.quality[self.count]["mean"]
                            rms+=self.quality[self.count]["rms"]
                            npeaks+=self.quality[self.count]["npeaks"]
                            dataok=(self.quality[self.count]["nblocksflagged"]<=self.maxblocksflagged)
                        if not dataok:
                            print " # Data flagged!"
                            break
                        #            print "Time:",time.clock()-self.t0,"s for reading."
                        if  self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"]>0:
                            lower_limit=self.quality[self.count]["mean"]-self.peak_rmsfactor*self.quality[self.count]["rms"]
                            upper_limit=self.quality[self.count]["mean"]+self.peak_rmsfactor*self.quality[self.count]["rms"]
                            self.cdata.randomizepeaks(lower_limit,upper_limit)
                        if self.doublefft:
                            self.cdataT.doublefft1(self.cdata,self.fullsize,self.nblocks,self.blocklen,offset)
                        else:
                            self.cdataT.fftw(self.cdata)
                        #            print "Time:",time.clock()-self.t0,"s for 1st FFT."
                        if self.dostride:
                            ofile=self.tmpfilename+str(offset)+"a"+self.tmpfileext
                            ofiles+=[ofile]
                            self.cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...
                    #Now sort the different blocks together (which requires a transpose over passes/strides)
                    #print "Time:",time.clock()-self.t0,"s for 1st FFT now doing 2nd FFT."
                    if dataok:
                        self.nspectraadded+=1
                        if not antennaID in self.antennas_used:
                            self.antennas_used.add(antennaID)
                            self.nofAntennas+=1
                        if self.doublefft:  # do second step of double fft, if required
                            for offset in range(self.stride):
                                if self.dostride:
                                    #print "#    Offset",offset
                                    self.tmpspecT[...].readfilebinary(Vector(ofiles),Vector(int,self.stride,fill=offset)*(self.nblocks_section*self.blocklen))
                                    #This transpose it to make sure the blocks are properly interleaved
                                    hTranspose(self.tmpspec,self.tmpspecT,self.stride,self.nblocks_section)
                                self.specT.doublefft2(self.tmpspec,self.nblocks_section,self.full_blocklen)
                                if self.dostride:
                                    ofile=self.tmpfilename+str(offset)+"b"+self.tmpfileext
                                    self.specT.writefilebinary(ofile)
                                    ofiles2+=[ofile]
                            #print "Time:",time.clock()-self.t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
                            for offset in range(self.nbands):
                                if (self.nspectraadded==1): # first chunk
                                    self.power.fill(0.0)
                                else: #2nd or higher chunk, so read data in and add new spectrum to it
                                    if self.dostride:
                                        self.power.readfilebinary(self.spectrum_file_bin,self.subspeclen*offset)
                                    self.power *= (self.nspectraadded-1.0)/(self.nspectraadded)
                                if self.dostride:
                                    #print "#    Offset",offset
                                    self.specT2[...].readfilebinary(Vector(ofiles2),Vector(int,self.stride,fill=offset)*(self.blocklen*self.nblocks_section))
                                    hTranspose(self.spec,self.specT2,self.stride,self.blocklen) # Make sure the blocks are properly interleaved
                                    if self.nspectraadded>1:
                                        self.spec/=float(self.nspectraadded)
                                    self.power.spectralpower(self.spec)
                                else: # no striding, data is allready fully in memory
                                    if self.nspectraadded>1:
                                        self.specT/=float(self.nspectraadded)
                                    self.power.spectralpower(self.specT)
                                self.frequencies.fillrange((self.start_frequency+offset*self.delta_band)/10**6,self.delta_frequency/10**6)
                                self.updateHeader(self.power,["nofAntennas","nspectraadded","nspectraflagged","antennas_used"])
                                self.power.write(self.spectrum_file,nblocks=self.nbands,block=offset,clearfile=clearfile)
                                clearfile=False
                                if self.doplot and offset==self.nbands/2 and self.nspectraadded%self.plotskip==0:
                                    self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].plot()
                                    #print "mean=",self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].mean()
                                    plt.draw(); plt.show()
                        else: # doublefft - i.e. here do just a single FFT
                            if self.nspectraadded>1:
                                self.cdataT/=float(self.nspectraadded)
                            self.power *= (self.nspectraadded-1.0)/(self.nspectraadded)
                            self.power.spectralpower(self.cdataT)
                            self.frequencies.fillrange((self.start_frequency)/10**6,self.delta_frequency/10**6)
                            self.updateHeader(self.power,["nofAntennas","nspectraadded","filenames","antennas_used"],fftLength="speclen",blocksize="fullsize")
                            self.power.write(self.spectrum_file)
                        #print "#  Time:",time.clock()-self.t0,"s for processing this chunk. Number of spectra added =",self.nspectraadded
                            if self.doplot and self.nspectraadded%self.plotskip==0:
                                self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].plot()
                                print "mean=",self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].mean()
                                plt.draw(); plt.show()
                    else: #data not ok
                        self.nspectraflagged+=1
                        #print "#  Time:",time.clock()-self.t0,"s for reading and ignoring this chunk.  Number of spectra flagged =",self.nspectraflagged
                print "# End   antenna =",antenna," Time =",time.clock()-self.t0,"s  nspectraadded =",self.nspectraadded,"nspectraflagged =",self.nspectraflagged
                if self.qualitycheck:
                    mean/=self.nchunks
                    rms/=self.nchunks
                    self.mean_antenna[self.nantennas_total]=mean
                    self.rms_antenna[self.nantennas_total]=rms
                    self.npeaks_antenna[self.nantennas_total]=npeaks
                    self.antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":self.quality[-self.nchunks:]}
                    print "#          mean =",mean,"rms =",rms,"npeaks =",npeaks
                    f=open(self.quality_db_filename+".py","a")
                    f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(self.antennacharacteristics[antennaID])+"\n")
                    f.close()
                self.nantennas_total+=1
            print "# End File",str(self.file_start_number)+":",fname
            self.file_start_number+=1
        self.file_start_number=original_file_start_number # reset to original value, so that the parameter file is correctly written.
        if self.qualitycheck:
            self.mean=asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms=asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms=asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms=asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks=asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms=asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor=1-(self.npeaks_rms/self.npeaks + self.rms_rms/self.rms)/2. if self.npeaks>0 else 1-(self.rms_rms/self.rms)
            print "Mean values for all antennas: Task.mean =",self.mean,"+/-",self.mean_rms,"(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =",self.rms,"+/-",self.rms_rms,"(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =",self.npeaks,"+/-",self.npeaks_rms,"(Task.npeaks_rms)"
            print "Quality factor =",self.homogeneity_factor * 100,"%"
        print "Finished - total time used:",time.clock()-self.t0,"s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '184 - Mean=  3.98, RMS=...'"
        print "To read back the spectrum type: sp=hArrayRead('"+self.spectrum_file+"')"
        if self.doplot:
            plt.ion()

    def qplot(self,entry=0,flaggedblock=0,block=-1,all=True):
        """
        If you see an output line like this,

    # Start antenna = 92 (ID= 17011092) - 4 passes:
    184 - Mean=  3.98, RMS=  6.35, Npeaks=  211, Nexpected=256.00 (Npeaks/Nexpected=  0.82), nsigma=  2.80, limits=( -2.80,   2.80)
    185 - Mean=  3.97, RMS=  6.39, Npeaks=  200, Nexpected=256.00 (Npeaks/Nexpected=  0.78), nsigma=  2.80, limits=( -2.80,   2.80)
    186 - Mean=  3.98, RMS=  6.40, Npeaks=  219, Nexpected=256.00 (Npeaks/Nexpected=  0.86), nsigma=  2.80, limits=( -2.80,   2.80)
    - Block   514: mean=  0.25, rel. rms=   2.6, npeaks=   16, spikyness=  15.00, spikeexcess= 16.00   ['rms', 'spikeexcess']

    this will tell you that Antenna 17011092 was worked on (the 92nd
    antenna in the data file) and the 186th chunk (block 514)
    contained some suspicious data (too many spikes). If you want to
    inspect this, you can call

    Task.qplot(186)

    This will plot the chunk and highlight the first flagged block
    (#514) in that chunk.

    Task.qplot(186,1)

    would highlight the second flagged block (which does not exist here).

    If the chunks are too long to be entirely plotted, use

    Task.qplot(186,all=False).
        """
        quality_entry=self.quality[entry]
        filename=quality_entry["filename"]
        datafile=crfile(filename)
        iantenna=datafile["Antennas"].find(quality_entry["antenna"])
        datafile["selectedAntennas"]=[iantenna]
        if block<0 and flaggedblock<len(quality_entry["flaggedblocks"]):
            block=quality_entry["flaggedblocks"][flaggedblock]
            s="flaggedblock # "+str(flaggedblock)+"/"+str(len(quality_entry["flaggedblocks"])-1)
        else:
            s=""
        print "Filename:",filename,"block =",block,"blocksize =",quality_entry["blocksize"],s
        if all:
            datafile["blocksize"]=quality_entry["size"]
            datafile["block"]=quality_entry["offset"]*quality_entry["blocksize"]/quality_entry["size"]
            y0=datafile["Fx"]
            y0.par.xvalues=datafile["Time"]
            y0.par.xvalues.setUnit("mu","")
            y0.plot()
        datafile["blocksize"]=quality_entry["blocksize"]
        datafile["block"]=block
        y=datafile["Fx"]
        y.par.xvalues=datafile["Time"]
        y.par.xvalues.setUnit("mu","")
        y.plot(clf=not all)



"""
class test1(tasks.Task):
#    " ""
    Documentation of task - parameters will be added automatically
#    " ""
#    parameters = {"x":{default:None,doc:"x-value - a positional parameter",positional:True},"y":{default:2, doc:"y-value - a normal keyword parameter"},"xy":{default:lambda ws:ws.y*ws.x,doc:"Example of a derived parameter."}}
    parameters = {"x":p_(None,"x-value - a positional parameter",positional=True),"y":p_(2,"y-value - a normal keyword parameter"),"xy":p_(lambda ws:ws.y*ws.x,"Example of a derived parameter.")}
    def init(self):
        print "Calling optional initialization routine - Nothing to do here."
    def run(self):
        print "Calling Run Function."
        print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy

class test2(tasks.Task):
    " ""
    Documentation of task - parameters will be added automatically
    " ""
    def call(self,x,y=2,xy=lambda ws:ws.x*ws.y):
        pass
    def init(self):
        print "Calling optional initialization routine - Nothing to do here."
    def run(self):
        print "Calling Run Function."
        print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy

"""
