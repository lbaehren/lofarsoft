"""Basic types for Vectors and hArrays.
"""

import numpy as np
import matplotlib.pyplot as plt

from hftools import *
from types import *

#========================================================================
# Convenience Vector Constructor
#========================================================================

def Vector(Type=float,size=-1,fill=None):
    """
    The basic Boost Python STL vector constructor takes no arguments and
    hence is a litte cumbersome to use. Here we provide a wrapper function
    that is useful for interactive use. If speed is of the essence, use
    the original vector constructors: BoolVec(), IntVec(), FloatVec(),
    ComplexVec(), StringVec()

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

    Vector() defaults to a float vector.

    Note, that size and fill take precedence over the list and tuple
    input. Hence if you create a vector with Vector([1,2,3],size=2) it
    will contain only [1,2]. Vector([1,2,3],size=2,fill=4) will give
    [4,4].
    """
    
    vtype=Type
    if type(size) in hListAndArrayTypes: size=len(size)
    if (type(vtype) in hAllArrayTypes):  # hArrayClass
        vtype=basetype(Type)
        vec=Vector(Type.vec())
    elif (type(vtype) in hAllListTypes):  #List or Vector
        vtype=type(Type[0])
        vec=type2vector(vtype)
        vec.extend(Type)
    elif type(vtype) == np.ndarray:  #List or Vector
        Type=list(Type.ravel())
        vtype=type(Type[0])
        vec=type2vector(vtype)
        vec.extend(Type)
    else:
        vec=type2vector(vtype)
    vec.type=vtype
    if (size>=0): vec.resize(size)
    if type(fill) in [tuple,list]:
        if len(fill)>0: fill=Vector(fill)
        else: fill=None
    if type(fill) in hAllArrayTypes: fill=fill.vec()
    if (not fill==None):
        vec.fill(fill)
    return vec

def multiply_list(l):
    """
    multiply_list([n1,n2,...]) -> n1*n2*n3 ....

    Multiplies all elements of a list with each other and returns the
    result.
    """
    return reduce(lambda x,y:x*y,l)

def hVector_list(self):
    """
    vec.list() -> [x1,x2,x3, ...]

    Retrieve the STL vector as a python list.
    """
    return list(self)

def hVector_val(self):
    """
    vec.val() -> a             : if length == 1
    vec.val() -> [a,b,c,...]   : if length > 1

    Retrieve the contents of the vector as python values: either as a
    single value, if the vector just contains a single value, or
    otherwise return a python list.
    """
    if len(self)==1: return self[0]
    else: return list(self)

def hVector_elem(self,n):
    """
    self.elem(n) -> nth element the vector

    """
    print n
    return self[n]

def hVector_vec(self):
    """
     vector.vec() -> vector Convenience method that allows one to
    treat hArrays and hVectors in the same way, i.e. using the vec()
    method returns the intrinsic vector for both.
    """
    return self;

def extendflat(self,l):
    """
    vector.extendflat([[e1-1,e1-2,...],[e2-1,e2-2,...],..]) -> [e1-1,e1-2,...,e2-1,e2-2,...]

    Appending all elements in a list of lists to a one-dimensional
    vector with a flat dat structure (just 1D).
    """
    map(lambda x:self.extend(x),l)
    return self

def isVector(vec):
    """
    isVector(vec) -> True or False

    Returns true if the argument is one of the standard c++ vectors i.e. those liste in hAllVectorTypes.
    """
    return type(vec) in hAllVectorTypes

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
    return name+"("+s+str(self.getDim())+"="+str(len(self))+", ["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+") -> [" +VecToString(self.getVector()[self.getBegin():self.getEnd()],maxlen)+"]"

def hArray_repr(self,maxlen=8,long=True):
    loops=""
    if self.loopingMode(): loops="*"
    name=self.getKey("name")
    if not name=="": name=', name="'+name+'"'
    s="hArray("+hTypeNamesDictionary[basetype(self)]
    if long:
        s+=","+str(list(self.getDim()))+name+") # len="+str(len(self))+", slice=["+str(self.getBegin())+":"+str(self.getEnd())+"]"+loops+", vec -> [" +VecToString(self.getVector()[self.getBegin():self.getEnd()],maxlen)+"]"
    else:
        s+=")"
    return s

