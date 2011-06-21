"""Provides an n-dimensional array class.
"""

import numpy as np
import time
import os
import pickle

import matplotlib.pyplot as plt

#import pdb; pdb.set_trace()

from hftools import *
from htypes import *
from vector import *

#======================================================================
#  hArray Class Methods/Attributes
#======================================================================

def hArray(Type=None,dimensions=None,fill=None,name=None,copy=None,properties=None, xvalues=None,units=None,par=None,header=None):
    """
    Python convenience constructor function for hArrays. If speed is
    of the essence, use the original vector constructors: BoolArray(),
    IntArray(), FloatArray(), ComplexArray(), StringArray(), e.g.::

        >>> hArray(Type=float,dimensions=[n1,n2,n3...],
                   fill=array/scalar,name="String",
                   copy=array, properties=array, xvalues=array,
                   units=("prefix","unit"), par={keyword==value,...})
        FloatArray

    ``Array(Type)`` - will create an empty array of type ``Type``,
    where ``Type`` is a basic Python type, i.e.  ``bool``, ``int``,
    ``float``, ``complex``, ``str``.

    ``Array([1,2,3,...])`` or ``Array((1,2,3,...))``
      if a list or a tuple is provided as first argument then an array
      is created of the type of the first element in the list or tuple
      (here an integer) and filled with the contents of the list or
      tuple.

    ``Array(vec)``
      will create an array of the type of a vector and use the vector
      as its underlying memory storage. To copy the value, use the
      fill parameter described below.

    ``Array(Type, dimensions)``
      will create an array of type ``Type``, specifiying its
      dimensions. Input for dimensions can be a list or a another
      array (who's dimensions are copied).

    ``Array(Type,dimensions,fill)``
      same as above but fill the array with particular values. Input
      can be a single value, a list, a vector, or another array.

    ``Array()``
      defaults to a float array.

    Note, that dimensions and fill take precedence over the list and
    tuple input. Hence if you create a array with
    ``Array([1,2,3],dimension=[2])`` it will contain only
    ``[1,2]``. ``Array([1,2,3],dimension=[2],fill=4)`` will give
    ``[4,4]``.


    Parameters:

    =========== ======================== =====================================================
    Parameter   Default value            Description
    =========== ======================== =====================================================
    dimensions  list or arrays           set the dimension of the array specified
                                         as a list of integers, or specified as a
                                         list of integers, or an array from which
                                         to copy the dimensions.

    copy        array                    make a copy of 'array' in terms of type,
                                         dimension, fill, and parameter object.

    properties  array                    make a new object with the same properties
                                         of 'array' in terms of type, dimension,
                                         and parameter object, but NOT copying the data.

    xvalues     array                    set the default x-axis values for plotting to
                                         array (simply sets ``self.par.xvalues`` to
                                         array).

    units       (prefixstring,unitname)  will set the initial units of the array accordingly,
                                         e.g. ``units=("M","Hz")`` states that the values
                                         are provided in units of MHz.

    par         dict of parameters       set additional (arbitrary) parameter values that
                                         are stored in the ``.par`` attribute of the array,
                e.g. dict(logplot="y")   and are used, e.g., by the plot method to use
                                         certain defaults. Use par=dict(logplot="y") to use
                                         a logplot for the y-axis by default when plotting
                                         this array.

    header                               a dict containing optional keywords and values that
                                         can be taken over from a datafile.

    =========== ======================== =====================================================

    """
    if type(copy) in hAllArrayTypes:
        if properties==None: properties=copy
        if fill==None: fill=copy
    if type(properties) in hAllArrayTypes:
        if Type==None: Type=basetype(properties)
        if name==None: name=properties.getKey("name")
        if dimensions==None: dimensions=properties.shape()
        if units==None: units=(properties.getUnitPrefix(),properties.getUnitName())
        if hasattr(properties,"par"):
            par=properties.par.__dict__
    if Type==None: Type=float
    if isVector(Type):  #Make a new array with reference to the input vector
        ary=type2array(basetype(Type))
        ary.stored_vector=Type
        ary.setVector(ary.stored_vector)
    elif ishArray(Type):  # Just make a copy with reference to same vector
        ary=Type.newreference()
    else: # Create a new vector
        if type(Type) == np.ndarray and not dimensions:
            dimensions=Type.shape
        if type(dimensions) in [int,long]:
            size=dimensions
        elif (type(dimensions) in [list,tuple,IntVec]):
            size=reduce(lambda x,y : x*y, dimensions)
        elif (type(dimensions) in hAllArrayTypes):
            size=reduce(lambda x,y : x*y, dimensions.shape())
        else:
            size=-1
        vec=Vector(Type=Type, size=size)
        ary=type2array(basetype(vec))
        ary.stored_vector=vec
        ary.setVector(ary.stored_vector)
    if not hasattr(ary,"par"): setattr(ary,"par",hArray_par())
    if not hasattr(ary.par,"hdr"): setattr(ary.par,"hdr",{})
    if type(dimensions) in [int,long]: ary.reshape([dimensions])
    elif (type(dimensions) in [list,tuple,IntVec]): ary.reshape(dimensions)
    elif (type(dimensions) in hAllArrayTypes): ary.reshape(dimensions.shape())
    if type(par) == dict:
        for k,v in par.items():
            setattr(ary.par,k,v)
    if type(par) == tuple: setattr(ary.par,par[0],par[1])
    if type(par) == list: map(lambda elem:setattr(ary.par,elem[0],elem[1]),par)
    if type(header) == dict: ary.par.hdr.update(header)
    if not (xvalues == None): ary.par.xvalues=xvalues
    if not (units == None):
        if type(units)==str: ary.setUnit("",units)
        elif type(units)==tuple: ary.setUnit(*units)
        else: print "Error - hArray: Wrong format for units specified."
    if not (fill == None):
        if type(fill) in hAllVectorTypes:
            #ary.vec().fill(fill)
            #if fill.loopingMode():
            #    pass
            #                iterate=True
            #                lfill=fill.getSize()
            #                lary=ary.getSize()
            #                ary2=hArray(ary.vec(),[,)
            #                while iterate:
            #                    ary.vec().fill(fill)
            #else:
            ary.vec().fill(fill)
        elif type(fill) in [tuple,list]:
            if len(fill)>0:
                ary.vec().fill(Vector(fill))
        else:
            ary.fill(fill)
    if type(name)==str: ary.setKey("name",name);
    return ary

