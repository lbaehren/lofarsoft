def p_(var): print var,"=",eval(var)
"""
========================================================================
                          pycrtools TUTORIAL
========================================================================

Version History:
 -    started March 1, 2010 by H. Falcke

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

The most convenient way is probably to define an alias in your .bashr
or .profile files, like

alias pycr='/sw/bin/python2.6 -i $LOFARSOFT/src/CR-Tools/implement/Pypeline/pycrinit.py'

then you can simply start the pyrtools with 'pycr'.

The file pycrinit.py also allows you to read in a second file to be
executed, like this one here. So, you can run the tutorial with

pycr -i tutorial.py

from the UNIX prompt.


(+) Getting Help
================

The pycrtools have a built-in help system, which can be accessed with:

>>> help()

To get information on a specific function or method type
help(func). This will essentially print the docstring of the python
object and list all its methods. Hence, help will work on any decently
implemented python object, including the standard ones.

For example,

>>> help(IntVec)

will give documentation on the integer vector, while

>>> help(IntVec.sin)

will give the documentation on the "sin" method associated with it.

For a listing of all available functions in the pycrtools type

>>> help(all).


(+) Vectors
===========

(++) Basics
-----------

The fundamental data structure we use is a standard c++ vector defined
in the c++ standard template library (STL). This is wrapped and
exposed to python using the Boost Python system.

(NB: Unfortunately different systems provide different python data
structures. Hence a function exposed to python with SWIG or SIP is not
diretcly able to accept a BOOST PYTHON wrapped vector as input or vice
versa. If you want to do this you have to provide extra conversion
routines.)

In line with the basic python philosophy, vectors are passed as
references. Since we are working with large sets of data processing
time is as important as convenience. Hence the basic principle is that
we try to avoid copying large chunks of data as much as possible.

The basic functions operating on the data in c++ either take STL
iterators as inputs (i.e. pointers to the begin and end of the memory
where the data in the STL vector is stored) or casa::Vectors, which
are created with shared memory (i.e. their physical memory is the same
as that of the STL vector).

For that reason MEMORY ALLOCATION is done almost exculsively in the
Python layer. The fast majority of c++ functions is not even able to
allocate or free any memory. This allows for very efficient memory
management and processing, but also means that the user is responsible
for providing properly sized vectors as inputs AND output
vectors. I.e.: you need to know beforehand what sized vector you
expect in return!

That may be annoying, but forces you to think carefully about how to
use memory. For example, if there is a processing done multiple times
using a scratch vector of fixed size, you reuse the same vector over
and over again, thus avoiding a lot of unnecessary allocation and
deallocation of memory and creation of vectors.

Also, we are currently not supporting multidimensional data
structures. Mulit-dimensionally data is simply written sequentially
into the memory - you need to know how your data is organized. On the
other hand, in many case you can then easily process an entire matrix
at once.
 

(++) Construction of STL Vectors
--------------------------------

A number of vector types are provided: bool, int, float, complex, and
str.

To creat a vector most efficently, use the original vector constructors:

- BoolVec()
- IntVec()
- FloatVec() 
- ComplexVec() 
- StringVec()

e.g.
"""
v=FloatVec()
"""
>>> v
Vec(0)=[]

will create a floating point vector of size 0. 


The vector can be filled with a python list or tuple, by using the extend attribute:
"""
v.extend([1,2,3,4])
"""
>>> v
Vec(4)=[1.0,2.0,3.0,4.0]

Note, that python has automatically converted the integers into
floats, since the STL vector does not allow any automatic typing.

The stl vector can be converted back to a python list by using the
python list creator:

>>> list(v)
[1.0, 2.0, 3.0, 4.0]



However, the basic Boost Python STL vector constructor takes no
arguments and is a bit cumbersome to use in the long run.  Here we
provide a wrapper function that is useful for interactive use.  

Usage:

Vector(Type) -  will create an empty vector of type "Type", where Type is
a basic Python type, i.e.  bool, int, float, complex, str.

Vector(Type,size) - will create an vector of type "Type", with length "size".

Vector(Type,size,fill) - will create an vector of type "Type", with length
"size" and initialized with the value "fill"

Vector([1,2,3,...]) or Vector((1,2,3,...)) - if a list or a tuple is
provided as first argument then a vector is created of the type of the
first element in the list or tuple (here an integer) and filled with
the contents of the list or tuple.

So, what we will now use is:

>>>v=Vector([1.,2,3,4])
>>> v
Vec(4)=[1.0,2.0,3.0,4.0]


Note, that size and fill take precedence over the list and tuple
input. Hence if you create a vector with Vector([1,2,3],size=2) it
will contain only [1,2]. Vector([1,2,3],size=2,fill=4) will give
[4,4].


(+++) Referencing, memory allocation, indexing, slicing
........................................................

Following basic python rules, the STL vector will persist in memory as
long as there is a python reference to it. If you destroy v also the
c++ memory will disappear. Hence, if you keep a pointer to the vector
in c++ and try to work on it after the python object was destroyed,
your programme may crash. That's why by default memory management is
done ONLY on one side, namely the python side!

To illustrate how Python deals with references, consider the following
example:

>>> x=v
>>> x[0]=3
>>> v
Vec(4)=[3.0,2.0,3.0,4.0]

Hence, the new python object x is actually a reference to the same c++
vector that was created in v. Modifying elements in x modifies
elements in v. If you destroy v or x, the vector will not be
destroyed, since there is still a reference to it left. Only if you
destroy x and v the memory will be freed.


As note above, this vector is subscriptable and sliceable, using the
standard python syntax.

>>> v[1:3]
Vec(2)=[2.0,3.0]

We can also resize vectors and change their memory allocation:

"""
v1=Vector([0.0,1,2,3,4,5])
v2=Vector(float,len(v1),2.0)
"""
>>>v1
Vec(6)=[0.0,1.0,2.0,3.0,4.0,5.0]
>>>v2
Vec(6)=[2.0,2.0,2.0,2.0,2.0,2.0]


With the resize attribute you allocate new memory while keeping the
data. It is not guaranteed that the new memory actually occupies the
same physical space.

"""
v2.resize(8)
"""
>>> v2
Vec(8)=[2.0,2.0,2.0,2.0,2.0,2.0,0.0,0.0] 

Resize a vector and fill with non-zero values:

"""
v2.resize(10,-1)
"""
>>> v2
Vec(10)=[2.0,2.0,2.0,2.0,2.0,2.0,0.0,0.0,-1.0,-1.0]

Resize a vector to same size as another vector:

"""
v2.resize(v1)
"""
>>> v2
Vec(6)=[2.0,2.0,2.0,2.0,2.0,2.0]

Make a new vector of same size and type as the original one:

"""
v3=v2.new()
"""
>>> v3
Vec(6)=[0.0,0.0,0.0,0.0,0.0,0.0]

Fill a vector with values
"""
v3.fill(-2)
"""
>>>v3
Vec(6)=[-2.0,-2.0,-2.0,-2.0,-2.0,-2.0]

(+++) Vector arithmetic
........................................................

The vectors have a number of mathematical functions attached to
them. A full list can be seen by typing

>>> dir(Vector(float))
['__add__', '__class__', '__contains__', '__delattr__', '__delitem__', '__dict__', '__div__', '__doc__', '__format__', '__getattribute__', '__getitem__', '__hash__', '__iadd__', '__idiv__', '__imul__', '__init__', '__instance_size__', '__isub__', '__iter__', '__len__', '__module__', '__mul__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__setitem__', '__sizeof__', '__str__', '__sub__', '__subclasshook__', '__weakref__', 'abs', 'acos', 'add', 'addadd', 'addaddconv', 'append', 'asin', 'atan', 'ceil', 'convert', 'copy', 'cos', 'cosh', 'div', 'divadd', 'divaddconv', 'downsample', 'exp', 'extend', 'extendflat', 'fft', 'fill', 'findgreaterequal', 'findgreaterequalabs', 'findgreaterthan', 'findgreaterthanabs', 'findlessequal', 'findlessequalabs', 'findlessthan', 'findlessthanabs', 'findlowerbound', 'floor', 'iadd', 'idiv', 'imul', 'isub', 'log', 'log10', 'mean', 'median', 'mul', 'muladd', 'muladdconv', 'new', 'norm', 'normalize', 'resize', 'sin', 'sinh', 'sort', 'sortmedian', 'sqrt', 'square', 'stddev', 'sub', 'subadd', 'subaddconv', 'sum', 'tan', 'tanh']

Some of the basic arithmetic is available in an intuitve way.

You can add a scalar to a vector by
>>> v1+3
Vec(6)=[3.0,4.0,5.0,6.0,7.0,8.0]

This will actually create a new vector (and destroy it right away,
since no reference was kept). The original vector is unchanged.

You can also add two vectors:
>>>v1+v2
Vec(6)=[2.0,3.0,4.0,5.0,6.0,7.0]

In order to change the vector, you can use the "in place" operators
+=, -=, /=, *= :

Adding a vector in place:
>>>v1+=v2;
>>>v1
Vec(6)=[2.0,3.0,4.0,5.0,6.0,7.0]

now v1 was actually modified such that v2 was added to the content of
v1 and the results is stored in v1.

Similarly you can do

- v1-=v2
- v1*=v2
- v1/=v2

Here is a list of functions you can use

  Mean: v1.mean() = 4.5 Median:
v1.median() = 5.0 Summing: v1.sum() = 27.0 Standard Deviation:
v1.stddev() = 1.87082869339



(+) File I/O
------------

(++) Opening and Closing a File
-------------------------------

Let's see how we can open a file. First define a filename, e.g.:

"""
LOFARSOFT=os.environ["LOFARSOFT"]
filename=LOFARSOFT+"/data/lopes/example.event"
#filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
"""
> '/Users/falcke/LOFAR/usg/data/lopes/sample.event'

We can create a new file object, using the "crfile" class, which is an
interface to the LOFAR CRTOOLS datareader class and was defined in pycrtools.py. 

The following will open a data file:
"""
file=crfile(filename)
file.set("Blocksize",1024)
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


