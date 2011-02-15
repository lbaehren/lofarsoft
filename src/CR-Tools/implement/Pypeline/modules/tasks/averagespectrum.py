"""Spectrum documentation.

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

#import pdb
#pdb.set_trace()

# What is exported by default
#__all__ = ['averagespectrum',"avspec","WorkSpace"]

#from pycrtools.tasks import Task, TaskLauncher
#import pycrtools.tasks
import tasks
from pycrtools import *
from tshortcuts import *
import time
import qualitycheck
from pycrtools import IO

#Defining the workspace parameters

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

class WorkSpace(tasks.WorkSpace("averagespectrum")):
    parameters = {

	"datafile":{default:lambda ws:crfile(ws.filename),export:False,
		    doc:"Data file object pointing to raw data."},

	"lofarmode":{default:"LBA_OUTER",
		     doc:"Which LOFAR mode was used (HBA/LBA_OUTER/LBA_INNER)"},   

	"stride_n":p_(0,"if >0 then divide the FFT processing in n=2**stride_n blocks. This is slower but uses less memory."),

	"delta_nu":{default:200,doc:"6 frequency resolution",unit:"Hz"},

	"blocklen":{default:2**10,doc:"15 The size of a block being read in."},

	"maxnantennas":{default:1,doc:"Maximum number of antennas to sum over,"},

	"maxchunks":{default:1,doc:"Maximum number of chunks of raw data to integrate spectrum over."},

	"maxblocksflagged":{default:2,doc:"Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."},
    
	"doplot":{default:False,doc:"Plot current spectrum while processing."},
    
	"stride":{default:lambda ws:2**ws.stride_n,
		  doc:"If stride>1 divide the FFT processing in n=stride blocks."},
    
	"tmpfileext":{default:".dat",
		      doc:"Extension of filename for temporary data files (e.g., used if stride>1.)"},
    
	"tmpfilename":{default:"tmp",
		       doc:"Root filename for temporary date files."},

	"filename":{default:lambda ws:LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5",
		    doc:"Filename of data file to read raw data from."},

	"spectrum_file":{default:lambda ws:ws.tmpfilename+"spec"+ws.tmpfileext,
			 doc:"Filename to store the final spectrum."},

	"quality_db_filename":{default:"qualitydatabase",
				     doc:"Root filename of log file containing the derived antenna quality values (uses .py and .txt extension)."},

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
		   doc:"Number of blocks"},

	"nbands":{default:lambda ws:(ws.stride+1)/2,
		  doc:"Number of sections/bands in spectrum"},
    
	"subspeclen":{default:lambda ws:ws.blocklen*ws.nblocks,
		      doc:"Size of one section/band of the final spectrum"},
    
	"nsamples_data":{default:lambda ws:float(ws.fullsize)/10**6,
			 doc:"Number of samples in raw antenna file",unit:"MSamples"},
    
	"size_data":{default:lambda ws:float(ws.fullsize)/1024/1024*16,
		     doc:"Number of samples in raw antenna file",unit:"MBytes"},

	"nantennas":{default:lambda ws:len(ws.antennas),
		     doc:"The actual number of antennas averaged (set through maxantennas)."},

	"nchunks_max":{default:lambda ws:float(ws.datafile.filesize)/ws.fullsize,
		       doc:"Maximum number of spectral chunks to average"},

	"nchunks":{default:lambda ws:min(ws.datafile.filesize/ws.fullsize,ws.maxchunks),
		   doc:"Number of spectral chunks that are averaged"},

	"nspectraadded":p_(lambda ws:0,"Number of spectra added at the end.",output=True),
    
	"nspectraflagged":p_(lambda ws:0,"Number of spectra flagged and not used.",output=True),
	
	"antennas":p_(lambda ws:hArray(range(min(ws.datafile["nofAntennas"],ws.maxnantennas))),"Antennas to be used"),
	
	"antennaIDs":p_(lambda ws:ashArray(hArray(ws.datafile["AntennaIDs"])[ws.antennas]),"Antenna IDs used in calculation."),


        "delta_t":p_(lambda ws:ws.datafile["sampleInterval"]),
        "fullsize_estimated":p_(lambda ws:1./ws.delta_nu/ws.delta_t),
        "blocklen_section":p_(lambda ws:ws.blocklen/ws.stride),
        "nsubblocks":p_(lambda ws:ws.stride*ws.nblocks),
        "nblocks_section":p_(lambda ws:ws.nblocks/ws.stride),
        "speclen":p_(lambda ws:ws.fullsize/2+1),
        "delta_frequency":p_(lambda ws:(ws.end_frequency-ws.start_frequency)/(ws.speclen-1.0)),
	"header":p_(lambda ws:ws.datafile.hdr,"Header of datafile",export=False),
	"freqs":p_(lambda ws:ws.datafile["Frequency"]),


#Now define all the work arrays used internally
    
#	"frequencies":{workarray:True,
#		       doc:"Frequencies for final spectrum (or part thereof if stride>1)",default:lambda ws:
#		       hArray(float,[ws.subspeclen/2],name="Frequency",units=("M","Hz"),header=ws.header)},

	"power":{workarray:True,
		 doc:"Frequencies for final spectrum (or part thereof if stride>1)",default:lambda ws:
		 hArray(float,[ws.subspeclen],name="Spectral Power",par=[("logplot","y")],header=ws.header)},
#		 hArray(float,[ws.subspeclen],name="Spectral Power",xvalues=ws.frequencies,par=[("logplot","y")],header=ws.header)},
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

class averagespectrum(tasks.Task):
    """class documentation.
    """
    WorkSpace = WorkSpace
    
    def init(self):

        """
	Initialize the task
        """

        # Start initialization

        self.ws.quality=[]
        self.ws.antennacharacteristics={}

        self.ws.dostride=(self.ws.stride>1)
        self.ws.nspectraflagged=0

        self.ws.header.update(self.ws.getParameters())

    def run(self):
        """Run the program.
        """
        #crfile=IO.open(filenames)

        #print "Time:",time.clock()-self.ws.t0,"s for set-up."

        self.ws.t0=time.clock() #; print "Reading in data and doing a double FFT."

        self.ws.datafile["blocksize"]=self.ws.blocklen #Setting initial block size

        writeheader=True # used to make sure the header is written the first time
        initialround=True

	npass = self.ws.nchunks*self.ws.stride
        self.ws.t0=time.clock();
	for iantenna in range(self.nantennas):
	    antenna=self.ws.antennas[iantenna]
            rms=0; mean=0; npeaks=0
            self.ws.datafile["selectedAntennas"]=[antenna]
            antennaID=self.ws.antennaIDs[iantenna]
            print "# Start antenna =",antenna,"(ID=",str(antennaID)+") -",npass,"passes:"
            for nchunk in range(self.ws.nchunks):
		#if self.ws.nchunks>1: sys.stdout.write("*")
                #print "#  Chunk ",nchunk,"/",self.ws.nchunks-1,". Reading in data and doing a double FFT."
                ofiles=[]; ofiles2=[]; ofiles3=[]
                for offset in range(self.ws.stride):
		    #if self.ws.stride>1: sys.stdout.write(".")
                    #print "#    Pass ",offset,"/",self.ws.stride-1,"Starting block=",offset+nchunk*self.ws.nsubblocks
                    blocks=range(offset+nchunk*self.ws.nsubblocks,(nchunk+1)*self.ws.nsubblocks,self.ws.stride)            
                    self.ws.cdata[...].read(self.ws.datafile,"Fx",blocks)
                    self.ws.quality.append(qualitycheck.CRQualityCheckAntenna(self.ws.cdata,datafile=self.ws.datafile,normalize=True,blockoffset=offset+nchunk*self.ws.nsubblocks,observatorymode=self.ws.lofarmode))
                    qualitycheck.CRDatabaseWrite(self.ws.quality_db_filename+".txt",self.ws.quality[-1])
                    mean+=self.ws.quality[-1]["mean"]
                    rms+=self.ws.quality[-1]["rms"]
                    npeaks+=self.ws.quality[-1]["npeaks"]
                    dataok=(self.ws.quality[-1]["nblocksflagged"]<=self.ws.maxblocksflagged)
                    if not dataok:
                        print " # Data flagged!"
                        break
                    #            print "Time:",time.clock()-self.ws.t0,"s for reading."
                    self.ws.cdataT.doublefft1(self.ws.cdata,self.ws.fullsize,self.ws.nblocks,self.ws.blocklen,offset)
                    #            print "Time:",time.clock()-self.ws.t0,"s for 1st FFT."
                    if self.ws.dostride:
                        ofile=self.ws.tmpfilename+str(offset)+"a"+self.ws.tmpfileext
                        ofiles+=[ofile]
                        self.ws.cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...
                #Now sort the different blocks together (which requires a transpose over passes/strides)
                #print "Time:",time.clock()-self.ws.t0,"s for 1st FFT now doing 2nd FFT."
                if dataok:
                    self.ws.nspectraadded+=1
                    for offset in range(self.ws.stride):
                        if self.ws.dostride:
                            #print "#    Offset",offset
                            self.ws.tmpspecT[...].readfilebinary(Vector(ofiles),Vector(int,self.ws.stride,fill=offset)*(self.ws.nblocks_section*self.ws.blocklen))
                            #This transpose it to make sure the blocks are properly interleaved
                            hTranspose(self.ws.tmpspec,self.ws.tmpspecT,self.ws.stride,self.ws.nblocks_section)
                        self.ws.specT.doublefft2(self.ws.tmpspec,self.ws.nblocks_section,self.ws.full_blocklen)
                        if self.ws.dostride:
                            ofile=self.ws.tmpfilename+str(offset)+"b"+self.ws.tmpfileext
                            self.ws.specT.writefilebinary(ofile)
                            ofiles2+=[ofile]
		    #print "Time:",time.clock()-self.ws.t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
                    for offset in range(self.ws.nbands):
                        if (self.ws.nspectraadded==1): # first chunk
                            self.ws.power.fill(0.0)
                        else: #2nd or higher chunk, so read data in and add new spectrum to it
                            self.ws.power.readfilebinary(self.ws.spectrum_file,self.ws.subspeclen*offset)
                            self.ws.power*= (self.ws.nspectraadded-1.0)/(self.ws.nspectraadded)                        
                        if self.ws.dostride:
                            #print "#    Offset",offset
                            self.ws.specT2[...].readfilebinary(Vector(ofiles2),Vector(int,self.ws.stride,fill=offset)*(self.ws.blocklen*self.ws.nblocks_section))
                            hTranspose(self.ws.spec,self.ws.specT2,self.ws.stride,self.ws.blocklen) # Make sure the blocks are properly interleaved
                            if self.ws.nspectraadded>1: self.ws.spec/=float(self.ws.nspectraadded)   
                            self.ws.power.spectralpower(self.ws.spec)
                        else: # no striding, data is all fully in memory
                            if self.ws.nspectraadded>1: self.ws.specT/=float(self.ws.nspectraadded)   
                            self.ws.power.spectralpower(self.ws.specT)
                        if self.ws.stride==1: self.ws.power[0:self.ws.subspeclen/2].write(self.ws.spectrum_file,nblocks=self.ws.nbands,block=offset,writeheader=writeheader)
                        else: self.ws.power.write(self.ws.spectrum_file,nblocks=self.ws.nbands,block=offset,writeheader=writeheader)
                        writeheader=False
                    #print "#  Time:",time.clock()-self.ws.t0,"s for processing this chunk. Number of spectra added =",self.ws.nspectraadded
                else: #data not ok
                    self.ws.nspectraflagged+=1
                    #print "#  Time:",time.clock()-self.ws.t0,"s for reading and ignoring this chunk.  Number of spectra flagged =",self.ws.nspectraflagged
            if self.ws.nchunks>0:
                mean/=self.ws.nchunks
                rms/=self.ws.nchunks
                self.ws.antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":self.ws.quality[-self.ws.nchunks:]}
		l={"mean":mean,"rms":rms,"npeaks":npeaks}
                f=open(self.ws.quality_db_filename+".py","a")
                f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(self.ws.antennacharacteristics[antennaID])+"\n")
                f.close()
                if self.ws.doplot:
                    #rp(0,plotsubspectrum,markpeaks=False,clf=False)
                    plt.draw()
	    else: l=""
	    print "# End   antenna =",antenna," Time =",time.clock()-self.ws.t0,"s  nspectraadded =",self.ws.nspectraadded,"nspectraflagged =",self.ws.nspectraflagged,l
        print "Finished - total time used:",time.clock()-self.ws.t0,"s."
        print "To read back the spectrum type: sp=hArrayRead('"+self.ws.spectrum_file+"')"

        #Now update the header file again ....
        self.ws.header.update(self.ws.getParameters())
        if self.ws.stride==1: self.ws.power.writeheader(self.ws.spectrum_file,nblocks=self.ws.nbands,dim=[self.ws.subspeclen/2])
        else: self.ws.power.writeheader(self.ws.spectrum_file,nblocks=self.ws.nbands)

# Create a TaskLauncher
#averagespectrum = TaskLauncher(AverageSpectrum)
#avspec = TaskLauncher(AverageSpectrum)
 
class ttest(tasks.Task):
    parameters = {"f":p_(lambda ws:ws.y*ws.z,"Example of a derived parameter."),"y":{doc:"y-Parameter"}}
    def init(self):
	print "Calling optional initialization routine"
	print self.ws
    def call(self,x,y=1,z=2):
	pass
    def run(self):
	print "Printing WorkSpace containing parameters"
	print self.ws
	print "Printing available parameters"
	print "self.x=",self.x,"self.y=",self.y,"self.z=",self.z,"self.f=",self.f
	print "Printing WorkSpace containing parameters again"
	print self.ws

#ttest = TaskLauncher(TTest)	