def hArray_repr(self, maxlen=100):

    # Check if looping is enabled
    loops=""
    if self.loopingMode():
        loops="*"

    # Get name (if any)
    name=self.getKey("name")

    if not name=="":
        name=', name="'+name+'"'

    # Compose string
#    s="hArray("+hTypeNamesDictionary[basetype(self)]+", "+str(list(self.getDim()))+name+" # len="+str(len(self))+", slice=["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+", vec -> "+VecToString(self.getVector()[self.getBegin():self.getEnd()],maxlen)+")"
#    s="hArray("+hTypeNamesDictionary[basetype(self)]+", "+str(list(self.getDim()))+", fill="+hPrettyString(self.vec(),self.getBegin(),self.getEnd(),maxlen)+name+") # len="+str(len(self))+" slice=["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+")"
    s="hArray("+hTypeNamesDictionary[basetype(self)]+", "+str(list(self.getDim()))+", fill="+hPrettyString(self.vec(),maxlen)+name+") # len="+str(len(self))+" slice=["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+")"

    return s

def hArray_none(self):
    """
    array.none() -> None

    Simply retuens the None object. Can be used to suppress unwanted
    output, when an operation returns an array.
    """
    return None

def hArray_newreference(self):
    """
    array.newreference() -> copy of array referencing the same vector

    Will make an exact copy of the array, and reference the same
    vector. Hence the new array can be assigned a new slice, but will
    still access the same underlying vector in memory.
    """
    ary=self.shared_copy()
    if hasattr(self,"par"): ary.par=self.par
    return ary

def hArray_shape(self):
    """
    self.shape() -> [dim1,dim2,...,dimN] or len(self)

    Retrieves the dimensions of a multidimensonal array as  a list of
    integers.

    Use array.reshape([dim1,dim2,...,dimN]) to set the dimensions.
    """
    return list(self.getDimensions())

def hArray_reshape(self, dimensions):
    """Reshape array to new dimensions if total number of elements is left
    unchanged.
    """
    apply(self.setDimensions,dimensions)

#   Raise ValueError otherwise. No, it is perfectly legal to rechange the vector if you want to ...
#
#    if reduce(lambda x, y : x*y, dimensions) == self.getSize():
#
#    else:
#        raise ValueError("Total size of new array must not be changed.")

def hArray_return_slice_start(val):
    """ Reduces a slice to its start value"""
    if val==Ellipsis:
        return 0
    elif type(val)==slice:
        if (val.start == None): return 0
        return val.start
    elif type(val) in hListAndArrayTypes:
        return Vector(val)
    else:
        return val

def hArray_return_slice_end(val):
    """ Reduces a slice to its end value"""
    if val==Ellipsis:
        return -1
    elif type(val)==slice:
        return hNone2Value(val.stop,-1)
    elif type(val) in hListAndArrayTypes:
        return Vector(val)
    else:
        return val+1

def hNone2Value(none,defval):
    """
    Returns a default value if the the first input is the None object,
    otherwise return the value of the first argument.
    """
    if none==None: return defval
    else: return none

def hSliceListElementToNormalValuesStart(s,dim):
    if s==None: return 0
    if s>=0: return s
    else: return s+dim

def hSliceListElementToNormalValuesEnd(s,dim):
    if s==None: return dim
    if s>=0: return s
    else: return s+dim

def hSliceToNormalValues(s,dim):
    """
    Returns a slice object where none and negative numbers are replaced by the appropriate integers, given a dimension (length) dim of the full slice.
    """
    if type(s) in [int,long]:
        if s<0: return s+dim
    if not type(s)==slice: return s
    s1=s.start;s2=s.stop;s3=s.step
    if type(s1) in hListAndArrayTypes:
        s1=Vector(map(lambda x:hSliceListElementToNormalValuesStart(x,dim),s1))
    elif s1==None: s1=0
    elif s1<0: s1+=dim
    elif s1>dim: s1=dim
    if type(s2) in hListAndArrayTypes:
        s2=Vector(map(lambda x:hSliceListElementToNormalValuesEnd(x,dim),s2))
    elif s2==None: s2=dim
    elif s2<0: s2+=dim
    elif s2>dim: s2=dim
    if s3==None: s3=1
    return slice(s1,s2,s3)

def hArray_vec(self):
    """
    array.vec() -> Vector([x1,x2,x3, ...])

    Retrieve the currently selected slice from the stored vector. If
    the entire vector is to be returned a reference to the internal
    data vector is returned. Otherwise, if a slice is active, a copy
    of that slice is returned. Use getVector() to ensure you always
    get only a reference.
    """
    beg=self.getBegin(); end=self.getEnd()
    if ((beg==0 )& (end==len(self))): return self.getVector()
    else: return self.getVector()[beg:end]

def hArray_array(self):
    """
    array.array() -> hArray(array.vec,properties=array)

    Retrieve the currently selected slice from the stored vector as an
    array. If the entire array is to be returned a new hArray with
    reference to the internal data vector is returned. Otherwise, if a
    slice is active, an hArray with a copy of that slice is
    returned. Use hArray(getVector(),...) to ensure you always get
    only a reference to the vector.

    NOTE: right now the dimensions for sliced arrays are not preserved
    properly. Needs more work!



    Example:
    x=hArray(int,[2,3,4],fill=range(6*4),name="test")
    self=x[1,1:3]
    """
    if hasattr(self,"__slice__"):
        dim=self.shape()
        if type(self.__slice__[-1])==slice:
            slc=hSliceToNormalValues(self.__slice__[-1],dim[len(self.__slice__)-1])
            newdim=[slc.stop-slc.start]+dim[len(self.__slice__):]
        else:
            newdim=dim[len(self.__slice__):]
        beg=self.getBegin(); end=self.getEnd()
        return hArray(self.getVector()[beg:end],dimensions=newdim,properties=self)
    else:
        return hArray(self.getVector(),properties=self)

def hArray_list(self):
    """
    array.list() -> [x1,x2,x3, ...]

    Retrieve the currently selected slice from the stored vector as a
    python list.
    """
    return list(self.getVector()[self.getBegin():self.getEnd()])

