"""Provides an n-dimensional array class.
"""

import numpy as np
import matplotlib.pyplot as plt

from hftools import *
from types import *
from vector import *

#======================================================================
#  hArray Class Methods/Attributes
#======================================================================

def hArray(
Type=None,dimensions=None,fill=None,name=None,copy=None,properties=None, xvalues=None,units=None,par=None):
    """
    Python convenience constructor function for hArrays. If speed is
    of the essence, use the original vector constructors: BoolArray(),
    IntArray(), FloatArray(), ComplexArray(), StringArray()

    Usage:

    hArray(Type=float,dimensions=[n1,n2,n3...],fill=array/scalar,name="String", copy=array, properties=array, xvalues=array,units=("prefix","unit"),par=(keyword,value)) -> FloatArray

    Array(Type) -  will create an empty array of type "Type", where Type is
    a basic Python type, i.e.  bool, int, float, complex, str.

    Array([1,2,3,...]) or Array((1,2,3,...)) - if a list or a tuple is
    provided as first argument then an array is created of the type of
    the first element in the list or tuple (here an integer) and
    filled with the contents of the list or tuple.

    Array(vec) - will create an array of the type of a vector and use
    the vector as its underlying memory storage. To copy the value,
    use the fill parameter described below.

    Array(Type,dimensions) - will create an array of type "Type",
    specifiying its dimensions. Input for dimensions can be a list or
    a another array (who's dimensions are coopied).

    Array(Type,dimensions,fill) - same as above but fill the array with
    particular values. Input can be a single value, a list, a vector,
    or another array.

    Array() defaults to a float array.

    Note, that dimensions and fill take precedence over the list and tuple
    input. Hence if you create a array with Array([1,2,3],dimension=[2]) it
    will contain only [1,2]. Array([1,2,3],dimension=[2],fill=4) will give
    [4,4].

    Parameters:
    ===========

    dimensions = list or array: set the dimension of the array
    specified as a list of integers, or an array from which to copy
    the dimensions.

    copy = array: make a copy of 'array' in terms of type, dimension,
    fill, and parameter object.

    properties = array: make a new object with the same properties of 'array' in terms of type, dimension,
    and parameter object, but NOT copying the data.

    xvalues = array: set the default x-axis values for plotting to array
    (simply sets self.par.xvalues to array)

    units = (prefixstring,unitname) - will set the initial units of the
    array accordingly, e.g. units=("M","Hz") states that the values
    are provided in units of MHz.

    par = tuple or list of tuples: set additional (arbitrary)
    parameter values that are stored in the .par attribute of the
    array, and are used, e.g., by the plot method to use certain
    defaults.

    """
    if type(copy) in hAllArrayTypes:
        if properties==None: properties=copy
        if fill==None: fill=copy
    if type(properties) in hAllArrayTypes:
        if Type==None: Type=basetype(properties)
        if name==None: name=properties.getKey("name")
        if dimensions==None: dimensions=properties.shape()
        if units==None: units=(properties.getUnitPrefix(),properties.getUnitName())
        par=properties.par.__list__()
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
        vec=Vector(Type=Type)
        ary=type2array(basetype(vec))
        ary.stored_vector=vec
        ary.setVector(ary.stored_vector)
    if not hasattr(ary,"par"): setattr(ary,"par",hArray_par())
    if type(dimensions)==int: ary.reshape([dimensions])
    elif (type(dimensions) in [list,tuple,IntVec]): ary.reshape(dimensions)
    elif (type(dimensions) in hAllArrayTypes): ary.reshape(dimensions.shape())
    if type(par) == tuple: setattr(ary.par,par[0],par[1])
    if type(par) == list: map(lambda elem:setattr(ary.par,elem[0],elem[1]),par)
    if not (xvalues == None): ary.par.xvalues=xvalues
    if not (units == None):
        if type(units)==str: ary.setUnit("",units)
        elif type(units)==tuple: ary.setUnit(*units)
        else: print "Error - hArray: Wrong format for units specified."
    if not (fill == None):
        if type(fill) in hAllVectorTypes: ary.vec().fill(fill)
        if type(fill) in [tuple,list]:
            if len(fill)>0: ary.vec().fill(Vector(fill))
        else: ary.fill(fill)
    if type(name)==str: ary.setKey("name",name);
    return ary

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
    ary.par=self.par
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
    unchanged. Raise ValueError otherwise.
    """
    N_new = reduce(lambda x, y : x*y, dimensions)
    N_old = reduce(lambda x, y : x*y, self.shape())

    if N_new == N_old:
        apply(self.setDimensions,dimensions)
    else:
        raise ValueError("total size of new array must be unchanged")

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
    if type(s)==int:
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

def hArray_copy_resize(self,ary):
    """
    Retrieve the first element of the currently selected slice from the stored vector.
    """
    ary.resize(len(self))
    ary.copy(self)
    return ary

def hArray_getitem(self,indexlist):
    """
    self[n1,n2,n3]-> return Element with these indices

    Retrieves a copy of the array with the internal slices set to reflect ...
    integers, or value if slice contains only one value.

    Use array.reshape([dim1,dim2,...,dimN]) to set the dimensions.
    """

    ary=self.getSlicedArray(indexlist)
    size=ary.getSize()

    if size == 0:
        raise IndexError("index out of bounds")
    elif size == 1:
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

    if type(indexlist)==tuple: indexlist=list(indexlist)
    else: indexlist=[indexlist]
    ary=hArray(self)
    ary.par=self.par
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

    """
    if (type(fill)) in hAllListTypes: fill=hArray(fill)
    hFill(hArray_getSlicedArray(self,dims),fill)

