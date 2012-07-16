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

    def __init__(self, filename, chunk=0, block=0):
        """Constructor.
        """
        # Useful to do unit conversion
        self.__conversiondict={"":1,"kHz":1000,"MHz":10**6,"GHz":10**9,"THz":10**12}
        
        # Current chunk number
        self.__chunk = chunk

        # Current block number
        self.__block = block

        # How many beams are there
        self.__nofBeamDataSets = len(filename)

        # Open files
        self.__files = [cr.hArrayRead(file,block=0,ext='beam') for file in filename]
        
        self.__filename = filename
        
        #Create keyword dict for easy access
        self.__setKeywordDict()

        # Mark file as opened
        self.closed = False

    def __setKeywordDict(self):

        self.__keyworddict={
            # NON-ICD KEYWORDS
            "CHUNK":lambda: self.__chunk,
            "BLOCK":lambda: self.__block,
            "NOF_BEAM_DATASETS": lambda: self.__nofBeamDataSets
            }

        for par2 in self.__files[0].par.hdr.keys():
            if par2=='BeamFormer':        
                for par2 in self.__files[0].par.hdr['BeamFormer'].keys():
                    self.__keyworddict['BEAM_'+par2.upper()] = self.__files[0].par.hdr['BeamFormer'][par2]
            elif 'BEAM_' in par2:
                self.__keyworddict[par2.upper()] = self.__files[0].par.hdr[par2]             
            else:
                self.__keyworddict['TBB_'+par2.upper()] = self.__files[0].par.hdr[par2]

        self.__keyworddict['NCHUNKS'] = self.getNchunks()
        self.__keyworddict['FILENAMES'] = self.__filename
        self.__keyworddict['STATION_NAME'] = lambda: [self.__files[i].par.hdr['STATION_NAME'][0] for i in range(self.__nofBeamDataSets)]
        self.__keyworddict['MAXIMUM_READ_LENGTH'] = self['NCHUNKS']*self['BEAM_BLOCKLEN']*self['BEAM_NBLOCKS']
        self.__keyworddict['BLOCKSIZE'] = lambda: self['BEAM_BLOCKLEN']
        self.__keyworddict['TIME'] = self['TBB_TIME']
        self.__keyworddict['FREQUENCY_INTERVAL'] = [self['TBB_FREQUENCY_INTERVAL']]
        self.__keyworddict['SAMPLE_INTERVAL'] = self['TBB_SAMPLE_INTERVAL']

