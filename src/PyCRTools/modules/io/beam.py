"""
This module implements the primary interface for reading complex beam data, which is the product of beamform.py

It contains one function `open` that is used to open a binary file containing complex beamformed data and returns a :class:`~BeamData` file object.

.. moduleauthor:: Emilio Enriquez <E.Enriquez@astro.ru.nl>


"""

import os
import numpy as np
import pycrtools as cr
from pycrtools import metadata as md

import pdb;# pdb.set_trace()

# This class implements the IO interface
from interfaces import IOInterface

class BeamData(IOInterface):
    """This class provides an interface to single file beamformed data.
    """

    def __init__(self, filename, block=0, nblocks=1):
        """Constructor.
        """
        # Useful to do unit conversion
        self.__conversiondict={"":1,"kHz":1000,"MHz":10**6,"GHz":10**9,"THz":10**12}
        
        # Current block number
        self.__block = block

        # Current number of blocks (in a chunk).
        self.__nblocks = nblocks

        # Open files
 #       self.__files, self.__filenames ,self.__Stations  = open_all(filename)
        
#        self.file. cr.hArrayRead(filename,block=0)
        
        self.__filename = filename
        
        #Create keyword dict for easy access
        self.__setKeywordDict()

        # Mark file as opened
        self.closed = False

    def __setKeywordDict(self):

        self.__keyworddict={}
        
        for par in self.__files[0].par.hdr.keys():
            self.__keyworddict[par] = self.__files[0].par.hdr[par]
            
        self.__keyworddict.update({
            #OTHER KEYWORDS
            "STATIONS":self.__Stations,
            "FILENAME":self.__filename,
            # DERIVED KEYWORDS
            "NBLOCKS":lambda: self.__nblocks,  #NOTE: the value used in beamformer is .... self.__files.par.hdr['BeamFormer']['nblocks'] .... NBLOCKS = number of blocks per chunk (more info in beamformer.py)
            "BLOCK":lambda: self.__block,
#            "FFT_DATA":lambda:(lambda x:x if self.getFFTData(x) else x)(self.empty("FFT_DATA")), 
            "TIME_DATA":self.getTimeData,
            "FREQUENCY_DATA":self.getFrequencies(),
            
            
            "erase":"yup"
            })

    setable_keywords=set(["BLOCK","NBLOCKS"])
    
    def __setitem__(self, key, value):
        if key not in self.setable_keywords:
            raise KeyError("Invalid keyword '"+str(key)+"' - vailable keywords: "+str(list(self.setable_keywords)))        
        elif key is "BLOCK":
            self.__block = value
        elif key is "NBLOCKS":
            self.__nblocks = value            
        else:
            raise KeyError(str(key) + " cannot be set. Available keywords: "+str(list(self.setable_keywords)))

    def __getitem__(self, *keys):
        """Implements keyword access.
        """
        #If multiple keywords are provided, return a list of results
        if type(keys[0])==tuple:
            return [self[k] for k in keys[0]]
        else:
            key=keys[0]

        if key not in self.keys():
            raise KeyError("Invalid keyword: "+key)
        else:
            if hasattr(self.__keyworddict[key],"__call__"):
                return self.__keyworddict[key]()
            else:
                return self.__keyworddict[key]

    def __contains__(self, key):
        """Allows inquiry if key is implemented.
        """
        
        return key in self.keys()

    def __repr__(self):
        """Display summary when printed.
        """
        return self.info(False, False)

    def info(self, verbose=True, show=True):
        """Display some information about the file. Short and long versions (verbose=False/True)
        """
        #Selecting a subset of keys.
        if verbose:
            key=self.keys()
            key = [k for k in key if 'EMPTY' not in k]
            key.sort()
        else:
            key=['FILENAME','STATIONS']

        output ='[Beamformed Data Object] Summary of object properties' 
        if show: print output.strip()

        #For the print out format.
        maxii=0
        for i in range(len(key)):
            maxii=max([maxii,len(key[i])])
        stringlength = maxii + 5
        und = ''

        #Loop over the selected keys.
        for k in key:
            s = ""
            if k == 'DATA_LENGTH_TIME' and not(verbose): 
                s =  k +' '*(stringlength-len(k))+' : '+str(self['DATA_LENGTH'][0]*self['SAMPLE_INTERVAL'][0])+' s'               
                if show: print s
                output += '\n'+s 
                continue
        
            ss = k +' '*(stringlength-len(k))+' : '
            
            if k == 'DATA_LENGTH' and not(verbose): 
                s =  ss+str(self[k][0])+' Samples ( '+str(self[k][0]/self['BLOCKSIZE'])+' BLOCKS, each of '+str(self['BLOCKSIZE'])+' Samples) '    
                if show: print s
                output += '\n'+s 
                continue
            if k == 'NOF_DIPOLE_DATASETS' and not(verbose):
                s =  ss+str(self[k])+'  ( '+str(self['NOF_SELECTED_DATASETS'])+'  NOF_SELECTED_DATASETS ) '               
                if show: print s
                output += '\n'+s 
                continue
            
            try:
                if type(self[k])==type([0,0]) and len(self[k])>7 :
                    if all(x == self[k][0] for x in self[k]):
                        if verbose:
                            s =  ss+'[ '+str(self[k][0])+', ...] x'+ str(len(self[k]))+' with '+str(type(self[k][0]))
                        else:
                            s =  ss+str(self[k][0])               
                    else:
                        s = ss+str(self[k][:3]+['...']+self[k][-3:])
                else:
                    if isinstance(self[k],(cr.core.hftools._hftools.ComplexArray,cr.core.hftools._hftools.IntArray,cr.core.hftools._hftools.BoolArray,cr.core.hftools._hftools.FloatArray,cr.core.hftools._hftools.StringArray)):
                        s = ss + str(self[k].__repr__(maxlen=10))
                    else:    
                        if self[k]=='UNDEFINED':
                            und += k+' , '
                            continue
                        else:
                            s = ss + str(self[k])                       
                if show: print s
                output += '\n'+s
                
            except IOError:
                pass

        if len(und) > 0:
            s = 'These keywords are UNDEFINED : ['+str(und)+']'
            if show: print s
            output += s

        if not show:
            return output.strip()

    def keys(self,excludedata=False):
        """Returns list of valid keywords.
        """
        return [k for k in self.__keyworddict.keys() if not k[-5:]=="_DATA"] if excludedata else self.__keyworddict.keys()
        
    def items(self,excludedata=False):
        """Return list of keyword/content tuples of all header variables
        """

        lst = []

        for k in self.keys(excludedata):
            try:
                lst.append((k,self.__keyworddict[k]() if hasattr(self.__keyworddict[k],"__call__") else self.__keyworddict[k]))
            except IOError:
                pass

        return lst

    def getHeader(self):
        """Return a dict with keyword/content pairs for all header variables."""
        return dict(self.items(excludedata=True))

    def getRelativeAntennaPositions(self):
        """Returns relative antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """
                
    def getClockOffset(self):
        """Return clock offset.
        """