def hArray_read(self,datafile,key):
    """
    array.read(file,"Time") -> read key Data Array "Time" from file into array.

    Will also set the attributes par.file and par.filename of the array and
    make a history entry.
    """
    self.par.filename=datafile.filename
    self.addHistory("read","Reading data from file "+self.par.filename)
    self.par.file=datafile.read(key,self)
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

def hArray_setUnit(self,*arg):
    self.setUnit_(*arg)
    return self

def hTranspose(self,ary):
    """
    Usage: aryT.transpose(ary)

    Transpose a 2-dimensional array into another 2-dimensional
    array. This is a simple interface to the function hRedistribute
    and is not well suited to deal with slicing and looping.

    See also: hRedistribute.

    Example:

    >> ary=hArray(int,[3,4],fill=range(12))
    >> ary[...].pprint(-1)

    [0,1,2,3]
    [4,5,6,7]
    [8,9,10,11]

    >> aryT=hArray(int,[4,3])
    >> hTranspose(aryT,ary)
    >> aryT[...].pprint(-1)

    [0,4,8]
    [1,5,9]
    [2,6,10]
    [3,7,11]
    """
    dim2=ary.shape()[-2]
    self.redistribute(ary[...],Vector(range(dim2)),Vector(int,dim2,fill=dim2))

def ishArray(ary):
    """
    ishArray(array) -> True or False

    Returns true if the argument is one of the hArray arrays, i.e. those listed in hAllVectorTypes.
    """
    return type(ary) in hAllArrayTypes

def hArray_toNumpy(self):
    """Returns a copy of the array as a numpy.ndarray object with the correct dimensions."""
    return np.asarray(self.vec()).reshape(self.shape())

# Fourier Transforms
setattr(FloatArray,"fft",hFFTCasa)

for v in hAllArrayTypes:
    setattr(v,"__repr__",hArray_repr)
    setattr(v,"toNumpy", hArray_toNumpy)
    setattr(v,"setPar",hArray_setPar)
    setattr(v,"shape",hArray_shape)
    setattr(v,"reshape",hArray_reshape)
    setattr(v,"getDim",hArray_shape) # Depricated
    setattr(v,"setDim",hArray_reshape) # Depricated
    setattr(v,"vec",hArray_vec)
    setattr(v,"val",hArray_val)
    setattr(v,"new",hArray_new)
    setattr(v,"none",hArray_none)
    setattr(v,"read",hArray_read)
    setattr(v,"list",hArray_list)
    setattr(v,"transpose",hTranspose)
    setattr(v,"plt",plt)
    setattr(v,"plot",hPlot_plot)
    setattr(v,"copy_resize",hArray_copy_resize)
    setattr(v,"newreference",hArray_newreference)
    setattr(v,"getSlicedArray",hArray_getSlicedArray)
    setattr(v,"__getitem__",hArray_getitem)
    setattr(v,"__setitem__",hArray_setitem)
    setattr(v,"setUnit",hArray_setUnit)