#        'BEAM_STATIONPOS'  add this here for all.
    
    setable_keywords=set(["CHUNK","BLOCK"])
    
    def __setitem__(self, key, value):
        """Set values to keywords if allowed. 
        """
        if key not in self.setable_keywords:
            raise KeyError("Invalid keyword '"+str(key)+"' - vailable keywords: "+str(list(self.setable_keywords)))        
        elif key is "CHUNK":
            self.__chunk = value
        elif key is "BLOCK":
            self.__block = value
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
             key=['FILENAMES','TBB_TIME_HR','DATA_LENGTH','DATA_LENGTH_TIME','STATION_NAME','NOF_BEAM_DATASETS']

        output ='[Beam Data] Summary of object properties' 
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
                s =  k+' '*(stringlength-len(k))+' : '+str(self['BEAM_BLOCKLEN']*self['NCHUNKS']*self['BEAM_NBLOCKS']*cr.asval(self["TBB_SAMPLE_INTERVAL"]))+' s'               
                if show: print s
                output += '\n'+s 
                continue
        
            if k == 'DATA_LENGTH' and not(verbose): 
                s =  k+' '*(stringlength-len(k))+' : '+str(self['NCHUNKS']*self['BEAM_NBLOCKS'])+' Blocks ( '+str(self['NCHUNKS'])+' Chunks, each of '+str(self['BEAM_NBLOCKS'])+' Blocks, each of '+str(self['BEAM_BLOCKLEN'])+' Samples) '
                if show: print s
                output += '\n'+s 
                continue
                     
            ss = k +' '*(stringlength-len(k))+' : '
            
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
        
    def getFrequencies(self):
        """Returns the frequencies that are applicable to the FFT data

        Output:
           This method returns a FloatVector with the selected frequencies in Hz.
        """
        
        return self['BEAM_FREQUENCIES']

    def getNchunks(self):
        """Find the maximum number of blocks beetwen the beams. Analogue to MAXIMUM_READ_LENGTH in tbbs.
        """
        
        all_nblocks = []
        
        for nbeam in range(self.__nofBeamDataSets):
            all_nblocks.append(self.__files[nbeam].par.nblocks)
        
        nblocks = min(all_nblocks)

        return nblocks

    def getITRFAntennaPositions(self):
        """Returns ITRF Station positions.

        Output:
        a one dimensional array containing the Cartesian position of
        each antenna in meters in IRTF coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

#        return self['BEAM_STATIONPOS']

        raise NotImplementedError


    def getRelativeAntennaPositions(self):
        """Returns relative Station positions.

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """
        

        pos=[]
        
        for file in self.__files:
            pos+=list(file.par.hdr['BeamFormer']['stationpos'])
        
        st_pos = cr.hArray(float,[self['NOF_BEAM_DATASETS'],3],pos)        
        
        return st_pos
        
    def getTimeData(self,data=None,chunk=-1):
        """Calculate time axis depending on sample frequency and
        number of blocks in a chunk. Create a new array, if
        none is provided, otherwise put data into array.
        """
        raise NotImplementedError

        if not data:
            data = cr.hArray(float,self['BEAM_NBLOCKS'])

        chunk=cr.asval(chunk)

        if chunk<0:
            chunk=self.__chunk
        else:
            self.__chunk=chunk

        data.fillrange(self['CHUNK']*self['BEAM_BLOCKLEN']*self['BEAM_NBLOCKS']*cr.asval(self["TBB_SAMPLE_INTERVAL"]),self['BEAM_BLOCKLEN']*cr.asval(self["TBB_SAMPLE_INTERVAL"]))

        return data

    def getFFTData(self, data, block):

        """Writes FFT data for selected stations to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of bock to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected stations and
        for the selected frequencies (all freqs and stations for now).
        
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.

        """

        block=cr.asval(block)

        if block<0:
            block=self.block
        else:
            self.__block=block

        dat = []
        
        for file in self.__filename:
            beam = cr.hArray(complex,self['BEAM_SPECLEN'])            
            beam.readfilebinary(os.path.join(file,"data.bin"),self['BLOCK']*self['BEAM_SPECLEN']*self['BEAM_NBLOCKS']*self[ 'BEAM_NBEAMS'])
            dat+=list(cr.hArray(complex,beam,beam))

        data = cr.hArray(complex,[self['NOF_BEAM_DATASETS'],self['BEAM_SPECLEN']],dat)
        print 'data',data

        return data

    def getTimeseriesData(self, data=None, chunk=-1):
        """Returns timeseries data for selected antennas.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write timeseries data to.
        *chunk*       index of chunk to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the timeseries data of the
        specified chunk for each of the selected antennae.
        So that if `a` is the returned array `a[i]` is an array of
        length blocksize of antenna i.

        """

        raise NotImplementedError

        chunk=cr.asval(chunk)

        if chunk<0:
            chunk = self.__chunk
        else:
            self.__chunk = chunk
            
        data = self.getFFTData(chunk=chunk)
        
        time_series_list = [ ]

        for i in range(self['NOF_BEAM_DATASETS']):
            time_series = cr.hArray(float,[self['BEAM_NBLOCKS'],self['BEAM_BLOCKLEN']])
            cr.hInvFFTw(time_series[...],data[i][...])
            time_series_list.append(time_series)

        return time_series_list

    def empty(self, key):
        """Return empty array for keyword data.
        Known keywords are: "TIMESERIES_DATA", "TIME_DATA", "FREQUENCY_DATA", "FFT_DATA".
        """

        if key == "TIMESERIES_DATA":
            return cr.hArray(float, dimensions=(self['NOF_BEAM_DATASETS'], self['BEAM_BLOCKLEN']),name="E-Field(t)",units=("","Counts"))
        elif key == "TIME_DATA":
            return cr.hArray(float, self["BLOCKSIZE"],name="Time",units=("","s"))
        elif key == "FREQUENCY_DATA":
            return cr.hArray(float, self['BEAM_SPECLEN'],name="Frequency",units=("","Hz"))
        elif key == "FFT_DATA":
            return cr.hArray(complex, dimensions=(self['NOF_BEAM_DATASETS'], self['BEAM_SPECLEN']),name="fft(E-Field)",xvalues=self.getFrequency(),logplot="y")
        else:
            raise KeyError("Unknown key: " + str(key))

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

    if not isinstance(filename, list):
        filename = [filename]
    
    for file in filename:
        if not os.path.isdir(file):
            raise IOError("No such directory: "+file)

    return BeamData(filename, *args, **kwargs)

