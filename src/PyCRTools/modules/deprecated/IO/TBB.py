"""This module takes care of multi-file input for the pycr tools.

With this module you are able to open multiple files and use operators on
them to obtain data and metadata. These operators are ported from the
pycrtools, but the output of that module is combined such that one object
of the correct dimension is possible.

In addition to this it is possible to make an antenna selection, such that
only data from some dipoles will be returned.

At the moment the antennaset (like "HBA", "LBA_OUTER") still has to be set
manually.

The modules provides methods which work on a list of crfiles (opened with
pycrtools) to read data and metadata and set parameters. Also it provides
a class which needs a list of filenames, antennaselection and antennaset
and then uses the methods of the modules and then should from the outside
work just like the pycrtools.

Example::

    import os
    from pycrtools import IO

    # Get list of files
    datadir=os.environ['LOFARSOFT']+'/data/lofar/trigger-2010-02-11/'
    tempfilenames=os.listdir(datadir)
    filenames=[]
    for file in tempfilenames:
        if '.h5' in file:
            filenames.append(datadir+file)

    # Open files
    crfile=IO.open(filenames)

    # Set the antennaset (Needed for antenna positions)
    crfile.setAntennaset('LBA_OUTER')

    # Set a selection of dipoles
    # All dipoles
    crfile.setAntennaSelection(None)
    # Selected by antenna ID
    crfile.setAntennaSelection([128002016,128004036])
    # Selected by sequence number in file
    crfile.setAntennaSelection([5,20])

    # Obtain keywords like the antenna IDs
    crfile['AntennaIDs']

    # Obtain antenna positions, this is possible in three systems
    # Relative to array centre
    crfile.getRelativeAntennaPositions()

    # In the ITRF frame
    crfile.getITRFAntennaPositions()

    # In a local frame w.r.t. CS002LBA center
    crfile["AntennaPositions"]

    # Read in data
    # Make an array first
    fxdata=crfile["emptyFx"]

    block=5
    crfile.readData(fxdata,block)

    # For all available keywords check
    crfile.keywords

"""

import pycrtools as cr
import numpy as np
import os

#from pycrtools import calibration
from pycrtools import metadata as md

# This class implements the IO interface
from interfaces import IOInterface

# Export these items by default
__all__ = ['TBBdata', 'open']