for v in hAllContainerTypes:
    for s in ["hFill","hSet","hFlip","hCopy","hSort","hZipper","hReadDump","hWriteDump","hRedistribute","hPPrint","hPrettyString"]:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning: function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hRealContainerTypes:
    for s in ["hMean","hMeanAbs","hMeanSquare","hStdDev","hMeanThreshold","hMeanInverse","hDownsample","hUpsample","hDownsampleSpikyData","hInterpolate2P","hInterpolate2PSubpiece","hNegate","hVectorLength","hNormalize","hAcos","hAsin","hAtan","hCeil","hFloor","hMeanGreaterThanThreshold","hMeanGreaterEqualThreshold","hMeanLessThanThreshold","hMeanLessEqualThreshold","hFindGreaterThan","hFindGreaterEqual","hFindGreaterThanAbs","hFindGreaterEqualAbs","hFindLessThan","hFindLessEqual","hFindLessThanAbs","hFindLessEqualAbs","hCountGreaterThan","hCountGreaterEqual","hCountGreaterThanAbs","hCountGreaterEqualAbs","hCountLessThan","hCountLessEqual","hCountLessThanAbs","hCountLessEqualAbs","hFindBetween","hFindBetweenOrEqual","hFindOutside","hFindOutsideOrEqual","hRunningAverage","hDelayToPhase","hInvFFTCasa","hFFTw","hInvFFTw","hSaveInvFFTw","hGetHanningFilter","hApplyHanningFilter","hGetHanningFilterHalf","hSpectralPower","hRFIDownsampling","hRFIBaselineFitting","hRFIFlagging","hLinearFitPolynomialX","hLinearFit","hBSplineFitXValues","hBSpline","hBSplineFit","hErrorsToWeights","hPolynomial"]:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning: function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hComplexContainerTypes:
    for s in ["hConj","hCrossCorrelateComplex","hFFTCasa","hInvFFTw","hSaveInvFFTw","hFFTw","hNyquistSwap","hPhaseToComplex","hAmplitudePhaseToComplex","hRFIDownsampling","hSetAmplitude"]:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning: function ",s," is not defined. Likely due to a missing library in hftools.cc."

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
    for s in ["hFillRange","hAbs","hMax","hMaxPos","hMin","hMinPos","hConvert","hConvertResize","hMul","hDiv","hSub","hAdd","hMulTo","hDivTo","hSubTo","hAddTo","hMulAdd","hDivAdd","hSubAdd","hAddAdd","hArg","hImag","hNorm","hReal","hCos","hCosh","hExp","hLog","hLog10","hLogSave","hSin","hSinh","hSqrt","hSquare","hTan","hTanh","hSum","hSumAbs","hSumSquare","hMulSum","hRandom","hSortMedian","hMedian","hFindLowerBound","hCrossProduct","hFmod","hMulAdd2","hDivAdd2","hSubAdd2","hAddAdd2"]:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning: function ",s," is not defined. Likely due to a missing library in hftools.cc."

#======================================================================
#  Pretty Printing
#======================================================================

#Some definitons to make pretty (and short) output of vectors
def VecToString(self,maxlen=10):
    veclen=len(self)
    stringval=""
    if veclen>0:
        stringval+=str(self[0])
    for i in range(1,min(len(self),maxlen)):
        stringval+=","+str(self[i])
    if veclen>maxlen:
        stringval+=",..."
    return stringval


def VecToPrintString(self,maxlen=5):
    s=typename(basetype(self))+","
    return "Vec("+s+str(len(self))+")=[" +VecToString(self,maxlen)+"]"