(++) Setting and retrieving metadata (parameters)
-------------------------------------------------

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

actually returns a vector. 

Here no difference is made where the data comes frome. The keyword
Obervatory accesses the header record in the data file while the
frequencRange accesses a method of the DataReader.

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
function. E.g. changing the blocksize we already did before. This is
simply
"""
blocksize=1024
file.set("Blocksize",blocksize)
""""
again the list of implemented keywords is visible with using
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
nofSelectedAntennas=file.get("nofSelectedAntennas"); p_("nofSelectedAntennas")
"""
> nofSelectedAntennas= 2

(++) Reading in Data
--------------------

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

>>> fxdata
Vec(2048)=[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,...]

Now we can read in the raw time series data, using "file.read" and the
keyword "Fx". (Currently implemented are the data fields "Fx",
"Voltage", "FFT", "CalFFT","Time", "Frequency".)

"""
file.read("Fx",fxdata)
"""
>>>fxdata
 Vec(2048)=[212.0,-278.0,196.0,236.0,4.0,-94.0,-192.0,208.0,-66.0,-62.0,...]

and voila the vector is filled with time series data from the data file.. 

Access the various antennas through slicing
"""
ant1data=fxdata[0:blocksize]
ant2data=fxdata[blocksize:2*blocksize]
"""
>>> ant2data
Vec(1024)=[-94.0,-165.0,-6.0,35.0,-310.0,-23.0,-128.0,97.0,239.0,289.0,...]

which makes a copy of the data vector if used in this way, while
"""
fxdata[0:3]=[0,1,2];
"""
>>>  fxdata;
 Vec(2046)=[0.0,1.0,2.0,-94.0,-192.0,208.0,-66.0,-62.0,-157.0,-120.0,...]

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

"""
(+) Fourier Transforms (FFT)
----------------------------