def hArray_new(self):
    """
    ary.new() -> new_array

    Create a new array of the same type and dimensions
    """
    return hArray(basetype(self),self)

def hArray_val(self):
    """
    ary.val() -> a             : if length == 1
    ary.val() -> [a,b,c,...]   : if length > 1

    Retrieve the contents of the vector as python values: either as a
    single value, if the vector just contains a single value, or
    otherwise return a python list.
    """
    return self.vec().val()

def hArray_toslice(self):
    """Usage: ary.toslice() -> slice(ary1,ary2,ary3)

    Returns the first one, two or three numbers of a vector or array as a Python
    slice specifier. This can then be used to index/slice another array.

    Example:
    >> ary1=hArray([0,3])
    >> ary2=hArray([0,1,2,3,4,5])
    >> ary2[ary1.toslice()]

    =>   hArray(int,[6]) # len=6, slice=[0:3], vec -> [0,1,2]
    """
    return slice(*(self.vec()[0:3]))

def hArray_copy_resize(self,ary):
    """
    Retrieve the first element of the currently selected slice from the stored vector.
    """
    ary.resize(len(self))
    ary.copy(self)
    return ary

def hArray_getitem(self,indexlist,asvec=False):
    """
    ary[n1,n2,n3]-> return Element with these indices

    Retrieves a copy of the array with the internal slices set to reflect ...
    integers, or value if slice contains only one value.

    To avoid getting a single value instead of a vector, when indexing
    one element, one has to use  ary.getSlicedArray(n1,n2,...).

    Use array.reshape([dim1,dim2,...,dimN]) to set the dimensions.
    """
    ary=self.getSlicedArray(indexlist)
    size=ary.getSize()
    if size == 0:
        raise IndexError("index out of bounds")
    elif size == 1 and not ary.loopingMode() and not asvec:
        return ary.val()
    else:
        return ary

def hArray_getSlicedArray(self,indexlist):
    """
    self[n1,n2,n3]-> return Element with these indices

    Retrieves a copy of the array with the internal slices set to reflect ...
    integers.

    Use array.reshape([dim1,dim2,...,dimN]) to set the dimensions.
    """
    if type(indexlist)==tuple:
        indexlist=list(indexlist)
    else:
        indexlist=[indexlist]
    ary=hArray(self)
    if hasattr(self,"par"):
        ary.par=self.par
    ary.__slice__=tuple(indexlist)
    dimensions=ary.shape()
    subslice_start=0; subslice_end=-1;
#   Now check if there is an ellipsis in the index list, which indicates looping.
    ellipsiscount=indexlist.count(Ellipsis)
    if ellipsiscount==0:
        ellipsislocation=0
    elif ellipsiscount==1:
        ellipsislocation=indexlist.index(Ellipsis)
        if ellipsislocation==0:
            indexlist[0]=slice(0,None,None) # replace ellipsis with slice
        else:
            if type(indexlist[ellipsislocation-1]) in [slice,list]: # check if Ellipsis is preceded by a slice specification over which to loop.
                del indexlist[ellipsislocation] # delete ellipsis
                ellipsislocation -= 1; #looping is over preceding index
            else:
                indexlist[ellipsislocation]=slice(0,None,None) # replace ellipsis with slice
    else:
        raise IndexError("only one Ellipsis (...) allowed in index list")
    nindices=len(indexlist)
    subslice_level=nindices-1
    for i in range(nindices):
        indexlist[i]=hSliceToNormalValues(indexlist[i],dimensions[i])
    indexliststarts=map(hArray_return_slice_start,indexlist)
    lastelement=indexlist[-1]
    if ellipsiscount:
        ellipsiselement=indexlist[ellipsislocation]
        if ellipsislocation<nindices-1: #The slice to return is itself not a slice over a full dimension, but just a part
            subslice_start=hArray_return_slice_start(lastelement);
            subslice_end=hArray_return_slice_end(lastelement)
            indexlist=indexlist[0:-1]; lastelement=indexlist[-1]
        else: #Take an entire dimension as a slice to return
            subslice_start=0
            subslice_end=1
        if type(ellipsiselement)==slice: #loop over a slice of a higher index
            ary.setSubSlice(subslice_start,subslice_end,subslice_level)
            ary.loop(Vector(int,ellipsislocation,fill=indexliststarts[0:ellipsislocation]),hNone2Value(ellipsiselement.start,0),hNone2Value(ellipsiselement.stop,-1),hNone2Value(ellipsiselement.step,1))
        elif type(lastelement)==list: # loop over a list of indices
            ary.setSubSlice(subslice_start,subslice_end,subslice_level)
            ary.loop(Vector(int,ellipsislocation,fill=indexliststarts[0:ellipsislocation]),Vector(indexlist[ellipsislocation]))
    elif type(lastelement)==slice: #Non-looping slice
        ary.setSubSlice(hArray_return_slice_start(lastelement),hArray_return_slice_end(lastelement),subslice_level)
        ary.setSliceVector(Vector(int,nindices-1,fill=indexliststarts[0:-1]))
    elif type(lastelement) in [list,IntVec,IntArray]: #make new vector
        if len(dimensions)==nindices: ## only implemented for last slice.
            arycopy=hArray(Type=basetype(ary),dimensions=[len(lastelement)])
            if nindices>1:
                ary.setSubSlice(subslice_start,subslice_end,subslice_level)
                ary.setSliceVector(Vector(int,nindices-1,fill=indexliststarts[0:-1]))
            arycopy.copy(ary,hArray(lastelement),-1)
            return arycopy
        else:
            raise IndexError("list of indices has to be at the position of the last index.")
    else: # normal integer index
        ary.setSubSlice(lastelement,lastelement+1,subslice_level)
        ary.setSliceVector(Vector(int,nindices-1,fill=indexliststarts[0:-1]))
    return ary;

