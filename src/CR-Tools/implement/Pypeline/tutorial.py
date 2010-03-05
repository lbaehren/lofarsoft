"""
========================================================================
                          pycrtools TUTORIAL
========================================================================
Run this file with "python -i tutorial.py" 

(+) StartUp
-----------

First one needs to load the library. This can be done by "from hftools
import *" which makes all the c++ functions and their python wrappers
available.

In addition there are some helpful definitons in python as well, which
are stored in "pycrtools.py". This file actually imports hftools for
you. Hence, all you actually need to do is (make sure the file is in
your search path)

"""
from pycrtools import *
"""
In addition, we may need a few other libraries for convencies, i.e. to
access operation system functions like environment variables and math
functions.
"""
import os
from math import *
"""

(+) FILE IO
-----------
Let's see how we can open a file. First define a filename, e.g.:

"""
LOFARSOFT=os.environ["LOFARSOFT"]
#filename=LOFARSOFT+"/data/lopes/example.event"
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
"""
> '/Users/falcke/LOFAR/usg/data/lopes/sample.event'

We can create a new file object, using the "crfile" class, which is an
interface to the LOFAR CRTOOLS datareader class and was defined in pycrtools.py. 

The following will open a data file:
"""
file=crfile(filename)
"""
>
[hftools.tmp.cc,3758]: Opening LOPES File=/Users/falcke/LOFAR/usg/data/lopes/example.event
[DataReader::summary]
 -- blocksize ............ = 65536
 -- FFT length ........... = 32769
 -- file streams connected?  0
 -- shape(adc2voltage) ... = [65536, 8]
 -- shape(fft2calfft) .... = [32769, 8]
 -- nof. antennas ........ = 8
 -- nof. selected antennas = 8
 -- nof. selected channels = 32769
 -- shape(fx) ............ = [65536, 8]
 -- shape(voltage) ....... = [65536, 8]
 -- shape(fft) ........... = [32769, 8]
 -- shape(calfft) ........ = [32769, 8]

The crfile class stores a pointer to the data reader object, which can
be retrieved with
"""
file.iptr
"""
> 17209856

and similarly the filename with
"""
file.filename
"""
> '/Users/falcke/LOFAR/usg/data/lopes/example.event'

The file will be automatically closed (and the DataReader object be
destroyed), whenever the crfile object is deleted, e.g. with
"file=0". One can also explicitly close the file with "file.close()".


Now we need to access the meta data in the file. This is done using a
single method "get". This method actually calls the function
"hFileGetParameter" defined in the c++ code.

Which observatory did we actually use?
"""
obsname=file.get("Observatory")
"""
> 'LOPES'

There are more keywords, of course. A list of implemented parameters
we can access is obtained by

"""
keywords=file.get("help")
"""
> hFileGetParameter - available keywords: nofAntennas, nofSelectedChannels, nofSelectedAntennas, nofBaselines, block, blocksize, stride, fftLength, nyquistZone, sampleInterval, referenceTime, sampleFrequency, antennas, selectedAntennas, selectedChannels, positions, increment, frequencyValues, frequencyRange, Date, Observatory, Filesize, dDate, presync, TL, LTL, EventClass, SampleFreq, StartSample, AntennaIDs, help

Note, that the results are returned as PythonObjects. Hence, this
makes use of the power of python with automatic typing. For, example
"""
file.get("frequencyRange")
"""
> Vec(2)=[40000000.0,80000000.0]

actually returns a vector. Note that the keyword Obervatory accesses
the headerrecord in the data file while the frequencRange accesses a
method of the DataReader. We make no distinction here, where the
metadata is stored.

Now we will define a number of useful variables that contain essential
parameters that we later will use.

"""
obsdate   =file.get("Date"); print "obsdate=",obsdate
filesize  =file.get("Filesize"); print "filesize=",filesize
blocksize =file.get("blocksize"); print "blocksize=",blocksize
nAntennas =file.get("nofAntennas"); print "nAntennas=",nAntennas
antennas  =file.get("antennas"); print "antennas=",antennas
antennaIDs=file.get("AntennaIDs"); print "antennaIDs=",antennaIDs
selectedAntennas=file.get("selectedAntennas"); print "selectedAntennas=",selectedAntennas
fftlength =file.get("fftLength"); print "fftlength=",fftlength
sampleFrequency =file.get("sampleFrequency"); print "sampleFrequency=",sampleFrequency
"""
>
obsdate= 1067339149
filesize= 65536
blocksize= 65536
nAntennas= 8
antennas= Vec(8)=[0,1,2,3,4,5,6,7]
antennaIDs= Vec(8)=[10101,10102,10201,10202,20101,20102,20201,20202]
selectedAntennas= Vec(8)=[0,1,2,3,4,5,6,7]
fftlength= 32769
sampleFrequency= 80000000.0


We can also change parameters in a very similar fashion, using the
"set" method, which is an implementation of the "hFileSetParameter"
function. E.g. changing the blocksize is simply
"""
blocksize=1024
file.set("Blocksize",blocksize)
""""
again the list of implemented keywords is visible with 
"""
file.set("help",0)
"""
> hFileSetParameter - available keywords: Blocksize, StartBlock,
  Block, Stride, SampleOffset, NyquistZone, ReferenceTime,
  SampleFrequency, Shift, SelectedAntennas, help

The set method actually returns the crfile object. Hence you can
append multiple set commands after each other.
"""
file.set("Block",2).set("SelectedAntennas",[0,2])
"""
> crfile</Users/falcke/LOFAR/usg/data/lopes/example.event>

Note, that we have now reduced the number of antennas to two: namely
antenna 0 and 2. So, we need to set

"""
nofSelectedAntennas=file.get("nofSelectedAntennas"); print "nofSelectedAntennas=",nofSelectedAntennas
"""
> nofSelectedAntennas= 2


The next step is to actually read in data. This is done with the read
method (accessing "hFileRead"). The data is read flatly into a 1D
vector. This is also true if mutliple antennas are read out at
once. Here simply the data from the antennas follow each other.

Also, by default memory allocation of the vectors has to be done in
python before calling any of the functions. This improves speed and
efficiency, but requires one to programme carefully and to understand
the data structrue.

First we create a FloatVec, which is BoostPython wrapped standard
(STL) vector of doubles.

"""
fxdata=FloatVec()
"""
and resize it to the size we need
"""
fxdata.resize(blocksize*nofSelectedAntennas)
"""

This is now a large vector filled with zeros.

"""
fxdata
"""
> Vec(2048)=[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,...]

Now we can read in the raw time series data, using "file.read" and the
keyword "Fx". (Currently implemented are the data fields "Fx",
"Voltage", "FFT", "CalFFT","Time", "Frequency".)

"""
file.read("Fx",fxdata)
fxdata
"""
> Vec(2048)=[212.0,-278.0,196.0,236.0,4.0,-94.0,-192.0,208.0,-66.0,-62.0,...]

and voila the vector is filled. 

Access the various antennas through slicing
"""
ant1data=fxdata[0:blocksize]
ant2data=fxdata[blocksize:2*blocksize]
ant2data
"""
> Vec(1024)=[-94.0,-165.0,-6.0,35.0,-310.0,-23.0,-128.0,97.0,239.0,289.0,...]

which makes a copy of the data vector if used in this way, while
"""
fxdata[0:3]=[0,1,2]; fxdata
"""
> Vec(2046)=[0.0,1.0,2.0,-94.0,-192.0,208.0,-66.0,-62.0,-157.0,-120.0,...]

actually modifies the original data vector.

(+) VECTORS
------------

The vectors come with quite a number of useful methods, which are
defined in c++ and added as methods in pycrtools.py. You can list the
available methods with "dir", e.g.
"""
dir(fxdata)
"""
> ['__add__', '__class__', '__contains__', '__delattr__', '__delitem__', '__dict__', '__div__', '__doc__', '__format__', '__getattribute__', '__getitem__', '__hash__', '__iadd__', '__idiv__', '__imul__', '__init__', '__instance_size__', '__isub__', '__iter__', '__len__', '__module__', '__mul__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__setitem__', '__sizeof__', '__str__', '__sub__', '__subclasshook__', '__weakref__', 'abs', 'acos', 'add', 'addadd', 'addaddconv', 'append', 'asin', 'atan', 'ceil', 'convert', 'copy', 'cos', 'cosh', 'div', 'divadd', 'divaddconv', 'downsample', 'exp', 'extend', 'extendflat', 'fill', 'findgreaterequal', 'findgreaterequalabs', 'findgreaterthan', 'findgreaterthanabs', 'findlessequal', 'findlessequalabs', 'findlessthan', 'findlessthanabs', 'findlowerbound', 'floor', 'iadd', 'idiv', 'imul', 'isub', 'log', 'log10', 'mean', 'median', 'mul', 'muladd', 'muladdconv', 'new', 'norm', 'normalize', 'resize', 'sin', 'sinh', 'sort', 'sortmedian', 'sqrt', 'square', 'stddev', 'sub', 'subadd', 'subaddconv', 'sum', 'tan', 'tanh']

"""