#        return [md.getClockCorrection(s) for s in self["STATION_NAME"]]
                
    def getFrequencies(self):
        """Returns the frequencies that are applicable to the FFT data

        Output:
        This method returns a FloatVector with the selected frequencies
        in Hz.
        """
        
        return self.__files[0].par.xvalues
        
    def getTimeData(self,data=None,block=-1):
        """Calculate time axis depending on sample frequency and
        blocksize (and later also time offset). Create a new array, if
        none is provided, otherwise put data into array.
        """

#        if not data:
#            data = self.empty("TIME_DATA")

        block=cr.asval(block)

        if block<0:
            block=self.__block
        else:
            self.__block=block

#        data.fillrange(self["BLOCK"]*self["BLOCKSIZE"]*cr.asval(self["SAMPLE_INTERVAL"]),cr.asval(self["SAMPLE_INTERVAL"]))
#        return data
        
    def getFFTData(self, data, block):
        """Writes FFT data for selected antennas to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of block to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected antennae and
        for the selected frequencies.
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.

        """  
        block=cr.asval(block)

        if block<0:
            block=self.__block
        else:
            self.__block=block

#        data.readfilebinary(os.path.join(self["BEAM_FILENAME"],"data.bin"),block* self["SPECLEN"]*self["NBLOCKS"])   #NOTE : Will need to edit this to be able to open several blocks at a time.
        data.readfilebinary(os.path.join(self["BEAM_FILENAME"],"data.bin"),block*self["FFTSIZE"])

    def getReferencePosition(self):
        """Returns reference position used for antenna position
        coordinate system.

        Output:
        a FloatVector with (lon,lat,height) in (rad,rad,m) of the WGS84
        position of the center used for the antenna position coordinate
        system.
        """
        
        if self.__files.par.hdr['BeamFormer']['phase_center'] == self.__files.par.hdr['ANTENNA_POSITIONS'][0]:
            pass
        else:
            return self.__files.par.hdr['BeamFormer']['phase_center']
        
               
    def close(self):
        """Closes file and sets the data attribute `.closed` to
        True. A closed object can no longer be used for I/O operations.
        `close()` may be called multiple times without error.
        """

        # Call file object destructor which should close the file
        del self.__files

        self.closed = True


def open(filename, *args, **kwargs):
    """Open file(s) with beamformed data.
    """

    if not os.path.isdir(filename):
        raise IOError("No such directory: "+filename)
    else:
        return BeamData(filename, *args, **kwargs)

def open_all(filename,polarization='pol0', *args, **kwargs):
    """This function opens multiple files.

        TO ADD:
         - A selection of stations/polarizations. 
         - (Maybe) aligns stations by setting the shift argument correctly
    """

    Stations = os.listdir(filename)

    mask = np.ones(len(Stations),dtype=bool)
    for i,Station in enumerate(Stations):
        if not os.path.isdir(Station) or len(Station)!=5:
            mask[i] = False

    Stations = [ item for item, flag in zip( Stations, mask ) if flag == 1 ]
    
    filenames = Stations
    for i,filenaam in enumerate(filenames):
        filenames[i] = os.path.join(filename,filenaam,polarization)
    
    files=[]
    for filename in filenames:
        if os.path.isfile(filename):
            files.append(cr.hArrayRead(filename,block=0))
        else:
            print "File",filename,"could not be opened. Trying next file ..."
    
    return files,filenames,Stations

