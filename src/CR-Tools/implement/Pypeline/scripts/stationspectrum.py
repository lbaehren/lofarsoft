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
import qualitycheck
from pycrtools import IO

import types

#plt.ioff()
#plt.clf()

#Defining a few keywords that are used for creating the parameters dict, spares one from using quotation marks
doc="doc"; default="default";unit="unit";workarray="workarray";dependencies="dependencies"; export="export"


def p_(default=None,doc="",unit="",**args):
    d={"default":default,"doc":doc,"unit":unit}
    if len(args)>0: d.update(args)
    return d


#------------------------------------------------------------------------
#Input values
#------------------------------------------------------------------------
datadir=os.environ['HOME']+'/data/saturn/'

#filename= LOFARSOFT+"/data/lofar/CS302C-B0T6:01:48.h5"
#filename= LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
#filenames=IO.getFilenames(datadir)
#------------------------------------------------------------------------

parameter_definitions={
    "lofarmode":{default:"LBA_OUTER",
                 doc:"Which LOFAR mode was used (HBA/LBA_OUTER/LBA_INNER)"},   

    "stride_n":p_(0,"if >0 then divide the FFT processing in n=2**stride_n blocks. This is slower but uses less memory."),

#    "stride_n":{default:0,
#                doc:"if >0 then divide the FFT processing in n=2**stride_n blocks. This is slower but uses less memory."},

    "delta_nu":{default:200,doc:"6 frequency resolution",unit:"Hz"},

    "blocklen":{default:2**10,doc:"15 The size of a block being read in."},

    "maxnantennas":{default:4,doc:"Maximum number of antennas to sum over,"},

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

    "datafile":{default:lambda ws:crfile(ws.filename),export:False,
                     doc:"Data file object pointing to raw data."},

    "spectrum_file":{default:lambda ws:ws.tmpfilename+"spec"+ws.tmpfileext,
                     doc:"Filename to store the final spectrum."},

    "quality_database_filename":{default:"qualitydatabase",
                     doc:"Root filename of log file containing the derived antenna quality values (uses .py and .txt extension)."},

    "start_frequency":{default:lambda ws:ws.freqs[0],
                       doc:"Start frequency of spectrum",unit:"Hz"},

    "end_frequency":{default:lambda ws:ws.freqs[-1],
                     doc:"End frequency of spectrum",unit:"Hz"},

    "delta_band":{default:lambda ws:(ws.end_frequency-ws.start_frequency)/ws.stride*2,
                  doc:"Frequency width of one section/band of spectrum",unit:"Hz"},

    "full_blocklen":{default:lambda ws:ws.blocklen*ws.stride,
                     doc:"Full block length",unit:"Samples"},

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
                 doc:"The number of antennas averaged"},

    "nchunks_max":{default:lambda ws:float(ws.datafile.filesize)/ws.fullsize,
                   doc:"Maximum number of spectral chunks to average"},

    "nchunks":{default:lambda ws:min(ws.datafile.filesize/ws.fullsize,ws.maxchunks),
               doc:"Number of spectral chunks that are averaged"},

#Now define all the work arrays used internally
    
    "frequencies":{workarray:True,dependencies:["subspeclen"],
       doc:"Frequencies for final spectrum (or part thereof if stride>1)",default:lambda ws:
       hArray(float,[ws.subspeclen/2],name="Frequency",units=("M","Hz"),header=ws.header)},

    "power":{workarray:True,dependencies:["subspeclen"],
       doc:"Frequencies for final spectrum (or part thereof if stride>1)",default:lambda ws:
       hArray(float,[ws.subspeclen],name="Spectral Power",xvalues=ws.frequencies,par=[("logplot","y")],header=ws.header)},

    "cdata":{workarray:True,dependencies:["nblocks","blocklen"],
       doc:"main input and work array",default:lambda ws:
       hArray(complex,[ws.nblocks,ws.blocklen],name="cdata",header=ws.header)},

    "cdataT":{workarray:True,dependencies:["blocklen","nblocks"],
       doc:"Secondary input and work array",default:lambda ws:
       hArray(complex,[ws.blocklen,ws.nblocks],name="cdataT",header=ws.header)},