# Examples
class TBBData(IOInterface):
    """This class provides an interface to multi-file Transient Buffer
    Board data
    """

    __metadataKeywords=["StationPhaseCalibration","CableDelays","RelativeAntennaPositions","AbsoluteAntennaPositions","ClockCorrection","StationPositions","AntennaPositions"]

    def __init__(self,filenames,blocksize=1024,selection=None):
        """Constructor

        Open files.

        *filenames* Name of TBB hdf5 files to open
        *blocksize* Size of 1 block in samples
        *selection* Which antennas to select. 3 options:
                    - list of Antenna IDs
                       (first 3 digits stationID
                        next 3 digits RSPid (=RCUid/8)
                        next 3 digits RCUid)
                    - list of antenna numbers in file
                    - None (select all antennas)
        """

        self.new_selection_method=False #True
        self.files=openAligned(filenames,blocksize)

        self.setAntennaSelection(selection)

        self.allfxdata=get(self.files,"emptyFx",False)

        self.setAntennaset()
        self.__keywords=self.files[0].keywords
        self.__keywords.extend(self.__metadataKeywords)

        self.__calibrate = None

        self.__nfmin = None
        self.__nfmax = None

    def keys(self):
        """Returns list of valid keys.
        """

        return self.__keywords

    def __contains__(self, key):
        """Returns True if key is a valid key and False if not.
        """

        return key in self.__keywords

    def setAntennaset(self,antennaset=None):
        """This should set the antennaset variable. If this can be obtained         from the data file, this function should be changed, to set the
        default value.

        *antennaset* Antennaset, e.g. "HBA, LBA_OUTER", default None
        """

        self.antennaset=antennaset

        nyquistZone=get(self.files,"nyquistZone",True)
        if antennaset and "HBA" in antennaset and 1 in nyquistZone:
            if get(self.files,"sampleFrequency") > 180000000.0:
                nyquistZone=2
            else:
                nyquistZone=3
            set(self.files,"nyquistZone",nyquistZone)
        else:
            nyquistZone=nyquistZone[0]

        self.set("nyquistZone",nyquistZone)
        return True

    def __getitem__(self,keyword):
        """This makes the get method available by using the normal bracket
        method.
        """

        return self.get(keyword)

    def __setitem__(self,keyword,value):
        """This makes the set method available by using the normal bracket
        method.
        """

        self.set(keyword,value)

    def get(self,keyword):
        """Get the values for the keyword for multiple files. Uses the
        modules get method on the files provided, unless the keyword is of
        calibration metadata type, and then applies a
        selection. Returns a single object.

        Calibration metadata keywords:

        A method to show which keywords are available should be added.

        *keyword* Variable (data or metadata) to be read.
        Example "antennaIDs" , "shift", "TIME"
        """

        if keyword in self.__metadataKeywords:
            data=md.get(keyword, self.get("antennaIDs"), self.antennaset, True)
            return data
        elif keyword is "maxblocknr":
            return ((self["shift"]-self["Filesize"])/(-self["blocksize"])).min()
        else:
            data=get(self.files,keyword,False)
            if not self.selection or keyword in ["increment","frequencyRange","frequencyValues","selectedChannels"] or ( keyword == "selectedAntennas" and self.new_selection_method ):
                return data
            else:
                return applySelection(self.selection,data)

    def getAllList(self,keyword):
        """Like the get method, but returns the value for all dipoles,
        per file read. Returns a list of objects.

        *keyword* Variable to read
        """

        return get(self.files,keyword,True)

    def getAll(self,keyword):
        """Returns the values for all dipoles, not only the selected.

        *keyword* Variable to read
        """

        return get(self.files,keyword,False)

    def set(self,keyword,value):
        """Uses the set method of the module to set the value for all
        files
        """

        set(self.files,keyword,value)
        if keyword == "blocksize":
            print "Adjusting internal allfxdata array"
            self.allfxdata=get(self.files,"emptyFx",False)

    def getTimeseriesData(self, data, block=-1):
        """Read a block of data for the selected antennas.

        *data* hArray to write the data in.
        *block*  Block for which the data should be returned. If negative
                 it will return the next block.
        """

        if self.new_selection_method:
            return readFx(self.files,data,block)
        if not self.selection:
            retval = readFx(self.files,self.allfxdata,block)
            if not data:
                return self.allfxdata
            else:
                data.copy(self.allfxdata)
                return retval
        else:
            readFx(self.files,self.allfxdata,block)
            return applySelection(self.selection,self.allfxdata,data)

    def setAntennaSelection(self, selection):
        """Set an antennaselection.

        *selection* list of Antenna IDs or antenna numbers in file.
        """

        if not selection:
            self.selection=selection
        elif selection[0] > 1000:
            #selection contains the RCUids, so needs to be converted.
            self.selection=antIDsToSelection(self.files,selection)
        else:
            self.selection=selection

        if self.new_selection_method:
            self.enableFastSelection()

        # Create empty arrays to store data
        self.scratchfx = self["emptyFx"]

        shape = self.scratchfx.shape()
        self.scratchfft = cr.hArray(complex, dimensions=[shape[0], shape[1]/2+1])

    def enableFastSelection(self):
        """This function enables selection of dipoles in the files,
        to read out only the data that is actually used. This is
        experimental and has to be tested to work. There is one problem
        that at least 1 dipole has to be selected for each file.
        """

        if self.selection:
            selVec=cr.Vector(self.selection)
            selVec2=np.array(self.selection)
            selForFile=cr.Vector(self.selection)
            antsPerFile=self.getAllList("nofAntennas")
            startAnt=0
            endAnt=0
            self.new_selection_method=True
            for filenr in range(len(self.files)):
                startAnt=endAnt
                endAnt+=antsPerFile[filenr]
                nrSel=selForFile.findbetweenorequal(selVec,startAnt,endAnt-1)
                self.files[filenr].set("selectedAntennas",list(selVec2[list(selForFile[0:nrSel])]-startAnt))

    def enableCalibration(self,antennaset=None):
        """Returns  a calibrator object from the calibration class.
        This can be used to do a phase calibration on the fftdata.
        For this the antennaset should have been specified or given as
        a parameter.

        *antennaset* Antennaset e.g. "HBA", "LBA_OUTER". If provided this
        will also set this antennaset for the TBBdata object.
        """

        if not antennaset:
            antennaset=self.antennaset
        if not self.antennaset:
            self.setAntennaset(antennaset)

        self.__calibrate = calibration.AntennaCalibration(self.files,antennaset,self.selection)

        # Sets a shift due to the clockoffsets such
        # that all the data starts at the same moment.
        # Only needed for multi-station data.
        self.__calibrate.applyShift(self.files)

    def getRelativeAntennaPositions(self):
        """Returns relative antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Arguments:
        None

        Return value:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        return self.get("RelativeAntennaPositions")

    def getITRFAntennaPositions(self):
        """Returns antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Arguments:
        None

        Return value:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        return self.get("AbsoluteAntennaPositions")

    def setFrequencyRangeByIndex(self, nfmin, nfmax):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`.
        If **frequencies** is the array of frequencies available for the
        selected blocksize, then subsequent calls to `getFFTData` will
        return data corresponding to frequencies[nfmin:nfmax].

        Arguments:
        *nfmin* minimum frequency as index into frequency array
        *nfmax* maximum frequency as index into frequency array

        Return value:
        This method does not return anything.
        It raises an IndexError if frequency selection cannot be set
        to requested values (e.g. index out of range)
        """

        if not isinstance(nfmin, int) or not isinstance(nfmax, int):
            raise ValueError("Provided values for nfmin and nfmax are not of the correct type 'int'.")

        elif not 0 <= nfmin <= nfmax < len(self.files[0]["Frequency"]):
            raise ValueError("Invalid frequency range.")

        else:
            self.__nfmin = nfmin
            self.__nfmax = nfmax

    def getFrequencies(self):
        """Returns the frequencies that are appicable to the FFT data

        Arguments:
        None

        Return value:
        This method returns a FloatVector with the selected frequencies
        in Hz.
        """

        frequencies = self.files[0]["Frequency"]

        if self.__nfmin != None and self.__nfmax != None:
            frequencies=frequencies[range(self.__nfmin, self.__nfmax)]

        return frequencies

    def getFFTData(self, data, block, hanning=True):
        """Writes FFT data for selected antennas to data array.

        Arguments:
        *data* data array to write FFT data to.
        *block* index of block to return data from.
        *hanning* apply Hannnig filter to timeseries data before the FFT.

        Return value:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected antennae and
        for the selected frequencies.
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.
        """

        # Get timeseries data
        self.getTimeseriesData(self.scratchfx, block)

        # Apply Hanning filter
        if hanning:
            self.scratchfx[...].applyhanningfilter()

        # For selected frequency range
        if self.__nfmin != None and self.__nfmax != None:
            # Perform FFT
            self.scratchfft[...].fftcasa(self.scratchfx[...], self["nyquistZone"])

            # Phase calibrate
            if self.__calibrate:
                print "Applying calibration."
                self.__calibrate.applyCalibration(self.scratchfft)

            data[...].copy(self.scratchfft[...,self.__nfmin:self.__nfmax])

        else:
            # Perform FFT
            data[...].fftcasa(self.scratchfx[...], self["nyquistZone"])

            # Phase calibrate
            if self.__calibrate:
                print "Applying calibration."
                self.__calibrate.applyCalibration(data)


def open(filenames,blocksize=1024,selection=None):
    """Open TBB hdf5 files. Returns a TBBdata object.

    *filenames* List of names from the files to open
    *blocksize* NR samples per data block (import for FFT resolution
    *selection* Which antennas to select. 3 options:
                - list of Antenna IDs
                   (first 3 digits stationID
                    next 3 digits RSPid (=RCUid/8)
                    next 3 digits RCUid)
                - list of antenna numbers in file
    """

    return TBBData(filenames,blocksize,selection)

def openAligned(filenames,blocksize=1024):
    """This function opens files and aligns them by setting the shift argument correctly.

    *filenames* list of the files to open
    """

    if not isinstance(filenames,list):
        filenames=[filenames]

    files=[]
    for filename in filenames:
        if os.path.isfile(filename):
            files.append(cr.crfile(filename))
        else:
            print "File",filename,"could not be opened. Trying next file ..."

    # check if any files could be opened
    assert len(files) > 0

    times=get(files,"TIME",False)
    print 'these are the time from OLD: ', times
    tmin=times.min()
    tmax=times.max()
    # time stamp cannot be handled by integer function
    assert tmax -tmin < 22

    shifts=[]
    for i in range(len(files)):
        files[i]["blocksize"]=blocksize
        shifts.append(files[i]["SAMPLE_NUMBER"])
        shifts[i].muladd(files[i]["TIME"]-tmin,cr.Vector(int,files[i]["nofAntennas"],int(files[i]["sampleFrequency"])))

    # search for maximum in shifts
    shiftmax=0
    for i in range(0,len(files)):
        localmax=shifts[i].max()
        if localmax>shiftmax:
            shiftmax=localmax

    for i in range(0,len(files)):
        shifts2=[]
        for j in range(0,len(shifts[i])):
            shifts2.append(shiftmax-shifts[i][j])
        files[i]["ShiftVector"]=shifts2

    return files

def get(files, keyword,return_as_list=True):
    """ Get data or metadata for the specified keyword. If not to be returned as list
    it tries to return is as 1 object.

    If the value is the same for all files, this will be returned.
    If the value is an IntVec, FloatVec or an hArray where the first dimensions is
    the same as the number of selected antennas, an equal type object will be returned
    with the value for all the antennas.
    If neither of these can be achieved it will return a list with values per file.

    *files* List of crfiles
    *keyword* Variable to return
    *return_as_list* Return a list with a value per file.
    """

    if return_as_list:
        ret=[]
        for i in range(len(files)):
            ret.append(files[i][keyword])
    else:
        ret=files[0][keyword]
        if isinstance(ret,(cr.IntVec,cr.FloatVec)) and ( len(ret)==files[0]["nofAntennas"] or len(ret)==files[0]["nofSelectedAntennas"]):
            for i in range(1,len(files)):
                ret.extend(files[i][keyword])
        elif isinstance(ret,(cr.IntArray,cr.FloatArray,cr.ComplexArray)) and ( ret.getDim()[0] == files[0]["nofAntennas"] or ret.getDim()[0] == files[0]["nofSelectedAntennas"]):
            NrAnts=get(files,"nofAntennas")
            TotNrAnts=sum(NrAnts)
            shape=ret.getDim()
            shape[0]=TotNrAnts
            ret.resize(reduce(lambda x,y : x*y, shape))
            ret.setDim(shape)
            StartAnt=0
            EndAnt=0
            for i in range(0,len(files)):
                EndAnt+=NrAnts[i]
                ret[StartAnt:EndAnt]=files[i][keyword]
                StartAnt=EndAnt
        else:
            retall=get(files,keyword,True)
            if isinstance(retall[0],(cr.IntVec,cr.FloatVec)):
                for i in range(0,len(files)-1):
                    if ( (retall[i]-retall[i+1]).stddev()>0.0 ):
                        return retall
            else:
                for i in range(0,len(files)-1):
                    if retall[i]!=retall[i+1]:
                        return retall
            if "nof" in keyword and "Channels" not in keyword:
                ret=sum(retall)
                if "nofBaselines" is keyword:
                    ret+=len(retall)-1
            else:
                ret=retall[0]

    return ret

def set(files, keyword, value):
    """ Set the keyword with the value for all the files. Currently two options are
    supported:
        - One value for all files, e.g. files.set("block",1024)
        - A list with values per file.

    Support for a hArray of Vector with values for all antennas should be added.
    """

    if isinstance(value,list):
        for num,file in enumerate(files):
            file.set(keyword,value[num])
    elif isinstance(value, int):
        for file in files:
            file.set(keyword,value)

    return True

def readFx(files,fxdata,block=-1):
    """read a block of raw timeseries data for all (selected) antennas.

    *fxdata* Array in which to return the data
    *block*  Block for which data should be read
    """

    if block == -1:
        block=get(files,"block",True)
        for num,bl in enumerate(block):
            block[num]=bl+1

    set(files,"block",block)

    selAnts=get(files,"nofSelectedAntennas")
    antBeg=0
    antEnd=0
    for num,nrA in enumerate(selAnts):
        antBeg=antEnd
        antEnd+=nrA
        fxdata[antBeg:antEnd].read(files[num],"Fx")

    return True

def applySelection(selection,array,rArray=None):
    """ Select from the data only the applied selection

    *selection* Number of antennas for which to return the data
                (NOT antennaIDs, the can be converted by antIDsToSelection)
    *array*     Array on which to apply the selection
    *rArray*    Array to return the selected data in.
    """

    # Still needs a dimenstion check, so may need files after all
    if not rArray:
        returnArray=True
    else:
        returnArray=False
    if not selection:
        if returnArray:
            return array
        else:
            rArray=array
            return True
    if isinstance(array,(cr.FloatVec,cr.IntVec)):
        array=cr.hArray(array)
    if isinstance(array,(cr.FloatArray,cr.IntArray,cr.ComplexArray,cr.BoolArray)):
        dim=array.getDim()
        if len(dim)>=2:
            dim[0]=len(selection)
            if not rArray:
                rArray=cr.hArray(copy=array,dimensions=dim,fill=0)
            rArray[range(dim[0]),...].copy(array[selection,...])
        else:
            dim=len(selection)
            if not rArray:
                rArray=cr.hArray(copy=array,dimensions=dim,fill=0)
            rArray[...].copy(array[selection][...])

        if returnArray:
            return rArray
        else:
            return True
    else:
        if returnArray:
            return array
        else:
            rArray=array
            return False

def antIDsToSelection(files,SelAntIDs):
    """Gives the numbers of the selected antennas, according to the antenna IDs

    *files*   A list of cr files
    *SelAntIDs* IDs of the antenna selection (1e6*stationID+1e3*RSPid+RCUid )
                where RSPid = RCUid / 8

    returns: List with antenna selection.
    """

    allantIDs=get(files,"antennaIDs",False)
    SelAntIDs.sort()
    selection=[]
    for (num,ant) in enumerate(allantIDs.val()):
        if ant in SelAntIDs:
            selection.append(num)

    return selection