def hArrayToPrintString(self,maxlen=5):
    s=typename(basetype(self))+", "
    loops=""
    if self.loopingMode(): loops="*"
    name=self.getKey("name");
    if name=="": name="hArray";
    return name+"("+s+str(self.shape())+"="+str(len(self))+", ["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+") -> [" +VecToString(self.getVector()[self.getBegin():self.getEnd()],maxlen)+"]"

def hArray_repr(self,maxlen=8,long=True):
    loops=""
    if self.loopingMode(): loops="*"
    name=self.getKey("name")
    if not name=="": name=', name="'+name+'"'
    s="hArray("+hTypeNamesDictionary[basetype(self)]
    if long:
        s+=","+str(list(self.shape()))+name+") # len="+str(len(self))+", slice=["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+", vec -> [" +VecToString(self.getVector()[self.getBegin():self.getEnd()],maxlen)+"]"
    else:
        s+=")"
    return s

#======================================================================
#  Define Plotting functions for vectors and arrays
#======================================================================

def hPlot_plot(self,xvalues=None,xlabel=None,ylabel=None,title=None,clf=True,logplot=None,xlim=None,ylim=None,legend=None):
    """
    array[0].plot(self,xvalues=vec,xlabel="x",ylabel="y",title="Title",clf=True,logplot="xy") -> plot the array (loglog)

    Method of arrays. Plots the current slice. If the array is in
    looping mode, multiple curves are plotted in one windows.

    You can set the plotting parameters also as attributes to the .par
    class of the array, e.g., "array.par.xvalues=x_vector; array.plot()"

    Parameters:

    xvalues: an array with corresponding x values, if "None" numbers
    from 0 to length of the array are used

    xlabel: the x-axis label, if not specified, use the "name" keyword
    of the xvalues array - units will be added automatically

    ylabel: the y-axis label, if not specified, use the "name" keyword
    of the array - units will be added automatically

    xlim: tuple with minimum and maximum limits for the x-axis

    ylim: tuple with minimum and maximum limits for the y-axis

    legend: plots a legen taking a tuple of strings for each plotted
    line as input, e.g. legend=("A","B",...)

    title: a title for the plot

    clf: if True (default) clear the screen beforehand (use False to
    compose plots with multiple lines from different arrays.

    logplot: can be used to make loglog or semilog plots:
            "x" ->semilog in x
            "y" ->semilog in y
            "xy"->loglog plot
    """
    if xvalues==None:
        if hasattr(self.par,"xvalues"): xvalues=self.par.xvalues
        else: xvalues=hArray(range(len(self.vec())))
    xunit=xvalues.getUnit().replace("\\mu","$\\mu$")
    if not xunit=="": xunit=" ["+xunit+"]"
    yunit=self.getUnit().replace("\\mu","$\\mu$")
    if not yunit=="": yunit=" ["+yunit+"]"
    if xlabel==None: xlabel=xvalues.getKey("name")
    if ylabel==None: ylabel=self.getKey("name")
    var="clf"; dflt=True; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="xlabel"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="ylabel"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="xlim"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="ylim"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="legend"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    var="title"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        elif hasattr(self.par,"filename"): exec(var+"=self.par.filename")
        else: exec("var=dflt")
    var="logplot"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec("var=dflt")
    if clf: self.plt.clf()
    if logplot=="x": _plot=self.plt.semilogx
    elif logplot=="y": _plot=self.plt.semilogy
    elif (logplot=="xy") | (logplot=="yx"): _plot=self.plt.loglog
    else: _plot=self.plt.plot
    iterate=True;
    while (iterate):
        _plot(xvalues.vec(),self.vec())
        xvalues.next()
        iterate=self.next().doLoopAgain()
    self.plt.ylabel(ylabel+yunit)
    self.plt.xlabel(xlabel+xunit)
    if not xlim==None: self.plt.xlim(*xlim)
    if not ylim==None: self.plt.ylim(*ylim)
    if not legend==None: self.plt.legend(legend)
    if not ((title=="") | (title==None)): self.plt.title(title)

