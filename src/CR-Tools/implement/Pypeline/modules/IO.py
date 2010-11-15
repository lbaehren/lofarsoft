"""This module takes care of multi-file input for the pycr tools.

With this module you are able to open multiple files and use operators on
them to obtain data and metadata. These operators are ported from the 
pycrtools, but the output of that module is combined such that one object
of the correct dimension is possible.

In addition to this it is possible to make an antenna selection, such that
only data from some dipoles will be returned.

At the moment the antennaset (like "HBA", "LBA_OUTER") still has to be set
manually.

The modules provides methods which work on a list of crfiles (open with pycrtools)
to read data and metadata and set parameters. Also it provides a class which needs
a list of filenames, antennaselection and antennaset and then uses the methods of the
modules and then should from the outside work just like the pycrtools.

"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
import pycrtools as hf
import numpy as np

# Examples
class TBBdata:
    """This class provides an interface to multi-file Transient Buffer 
    Board data"""

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
        
        self.files=openfiles(filenames,blocksize)
        
        self.setSelection(selection)
                
        self.allfxdata=get(self.files,"emptyFx",False)

        #self.antennaset=antennaset
        
        self.setAntennaset()

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
        method."""
        return self.get(keyword)

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
        metadatakeywords=["StationPhaseCalibration","CableDelays","RelativeAntennaPositions","AbsoluteAntennaPositions","ClockCorrection","StationPositions","AntennaPositions"]
        if keyword in metadatakeywords: 
            import metadata as md
            data=md.get(keyword, self.get("antennaIDs"), self.antennaset, True)
            return data
        else:
            data=get(self.files,keyword,False)
            if not self.selection:
                return data
            else:
                return applySelection(self.selection,data)#get(self.files,keyword,False))

    def get_all_list(self,keyword):
        """Like the get method, but returns the value for all dipoles, 
        per file read. Returns a list of objects.
        
        *keyword* Variable to read 
        """
        return get(self.files,keyword,True)

    def get_all(self,keyword):
        """Returns the values for all dipoles, not only the selected.
        
        *keyword* Variable to read
        """
        return get(self.files,keyword,False)


    def set(self,keyword,value):
        """Uses the set method of the module to set the value for all 
        files"""
        set(self.files,keyword,value)
    
    def readdata(self,fxdata=None,block=-1):
        """Read a block of data for the selected antennas. 
        
        *fxdata* hArray to write the data in. If not provided, it will
                 return an array.
        *block*  Block for which the data should be returned. If negative
                 it will return the next block.
        """
        
        if not self.selection:
            retval = readFx(self.files,self.allfxdata,block)
            if not fxdata:
                return self.allfxdata
            else:
                fxdata.copy(self.allfxdata)
                return retval
        else:
            readFx(self.files,self.allfxdata,block)
            return applySelection(self.selection,self.allfxdata,fxdata)

    def setSelection(self,selection):
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

    def getCalibrator(self,antennaset=None):
        """Returns  a calibrator object from the calibration class. 
        This can be used to do a phase calibration on the fftdata.
        For this the antennaset should have been specified or given as
        a parameter.

        *antennaset* Antennaset e.g. "HBA", "LBA_OUTER". If provided this
        will also set this antennaset for the TBBdata object.
        """

        if not antennaset:
            antennaset=self.antennaset
        import calibration as crCal
        if not self.antennaset:
            self.setAntennaset(antennaset)
        
        return crCal.AntennaCalibration(self.files,antennaset,self.selection)

    def applyCalibrationShift(self,calibrator=None):
        """Obtains a calibrator and sets a shift due to the clockoffsets such
        that all the data starts at the same moment. Only needed for multi-station 
        data.
        """
        if not calibrator:
            calibrator=self.getCalibrator()
        
        calibrator.applyShift(self.files) 
     
    
        
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
    return TBBdata(filenames,blocksize,selection)

def openfiles(filenames,blocksize=1024):
    """This function opens files and aligns them by setting the shift argument correctly.

    *filenames* list of the files to open

    """

    files=[]
    for filename in filenames:
        files.append(hf.crfile(filename))
    
    times=get(files,"TIME",False)
    print 'these are the time from OLD: ', times
    tmin=times.min()
    tmax=times.max()
    #time stamp cannot be handled by integer function
    assert tmax -tmin < 22

    shifts=[]
    for i in range(len(files)):
        files[i]["blocksize"]=blocksize
        shifts.append(files[i]["SAMPLE_NUMBER"])
        shifts[i].muladd(files[i]["TIME"]-tmin,hf.Vector(int,files[i]["nofSelectedAntennas"],int(files[i]["sampleFrequency"])))

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
        if isinstance(ret,(hf.IntVec,hf.FloatVec)) and len(ret)==files[0]["nofSelectedAntennas"]:
                for i in range(1,len(files)):
                    ret.extend(files[i][keyword])
        elif isinstance(ret,(hf.IntArray,hf.FloatArray,hf.ComplexArray)) and ret.getDim()[0] == files[0]["nofSelectedAntennas"]:
            NrAnts=get(files,"nofSelectedAntennas")
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
            if isinstance(retall[0],(hf.IntVec,hf.FloatVec)):
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
        #fxdata.setSlice([antBeg*dim[1],antEnd*dim[1]])
        fxdata[antBeg:antEnd].read(files[num],"Fx")

    #fxdata.setSlice(0,antEnd*dim[1])
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
    if isinstance(array,(hf.FloatVec,hf.IntVec)):
        array=hf.hArray(array)
    if isinstance(array,(hf.FloatArray,hf.IntArray,hf.ComplexArray,hf.BoolArray)):
        dim=array.getDim()
        if len(dim)>=2:
            dim[0]=len(selection)
            if not rArray:
                rArray=hf.hArray(copy=array,dimensions=dim,fill=0)
            rArray[range(dim[0]),...].copy(array[selection,...])
        else:
            dim=len(selection)
            if not rArray:
                rArray=hf.hArray(copy=array,dimensions=dim,fill=0)
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

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

