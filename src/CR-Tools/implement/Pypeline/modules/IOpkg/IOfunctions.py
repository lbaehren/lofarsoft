

import pycrtools as cr

################################################################################################
################################################################################################
#
#      A SERIES OF GENERAL 'GET' FUNCTIONS
#   	(author: S. terVeen, s.terveen@astro.ru.nl)
#
################################################################################################
################################################################################################	

        
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
        files.append(cr.crfile(filename))
    
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
        shifts[i].muladd(files[i]["TIME"]-tmin,cr.Vector(int,files[i]["nofSelectedAntennas"],int(files[i]["sampleFrequency"])))

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
        if isinstance(ret,(cr.IntVec,cr.FloatVec)) and len(ret)==files[0]["nofSelectedAntennas"]:
                for i in range(1,len(files)):
                    ret.extend(files[i][keyword])
        elif isinstance(ret,(cr.IntArray,cr.FloatArray,cr.ComplexArray)) and ret.getDim()[0] == files[0]["nofSelectedAntennas"]:
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


def frange(start, end=None, inc=None):
    "A range function, that does accept float increments..."

    if end == None:
        end = start + 0.0
        start = 0.0

    if inc == None:
        inc = 1.0

    L = []
    while 1:
        next = start + len(L) * inc
        if inc > 0 and next >= end:
            break
        elif inc < 0 and next <= end:
            break
        L.append(next)
        
    return L

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()