def hArray_setitem(self,dims,fill):
    """
    vec[n1,n2,..] = [0,1,2] -> set slice of array to input vector/value
    vec[indexvector] = val -> set the elements with indices provided in indexvector to the respective values.

    Examples:

    a=hArray(range(9),dimensions=[3,3]) -> hArray(int, [3, 3], fill=[0, 1, 2, 3, 4, 5, 6, 7, 8]) # len=9 slice=[0:9])

    a[0]=11 -> hArray(int, [3, 3], fill=[11, 11, 11, 3, 4, 5, 6, 7, 8]) # len=9 slice=[0:9])

    a[0,1]=22 -> hArray(int, [3, 3], fill=[11, 22, 11, 3, 4, 5, 6, 7, 8]) # len=9 slice=[0:9])

    a[0,[0,2]]=33 -> hArray(int, [3, 3], fill=[33, 33, 33, 3, 4, 5, 6, 7, 8]) # len=9 slice=[0:9])

    a[0,[0,2]]=44 -> hArray(int, [3, 3], fill=[44, 33, 44, 3, 4, 5, 6, 7, 8]) # len=9 slice=[0:9])

    a[...]=[55,56,57] -> hArray(int, [3, 3], fill=[55, 56, 57, 55, 56, 57, 55, 56, 57]) # len=9 slice=[0:9])

    a[...,0]=66 -> hArray(int, [3, 3], fill=[66, 56, 57, 66, 56, 57, 66, 56, 57]) # len=9 slice=[0:9])


    """
    if type(fill) in [list,tuple]:
        fill=hVector(fill)
    if type(dims) in [int,long]:
        hFill(hArray_getSlicedArray(self,dims),fill)
    elif type(dims)==tuple:# (multi-d index)
        if type(dims[-1]) in [list,IntVec]:
            hSet(hArray_getSlicedArray(self,dims[:-1]),hArray(dims[-1]),fill)
        elif type(dims[-1]) == IntArray:
            hSet(hArray_getSlicedArray(self,dims[:-1]),dims[-1],fill)
        else:
            hFill(hArray_getSlicedArray(self,dims),fill)
    elif type(dims) in [list,IntVec]:
        self.set(hArray(dims),fill)
    elif type(dims) == IntArray:
        self.set(dims,fill)
    elif dims == Ellipsis or type(dims)==slice:
        hFill(hArray_getSlicedArray(self,dims),fill)
    else:
        raise IndexError ("Wrong type of index for array: " + str(dims))

def hArray_read(self,datafile,key,block=-1,antenna=-1):
    """
    array.read(file,"Time",block=-1) -> read key Data Array "Time" from file into array.

    Will also set the attributes par.file and par.filename of the array and
    make a history entry.

    block: this allows you to specify the block to be read in. If
    specified as a list, the read operation will loop over the array
    (if ellipses are used).
    """
    self.par.filename=datafile["FILENAME"]
    self.addHistory("read","Reading data from file "+self.par.filename)
    if type(block) in [int,long]: block=Vector([block])
    if type(block) in [list,tuple]: block=Vector(block)
    self.par.file=self
#    datafile.read(key,self,block,antenna)
    datafile.read(key,self,block)
    return self


def hArray_setPar(self,key,value):
    """
    array.setPar("keyword",value) -> array.par.keyword=value

    returns array.

    Provides a method to set a paramter associated with an array, e.g.,

    Example:

    fftdata=datafile["FFT"].setPar("xvalue",datafile["Frequency"])

    to set the default x-axis values.
    """
    setattr(self.par,key,value)
    return self

class hArray_par:
    """
    Parameter attribute. Used to hold and inherit arbitrary additional
    paramaters used by python methods (e.g. xvalues for the plotting
    command).
    """
    def __repr__(self):
        s=""
        for attr in dir(self)[2:]:
            if not (attr.find("_")==0):
                s+="par."+str(attr)+" = "+str(getattr(self,attr))+"\n"
        return s
    def __list__(self):
        l=[]
        for attr in dir(self)[2:]:
            if not (attr.find("_")==0):
                l.append((str(attr),getattr(self,attr)))
        return l
    def __dict__(self):
        d={}
        for attr in dir(self)[2:]:
            if not (attr.find("_")==0):
                d[str(attr)]=getattr(self,attr)
        return d

def hArray_setUnit(self,*arg):
    self.setUnit_(*arg)
    return self

def hArray_mprint(self):
    """ary.mprint() - > print the array in matrix style"""
    self[...].pprint(-1)

def hArray_transpose(self,ary=None):
    """
    Usage:
    aryT=ary.transpose() -> return newly created transposed array from ary
    aryT.transpose(ary) -> aryT contains transposed array data from ary (aryT will be reshaped if necessary)

    Transpose the current matrix (i.e. interchange the two lowest
    dimensions). This is always a copying operation, either a new
    array is created or data is copied to the array from the array
    being provided as argument. The output array will be reshaped if
    necesary.  This is a simple interface to the function
    hRedistribute and is not well suited to deal with slicing and
    looping.

    Note: vec.transpose = hArray_transpose (and not the c++ function
    hTranspose). This function (hArray_transpose) is a python
    interface to hTranspose that also changes the shape of the array
    (which is something the c++ function cannot do by design of the
    hftools).

    See also: hRedistribute, hTranspose

    Example:

    >> ary=hArray(int,[3,4],fill=range(12))
    >> ary.mprint()

    [0,1,2,3]
    [4,5,6,7]
    [8,9,10,11]

    >> aryT=hArray(int,[4,3])
    >> hArray_transpose(aryT,ary)
    >> aryT.mprint()

    [0,4,8]
    [1,5,9]
    [2,6,10]
    [3,7,11]

    or

    >> aryT.transpose(ary)
    >> aryT.mprint()

    or simply

    >> ary.transpose().mprint()

    """
    if ary==None:
        ary=self.new()
        dim=self.shape(); ncols=dim[-1]; nrows=dim[-2];
        dim=dim[:-2]+[ncols,nrows]
        ary.reshape(dim)
        hTranspose(ary,self,ncols)
        return ary
    else:
        dim=ary.shape(); ncols=dim[-1]; nrows=dim[-2];
        dim=dim[:-2]+[ncols,nrows]
        self.reshape(dim)
        hTranspose(self,ary,ncols)
        return self


def ishArray(ary):
    """
    ishArray(array) -> True or False

    Returns true if the argument is one of the hArray arrays, i.e. those listed in hAllVectorTypes.
    """
    return type(ary) in hAllArrayTypes