#Note, that all the following arrays have the same memory als cdata and cdataT

    "tmpspecT":{workarray:True,dependencies:["stride","nblocks_section","blocklen"],
       doc:"Wrapper array for cdataT",default:lambda ws:
       hArray(ws.cdataT.vec(),[ws.stride,ws.nblocks_section,ws.blocklen],header=ws.header)},

    "tmpspec":{workarray:True,dependencies:["nblocks_section","full_blocklen"],
       doc:"Wrapper array for cdata",default:lambda ws:
       hArray(ws.cdata.vec(),[ws.nblocks_section,ws.full_blocklen],header=ws.header)},

    "specT":{workarray:True,dependencies:["full_blocklen","nblocks_section"],
       doc:"Wrapper array for cdataT",default:lambda ws:
       hArray(ws.cdataT.vec(),[ws.full_blocklen,ws.nblocks_section],header=ws.header)},

    "specT2":{workarray:True,dependencies:["stride","blocklen","nblocks_section"],
       doc:"Wrapper array for cdataT",default:lambda ws:
       hArray(ws.cdataT.vec(),[ws.stride,ws.blocklen,ws.nblocks_section],header=ws.header)},

    "spec":{workarray:True,dependencies:["blocklen","nblocks"],
       doc:"Wrapper array for cdata",default:lambda ws:
       hArray(ws.cdata.vec(),[ws.blocklen,ws.nblocks],header=ws.header)}
}

