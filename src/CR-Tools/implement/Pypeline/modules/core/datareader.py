"""Wrapper around the datareader class.
"""

from hftools import *
from config import *
from harray import *

#------------------------------------------------------------------------
# cr DataReader Class
#------------------------------------------------------------------------

def DataReader_read(self,key,aryvec,block=-1,antenna=-1):
    """
    Usage:
    
    datafile.read(ary,"Time",block=-1,antenna=-1)
    -> read key Data Array 'Time' from file and current block for all antennas into array 'ary'.

    aryvec: can be an array or a vector to contain the data
    
    blocks: this allows you to specify the block to be read in. If
    specified as a list, the read operation will loop over the array
    (if ellipses are used).

    antenna: allows you to select (a single) antenna to read. -1 means all.
    """
    hFileRead(self,key,aryvec,block,antenna)
    return self

def DataReader_repr(self):
    if hasattr(self,"filename"):
        return "crfile('"+self.filename+"')"
    else:
        return "DataReaderObject(filename unknown)"

def DataReader_getHeaderVariables(self):
    """
    Method to read out the header information from the DataReader and
    put it into attributes of the DataReader object.
    """
    self.keywords=map(lambda s:s[0].lower()+s[1:],set(self.get("keywords").split(", ")).difference(['keywords','help', 'positions','dDate', 'presync', 'TL', 'LTL', 'EventClass', 'SampleFreq', 'StartSample']))
    for v in self.keywords:
        setattr(self,v,self.get(v))

def DataReader_getitem(self,*keys):
    """
    datafile["key"] -> value
    datafile["key1","key2",...] -> [value1, value2,...]

    Method to obtain header information from the DataReader.
    """
    keys0=keys[0]
    if type(keys0)==tuple: return map(lambda k:hFileGetParameter(self,k),keys0)
    if not type(keys0)==str:
        print "Error - DataReader(Py): keyword",keys0,"must be a string!"
        return
    emptyarray=(keys0[0:5]=='empty')
    if emptyarray: keys0=keys0[5:]
    if keys0 in ["Time","Frequency","Fx","Voltage","FFT","CalFFT","TimeLag"]: #these are the data vectors
        if keys0=="Time": ary=hArray(float,dimensions=[self["blocksize"]],name="Time",units="s")
        if keys0=="Frequency": ary=hArray(float,dimensions=[self["fftLength"]],name="Frequency",units="Hz")
        if keys0=="Fx": ary=hArray(float,dimensions=[self["nofSelectedAntennas"],self["blocksize"]],name="E-Field",units="ADC Counts")
        if keys0=="Voltage": ary=hArray(float,dimensions=[self["nofSelectedAntennas"],self["blocksize"]],name="Voltage",units="V")
        if keys0=="FFT": ary=hArray(complex,dimensions=[self["nofSelectedAntennas"],self["fftLength"]],name="FFT(E-Field)",units="ar.u.")
        if keys0=="CalFFT": ary=hArray(complex,dimensions=[self["nofSelectedAntennas"],self["fftLength"]],name="CalFFT(E-Field)",units="ar.u.")
        if keys0=="TimeLag": ary=hArray(float,dimensions=[self["blocksize"]],name="Time Lag",units="s")
        if not emptyarray:
            if keys0=="TimeLag": ary.fillrange(-self["blocksize"]/2*self["sampleInterval"],self["sampleInterval"])
            else: ary.read(self,keys0)
        return ary
    else:
        return hFileGetParameter(self,keys0)

def DataReader_set(self,key,val):
    """
    datafile.set("key",value) -> set the appropriate header parameter

    Method to set header information in the DataReader and
    put it into attributes of the DataReader object.
    """
    ok=hFileSetParameter(self,key,val)
    self.getHeaderVariables()
    return self

def DataReader_setitem(self,*keyval):
    """
    datafile["key"]=value -> set the appropriate header parameter
    datafile[["key1","key2",...]]=[value1, value2,...] -> set multiple  header parameters

    Method to set header information in the DataReader and
    put it into attributes of the DataReader object.
    """
    if len(keyval)!=2:
        print "Wrong NUmber of Argument for DataReader['key']=value!"
        return
    key=keyval[0]
    val=keyval[1]
    if (type(key)==tuple):
        for k,v in zip(key,val):
            self.set(k,v)
    else: self.set(key,val)

def crfile(filename):
    """
    file=crfile("FILENAME.h5") -> DataReader Object

    This is a simple class to interface with the LOFAR CRTOOLS
    datareader class. It deals with LOFAR and LOEPS data alike. You
    can read data vectors, read or set parameters and get summary of the contents.
    """
    cfile=hFileOpen(filename)
    cfile.filename=filename
    cfile.getHeaderVariables()
    return cfile

#------------------------------------------------------------------------
# Getting Metadata like Antenna positions or instrumental delays
#------------------------------------------------------------------------

def hgetCalData(datafile,keyword):
    obsname=datafile["observatory"]
    if obsname=='LOPES':
        result=hCalTableVector(LOFARSOFT+"/data/lopes/LOPES-CalTable",keyword,datafile["date"],list(datafile["antennaIDs"]))
    else:
        print "ERROR: No caltables for non-LOPES files."
        return
    if keyword=="Position": return hArray(result,dimensions=[datafile["nofAntennas"],3])
    elif keyword=="Delay": return hArray(result,dimensions=[datafile["nofAntennas"]])
    else: return



DataReader.read=DataReader_read
DataReader.set=DataReader_set
DataReader.__setitem__=DataReader_setitem
DataReader.__getitem__=DataReader_getitem
DataReader.getHeaderVariables=DataReader_getHeaderVariables
DataReader.__doc__=crfile.__doc__
DataReader.getCalData = hgetCalData
DataReader.__repr__=DataReader_repr

