## This is a template Python module.
#  Use this template for each module in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For scripts (e.g. files that you want to execute from the commandline)
#  you should use scripts/template.py instead.

"""Each module should have a documentation string describing it.

Style notes:
For Python, PEP 8  has emerged as the style guide that most projects adhere to; it promotes a very readable and eye-pleasing coding style. Every Python developer should read it at some point; here are the most important points extracted for you:

* Use 4-space indentation, and no tabs. 4 spaces are a good compromise between small indentation (allows greater nesting depth) and large indentation (easier to read). Tabs introduce confusion, and are best left out.

* Wrap lines so that they don't exceed 79 characters. This helps users with small displays and makes it possible to have several code files side-by-side on larger displays.

* Use blank lines to separate functions and classes, and larger blocks of code inside functions.

* When possible, put comments on a line of their own.

* Use docstrings.

* Use spaces around operators and after commas, but not directly inside bracketing constructs: a = f(1, 2) + g(3, 4).

* Name your classes and functions consistently; the convention is to use CamelCase for classes and lower_case_with_underscores for functions and methods. Always use self as the name for the first method argument (see A First Look at Classes for more on classes and methods).

* Don't use fancy encodings if your code is meant to be used in international environments. Plain ASCII works best in any case.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
import pycrtools as hf
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt

hf.trackHistory(False)
# Examples
class Def_Pyp_Set:
    """ This class contains default values to be
    passed to the pipeline startup.
    Eventually we will need at most a few default
    values that can be twiddled easily without modifying
    the script much.
    """

    def __init__(self,mode=None):
        """Constructor"""
        
	self.set_selection(None)
	
        self.files=openfiles(filenames,blocksize)
        
        self.setSelection(selection)
                
        self.allfxdata=get(self.files,"emptyFx",False)

        #self.antennaset=antennaset
        
        self.setAntennaset()
    
    def set_selection
    
    def setAntennaset(self,antennaset=None):
        """This should set the antennaset variable. If this can be obtained from the 
        data file, this function should be changed, to set the default value.
        
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
        return self.get(keyword)

    def get(self,keyword):
        data=get(self.files,keyword,False)
        if not self.selection:
            return data
        else:
            return applySelection(self.selection,data)#get(self.files,keyword,False))

    def get_all_list(self,keyword):
        """return the value for all dipoles, per file read
        
        *keyword* Variable to read 
        """
        return get(self.files,keyword,True)

    def get_all(self,keyword):
        """returns the value for all dipoles, not only selected
        
        *keyword* Variable to read
        """
        return get(self.files,keyword,False)


    def set(self,keyword,value):
        set(self.files,keyword,value)
    
    def readdata(self,fxdata=None,block=-1):
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
        if not selection:
            self.selection=selection
        elif selection[0] > 1000:
            #selection contains the RCUids, so needs to be converted.
            self.selection=antIDsToSelection(self.files,selection)
        else:
            self.selection=selection

    def getCalibrator(self,antennaset=None):
        if not antennaset:
            antennaset=self.antennaset
        import calibration as crCal
        if not self.antennaset:
            self.setAntennaset(antennaset)
        
        return crCal.AntennaCalibration(self.files,antennaset,self.selection)

    def applyCalibrationShift(self,calibrator=None):
        if not calibrator:
            calibrator=self.getCalibrator()
        
        calibrator.applyShift(self.files) 
     
    
        
def open(filenames,blocksize=1024,selection=None):
    #import IO
    return TBBdata(filenames,blocksize,selection)

def openfiles(filenames,blocksize=1024):
    """This function opens files and aligns them by setting the shift argument correctly.

    *filenames* list of the files to open

    Examples (also for doctests):

    >>> filenames(1,2)
    (1, 2)
    """

    files=[]
    for filename in filenames:
        files.append(hf.crfile(filename))
    
    times=get(files,"TIME",False)
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
    if isinstance(value,list):
        for num,file in enumerate(files):
            file.set(keyword,value[num])
    elif isinstance(value, int):
        for file in files:
            file.set(keyword,value)
    
        
    return True

def readFx(files,fxdata,block=-1):
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
        print "selection could not be made"
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

def example_function(a, b):
    """This is a docstring, each function should have one.

    *a* description of parameter a
    *b* description of parameter b

    Examples (also for doctests):

    >>> example_function(1,2)
    (1, 2)
    """

    return a, b

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