def hArray_toNumpy(self):
    """Returns a copy of the array as a numpy.ndarray object with the correct dimensions."""
    if isinstance(self, FloatArray):
        array = np.empty(shape=self.shape(), dtype='float')
        hCopy(array, self)
    elif isinstance(self, IntArray):
        array = np.empty(shape=self.shape(), dtype='int')
        hCopy(array, self)
    elif isinstance(self, ComplexArray):
        array = np.empty(shape=self.shape(), dtype='complex')
        hCopy(array, self)
    else:
        print "Cannot use hCopy for this type, falling back to (slower) list constructor"
        array = np.asarray(self.vec()).reshape(self.shape())

    return array

# Pickling
def hArray_getinitargs(self):
    """Get arguments for hArray constructor.

    .. warning:: This is not the hArray factory function but the constructor for the actual type. E.g. :meth:`IntArray.__init__`

    """
    return ()

def hArray_getstate(self):
    """Get current state of hArray object for pickling.
    """
    return (hTypeNamesDictionary[basetype(self)], self.shape(), self.writeRaw())

def hArray_setstate(self, state):
    """Restore state of hArray object for unpickling.
    """
    size = 1
    for d in state[1]:
        size *= d
    self.resize(size)
    self.reshape(state[1])
    self.readRaw(state[2])

def hArray_hasHeader(self,parameter_name=None):
    """
    Usage:

    ary.hasHeader() -> return true if array has a header par
    ary.getHeader('parameter_name') -> return true if array has a header par and a key word with name parameter_name
    """
    hasheader=hasattr(self,"par") and hasattr(self.par,"hdr")
    if type(parameter_name)==str and hasheader:
        hasheader=self.par.hdr.has_key(parameter_name)
    return hasheader

def hArray_setHeader(self,**kwargs):
    """
    Usage:

    ary.setHeader(par1=val1,par2=val2, ...)

    Set the parameters parN in the header of the array to the values
    valN. The header is stored as a dict in ary.par.hdr.
    """
    if not hasattr(self,"par"):
        setattr(self,"par",hArray_par())
    if not hasattr(self.par,"hdr"):
        setattr(self.par,"hdr",{})
    self.par.hdr.update(kwargs)

def hArray_getHeader(self,parameter_name=None):
    """
    Usage:

    ary.getHeader() -> entire header dict
    ary.getHeader('parameter_name') -> header value

    Get the parameter 'parameter_name' from the header of the array or
    return the entire dict, if no parameter name is specified. The
    header is stored as a dict in ary.par.hdr.
    """
    if hasattr(self,"par") and hasattr(self.par,"hdr"):
        if parameter_name==None:
            return self.par.hdr
        elif self.par.hdr.has_key(parameter_name):
            return self.par.hdr[parameter_name]
        else:
            print "Error: Header keyword ",parameter_name,"not known."
            return None


#------------------------------------------------------------------------
# Reading and Writing an array
#------------------------------------------------------------------------
default_blockedIOnames=set(["par.xvalues"])

def hArray_write(self, filename,nblocks=1,block=0,dim=None,writeheader=None,varname='',clearfile=None,blockedIOnames=default_blockedIOnames):
    """
    Usage:

    array.write(filename,nblocks=1,block=0,dim=None,writeheader=None,clearfile=None)

    Write an hArray to disk including a header file to read it back
    again. This is a simple interface to hArray_writeheader and then
    hWriteFileBinary.

    Attention: if a file exists already, then it will not be
    automatically overwritten, unless it is explicitly asked for. This
    can be dangerous if one writes a smaller file to a pre-existing
    older and larger file because then the 'new' file will contain the rest
    of the old file.

    By default a header will be written and the file will be
    overwritten, if one writes the first block. This can be explicitly
    switched on or off with keywords 'writeheader' and 'clearfile'.

    *filename* - the filename where the data will be dumped. The header
    filename will have the ending".hdr" the data file a ".dat" ending (the filename can contain either of those endings or none).

    *nblocks* - Allows one to specify that one will write the same
    vector multiple times to the same file, so that the data file size
    in the end is actually nblocks times the original hArray
    size. This needs to be known at creation time. In case multiple
    blocks will be written, the function will write the first block to
    disk.

    *block* - which block to write. If the block is larger than 0, then
    the header file is not being written.

    *varname* - you can store the original variable name in which the
    hArray was stored. Currently not really used.

    *writeheader* - force header file to be written or not (for
    True/False resp.). Use default behavior if not set (i.e. None).

    *dim* - Specify a different dimension vector [dim1,dim2,dim3,
    ....,dimn] under which the array should be restored. The user is
    responsible for consistency.

    *blockedIOnames* = set(['par.xvalues']) - a set containing a list
     of attribute names that need to be written with the same blocking
     subdivision as the actual array data (see block, nblock) - the
     paramters are specified as attributes with a dot,
     e.g. 'par.xvalues' (default). An array in the header would be
     specified as 'par.hdr.arrayname.'

    Example:

xvals=hArray(range(10))
x=hArray(float,[10],fill=range(10),name="test",xvalues=xvals)
x.write("test.pcr")
y=hArrayRead("test")
y -> hArray(float, [4], name="test" # len=4, slice=[0:4], vec -> [1.0, 2.0, 3.0, 4.0])
y.par.xvalues -> hArray(int, [10], fill=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]) # len=10 slice=[0:10])

The data files are stored in a directory with name test.pcr:

ls test.pcr/ ->  data.bin               header.hdr              par.xvalues.pcr/

Example using blocks:

xvals=hArray(range(10))
x=hArray(float,[10],fill=range(10),name="test",xvalues=xvals)
x.write("test.pcr",block=0,nblocks=2)
x.write("test.pcr",block=1,nblocks=2)
y=hArrayRead("test")
y -> hArray(float, [20], fill=[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0], name="test") # len=20 slice=[0:20])
y.par.xvalues -> hArray(int, [20], fill=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]) # len=20 slice=[0:20])
    """

    if type(self)==StringArray:
        print "Attention: StringArrays cannot be dumped to disk!"
        return


    fn=os.path.expandvars(os.path.expanduser(filename))

    #Add proper ending if missing
    if not fn[-4:].upper() == ".PCR": fn+=".pcr"
    binfile=os.path.join(fn,"data.bin")

    #Check if data and header directory is available
    if not os.path.exists(fn):
        os.mkdir(fn)
    elif not os.path.isdir(fn):
        print "Error: Cannot write File. File",fn,"already exists and is not a directory!"
        return

    #Write header file if requested
    if not (writeheader==False):
        hArray_writeheader(self,fn,nblocks=nblocks,block=block,dim=dim,varname=varname,blockedIOnames=blockedIOnames,writeheader=writeheader,clearfile=clearfile)
        if (block==0) and not (clearfile==False):
            if os.path.exists(binfile):
                os.remove(binfile)

    if (clearfile):
        if os.path.exists(binfile):
            os.remove(binfile)
    self.writefilebinary(binfile,block*self.getSize())

