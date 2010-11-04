
################################################################################################
################################################################################################
#
#   	ORIGINAL IO MODULE CLASS
#   	(author: S. ter Veen, s.terveen@astro.ru.nl)
#
################################################################################################
################################################################################################  

"""This class takes care of multi-file input for the pycr tools.

With this class you are able to open multiple files and use operators on
them to obtain data and metadata. These operators are ported from the 
pycrtools, but the output of that module is combined such that one object
of the correct dimension is possible.

In addition to this it is possible to make an antenna selection, such that
only data from some dipoles will be returned.

At the moment the antennaset (like "HBA", "LBA_OUTER") still has to be set
manually.

The class provides methods which work on a list of crfiles (open with pycrtools)
to read data and metadata and set parameters. Also it provides a class which needs
a list of filenames, antennaselection and antennaset and then uses the methods of the
modules and then should from the outside work just like the pycrtools.

"""

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
        metadatakeywords=["StationPhaseCalibration","CableDelays","RelativeAntennaPositions","AbsoluteAntennaPositions","ClockCorrection","StationPositions"]
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


## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

    