We can make a FFT of a float vector. This function will only return
the non-redundant part of the FFT (i.e., just one half).  Again we
need to provide a properly sized output vector (input length/2+1). We
also have to specify as a second parameter in which NyquistZone the
data was take. 

Nyquist sampling means that one needs, for example, 200 MHz sampling
rate to digitize a bandwidth of 100 MHz. The first Nyquist zone is
then 0-100 MHz, and the second is 100-200 MHz.

So, let's do the transform:
"""
fftdata=ComplexVec()
fftdata.resize(513)
fxdata[0:1024].fft(fftdata,1)
"""
>>> fftdata
Vec(513)=[(6078+0j),(99.3936739874-28.663986893j),(-93.6321366929-4.95059820124j),(82.9590664565+28.8729314743j),(-83.6744655239+4.46573054789j),(169.1861864-61.2949652607j),(-118.623662378+53.2694320202j),(75.764787806-74.6606191354j),(-115.629434646+29.4373842905j),(98.0844400537-16.0574421952j),...]


Here we have used the fft method of the float vector, which is just a
call to the stand-alone function hFFT defined in hftools.cc.
"""

"""
(+) Basic Plotting
------------------

In order to plot the data, we can use external packages. Two packages
are being provided here: matplotlib and mathgl. The former is
specifically designed for python and thus slightly easier to use
interactively. Since version 0.99 it is supposed to bea capable of 3D
plots (at the time of writing we use 0.98). Mathgl is perhaps a little
faster and convenient for real time applications and hence is used in
our GUI programming.

(++) Mathgl

Here is a simple example on how to use mathgl code (here without a
widget)

from mathgl import *

width=800
height=600
size=1024
gr=mglGraph(mglGraphPS,width,height)
y=mglData(size)
y.Modify("cos(2*pi*x)")
x=mglData(size)
x.Modify("x*1024");
ymax=y.Maximal()
ymin=y.Minimal()
gr.Clf()
gr.SetRanges(0,0.5,ymin,ymax)
gr.Axis("xy")
gr.Title("Test Plot x")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.Plot(x,y);
gr.WriteEPS("test-y.eps","Test Plot")

(++) Matplotlib

Now we import matplotlib
"""
#import matplotlib.pyplot as plt
#plt.plot([1,2,3])
#plt.ylabel("some numbers")
#plt.show()

"""

(+) Coordinates
---------------

We also have access to a few functions dealing with astronomical
coordinates. Assume, we have a source at an Azmiut/Elevation position
of (178 Degree,28 Degree) and we want to convert that into Cartesian
coordinates (which, for example, is required by our beamformer).

We first turn this into std vector and create a vector that is
supposed to hold the Cartesian coordinates. Note that the AzEL vector
is actually AzElRadius, where we need to set the radius to unity.

"""
azel=FloatVec()
azel.extend((178,28,1))
cartesian=azel.new()
"""
>>> azel
Vec(3)=[178.0,28.0,1.0]
>>> cartesian
Vec(3)=[0.0,0.0,0.0]

We then do the conversion, using 
"""
hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
"""
yielding the following output vector:

>>> cartesian
Vec(3)=[0.0308144266055,-0.882409725042,0.469471562786]

"""