class ParameterWorkSpace(object):
    """
    This class holds all parameters, scratch arrays and vectors used
    by the various tasks. Hence this is the basic workspace in the
    memory.
    """
    def __init__(self,parameter_definitions={}):
        self.parameter_definitions={}
        self.parameterlist=set()
        self._parameterlist=set()
        self._default_parameter_definition={doc:"", unit:"", default:None, workarray:False, export:True}
        self._default_parameter_order=(default,doc,unit)
        self._known_methods=set()
        self._known_methods.update(set(dir(self)))
        self.add_parameters(parameter_definitions)
    def __getitem__(self,par):
        if hasattr(self,"_"+par):   # Return locally stored value
            return getattr(self,"_"+par) 
        elif self.parameter_definitions.has_key(par):
            if self.parameter_definitions[par].has_key(default):   #return default value or function
                f_or_val=self.parameter_definitions[par][default]
                if type(f_or_val)==types.FunctionType: setattr(self,"_"+par,f_or_val(self))
                else: setattr(self,"_"+par,f_or_val)
            else:
                print "ERROR in Workspace",self.__module__,": Parameter ", par,"does not have default values!"
            return getattr(self,"_"+par)
        else:
            print "ERROR in Workspace",self.__module__,": Parameter ", par,"not known!"
            return None
    def __setitem__(self,par,value):
        if hasattr(self,"_"+par):   #replace stored value
            setattr(self,"_"+par,value)
        else:
            setattr(self,"_"+par,value) # create new parameter with default parameters
            self.parameter_definitions[par]=self._default_parameter_definition
            self.add(par)
    def add_parameter_definition(self,p,v):
        self.parameter_definitions[p]=self._default_parameter_definition.copy()
        self.parameter_definitions[p].update(v) # then copy the ones explicitly provided 
        #    def add_property(self, name, *funcs):
        #        setattr(self.__class__, name, property(*funcs)) #property(getx, setx, delx, "I'm the property.")
    def delx(self,name):
        print "Deleting ",name
    def add_property(self, name, *funcs):
        setattr(self.__class__, name, property(*funcs)) #property(getx, setx, delx, "I'm the property.")
    def partuple_to_pardict(self,tup):
        """Converts a tuple of parameter description values into a
        properly formatted dict. If the tuple is shorter than default
        values are used.

        Example: partuple_to_pardict(self,(value,"Parameter description","MHz")) -> {"default":value,"doc":"Parameter description","unit":"MHz"}
        """
        pardict=self._default_parameter_definition.copy()
        for i in range(len(tup)): pardict[self._default_parameter_order[i]]=tup[i]
        return pardict
    def add_parameters(self,parlist):
        """
        This provides an easy interface to add a number of parameters, either as a list or as a dict with properties.

        ws.add_parameters(["par1","par2",...]) will simply add the parameters parN without documentation and default values

        ws.add_parameters([("par1",val1, doc1, unit1),(,"par2",...),...]) will add the parameters parN with the respective
        properties. The properties are assigned based on their position in the tuple:
            pos 0 = parmeter name
            pos 1 = default value
            pos 2 = doc string
            pos 3 = unit of values

        ws.add_parameters({"par1":{"default":val1,"doc":doc1,"unit":unit1},"par2":{...},...}) will add the parameters parN with the respective
        parameters.
        """
        if type(parlist)==dict:
            for p,v in parlist.items():
                self.add(p,**v)
        elif type(parlist)==list:
            for p in parlist:
                if type(p)==tuple:
                    self.add(p[0],**(self.partuple_to_pardict(p[1:])))
                else:
                    self.add(p)
    def add(self,name,**properties):
        """
        Add a new parameter to the workspace, providing additional
        information, such as documentation and default values. The
        named parameters describe properties of the parameters. If no
        named parameters are given default values are used and added.

        Example:
        ws.add(par,default=0,doc="Parameter documentation",unit="km/h")

        The default values can also be a function, which takes as
        argument the workspace itself,e.g.

        ws.add(par,default=lambda ws:ws["other_parameter"]+1,doc="This parameter is the value of another parameter plus one",unit="km/h")

        If another parameter is referenced it will be retrieved
        automatically, and set to a default value if necessary. This
        way one can recursively go through multiple parameters upon
        retrieval.
        """
        self._known_methods.add(name)
        self._known_methods.add("_"+name)
        self.parameterlist.add(name)
        self._parameterlist.add("_"+name)
        self.add_property(name,lambda ws:ws[name],lambda ws,x:ws.__setitem__(name,x),lambda ws:ws.delx(name),"This is parameter "+name)
        self.add_parameter_definition(name,properties)
    def get_internal_parameters(self):
        """
        Return a list that contains all method names that simply contain
        a value, but were not assigned through self.add(), i.e. which
        do not have a getter and setter function or any
        description. These are typically inetranl variables that are
        not well documented.
        """
        return set(dir(self)).difference(self._known_methods)
    def list_internal_parameters(self):
        """
        Return a string that contains all methods that simply contain
        a value, but were not assigned through self.add(), i.e. which
        do not have a getter and setter function or any
        description. These are typically inetranl variables that are
        not well documented.
        """
        s=""
        for p in self.get_internal_parameters():
            if hasattr(self,p): val=getattr(self,p)
            else: val="*UNDEFINED*"
            s+="{0:<15} = {1!r:20} \n".format(p,val) 
        s+="#-----------------------------------------------------------------------\n"
        return s
    def printall(self):
        """
        ws.printall()

        Print all parameters stored in the workspace including internal parameters.
        """
        print self.__repr__(True)
    def evalall(self):
        """
        Evaluates all parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.parameterlist: self[p]
    def parameters(self,internals=False,excludeworkarrays=True,excludenonexports=True):
        """
        Returns a python dictionary containing all the paramters and their values as key/value pairs.

        If internals = True all stored parameters are returned,
        including those not added by ws.add and which are typically
        only used for internal purposes.

        ws.paramters() -> {"par1":value1, "par2":value2,...}
        """
        pdict={}
        for p in self.parameterlist:
            if ((excludenonexports and self.parameter_definitions[p].has_key(export) and (not self.parameter_definitions[p][export])) or
                (excludeworkarrays and self.parameter_definitions[p].has_key(workarray) and self.parameter_definitions[p][workarray])):
                pass # do not return parameter since it is a work array or is explicitly excluded
            else:
                pdict[p]=self[p]
        if internals:
            for p in self.get_internal_parameters(): pdict[p]=self[p]
        return pdict
    def __repr__(self,internals=False):
        """
        String representation of a work space, listing all explicitly defined parameters.
        """
        s="\n#WorkSpace parameters:\n#-----------------------------------------------------------------------\n"
        s2=""
        for p,v in self.parameter_definitions.items():
            if hasattr(self,"_"+p): val=getattr(self,"_"+p)
            else: val="*UNDEFINED*"
            if (v.has_key(workarray)) and (v[workarray]):
                s2+="#{2:s}\n{0:<15} = {1!r:20}\n".format(p,val,v[doc])
            else:
                if (v[unit]==""): s+="{0:<15} = {1!r:20} #            # {2:s}\n".format(p,val,v[doc])
                else: s+="{0:<15} = {1!r:20} # {2:^10s} # {3:s}\n".format(p,val,v[unit],v[doc]) 
        s+="#-----------------------------------------------------------------------\n"
        if not s2=="": s+=s2+"#-----------------------------------------------------------------------\n"
        if internals: s+=self.list_internal_parameters()
        return s

class WorkSpace(ParameterWorkSpace):
    parameter_definitions2=parameter_definitions

ws=WorkSpace(parameter_definitions)



#crfile=IO.open(filenames)

ws.add_parameters(["antennas",("nspectraadded",0),"nspectraflagged"])

ws.header=ws.datafile.hdr
ws.freqs=ws.datafile["Frequency"]

ws.antennas=hArray(range(min(ws.datafile["nofAntennas"],ws.maxnantennas)))
ws.antennaIDs=ashArray(hArray(ws.datafile["AntennaIDs"])[ws.antennas])

ws.quality=[]
ws.antennacharacteristics={}

ws.delta_t=ws.datafile["sampleInterval"]
ws.fullsize_estimated=1./ws.delta_nu/ws.delta_t

ws.fullsize=ws.nblocks*ws.full_blocklen
ws.blocklen_section=ws.blocklen/ws.stride
ws.nsubblocks=ws.stride*ws.nblocks
ws.nblocks_section=ws.nblocks/ws.stride
ws.speclen=ws.fullsize/2+1
ws.dostride=(ws.stride>1)
ws.nspectraflagged=0

ws.delta_frequency=(ws.end_frequency-ws.start_frequency)/(ws.speclen-1.0)

ws.header.update(ws.parameters())

t0=time.clock(); print "Setting up."

#Delete arrays first, in case they were already existing, to make sure
#enough memory is available when re-running the script
#ws.tmpspecT=None
#ws.tmpspec=None
#ws.specT=None
#ws.specT2=None
#ws.spec=None
#ws.power=None
#ws.frequencies=None
#ws.cdata=None
#ws.cdataT=None

#Defining work arrays  


writeheader=True # used to make sure the header is written the first time
initialround=True

print "Time:",time.clock()-t0,"s for set-up."

t0=time.clock(); print "Reading in data and doing a double FFT."

ws.datafile["blocksize"]=ws.blocklen #Setting initial block size

t0=time.clock();
for antenna in ws.antennas:
    rms=0; mean=0; npeaks=0
    ws.datafile["selectedAntennas"]=[antenna]
    antennaID=ws.antennaIDs[antenna]
    print "#Antenna =",antenna,"( ID=",antennaID,")"
    for nchunk in range(ws.nchunks):
        print "#  Chunk ",nchunk,"/",ws.nchunks-1,". Reading in data and doing a double FFT."
        ofiles=[]; ofiles2=[]; ofiles3=[]
        for offset in range(ws.stride):
            print "#    Pass ",offset,"/",ws.stride-1,"Starting block=",offset+nchunk*ws.nsubblocks
            blocks=range(offset+nchunk*ws.nsubblocks,(nchunk+1)*ws.nsubblocks,ws.stride)            
            ws.cdata[...].read(ws.datafile,"Fx",blocks)
            ws.quality.append(qualitycheck.CRQualityCheckAntenna(ws.cdata,datafile=ws.datafile,normalize=True,blockoffset=offset+nchunk*ws.nsubblocks,observatorymode=ws.lofarmode))
            qualitycheck.CRDatabaseWrite(ws.quality_database_filename+".txt",ws.quality[-1])
            mean+=ws.quality[-1]["mean"]
            rms+=ws.quality[-1]["rms"]
            npeaks+=ws.quality[-1]["npeaks"]
            dataok=(ws.quality[-1]["nblocksflagged"]<=ws.maxblocksflagged)
            if not dataok:
                print "##Data flagged - not taking it."
                break
            #            print "Time:",time.clock()-t0,"s for reading."
            ws.cdataT.doublefft1(ws.cdata,ws.fullsize,ws.nblocks,ws.blocklen,offset)
            #            print "Time:",time.clock()-t0,"s for 1st FFT."
            if ws.dostride:
                ofile=ws.tmpfilename+str(offset)+"a"+ws.tmpfileext
                ofiles+=[ofile]
                ws.cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...
        #Now sort the different blocks together (which requires a transpose over passes/strides)
        print "Time:",time.clock()-t0,"s for 1st FFT now doing 2nd FFT."
        if dataok:
            ws.nspectraadded+=1
            for offset in range(ws.stride):
                if ws.dostride:
                    print "#    Offset",offset
                    ws.tmpspecT[...].readfilebinary(Vector(ofiles),Vector(int,ws.stride,fill=offset)*(ws.nblocks_section*ws.blocklen))
                    #This transpose it to make sure the blocks are properly interleaved
                    hTranspose(ws.tmpspec,ws.tmpspecT,ws.stride,ws.nblocks_section)
                ws.specT.doublefft2(ws.tmpspec,ws.nblocks_section,ws.full_blocklen)
                if ws.dostride:
                    ofile=ws.tmpfilename+str(offset)+"b"+ws.tmpfileext
                    ws.specT.writefilebinary(ofile)
                    ofiles2+=[ofile]
            print "Time:",time.clock()-t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."
            for offset in range(ws.nbands):
                if (ws.nspectraadded==1): # first chunk
                    ws.power.fill(0.0)
                else: #2nd or higher chunk, so read data in and add new spectrum to it
                    ws.power.readfilebinary(ws.spectrum_file,ws.subspeclen*offset)
                    ws.power*= (ws.nspectraadded-1.0)/(ws.nspectraadded)                        
                if ws.dostride:
                    print "#    Offset",offset
                    ws.specT2[...].readfilebinary(Vector(ofiles2),Vector(int,ws.stride,fill=offset)*(ws.blocklen*ws.nblocks_section))
                    hTranspose(ws.spec,ws.specT2,ws.stride,ws.blocklen) # Make sure the blocks are properly interleaved
                    if ws.nspectraadded>1: ws.spec/=float(ws.nspectraadded)   
                    ws.power.spectralpower(ws.spec)
                else: # no striding, data is all fully in memory
                    if ws.nspectraadded>1: ws.specT/=float(ws.nspectraadded)   
                    ws.power.spectralpower(ws.specT)
                if ws.stride==1: ws.power[0:ws.subspeclen/2].write(ws.spectrum_file,nblocks=ws.nbands,block=offset,writeheader=writeheader)
                else: ws.power.write(ws.spectrum_file,nblocks=ws.nbands,block=offset,writeheader=writeheader)
                writeheader=False
            print "#  Time:",time.clock()-t0,"s for processing this chunk. Number of spectra added =",ws.nspectraadded
        else: #data not ok
            ws.nspectraflagged+=1
            print "#  Time:",time.clock()-t0,"s for reading and ignoring this chunk.  Number of spectra flagged =",ws.nspectraflagged
    print "#  End of all chunks (Antenna =",antenna,") -------------------------------"
    if ws.nchunks>0:
        mean/=ws.nchunks
        rms/=ws.nchunks
        ws.antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":ws.quality[-ws.nchunks:]}
        print "#Antenna characteristics ",antennaID," =",{"mean":mean,"rms":rms,"npeaks":npeaks}
        f=open(ws.quality_database_filename+".py","a")
        f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(ws.antennacharacteristics[antennaID])+"\n")
        f.close()
        if ws.doplot:
            #rp(0,plotsubspectrum,markpeaks=False,clf=False)
            plt.draw()
print "##End of all Antennas - nspectraadded=",ws.nspectraadded,"nspectraflagged=",ws.nspectraflagged
print "Total time used:",time.clock()-t0,"s."
print "To read back the spectrum type: sp=hArrayRead('"+ws.spectrum_file+"')"

#Now update the header file again ....
ws.header.update(ws.parameters())
if ws.stride==1: ws.power.writeheader(ws.spectrum_file,nblocks=ws.nbands,dim=[ws.subspeclen/2])
else: ws.power.writeheader(ws.spectrum_file,nblocks=ws.nbands)

#----------------------

"""
parameters_mergeheader(parameters,power.par.hdr)

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


##cdata=hArray(float,[nblocks,blocklen]) # creating input and work array
##cdata.read(datafile,"Fx")
##cdata[0].plot()

#pa 0,141
"""
"""
