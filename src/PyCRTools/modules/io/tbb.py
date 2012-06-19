"""This module implements the primary interface for reading LOFAR TBB data.

It contains one function `open` that is used to open an HDF5 file containing LOFAR TBB data and returns a :class:`~TBBData` file object.

.. moduleauthor:: Pim Schellart <P.Schellart@astro.ru.nl>

This module assumes that correct ICD specified values are set for the folowing attributes:

* ANTENNA_SET
* ANTENNA_POSITION
* ANTENNA_POSITION_UNIT
* NYQUIST_ZONE

If this is not true for your files use the ``fix_metadata.py`` script to fix this.

"""

import os
import numpy as np
import pycrtools as cr
from datetime import datetime
from pycrtools import metadata as md

# This class implements the IO interface
from interfaces import IOInterface

class TBBData(IOInterface):
    """This class provides an interface to single file Transient Buffer
    Board data.
    """

    def __init__(self, filename, blocksize=1024, block=0):
        """Constructor.
        """
        # Useful to do unit conversion
        self.__conversiondict={"":1,"kHz":1000,"MHz":10**6,"GHz":10**9,"THz":10**12}
        
        # Store blocksize for readout
        self.__blocksize = blocksize

        # Current block number
        self.__block = block

        # Open file
        self.__file = cr.TBBData(filename)

        # Get a list of antenna IDs for which data is stored in the file
        self.__dipoleNames= self.__file.dipoleNames()

        # How many dipoles are there
        self.__nofDipoleDatasets = self.__file.nofDipoleDatasets()

        # Select all antennas by default
        self.__selectedDipoles = self.__file.dipoleNames()

        # Find reference antenna
        self.__refAntenna = self.__file.alignment_reference_antenna()

        #Create keyword dict for easy access
        self.__setKeywordDict()

        # Selection dependent initialization
        self.__initSelection()

        # Mark file as opened
        self.closed = False

    def __setKeywordDict(self):

        self.__keyworddict={
            # NON-ICD KEYWORDS
            "ALIGNMENT_REFERENCE_ANTENNA":self.__file.alignment_reference_antenna,
            "RELATIVEANTENNA_POSITIONS":self.getRelativeAntennaPositions,
            "ITRFANTENNA_POSITIONS":self.getITRFAntennaPositions,
            "ANTENNA_POSITIONS":self.getRelativeAntennaPositions,
            "TIMESERIES_DATA":lambda:(lambda x:x if self.getTimeseriesData(x) else x)(self.empty("TIMESERIES_DATA")), 
            "TIME_DATA":self.getTimeData,
            "FREQUENCY_DATA":self.getFrequencies,
            "FFT_DATA":lambda:(lambda x:x if self.getFFTData(x) else x)(self.empty("FFT_DATA")), 
            "EMPTY_TIMESERIES_DATA":lambda:self.empty("TIMESERIES_DATA"),
            "EMPTY_FFT_DATA":lambda:self.empty("FFT_DATA"),
            "SAMPLE_FREQUENCY":lambda:[v*self.__conversiondict[u] for v,u in zip(self.__file.sample_frequency_value(),self.__file.sample_frequency_unit())],
            "SAMPLE_INTERVAL":lambda:[1/(v*self.__conversiondict[u]) for v,u in zip(self.__file.sample_frequency_value(),self.__file.sample_frequency_unit())],
            "FREQUENCY_INTERVAL":lambda:[v*self.__conversiondict[u]/self["BLOCKSIZE"] for v,u in zip(self.__file.sample_frequency_value(),self.__file.sample_frequency_unit())],
            "FREQUENCY_RANGE":lambda:[(f/2*(n-1),f/2*n) for f,n in zip(self["SAMPLE_FREQUENCY"],self["NYQUIST_ZONE"])],
            "STATION_NAME":lambda:[md.idToStationName(i / 1000000) for i in self["CHANNEL_ID"]],
            "FFTSIZE":lambda:self["BLOCKSIZE"]/2+1,
            "BLOCKSIZE":lambda: self.__blocksize,
            "BLOCK":lambda: self.__block,
            "MAXIMUM_READ_LENGTH":self.__file.maximum_read_length(self.__refAntenna),
            "TIME_HR":lambda:[str(datetime.utcfromtimestamp(t)) for t in self["TIME"]],
            "SELECTED_DIPOLES_INDEX":lambda:[i for i, n in enumerate(self["DIPOLE_NAMES"]) if n in self["SELECTED_DIPOLES"]],

            # ICD KEYWORDS
            "FILENAME":self.__file.filename,
            "ANTENNA_SET":lambda:self.antenna_set if hasattr(self,"antenna_set") else self.__file.antenna_set(),
            "ANTENNA_POSITION":self.__file.antenna_position,
            "ANTENNA_POSITION_ITRF":self.__file.antenna_position_itrf,
            "NYQUIST_ZONE":lambda:[2 for i in self["SELECTED_DIPOLES"]] if "HBA" in self["ANTENNA_SET"] else [1 for i in self["SELECTED_DIPOLES"]],
            "TIME":self.__file.time,
            "SAMPLE_NUMBER":self.__file.sample_number,
            "SAMPLE_FREQUENCY_VALUE":self.__file.sample_frequency_value,
            "SAMPLE_FREQUENCY_UNIT":self.__file.sample_frequency_unit,
            "CABLE_DELAY":self.__file.cable_delay,
            "CABLE_DELAY_UNIT":self.__file.cable_delay_unit,
            "DATA_LENGTH":self.__file.data_length,
            "NOF_STATION_GROUPS":self.__file.nofStationGroups,
            "NOF_DIPOLE_DATASETS":self.__file.nofDipoleDatasets,
            "NOF_SELECTED_DATASETS":self.__file.nofSelectedDatasets,
            "DIPOLE_NAMES":self.__file.dipoleNames,
            "SELECTED_DIPOLES":self.__file.selectedDipoles,
            "CHANNEL_ID":self.__file.channelID,
            "FILETYPE":self.__file.filetype,
            "FILEDATE":self.__file.filedate,
            "TELESCOPE":self.__file.telescope,
            "OBSERVER":self.__file.observer,
            "CLOCK_OFFSET":self.getClockOffset,
            "CLOCK_FREQUENCY":self.__file.clockFrequency,
            "CLOCK_FREQUENCY_UNIT":self.__file.clockFrequencyUnit,
            "FILTER_SELECTION":self.__file.filterSelection,
            "TARGET":self.__file.target,
            "SYSTEM_VERSION":self.__file.systemVersion,
            "PIPELINE_NAME":self.__file.pipelineName,
            "PIPELINE_VERSION":self.__file.pipelineVersion,
            "NOTES":self.__file.notes,
            "PROJECT_ID":self.__file.projectID,
            "PROJECT_TITLE":self.__file.projectTitle,
            "PROJECT_PI":self.__file.projectPI,
            "PROJECT_CO_I":self.__file.projectCoI,
            "PROJECT_CONTACT":self.__file.projectContact,
            "OBSERVATION_ID":self.__file.observationID,
            "OBSERVATION_START_MJD":self.__file.startMJD,
            "OBSERVATION_START_TAI":self.__file.startTAI,
            "OBSERVATION_START_UTC":self.__file.startUTC,
            "OBSERVATION_END_MJD":self.__file.endMJD,
            "OBSERVATION_END_TAI":self.__file.endTAI,
            "OBSERVATION_END_UTC":self.__file.endUTC,
            "OBSERVATION_NOF_STATIONS":self.__file.nofStations,
            "OBSERVATION_STATION_LIST":self.__file.stationList,
            "OBSERVATION_FREQUENCY_MIN":self.__file.frequencyMin,
            "OBSERVATION_FREQUENCY_MAX":self.__file.frequencyMax,
            "OBSERVATION_FREQUENCY_CENTER":self.__file.frequencyCenter,
            "OBSERVATION_FREQUENCY_UNIT":self.__file.frequencyUnit
            }

        if self.__file.version() >= 1:
            self.__keyworddict["DIPOLE_CALIBRATION_DELAY"] = self.__file.dipole_calibration_delay
            self.__keyworddict["DIPOLE_CALIBRATION_DELAY_UNIT"] = self.__file.dipole_calibration_delay_unit

    def __initSelection(self):
        """Selection dependent initialization.
        """
        
        # Align data
        self.__alignment_offset = cr.hArray(self.__file.alignment_offset(self.__refAntenna))

        # Get antenna set
        if not hasattr(self,"antenna_set"):
            self.antenna_set = self.__file.antenna_set()

        # Get Nyquist zone for each antenna
        self.__nyquist_zone = self.__file.nyquist_zone()

        # Get Sample frequency value and units for each antenna
        self.__sample_frequency_value = self.__file.sample_frequency_value()
        self.__sample_frequency_unit = self.__file.sample_frequency_unit()

        # Generate scrach arrays
        self.__makeScratch()

        # Generate FFTW plan
        self.__plan = cr.FFTWPlanManyDftR2c(self.__blocksize, self.__file.nofSelectedDatasets(), 1, self.__blocksize, 1, self.__blocksize / 2 + 1, cr.fftw_flags.ESTIMATE)


    def info(self, verbose=True, show=True):
        """Display some information about the file. Short and long versions (verbose=False/True)
            .. function_author:: Emilio Enriquez <E.Enriquez@astro.ru.nl>
        """
        #Selecting a subset of keys.
        if verbose:
            key=self.keys()
            key = [k for k in key if 'EMPTY' not in k]
            key.sort()
        else:
            key=['FILENAME','TIME_HR','DATA_LENGTH','DATA_LENGTH_TIME','STATION_NAME','NOF_DIPOLE_DATASETS']

        if self['ANTENNA_SET'] == 'UNDEFINED':
            print "WARNING: ANTENNA_SET == UNDEFINED cannot read antenna positions."
            key = [k for k in key if 'POSITION' not in k]

        output ='[TBB_Timeseries] Summary of object properties' 
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
    def __repr__(self):
        """Display summary when printed.
        """
        return self.info(False, False)

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

    def next(self,step=1):
        """Advance to next block.
        *step* = 1 - advance by 'step' blocks (optional)
        """
        self.__block+=step
        
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
        
    def getitem__old(self, key):

        """Implements keyword access.
        """

        if key not in self.keys():
            raise KeyError("Invalid keyword")

        elif key is "FILENAME":
            return self.__file.filename()
        elif key is "BLOCKSIZE":
            return self.__blocksize
        elif key is "ANTENNA_SET":
            return self.__file.antenna_set()
        elif key is "NYQUIST_ZONE":
            return self.__file.nyquist_zone()
        elif key is "TIME":
            return self.__file.time()
        elif key is "SAMPLE_NUMBER":
            return self.__file.sample_number()
        elif key is "SAMPLE_FREQUENCY_VALUE":
            return self.__file.sample_frequency_value()
        elif key is "SAMPLE_FREQUENCY_UNIT":
            return self.__file.sample_frequency_unit()
        elif key is "DATA_LENGTH":
            return self.__file.data_length()
        elif key is "NOF_STATION_GROUPS":
            return self.__file.nofStationGroups()
        elif key is "NOF_DIPOLE_DATASETS":
            return self.__file.nofDipoleDatasets()
        elif key is "NOF_SELECTED_DATASETS":
            return self.__file.nofSelectedDatasets()
        elif key is "DIPOLE_NAMES":
            return self.__file.dipoleNames()
        elif key is "SELECTED_DIPOLES":
            return self.__file.selectedDipoles()
        elif key is "CHANNEL_ID":
            return self.__file.channelID()
        elif key is "FILETYPE":
            return self.__file.filetype()
        elif key is "FILEDATE":
            return self.__file.filedate()
        elif key is "TELESCOPE":
            return self.__file.telescope()
        elif key is "OBSERVER":
            return self.__file.observer()
        elif key is "CLOCK_FREQUENCY":
            return self.__file.clockFrequency()
        elif key is "CLOCK_FREQUENCY_UNIT":
            return self.__file.clockFrequencyUnit()
        elif key is "FILTER_SELECTION":
            return self.__file.filterSelection()
        elif key is "TARGET":
            return self.__file.target()
        elif key is "SYSTEM_VERSION":
            return self.__file.systemVersion()
        elif key is "PIPELINE_NAME":
            return self.__file.pipelineName()
        elif key is "PIPELINE_VERSION":
            return self.__file.pipelineVersion()
        elif key is "NOTES":
            return self.__file.notes()
        elif key is "PROJECT_ID":
            return self.__file.projectID()
        elif key is "PROJECT_TITLE":
            return self.__file.projectTitle()
        elif key is "PROJECT_PI":
            return self.__file.projectPI()
        elif key is "PROJECT_CO_I":
            return self.__file.projectCoI()
        elif key is "PROJECT_CONTACT":
            return self.__file.projectContact()
        elif key is "OBSERVATION_ID":
            return self.__file.observationID()
        elif key is "OBSERVATION_START_MJD":
            return self.__file.startMJD()
        elif key is "OBSERVATION_START_TAI":
            return self.__file.startTAI()
        elif key is "OBSERVATION_START_UTC":
            return self.__file.startUTC()
        elif key is "OBSERVATION_END_MJD":
            return self.__file.endMJD()
        elif key is "OBSERVATION_END_TAI":
            return self.__file.endTAI()
        elif key is "OBSERVATION_END_UTC":
            return self.__file.endUTC()
        elif key is "OBSERVATION_NOF_STATIONS":
            return self.__file.nofStations()
        elif key is "OBSERVATION_STATION_LIST":
            return self.__file.stationList()
        elif key is "OBSERVATION_FREQUENCY_MIN":
            return self.__file.frequencyMin()
        elif key is "OBSERVATION_FREQUENCY_MAX":
            return self.__file.frequencyMax()
        elif key is "OBSERVATION_FREQUENCY_CENTER":
            return self.__file.frequencyCenter()
        elif key is "OBSERVATION_FREQUENCY_UNIT":
            return self.__file.frequencyUnit()

    setable_keywords=set(["BLOCKSIZE","BLOCK","SELECTED_DIPOLES","ANTENNA_SET"])
    
    def __setitem__(self, key, value):
        if key not in self.setable_keywords:
            raise KeyError("Invalid keyword '"+str(key)+"' - vailable keywords: "+str(list(self.setable_keywords)))
        
        elif key is "BLOCKSIZE":
            self.__blocksize = value
            self.__initSelection()
        elif key is "BLOCK":
            self.__block = value
        elif key is "SELECTED_DIPOLES":
            self.setAntennaSelection(value)
        elif key is "ANTENNA_SET":
            self.antenna_set=value
        else:
            raise KeyError(str(key) + " cannot be set. Available keywords: "+str(list(self.setable_keywords)))

    def __contains__(self, key):
        """Allows inquiry if key is implemented.
        """
        
        return key in self.keys()

    def getTimeData(self,data=None,block=-1):
        """Calculate time axis depending on sample frequency and
        blocksize (and later also time offset). Create a new array, if
        none is provided, otherwise put data into array.
        """

        if not data:
            data = self.empty("TIME_DATA")

        block=cr.asval(block)

        if block<0:
            block=self.__block
        else:
            self.__block=block

        data.fillrange(self["BLOCK"]*self["BLOCKSIZE"]*cr.asval(self["SAMPLE_INTERVAL"]),cr.asval(self["SAMPLE_INTERVAL"]))
        return data
    
    def __makeScratch(self):
        """Create scratch arrays.
        """

        # Create scratch array
        self.__scratch = cr.hArray(float, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize))
        self.__scratchFFT = cr.hArray(complex, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize / 2 + 1))

    def setAntennaSelection(self, selection):
        """Sets the antenna selection used in subsequent calls to
        `getAntennaPositions`, `getFFTData`, `getTimeseriesData`.

        Required Arguments:

        =========== =================================================
        Parameter   Description
        =========== =================================================
        *selection* Either Python list (or hArray, Vector)
                    with index of the antenna as
                    known to self (integers (e.g. ``[1, 5, 6]``))
                    Or list of IDs to specify a LOFAR dipole
                    (e.g. ``['142000005', '3001008']``)
                    or say ``odd`` or ``even`` to select odd or even
                    antennas.
        =========== =================================================

        Output:
        This method does not return anything.

        Raises:
        It raises a `ValueError` if antenna selection cannot be set
        to requested value (e.g. specified antenna not in file).

        Example:
           file["SELECTED_DIPOLES"]="odd"
        """
        if type(selection)==str:
            if selection.upper() == "ODD":
                selection = list(cr.hArray(self["DIPOLE_NAMES"]).Select("odd"))
            elif selection.upper() == "EVEN":
                selection = list(cr.hArray(self["DIPOLE_NAMES"]).Select("even"))
            else:
                raise ValueError("Selection needs to be a list of IDs or 'odd' or 'even'.")
        elif type(selection) in cr.hAllContainerTypes:
            selection = list(selection.vec())
            
        if not isinstance(selection, list):
            raise ValueError("Selection needs to be a list.")

        if not len(selection)>0:
            raise ValueError("No antennas selected.")

        if type(selection[0])==int or type(selection[0])==long:
            # Selection by antenna number
            self.__selectedDipoles = [self.__dipoleNames[i] for i in selection if i < self.__nofDipoleDatasets]

        elif type(selection[0])==str:
            # Selection by antenna ID
            self.__selectedDipoles = [antennaID for antennaID in selection if antennaID in self.__dipoleNames]

        else:
            raise ValueError("No antenna ID or number found.")

        # Apply selection
        self.__file.selectDipoles(self.__selectedDipoles)

        # Check if selection was succesful
        if self.__file.nofSelectedDatasets() != len(selection):
            raise Exception("Not all antennas in selection are in file.")

        # Selection dependent initialization
        self.__initSelection()

    def getTimeseriesData(self, data, block=-1):
        """Returns timeseries data for selected antennas.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write timeseries data to.
        *block*       index of block to return data from. Use last set
                      block if not provided or None
        ============= =================================================

        Output:
        a two dimensional array containing the timeseries data of the
        specified block for each of the selected antennae.
        So that if `a` is the returned array `a[i]` is an array of
        length blocksize of antenna i.

        """
        block=cr.asval(block)

        if block<0:
            block=self.__block
        else:
            self.__block=block

        cr.hReadTimeseriesData(data, self.__alignment_offset+block*self.__blocksize, self.__blocksize, self.__file)


    def shiftTimeseriesData(self, sample_offset=0):
        """Shifts timeseries data for selected antennas.

        Required Arguments:

        =============== =================================================
        Parameter       Description
        =============== =================================================
        *sample_offset* Number of samples to offset timeseries data.
        =============== =================================================

        """    
        if sample_offset > self.__keyworddict["MAXIMUM_READ_LENGTH"] :
            raise ValueError('Sample offset > MAXIMUM_READ_LENGTH !!')

        self._TBBData__alignment_offset[...] += sample_offset
        self.__keyworddict["MAXIMUM_READ_LENGTH"] -= sample_offset
        

    def getFFTData(self, data, block=-1, hanning=True):
        """Writes FFT data for selected antennas to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of block to return data from.
        *hanning*     apply Hannnig filter to timeseries data before
                      the FFT.
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

        # Get timeseries data
        self.getTimeseriesData(self.__scratch, block)

        # Apply Hanning filter
        if hanning:
            self.__scratch[...].applyhanningfilter()

        # Perform FFT
        cr.hFFTWExecutePlan(data, self.__scratch, self.__plan)

        # Swap Nyquist zone if needed
        data[...].nyquistswap(self.__nyquist_zone[0])

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

        return md.get("RelativeAntennaPositions", self.__selectedDipoles, self.antenna_set, True)

    def getITRFAntennaPositions(self):
        """Returns antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        return md.get("AbsoluteAntennaPositions", self.__selectedDipoles, self.antenna_set, True)

    def getFrequencies(self, block=-1):
        """Returns the frequencies that are applicable to the FFT data

        Arguments:
        *block* = -1 - Is without function, just for compatibility.

        Return value:
        This method returns a FloatVector with the selected frequencies
        in Hz.

        """
        
        # Get empty array of correct size
        frequencies = self.empty("FREQUENCY_DATA")

        # Calculate sample frequency in Hz
        cr.hFFTFrequencies(frequencies, self["SAMPLE_FREQUENCY"][0], self["NYQUIST_ZONE"][0])
    
        return frequencies

    def getClockOffset(self):
        """Return clock offset.
        """

        return [md.getClockCorrection(s) for s in self["STATION_NAME"]]

    def empty(self, key):
        """Return empty array for keyword data.
        Known keywords are: "TIMESERIES_DATA", "TIME_DATA", "FREQUENCY_DATA", "FFT_DATA".
        """

        if key == "TIMESERIES_DATA":
            return cr.hArray(float, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize),name="E-Field(t)",units=("","Counts"))
        elif key == "TIME_DATA":
            return cr.hArray(float, self["BLOCKSIZE"],name="Time",units=("","s"))
        elif key == "FREQUENCY_DATA":
            return cr.hArray(float, self.__blocksize/2+1,name="Frequency",units=("","Hz"))
        elif key == "FFT_DATA":
            return cr.hArray(complex, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize / 2 + 1),name="fft(E-Field)",xvalues=self["FREQUENCY_DATA"],logplot="y")
        else:
            raise KeyError("Unknown key: " + str(key))

    def read(self, key, data, *args, **kwargs):
        """Generic read function supporting keyword arguments.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *key*         Data type to read, one of:
                      *TIMESERIES_DATA*
                      *FFT_DATA*
                      *FREQUENCY_DATA*
                      *TIME_DATA*
        *data*        array to write data to.
        ============= =================================================

        """

        if key == "TIMESERIES_DATA":
            return self.getTimeseriesData(data, *args, **kwargs)
        elif key == "TIME_DATA":
            return self.getTimeData(data, *args, **kwargs)
        elif key == "FREQUENCY_DATA":
            return data.copy(self.getFrequencies(*args, **kwargs))
        elif key == "FFT_DATA":
            return self.getFFTData(data, *args, **kwargs)
        else:
            raise KeyError("Unknown key: " + str(key))

    def close(self):
        """Closes file and sets the data attribute `.closed` to
        True. A closed object can no longer be used for I/O operations.
        `close()` may be called multiple times without error.
        """

        # Call file object destructor which should close the file
        del self.__file

        self.closed = True

def open(filename, *args, **kwargs):
    """Open file with LOFAR TBB data.
    """

    if not os.path.isfile(filename):
        raise IOError("No such file or directory: "+filename)
    else:
        return TBBData(filename, *args, **kwargs)