#======================================================================
#  Vector Methods/Attributes
#======================================================================

"""
Here we add the functions defined in the hftools.cc as
methods/attributes to the (STL) vector classes in python.
"""

Vectordoc= """
A number of vector types are provided: bool, int, float, complex, and
str.

This vector is subscriptable and sliceable, using the
standard python syntax, e.g. v[1:3], or v[2]=0.

To creat a vector most efficently, use the original vector
constructors: BoolVec() - IntVec() - FloatVec() - ComplexVec() -
StringVec()

There is also a more convenient constructor function "Vector" that is
useful for interactive use:

Vector(Type) , where Type=float,int,complex,str, or bool
Vector(Type,size)
Vector(Type,size,fill)
Vector(list), Vector(tuple)
Vector() defaults to a float vector

Use the resize method to allocate more or less memory.

Some of the basic arithmetic is available in an intuitve way, e.g.,
vec+1, vec1+vec2, vec+=vec2).

See the tutorial (tutorial.py) for more details.
"""

IntVec.__doc__="c++ standard template library (STL) vector of type 'int'\n"+Vectordoc
FloatVec.__doc__="c++ standard template library (STL) vector of type 'float'\n"+Vectordoc
StringVec.__doc__="c++ standard template library (STL) vector of type 'str'\n"+Vectordoc
BoolVec.__doc__="c++ standard template library (STL) vector of type 'bool'\n"+Vectordoc
ComplexVec.__doc__="c++ standard template library (STL) vector of type 'complex'\n"+Vectordoc

IntVec.__name__="IntVec"
FloatVec.__name__="FloatVec"
BoolVec.__name__="BoolVec"
ComplexVec.__name__="ComplexVec"
StringVec.__name__="StringVec"

# Operator:  +=
def Vec_iadd(vec1,vec2):
    """
    Provides the += operator for adding two vectors in place.
    vec1+=vec2 will add all elements in vec2 to the corresponding
    elements in vec1 and store the result in vec1.
    """
    hAdd(vec1,vec2)
    return vec1

# Operator:  -=
def Vec_isub(vec1,vec2):
    "Provides the -= operator for addig two vectors in place. \
    vec1-=vec2 will subtract all elements in vec2 from the corresponding elements in vec1\
    and store the result in vec1."
    hSub(vec1,vec2)
    return vec1

# Operator:  /=
def Vec_idiv(vec1,vec2):
    "Provides the /= operator for addig two vectors in place. \
    vec1/=vec2 will divide all elements in vec1 by the corresponding elements in vec2\
    and store the result in vec1."
    hDiv(vec1,vec2)
    return vec1

# Operator:  *=
def Vec_imul(vec1,vec2):
    "Provides the *= operator for addig two vectors in place. \
    vec1=vec2 will multiply all elements in vec1 with the corresponding elements in vec2\
    and store the result in vec1."
    hMul(vec1,vec2)
    return vec1

# Operator:  +
def Vec_add(vec1,val):
    "Provides the + operator for adding two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hAdd(vecout,vec1,val)
    return vecout

# Operator:  -
def Vec_sub(vec1,val):
    "Provides the - operator for subtracting two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hSub(vecout,vec1,val)
    return vecout

# Operator:  *
def Vec_mul(vec1,val):
    "Provides the * operator for multiplying two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hMul(vecout,vec1,val)
    return vecout

# Operator:  /
def Vec_div(vec1,val):
    "Provides the / operator for dividing two vectors or a vector by a scalar. The result will be a new vector."
    vecout=vec1.new()
    hDiv(vecout,vec1,val)
    return vecout

# Fourier Transforms
setattr(FloatVec,"fft",hFFTCasa)

for v in hAllVectorTypes:
    setattr(v,"__repr__",VecToPrintString)
    setattr(v,"extendflat",extendflat)
    setattr(v,"elem",hVector_elem)
    setattr(v,"val",hVector_val)
    setattr(v,"vec",hVector_vec)
    setattr(v,"list",hVector_list)
    for s in ["hResize","hNew"]:
        setattr(v,s[1:].lower(),eval(s))

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