class hFileContainer():
    """
    Dummy class to hold a filename where an hArray is stored.
    """
    def __init__(self,path,name,vector=False):
        self.path=path
        self.name=name
        self.vector=vector
        self.__hFileContainer__=True
    def __call__(self,path,name):
        self.path=path
        self.name=name
        self.vector=vector


def hArrayReadDictArray(dictionary,path,block=-1,blockedIOnames=default_blockedIOnames,amalgateblocks=True):
    """
    Recursively goes through a dict (of dicts) and replaces all placeholder (hFileCOntainer) with hArrays or Vectors read from disk.
    """
    newdictionary=dictionary.copy()
    for k,v in newdictionary.items():
        if hasattr(v,"__hFileContainer__"):
            filename=os.path.join(path,v.name)
            parname=v.name[:-4]
            if parname in blockedIOnames:
                bblock=block
            else:
                bblock=-1
            if v.vector:
                newdictionary[k]=hArrayRead(filename,block=bblock,amalgateblocks=amalgateblocks,restorevar=False).vec()
            else:
                newdictionary[k]=hArrayRead(filename,block=bblock,amalgateblocks=amalgateblocks,restorevar=False)
        elif type(v) == dict:
            newdictionary[k]=hArrayReadDictArray(v,path)
    return newdictionary

def hArrayWriteDictArray(dictionary,path,prefix,nblocks=1,block=0,writeheader=None,clearfile=None,blockedIOnames=default_blockedIOnames):
    """
    Recursively goes through a dict (of dicts) and replaces all values which are hArray with a placeholder and writes the array to disk.
    """
    newdictionary=dictionary.copy()
    for k,v in newdictionary.items():
        if type(v) in [StringArray,StringVec]:
            newdictionary[k]=list(v)
        elif type(v) in hAllVectorTypes:
            parname=prefix+"."+str(k)
            filename=parname+".pcr"
            if parname in blockedIOnames:
                hArray_write(hArray(v), os.path.join(path,filename),nblocks=nblocks,block=block,dim=None,writeheader=writeheader,varname=str(k),clearfile=clearfile)
            else:
                hArray_write(hArray(v), os.path.join(path,filename),nblocks=1,block=0,dim=None,writeheader=writeheader,varname=str(k),clearfile=clearfile)
            newdictionary[k]=hFileContainer(path,filename,vector=True)
        elif type(v) in hAllArrayTypes:
            parname=prefix+"."+str(k)
            filename=parname+".pcr"
            filename=prefix+"."+str(k)+".pcr"
            if parname in blockedIOnames:
                hArray_write(v, os.path.join(path,filename),nblocks=nblocks,block=block,dim=None,writeheader=writeheader,varname=str(k),clearfile=clearfile)
            else:
                hArray_write(v, os.path.join(path,filename),nblocks=1,block=0,dim=None,writeheader=writeheader,varname=str(k),clearfile=clearfile)
            newdictionary[k]=hFileContainer(path,filename)
        elif type(v) == dict:
            newdictionary[k]=hArrayWriteDictArray(v,path,prefix+"."+str(k))
    return newdictionary

def hArray_writeheader(self, filename,nblocks=None,block=0,varname='',dim=None,blockedIOnames=default_blockedIOnames,writeheader=None,clearfile=None):
    """
    Usage:

    ary.writeheader(filename,nblocks=1,dim=None)

    Write a header for an hArray binary data file, which was written with hWriteFileBinary.

    *filename* - the filename where the data was dumped. The header
    filename will have the ending".hdr", replacing a ".dat" ending if present.

    *nblocks* - Allows one to specify that one has written the same
    vector multiple times to the same file, so that the data file
    size is actually nblocks times the original hArray size.

    *blocks* - Which block to write header for (usually only used when
    called from hArray_write)

    *dim* - Specify a different dimension vector [dim1,dim2,dim3,
    ....,dimn] under which the array should be restored. The user is
    responsible for consistency.

    *array* - if an array is specified and it contains nblocks and dim
     as parameter, those values will be takent to write the header

    *varname* - you can store the original variable name in which the
    hArray was stored.

Example:
x=hArray([1.0,2.0,3,4],name="test")
x.writeheader("test.pcr")
x.writefilebinary("test.pcr")
y=hArrayRead("test")
y -> hArray(float, [4], name="test" # len=4, slice=[0:4], vec -> [1.0, 2.0, 3.0, 4.0])
    """
    fn=os.path.expandvars(os.path.expanduser(filename))
    if fn[-4:].upper() in [".PCR",".DAT",".HDR"]:
        fn=fn[:-4]+'.pcr'
    else:
        fn=fn+'.pcr'
    if not os.path.exists(fn):
        os.mkdir(fn)
    elif not os.path.isdir(fn):
        print "Error: Cannot write header. File",fn,"already exists!"
        return
    f=open(os.path.join(fn,"header.hdr"),"wb")
    slicelength=self.getEnd()-self.getBegin()
    if self and hasattr(self,"par"):
        if hasattr(self.par,"nblocks") and not nblocks:
            nblocks=self.par.nblocks
        if hasattr(self.par,"dim") and not dim:
            dim=self.par.dim
    if not nblocks: nblocks=1
    arydim=self.getDim()
    if not slicelength==len(self): arydim=[slicelength]
    if dim: arydim=dim
    f.write("# Header for hArray vector written on "+time.ctime()+"\n")
    f.write("ha_filename = '"+filename+"'\n")
    f.write("ha_type = "+typename(basetype(self))+"\n")
    f.write("ha_dim = "+str(arydim)+"\n")
    f.write("ha_nblocks = "+str(nblocks)+"\n")
    f.write("ha_name = '" + self.getKey("name")+"'\n")
    if basetype(self) in hNumericalTypes:
        f.write("ha_units = ('" +self.getUnitPrefix()+"', '" + self.getUnitName()+"')\n")
    else:
        f.write("ha_units = None\n")
    if hasattr(self,"__slice__"):
        f.write("ha_slice = " + str(self.__slice__)+"\n")
    else:
        f.write("ha_slice = None\n")
    f.write("ha_varname = '" + varname+"'\n")
    if hasattr(self,"par"):
        par=hArrayWriteDictArray(self.par.__dict__,fn,"par",nblocks=nblocks,block=block,writeheader=writeheader,clearfile=clearfile)
        f.write('ha_parameters="""'+pickle.dumps(par)+'"""\n')
    else:
        f.write('ha_parameters=None\n')
    f.close()

