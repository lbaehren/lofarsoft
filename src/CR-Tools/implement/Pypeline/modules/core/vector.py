"""Basic types for Vectors and hArrays.
"""

import numpy as np

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
    # Check the 'type' parameter
    if type(size) in hListAndArrayTypes:
        size=len(size)
    elif isinstance(size, int) or isinstance(size, long):
        size=size
    else:
        raise TypeError("size must be a list or vector or a scalar of type integer")

    if (type(vtype) in hAllArrayTypes):  # hArrayClass
        vtype=basetype(Type)
        vec=Vector(Type.vec())
    elif (type(vtype) in hAllListTypes):  # List or Vector
        vtype=type(Type[0])
        vec=type2vector(vtype)
        vec.extend(Type)
    elif type(vtype) == np.ndarray:  # Numpy ndarray
        if vtype.dtype == np.dtype('float'):
            vec = FloatVec()
        elif vtype.dtype == np.dtype('complex'):
            vec = ComplexVec()
        elif vtype.dtype == np.dtype('int'):
            vec = IntVec()
        else:
            raise TypeError("No available copy constructor for dtype: '"+str(vtype.dtype)+"'")

        vec.resize(vtype.size)
        hCopy(vec, vtype)
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
def VecToString(self, maxlen=100):
    if len(self) == 0:
        return "[]"
    elif len(self) <= maxlen:
        s = "["
        for i in range(len(self)-1):
            s += str(self[i]) + ", "
        s += str(self[len(self)-1]) + "]"
    else:
        s = "[" + str(self[0]) + ", " + str(self[1]) + ", " + str(self[2]) + ", ..., " + str(self[-3]) + ", " + str(self[-2]) + ", " + str(self[-1]) + "]"

    return s

def hVector_repr(self,maxlen=100):
    return "Vector("+typename(basetype(self))+", "+str(len(self))+", "+VecToString(self, maxlen)+")"

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

# Pickling
def hVector_getinitargs(self):
    """Get arguments for hVector constructor.

    .. warning:: This is not the hVector factory function but the constructor for the actual type. E.g. :meth:`IntArray.__init__`

    """

    return ()

def hVector_getstate(self):
    """Get current state of hVector object for pickling.
    """

    return (len(self), hWriteRawVector(self))

def hVector_setstate(self, state):
    """Restore state of hVector object for unpickling.
    """

    self.resize(state[0])
    hReadRawVector(self, state[1])

# Fourier Transforms
setattr(FloatVec,"fft",hFFTCasa)

for v in hAllVectorTypes:
    setattr(v,"__repr__",hVector_repr)
    setattr(v,"__getinitargs__",hVector_getinitargs)
    setattr(v,"__getstate__",hVector_getstate)
    setattr(v,"__setstate__",hVector_setstate)
    setattr(v,"__getstate_manages_dict__",1)
    setattr(v,"extendflat",extendflat)
    setattr(v,"elem",hVector_elem)
    setattr(v,"val",hVector_val)
    setattr(v,"vec",hVector_vec)
    setattr(v,"list",hVector_list)
    for s in hAllVectorMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hAllVectorMethods: function ",s," is not defined. Likely due to a missing library in hftools.cc."
    
for v in hAllContainerTypes:
    for s in hAllContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hAllContainerMethods(v): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hRealContainerTypes:
    for s in hRealContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hRealContainerMethods(v): function ",s," is not defined. Likely due to a missing library in hftools.cc."

for v in hComplexContainerTypes:
    for s in hComplexContainerMethods:
        if s in locals(): setattr(v,s[1:].lower(),eval(s))
        else: print "Warning hComplexContainerTypes(v): function ",s," is not defined. Likely due to a missing library in hftools.cc."

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
        else: print "Warning hNumericalContainerMethods(v): function ",s," is not defined. Likely due to a missing library in hftools.cc."