def hArrayRead(filename,block=-1,restorevar=False,blockedIOnames=default_blockedIOnames,amalgateblocks=True):
    """
    Usage::

      >>> ary = hArrayRead("testdata.hdr")
      New hArray read from testdata.hdr and testdata.dat

    Create and return a new hArray which contains data read from a
    binary data file with ``hWriteFileBinary`` and which has a
    ``.hdr`` header file written with ``hArray_writeheader``.

    =============== ======== ===============================================
    Parameter       Default  Description
    =============== ======== ===============================================
    filename                 the filename of either the ``.dat`` file
                             or the ``.hdr`` file (but both files need
                             to be present). You can only give the
                             filename without the extension.
    block           -1       Allows one to specify that one has written
                             the same vector multiple times to the same
                             file, so that the data file size is actually
                             ``nblocks`` (see ``hArray_writeheader``)
                             times the original ``hArray`` size. if
                             ``block < 0`` then the entire file is being
                             read and a big array is returned, otherwise
                             only the block specified will be read and
                             returned.
    amalgateblocks  True     If ``True`` and multiple blocks were written
                             to file, then the array read back will have
                             the same number of dimensions as the original
                             data set with just the first dimension being
                             ``nblock`` times bigger. If set to ``False``,
                             then an array with dimensions
                             ``[nblocks,original dimensions ...]`` will be
                             returned.
    =============== ======== ===============================================

    Example::

      x=hArray([1.0,2.0,3,4],name="test")
      x.write("test.dat")
      y=hArrayRead("test")
      y -> hArray(float, [4], name="test" # len=4, slice=[0:4], vec -> [1.0, 2.0, 3.0, 4.0])

      x.write("test.dat",nblocks=2)
      x.writefilebinary("test.dat",1*4)
      y=hArrayRead("test")
      y -> hArray(float, [2, 4], name="test" # len=8, slice=[0:8], vec -> [1.0, 2.0, 3.0, 4.0, 1.0, 2.0, 3.0, 4.0])

    """
    ha_slice=None # Put there to assure backward compatibility
    fn=os.path.expandvars(os.path.expanduser(filename))
    if fn[-4:].upper() == ".PCR": fn=fn[:-4]+".pcr"
    else: fn+=".pcr"
    binfile=os.path.join(fn,"data.bin")
    hdrfile=os.path.join(fn,"header.hdr")
    if not os.path.exists(fn):
        print "Error hArrayRead: data directory ",fn," does not exist!"
        return -1
    if not os.path.exists(hdrfile):
        print "Error hArrayRead: data file ",hdrfile," does not exist!"
        return -1
    f=open(hdrfile,"rb")
    exec f in locals()
    f.close()
    if ha_nblocks == 1 :
        block=0
    if block<0:
        if amalgateblocks:
            dim = [ha_dim[0]*ha_nblocks]+ha_dim[1:]
        else:
            dim = [ha_nblocks]+ha_dim
        block=0
    else:
        dim=ha_dim
    if ha_parameters:
        par=pickle.loads(ha_parameters)
        par=hArrayReadDictArray(par,fn,blockedIOnames=blockedIOnames,amalgateblocks=amalgateblocks)
    else:
        par=hArray_par()
    if ha_slice:
        ary=hArray(ha_type,dim,name=ha_name,units=ha_units,par=par)[ha_slice]
    else:
        ary=hArray(ha_type,dim,name=ha_name,units=ha_units,par=par)
    ary.par.nblocks=ha_nblocks
    ary.par.dim=ha_dim
    if sum(dim)>0:
        if not os.path.exists(binfile):
            print "Error hArrayRead: data file ",binfile," does not exist!"
        else:
            ary.readfilebinary(binfile,block*ary.getSize())
#    if restorevar & (not l["ha_varname"] == ''):
#        print "Creating new object",l["ha_varname"]
#        exec ("global "+l["ha_varname"]+"\n"+l["ha_varname"]+" = ary")
    return ary

def ashArray(val):
    """Usage::

      ashArray(val) -> hArray(val)

    Returns an hArray withe the argument val unless val is already an
    hArray. In this case return the argument unchanged.

    Example::

      ashArray(3) -> hArray(int, [1] # len=1, slice=[0:1], vec -> [3])
      ashArray([3,4]) -> hArray(int, [2] # len=2, slice=[0:2], vec -> [3, 4])
      x=ashArray([3,4]); ashArray(x) -> hArray(int, [2] # len=2, slice=[0:2], vec -> [3, 4])

    """
    if type(val) in hAllArrayTypes:
        return val
    elif type(val) in hAllListTypes:
        return hArray(val)
    else:
        return hArray([val])

def asList(val):
    """Usage::

      >>> asList(val)
      list(val) if val is not a list already

    Returns its argument as a list, but leave it unchanged if it is
    already a list. Sets, vectors, arrays, etc. will be turned into
    list, strings will not.

    Example::

      >>> asList(3)
      [3]

      >>> asList([3])
      [3]

    """
    if type(val)==list:
        return val
    elif type(val)==str:
        return [val]
    else:
        return list(val)

hArray_Find_functions={
    "=":(hCount,hFind),
    ">":(hCountGreaterThan,hFindGreaterThan),
    "<":(hCountLessThan,hFindLessThan),
    ">=":(hCountGreaterEqual,hFindGreaterEqual),
    "<=":(hCountLessEqual,hFindLessEqual),
    "even":(hCountEven,hFindEven),
    "odd":(hCountOdd,hFindOdd),
    "between":(hCountBetween,hFindBetween),
    "outside":(hCountOutside,hFindOutside)
    }

def hArray_Find(self,operator,threshold1=None,threshold2=None):
    """
    Usage:

    ``ary.Find("operator",threshold1,(threshold2)) -> indexlist of locations``

    Description:

    This function will take an array or a vector as input an
    return a vector with zero-based indices pointing to the locations
    in the vector/array where the imposed criterion is true.

    *operator* = '=','>','<','>=','<=','between','outside','even','odd'

    *threshold1* = the threshold to compare vector values with (for  '=','>','<','>=','<=')

    *threshold2* = the second threshold if applicable (for between, outside)

    'even','odd' don't take any additional argument.

    Example::

        >>> v=hArray(range(10)) -> hArray(int, [10L], fill=[0,1,2,3,4,5,6,7,8,9]) # len=10 slice=[0:10])
        >>> v.Find(">",7) -> hArray(int, [2L], fill=[8,9]) # len=2 slice=[0:2])

    """
    #Find how many elements satisfy condition
    if threshold1 == None:
        n = hArray_Find_functions[operator][0](self)
    elif threshold2 == None:
        n = hArray_Find_functions[operator][0](self,threshold1)
    else:
        n = hArray_Find_functions["operator"][0](self,threshold1,threshold2)

    #Now create a fitting vector or hArray with the right size
    if type(self) in hAllVectorTypes:
        indexlist=Vector(int,n)
    elif type(self) in hAllArrayTypes:
        indexlist=hArray(int,[n[-1]])
    else:
        return None

    #Search and return the respective element indices
    if threshold1 == None:
        hArray_Find_functions[operator][1](indexlist,self)
    elif threshold2 == None:
        hArray_Find_functions[operator][1](indexlist,self,threshold1)
    else:
        hArray_Find_functions[operator][1](indexlist,self,threshold1,threshold2)
    return indexlist


def hArray_Select(self,*args,**kwargs):
    """
    Usage:

    ``ary.Select('operator',threshold1,(threshold2)) -> [ary_i </=/> threshold]``

    Description:

    This function will return an array with elements the input array
    which fulfill the search criterion (i.e. to be equal, or
    above/below a threshold value).

    *operator* = '=','>','<','>=','<=','between','outside','even','odd'

    *threshold1* = the threshold to compare vector values with (for  '=','>','<','>=','<=')

    *threshold2* = the second threshold if applicable (for between, outside)

    'even','odd' don't take any additional argument.

    See also:

    hArray_Find
    
    Example::

        v=hArray(range(10)) -> hArray(int, [10L], fill=[0,1,2,3,4,5,6,7,8,9]) # len=10 slice=[0:10])
        v.Select(">",7) -> hArray(int, [2L], fill=[8,9]) # len=2 slice=[0:2])

        a=hArray(['0','1','2']); a.Select("odd") -> '1'

        a=hArray(['0','1','2']); a.Select("even") -> ['0','2']

    """
    return self[self.Find(*args,**kwargs)]


# Fourier Transforms
setattr(FloatArray,"fft",hFFTCasa)

setattr(IntArray,"toslice",hArray_toslice)

for v in hAllArrayTypes:
    setattr(v,"__repr__",hArray_repr)
    setattr(v,"__getinitargs__",hArray_getinitargs)
    setattr(v,"__getstate__",hArray_getstate)
    setattr(v,"__setstate__",hArray_setstate)
    setattr(v,"__getstate_manages_dict__",1)
    setattr(v,"toNumpy", hArray_toNumpy)
    setattr(v,"setPar",hArray_setPar)
    setattr(v,"shape",hArray_shape)
    setattr(v,"reshape",hArray_reshape)
    setattr(v,"getDim",hArray_shape) # Depriciated use shape
    setattr(v,"setDim",hArray_reshape) # Depriciated
    setattr(v,"array",hArray_array)
    setattr(v,"vec",hArray_vec)
    setattr(v,"val",hArray_val)
    setattr(v,"new",hArray_new)
    setattr(v,"none",hArray_none)
    setattr(v,"read",hArray_read)
    setattr(v,"list",hArray_list)
    setattr(v,"write",hArray_write)
    setattr(v,"writeheader",hArray_writeheader)
    setattr(v,"mprint",hArray_mprint)
    setattr(v,"Transpose",hArray_transpose)
    setattr(v,"copy_resize",hArray_copy_resize)
    setattr(v,"newreference",hArray_newreference)
    setattr(v,"getSlicedArray",hArray_getSlicedArray)
    setattr(v,"__getitem__",hArray_getitem)
    setattr(v,"__setitem__",hArray_setitem)
    setattr(v,"setUnit",hArray_setUnit)
    setattr(v,"hasHeader",hArray_hasHeader)
    setattr(v,"getHeader",hArray_getHeader)
    setattr(v,"setHeader",hArray_setHeader)


for v in hAllContainerTypes:
    setattr(v,"Find",hArray_Find)
    setattr(v,"Select",hArray_Select)
    for s in hAllContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hAllContainerMethods(a): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hRealContainerTypes:
    for s in hRealContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hRealContainerMethods(a): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hComplexContainerTypes:
    for s in hComplexContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hComplexContainerTypes(a): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hNumericalContainerTypes:
    setattr(v,"__add__",Vec_add)
    setattr(v,"__sub__",Vec_sub)
    setattr(v,"__div__",Vec_div)
    setattr(v,"__mul__",Vec_mul)
    setattr(v,"__iadd__",Vec_iadd)
    setattr(v,"__imul__",Vec_imul)
    setattr(v,"__idiv__",Vec_idiv)
    setattr(v,"__isub__",Vec_isub)
    v.phase = hArg
    for s in hNumericalContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hNumericalContainerMethods(a): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hNumericalAndStringContainerTypes:
    for s in hNumericalAndStringContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hNumericalAndStringContainerTypes(a): function ",s," is not defined. Likely due to a missing library in hftools.cc."

