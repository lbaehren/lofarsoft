""" objects -- This module defines a class and set of functions for
manipulating arrays of Python objects.  It defines functions which
correspond to most of the operators defined in Python's operator
module, and provides names compatible with most of numarray's
universal functions.

>>> a = fromlist(["t","u","w"])
>>> a
ObjectArray(['t', 'u', 'w'])
>>> a+a
ObjectArray(['tt', 'uu', 'ww'])
>>> add(a,a)
ObjectArray(['tt', 'uu', 'ww'])
>>> b=a.copy()
>>> add(a,a,b)
>>> b
ObjectArray(['tt', 'uu', 'ww'])
>>> a
ObjectArray(['t', 'u', 'w'])
>>> a[1:2]+a[2:]
ObjectArray(['uw'])
>>> a = fromlist(["a","b","c"])
>>> add.areduce(a)
ObjectArray('abc')
>>> add.reduce(a)
'abc'
>>> add.reduce(a[1:])
'bc'
>>> add.accumulate(a[1:])
ObjectArray(['b', 'bc'])
>>> b = _gen._broadcast(a, (2,5,3))
>>> add.reduce(b, dim=0)
ObjectArray([['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc']])
>>> add.areduce(b, dim=0)
ObjectArray([['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc'],
             ['aa', 'bb', 'cc']])
>>> add.reduce(b, dim=1)
ObjectArray([['aaaaa', 'bbbbb', 'ccccc'],
             ['aaaaa', 'bbbbb', 'ccccc']])
>>> add.areduce(b, dim=1)
ObjectArray([['aaaaa', 'bbbbb', 'ccccc'],
             ['aaaaa', 'bbbbb', 'ccccc']])
>>> add.reduce(b, dim=2)
ObjectArray([['abc', 'abc', 'abc', 'abc', 'abc'],
             ['abc', 'abc', 'abc', 'abc', 'abc']])
>>> add.areduce(b, dim=2)
ObjectArray([['abc', 'abc', 'abc', 'abc', 'abc'],
             ['abc', 'abc', 'abc', 'abc', 'abc']])
>>> _dtp(add.accumulate(b, dim=0))
. ObjectArray([[['a', 'b', 'c'],
.               ['a', 'b', 'c'],
.               ['a', 'b', 'c'],
.               ['a', 'b', 'c'],
.               ['a', 'b', 'c']],
.
.              [['aa', 'bb', 'cc'],
.               ['aa', 'bb', 'cc'],
.               ['aa', 'bb', 'cc'],
.               ['aa', 'bb', 'cc'],
.               ['aa', 'bb', 'cc']]])
>>> _dtp(add.accumulate(b, dim=1))
. ObjectArray([[['a', 'b', 'c'],
.               ['aa', 'bb', 'cc'],
.               ['aaa', 'bbb', 'ccc'],
.               ['aaaa', 'bbbb', 'cccc'],
.               ['aaaaa', 'bbbbb', 'ccccc']],
.
.              [['a', 'b', 'c'],
.               ['aa', 'bb', 'cc'],
.               ['aaa', 'bbb', 'ccc'],
.               ['aaaa', 'bbbb', 'cccc'],
.               ['aaaaa', 'bbbbb', 'ccccc']]])
>>> _dtp(add.accumulate(b, dim=2))
. ObjectArray([[['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc']],
.
.              [['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc'],
.               ['a', 'ab', 'abc']]])

>>> a=fromlist(range(1,5))
>>> subtract(a,a)
ObjectArray([0, 0, 0, 0])
>>> divide(a,a)
ObjectArray([1, 1, 1, 1])
>>> remainder(a,2)
ObjectArray([1, 0, 1, 0])

>>> abs(range(-5,5))
ObjectArray([5, 4, 3, 2, 1, 0, 1, 2, 3, 4])

>>> import numarray as _num
>>> a = _num.zeros((3,2))
>>> b = _num.ones((3,2))
>>> c = fromlist([["x","y"],["z","w"]])
>>> c[a,a]
ObjectArray([['x', 'x'],
             ['x', 'x'],
             ['x', 'x']])
>>> c[a,b]
ObjectArray([['y', 'y'],
             ['y', 'y'],
             ['y', 'y']])
>>> c[b,a]
ObjectArray([['z', 'z'],
             ['z', 'z'],
             ['z', 'z']])
>>> c[b,b]
ObjectArray([['w', 'w'],
             ['w', 'w'],
             ['w', 'w']])
>>> a=fromlist(["x"]*25, shape=(5,5)); a
ObjectArray([['x', 'x', 'x', 'x', 'x'],
             ['x', 'x', 'x', 'x', 'x'],
             ['x', 'x', 'x', 'x', 'x'],
             ['x', 'x', 'x', 'x', 'x'],
             ['x', 'x', 'x', 'x', 'x']])
>>> b=arange(5)
>>> a[b,b]  = 'y'; a
ObjectArray([['y', 'x', 'x', 'x', 'x'],
             ['x', 'y', 'x', 'x', 'x'],
             ['x', 'x', 'y', 'x', 'x'],
             ['x', 'x', 'x', 'y', 'x'],
             ['x', 'x', 'x', 'x', 'y']])
>>> a[b,b]
ObjectArray(['y', 'y', 'y', 'y', 'y'])

True and Floor division

>>> truediv(fromlist(range(10)), 4)
ObjectArray([0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25])
>>> floordiv(fromlist(range(10)), 4)
ObjectArray([0, 0, 0, 0, 1, 1, 1, 1, 2, 2])



Inplace operators

>>> a = fromlist(["a"]*3);  a += "b";   a *= 2;   a
ObjectArray(['abab', 'abab', 'abab'])

Unary operators

>>> -fromlist(range(3))
ObjectArray([0, -1, -2])
>>> +fromlist(range(3))
ObjectArray([0, 1, 2])
>>> ~fromlist(range(3))
ObjectArray([-1, -2, -3])


Put / Take

>>> a=fromlist(arange(20),shape=(4,5)); a
ObjectArray([[0, 1, 2, 3, 4],
             [5, 6, 7, 8, 9],
             [10, 11, 12, 13, 14],
             [15, 16, 17, 18, 19]])
>>> take(a,(-1,))
ObjectArray([[15, 16, 17, 18, 19]])
>>> take(a,(3,))
ObjectArray([[15, 16, 17, 18, 19]])
>>> take(a, (0,1))
ObjectArray([[0, 1, 2, 3, 4],
             [5, 6, 7, 8, 9]])
>>> take(a, (0,-1))
ObjectArray([[0, 1, 2, 3, 4],
             [15, 16, 17, 18, 19]])
>>> take(a, (arange(4),0), axis=(0,))
ObjectArray([0, 5, 10, 15])
>>> x=fromlist(arange(10.)*100)
>>> take(x,[[2,4],[1,2]])
ObjectArray([[200.0, 400.0],
             [100.0, 200.0]])
>>> a0=fromlist(arange(100, shape=(10,10)))
>>> take(a0, 1000)
Traceback (most recent call last):
...
IndexError: Index out of range
>>> b=arange(5)
>>> put(a,(3,3),1,axis=(0,)); a
ObjectArray([[0, 1, 2, 3, 4],
             [5, 6, 7, 8, 9],
             [10, 11, 12, 13, 14],
             [15, 16, 17, 1, 19]])
>>> put(a,(3,b),0,axis=(0,)); a
ObjectArray([[0, 1, 2, 3, 4],
             [5, 6, 7, 8, 9],
             [10, 11, 12, 13, 14],
             [0, 0, 0, 0, 0]])
>>> a0=fromlist(arange(100, shape=(10,10)))
>>> put(a0, 1000, 1)
Traceback (most recent call last):
...
IndexError: Index out of range
>>> a=fromlist(arange(12*2),shape=(4,3,2))
>>> _dtp(a)
. ObjectArray([[[0, 1],
.               [2, 3],
.               [4, 5]],
.
.              [[6, 7],
.               [8, 9],
.               [10, 11]],
.
.              [[12, 13],
.               [14, 15],
.               [16, 17]],
.
.              [[18, 19],
.               [20, 21],
.               [22, 23]]])
>>> b=take(a, (((1,1),(2,2)), ((1,2),(1,2))), axis=(0,) )
>>> _dtp(b)
. ObjectArray([[[8, 9],
.               [10, 11]],
.
.              [[14, 15],
.               [16, 17]]])

SF# [ 924841 ] numarray.objects rank-0 results

>>> abs(-1.0)
1.0
>>> add(1,1)
2
>>> add.reduce(range(10))
45
>>> add.areduce(range(10))
ObjectArray(45)

Array rank is determined by the first object which does not match the
type of the original sequence.  

>>> array([[[1,2,3],(99,100)]])
ObjectArray([[[1, 2, 3], (99, 100)]])
>>> array([_nc.array(1),[[1,2,3],(99,100)]])
ObjectArray([array(1), [[1, 2, 3], (99, 100)]])

Rank can also be explicitly specified.

>>> array([[1,2,3],[4,5,6]]).rank
2
>>> array([[1,2,3],[4,5,6]], rank=1).rank
1
"""
import generic as _gen
import memory  as _mem
import operator as _op
import numarraycore as _nc
import numerictypes as _nt
import _objectarray
from numtest import dtp as _dtp
import types as _types

from numarraycore import arange
from generic import put, take

_PROTOTYPE = 0   # Set _PROTOTYPE to 1 for Python prototype code.
                 # Set _PROTOTYPE to 0 for accelerated C-code.

# =======================================================================
#  object store _objectarray

def _flatten(objects, rank):
    
    """_flatten() converts a nested list into a 1D list."""
    if rank == 0:
        return [objects]
    elif rank == 1:
        return objects
    else:
        l = []
        for o in objects:
            l.extend(_flatten(o, rank-1))
        return l

def _getRank(s):
    if isinstance(s, type("")):
        return 0
    try:
        l = len(s)
    except TypeError:
        return 0
    except AttributeError:
        return 0
    else:
        t = type(s)
        for o in s:
            if type(o) != t:
                return 1
        if (l > 0):
            return 1 + _getRank(s[0])
        else:
            return 1
    
def _checkSequenceConsistency(s, rank):
    if rank > 1:
        l = len(s[0])
        for o in s:
            if len(o) != l:
                raise ValueError("Inconsistent sequence shapes")
            _checkSequenceConsistency(o, rank-1)
                
def _shapeFromNestedSequence(s, rank, firstcall=1):    
    """_shapeFromNestedSequence() determines the shape of a nested
    sequence"""

    if firstcall:
        _checkSequenceConsistency(s, rank)

    if rank == 0:
        return []
    elif rank == 1:
        return [len(s)]
    else:
        if isinstance(s, type("")):
            return []
        try:
            l = len(s)
        except TypeError:
            return []            
        except AttributeError:
            return []            
        return [l] + _shapeFromNestedSequence(s[0], rank-1, 0)

def _stridesFromShape( shape ):
    
    """_stridesFromShape() computes the strides which correspond to
    the specified shape."""

    if len(shape) > 1:
        head = [_gen.product(shape[1:])]
        tail = _stridesFromShape(shape[1:])
        head.extend(tail)
        return head
    else:
        return [1]


_dummyBuffer = _mem.new_memory(0)
    
if _PROTOTYPE:
    class _objarr(_gen.NDArray):
        def __init__(self, shape, objects=None):
            if objects is None:
                objects = [None] * _gen.product(shape)
            _gen.NDArray.__init__(
                    self, shape=shape, buffer=_dummyBuffer, itemsize=1)
            self._objects = objects[:]

        def _getitem(self, offset):
            return self._objects[ offset ]

        def _setitem(self, offset, value):
            self._objects[offset] = value

        def _copyObjects(self, other, selfoffset, ooffset, dim):

            """_copyObjects() recurively copies from a sequence sequence to
            an array object."""
            
            dlen = len(self._shape)
            
            if dlen == dim:  # handle rank-0
                self._objects[selfoffset] = other._objects[ooffset]
            elif dlen > dim+1:  # handle higher dimensions
                for i in xrange(self._shape[dim]):
                    self._copyObjects( other,
                                       selfoffset + i*self._strides[dim],
                                       ooffset + i*other._strides[dim],
                                       dim+1)
            else:  # handle final dimension w/o elementwise recursion
                for i in xrange(self._shape[dim]):
                    self._objects[selfoffset+self._strides[dim]*i] = \
                           other._objects[ooffset+other._strides[dim]*i]

    def _applyObjects1(f, in1, out):
        
        """_applyObjects2() applies function 'f' elementwise to
        inputs 'in1' and 'in2', storing the result in 'out'."""
        
        if isinstance(in1, ObjectArray) and len(in1._shape) > 1:
            for i in xrange(len(in1)):
                _applyObjects1(f, in1[i], out[i])
        else:
            for i in xrange(len(in1)):
                out[i] = f(in1[i])

    def _applyObjects2(f, in1, in2, out):
        
        """_applyObjects2() applies function 'f' elementwise to
        inputs 'in1' and 'in2', storing the result in 'out'."""
        
        if isinstance(in1, ObjectArray) and len(in1._shape) > 1:
            for i in xrange(len(in1)):
                _applyObjects2(f, in1[i], in2[i], out[i])
        else:
            for i in xrange(len(in1)):
                out[i] = f(in1[i], in2[i])

else:
    _objarr = _objectarray._objectarray
    from _objectarray import _applyObjects1, _applyObjects2
    
# =======================================================================
# object array class ObjectArrray

def _reduceObjects(f, in1, out):
    
    if out.rank > 1:
        for i in xrange(len(out)):
            _reduceObjects(f, in1[i], out[i])
    elif out.rank == 1:
        for i in xrange(len(out)):
            in1a = in1[i]
            temp = in1a[0]
            for j in xrange(1,len(in1a)):
                temp = f(temp, in1a[j])
            out[i] = temp
    else :
        temp = in1[0]
        for j in xrange(1, len(in1)):
            temp = f(temp, in1[j])
        out[()] = temp

def _accumulateObjects(f, in1, out):
    
    if len(out._shape) > 1:
        for i in xrange(len(out)):
            _accumulateObjects(f, in1[i], out[i])
    else:
        out[0] = temp = in1[0]
        for i in xrange(1, len(in1)):
            temp = f(temp, in1[i])
            out[i] = temp

def _fix_rank0(a):
    if a.rank is 0:
        return a[()]
    else:
        return a

class _UnaryObjectUfunc:
    def __init__(self, name, func):
        self.name = name
        self.func = func

    def __repr__(self):
        return "_UnaryObjectUfunc<" + repr(self.name) + ">"

    def __call__(self, ina, out=None):
        in1 = asarray(ina)
        if out is None:
            out1 = in1._clone(in1._shape)
        else:
            out1 = out
        _applyObjects1(self.func, in1, out1)
        if out is None:
            return _fix_rank0(out1)

class _BinaryObjectUfunc:
    def __init__(self, name, func):
        self.name = name
        self.func = func

    def __repr__(self):
        return "_BinaryObjectUfunc<" + repr(self.name) + ">"

    def __call__(self, ina, inb, out=None):
        in1 = asarray(ina)
        in2 = asarray(inb)
        in1, in2 = in1._dualbroadcast(in2)
        if out is None:
            out1 = in1._clone(in1._shape)
        else:
            out1 = out
        _applyObjects2(self.func, in1, in2, out1)
        if out is None:
            return _fix_rank0(out1)

    def areduce(self, a, out=None, dim=0):
        a = asarray(a)
        if dim == len(a._shape)-1: # Since last dim won't exist...
            dim = -1
        if dim != -1:
            a.swapaxes(-1, dim)
        if out is None:
            col = a[...,0].copy()
            out1 = col._clone(col._shape)
        else:
            out1 = out
            if dim != -1:
                out1.swapaxes(-1, dim)
        _reduceObjects(self.func, a, out1)
        if dim != -1:
            a.swapaxes(-1, dim)
            out1.swapaxes(-1, dim)
        if out is None:
            return out1

    def reduce(self, a, out=None, dim=0):
        result = self.areduce(a, out, dim)
        if out is None:
            return _fix_rank0(result)

    def accumulate(self, a, out=None, dim=0):
        a = asarray(a)
        if out is None:
            out1 = a._clone(a._shape)
        else:
            out1 = out
        if dim != -1:
            a.swapaxes(-1, dim)
            out1.swapaxes(-1, dim)
        _accumulateObjects(self.func, a, out1)
        if dim != -1:
            a.swapaxes(-1, dim)
            out1.swapaxes(-1, dim)
        if out is None:
            return out1

class ObjectArray(_objarr, _gen.NDArray):
    def __init__(self, objects=None, shape=None, rank=None):
        """
        >>> ObjectArray(shape=(1,))
        ObjectArray([None])
        >>> ObjectArray(objects=['t','u'])
        ObjectArray(['t', 'u'])
        >>> ObjectArray(objects=['t','u'], shape=(3,))
        Traceback (most recent call last):
        ...
        ValueError: object list is not compatible with shape
        """
        if rank is None:
            rank = _getRank(objects)

        if objects is not None:
            oshape = _shapeFromNestedSequence(objects, rank)
            if shape is None:
                shape = oshape
            else:
                if _gen.product(shape) != _gen.product(oshape):
                    raise ValueError(
                        "object list is not compatible with shape")
            objects = _flatten(objects, rank)
        else:
            if shape is None:
                shape = (1,)
                
        if isinstance(objects, _gen.NDArray) and shape == ():
            objects = [objects[()]]

        if not isinstance(shape, (list,tuple)):
            shape = (shape,)
            
        _objarr.__init__(self, shape, objects)
        
    def view(self):
        """view() returns a new array object which refers to the 'data'
        of the original array.
        """
        v = _gen.NDArray.view(self)
        v._objects = self._objects
        return v

    def __getstate__(self):
        state = _gen.NDArray.__getstate__(self)
        state["objectlist"] = self.tolist()
        return state

    def __setstate__(self, state):
        """
        >>> a = fromlist(["t","u",1])
        >>> import cPickle; s = cPickle.dumps(a)
        >>> cPickle.loads(s)
        ObjectArray(['t', 'u', 1])
        """
        self._objects = state["objectlist"]
        del state["objectlist"]
        _gen.NDArray.__setstate__(self, state)
        _objarr.__init__(self, self._shape,
                         _flatten(self._objects, self.rank))

    def __get_array_typestr__(self):
        return '|O%d' % self._itemsize

    __array_typestr__ = property(__get_array_typestr__, None, "")

    def _copyFrom(self, other):
        """_copyFrom copies another array or object into self.

        >>> a = ObjectArray(shape=(2,2)); a[:] = "t"; a
        ObjectArray([['t', 't'],
                     ['t', 't']])
        >>> b = fromlist(['u']); a[:] = b; a
        ObjectArray([['u', 'u'],
                     ['u', 'u']])
        >>> c = ObjectArray(objects=['t', 'u', \
                                     'w', 'v'], shape=(2,2))
        >>> a._copyFrom(c); a
        ObjectArray([['t', 'u'],
                     ['w', 'v']])
        """         
        if not isinstance(other, ObjectArray):
            other = fromlist([other])
        bother = self._broadcast(other)
        self._copyObjects( bother, self._byteoffset,
                           bother._byteoffset, 0)

    def _format(self, x):
        return repr(x)

    def isbyteswapped(self):
        return 0

    def copy(self):
        """copy() returns a copy of 'self' which has it's own object store
        referring to the same objects as 'self'.  Thus, modifications to
        the copy array do not affect the original, but modifications to the
        objects referred to by both arrays affect both arrays.
        """
        c = ObjectArray(shape=self._shape)
        c._copyFrom(self)
        return c
        # return ObjectArray(objects=self, shape=self._shape)

    def _clone(self, shape):
        """returns an empty array identical to 'self' but with 'shape'
        """
        c = self.view()
        c._shape = shape
        c._strides = _stridesFromShape(shape)
        c._byteoffset = 0
        c._objects = [None] * _gen.product(shape)
        return c

    def astype(self, type=None):
        """Convert ObjectArray 'self' to another array of 'type'.
        'type' can be Object, 'O',  or a numerical type.
        """
        if type == _nt.Object:
            return self.copy()
        else:
            a = _nc.NumArray(buffer=None, shape=self.shape, type=type)
            a.fromlist(self)
        return a
            
    def resize(self, size):
        raise NotImplementedError("Object array resize hasn't been implemented.")

    def tostring(self):
        raise TypeError("Can't convert and ObjectArray to a string.")

    def tofile(self):
        raise TypeError("Can't write an ObjectArray to a file.")


    def __add__(self, other):       return add(self, other)
    
    def __sub__(self, other):       return sub(self, other)
    
    def __mul__(self, other):       return mul(self, other)
    
    def __div__(self, other):       return div(self, other)
    
    def __truediv__(self, other):   return truediv(self, other)
    
    def __floordiv__(self, other):  return floordiv(self, other)
    
    def __mod__(self, other):       return mod(self, other)

    def __or__(self, other):        return or_(self, other)
    
    def __and__(self, other):       return and_(self, other)
    
    def __xor__(self, other):       return xor(self, other)

    def __lshift__(self, other):    return lshift(self, other)

    def __rshift__(self, other):    return rshift(self, other)

    def __pow__(self, other):       return pow(self, other)
    
    
    def __radd__(self, other):      return add(other, self)
    
    def __rsub__(self, other):      return sub(other, self)
    
    def __rmul__(self, other):      return mul(other, self)
     
    def __rdiv__(self, other):      return div(other, self)
    
    def __truediv__(self, other):   return truediv(other, self)
    
    def __floordiv__(self, other):  return floordiv(other, self)
    
    def __rmod__(self, other):      return mod(other, self)

    def __ror__(self, other):       return or_(other, self)
    
    def __rand__(self, other):      return and_(other, self)
    
    def __rxor__(self, other):      return xor(other, self)
    
    def __rlshift__(self, other):   return lshift(other, self)

    def __rrshift__(self, other):   return rshift(other, self)

    def __rpow__(self, other):      return pow(other, self)
    


    def __iadd__(self, other):      add(self, other, self); return self

    def __isub__(self, other):      sub(self, other, self); return self

    def __imul__(self, other):      mul(self, other, self); return self

    def __idiv__(self, other):      div(self, other, self); return self

    def __itruediv__(self, other):  truediv(self, other, self); return self

    def __ifloordiv__(self, other):  floordiv(self, other, self); return self

    def __imod__(self, other):      mod(self, other, self); return self

    def __ipow__(self, other):      pow(self, other, self); return self

    def __ilshift__(self, other):   lshift(self, other, self); return self

    def __irshift__(self, other):   rshift(self, other, self); return self

    def __iand__(self, other):      and_(self, other, self); return self

    def __ixor__(self, other):      xor(self, other, self); return self

    def __ior__(self, other):       or_(self, other, self); return self


    def __neg__(self):              return neg(self)
    
    def __pos__(self):              return pos(self)
    
    def __abs__(self):              return abs(self)
    
    def __invert__(self):           return invert(self)
    

    def __lt__(self, other):        return lt(self, other)
    
    def __le__(self, other):        return le(self, other)
    
    def __gt__(self, other):        return gt(self, other)
    
    def __ge__(self, other):        return ge(self, other)
    
    def __eq__(self, other):        return eq(self, other)
    
    def __ne__(self, other):        return ne(self, other)


    def info(self):
        print "class:", self.__class__
        print "shape:", self._shape
        print "strides:", self._strides
        print "byteoffset:", self._byteoffset

# =======================================================================
# object module functions

def array(sequence, shape=None, typecode='O', rank=None):
    if typecode != 'O':
        raise ValueError("typecode must be 'O'")
    return fromlist(sequence, shape=shape, rank=rank)

def asarray(obj, rank=None):
    if not isinstance(obj, ObjectArray):
        return ObjectArray(objects=obj, rank=rank)
    else:
        return obj

inputarray = asarray # Obsolete synonym.

def fromlist(l, shape=None, rank=None):
    """creates an object array from the objects in l.  If
    no shape is specified, it is implied by list nesting.

    >>> fromlist(["t",1,"u",2.0])
    ObjectArray(['t', 1, 'u', 2.0])
    >>> fromlist(["t","u","w",1], shape=(2,2))
    ObjectArray([['t', 'u'],
                 ['w', 1]])
    >>> fromlist([['t', 'u'],['w', 1]])                
    ObjectArray([['t', 'u'],
                 ['w', 1]])
    """
    return ObjectArray(objects=l, shape=shape, rank=rank)

def fromfile(file, shape=None):
    """Not supported for ObjectArray"""
    raise TypeError("ObjectArray can't be read from a file.")
 
def fromstring(s, shape=None):
    """Not supported for ObjectArray"""
    raise TypeError("ObjectArray can't be constructed from a string")

def _choose(selector, population, out):
    s0 = selector._shape[0]
    if len(selector._shape) == 1:
        for i in xrange(s0):
            out[i] = population[selector[i]][i]
    else:
        for i in xrange(s0):
            newpops = [ p[i] for p in population]
            newsel = selector[i]
            newout = out[i]
            _choose(newsel, newpops, newout)

def choose(selector, population, output = None):
    """choose() picks the elements of the tuple of arrays 'population'
    which are specified by the 'selector' array.  The shape of the
    output array is determined by the broadcasting the selector with
    all of the population arrays.

    >>> choose(_nc.arange(5)%2, ("t", 1))
    ObjectArray(['t', 1, 't', 1, 't'])
    >>> choose(_nc.arange(3, shape=(3,1))%2, (fromlist(["t","u"]), "v"))
    ObjectArray([['t', 'u'],
                 ['v', 'v'],
                 ['t', 'u']])
    >>> a = ObjectArray(shape=(3,2))
    >>> choose(_nc.arange(3, shape=(3,1))%2, (fromlist(["t","u"]), "v"), a); a
    ObjectArray([['t', 'u'],
                 ['v', 'v'],
                 ['t', 'u']])
    """
    if not isinstance(selector, _nc.NumArray):
        selector = _nc.fromlist(selector)
    population = list(population)
    for i in xrange(len(population)):
        p = population[i]
        if not isinstance(p, _gen.NDArray):
            population[i] = fromlist([population[i]])
    inputs = _gen._nWayBroadcast([selector]+population)
    selector, population = inputs[0], inputs[1:]
    if output is None:
        output0 = ObjectArray(shape=selector.shape)
    else:
        output0 = output
    _choose(selector, tuple(population), output0)
    if output is None:
        return output0

def _sort(objects, sorted):
    if len(objects._shape) == 1:
        l = objects.tolist()
        l.sort()
        o = fromlist(l)
        sorted._copyFrom(o)
    else:
        for i in xrange(objects.shape[0]):
            _sort(objects[i], sorted[i])

def sort(objects, axis=-1, output=None):
    """sort() sorts 'objects' along 'axis' putting the result in 'output'
    or returning the result if no output is specified.

    >>> a = ObjectArray(shape=(5,5))
    >>> a[:] = range(5,0,-1)
    >>> sort(a)
    ObjectArray([[1, 2, 3, 4, 5],
                 [1, 2, 3, 4, 5],
                 [1, 2, 3, 4, 5],
                 [1, 2, 3, 4, 5],
                 [1, 2, 3, 4, 5]])
    >>> a.transpose()
    >>> sort(a, axis=0)
    ObjectArray([[1, 1, 1, 1, 1],
                 [2, 2, 2, 2, 2],
                 [3, 3, 3, 3, 3],
                 [4, 4, 4, 4, 4],
                 [5, 5, 5, 5, 5]])
    """
    if output is None:
        output0 = objects._clone(objects._shape)
    else:
        output0 = output
    if axis != -1:
        objects.swapaxes(-1, axis)
        output0.swapaxes(-1, axis)
    _sort(objects, output0)
    if axis != -1:
        objects.swapaxes(-1, axis)
        output0.swapaxes(-1, axis)
    if output is None:
        return output0

def _argsort(objects, witness):
    if len(objects._shape) == 1:
        lo = objects.tolist()
        lw = range(len(lo))
        z = zip(lo, lw)
        z.sort()
        sw = [ w for o, w in z ]
        w = _nc.fromlist(sw)
        witness._copyFrom(w)
    else:
        for i in xrange(objects._shape[0]):
            _argsort(objects[i], witness[i])
    
def argsort(objects, axis=-1, output=None):
    """argsort() returns the sort order for 'objects' sorted
    along 'axis'.

    >>> a = ObjectArray(shape=(5,5))
    >>> a[:] = ['e','d','c','b','a']
    >>> argsort(a)
    array([[4, 3, 2, 1, 0],
           [4, 3, 2, 1, 0],
           [4, 3, 2, 1, 0],
           [4, 3, 2, 1, 0],
           [4, 3, 2, 1, 0]])
    """
    if output is None:
        output0 = _nc.zeros(objects._shape)
    else:
        output0 = output
    if axis != -1:
        objects.swapaxes(-1, axis)
        output0.swapaxes(-1, axis)
    _argsort(objects, output0)
    if axis != -1:
        objects.swapaxes(-1, axis)
        output0.swapaxes(-1, axis)
    if output is None:
        return output0

def _take(array, indices):
    array = asarray(array)
    return array.take(indices)

def _put(array, indices, values, axis=0):
    array = asarray(array)
    return array.take(indices)

def take(array, indices, axis=0):
    """take picks elements of 'array' specified by 'indices'.

    parameters which must be specified by keyword:

    array           data to be indexed & collected (taken from).

    indices         An integer sequence, or tuple of integer sequences
                    specifying the array coordinates from which data
                    is to be taken.  Partial indices result in entire
                    inner blocks being taken.

    axis=0          selects the axis (or axes) along which the take
                    should be performed.

    >>> o = fromlist(range(10))
    >>> a = arange(5)*2
    >>> take(o, a)
    ObjectArray([0, 2, 4, 6, 8])
    """
    if axis == 0:
        array = asarray(array)
        return array._take((indices,))
    elif isinstance(axis, (int, long)):
        if isinstance(indices, (int,long,float)):
            raise ValueError("indices must be a sequence")
        work = _gen.swapaxes(array, 0, axis)
        work = work._take((indices,))
        return _gen.swapaxes(work, 0, axis)
    else:
        def_axes = range(array.rank)
        for x in axis:
            def_axes.remove(x)
        axis = list(axis) + def_axes
        work = _gen.transpose(array, axis)
        return work._take(indices)

def put(array, indices, values, axis=0):
    """put stores 'values' into 'array' at 'indices...'.

    parameters which must be specified by keyword:

    array           data to be indexed & stuffed (put to).

    indices         An integer sequence, or tuple of integer sequences
                    specifying the array coordinates to hich data
                    is to be put.  Partial indices result in entire
                    inner blocks being overwritten.

    values          A sequence of values to be written to the specified
                    indices of array.

    axis=0          selects the axis along which the put should be performed.

    >>> o = fromlist(range(10))
    >>> a = arange(5)*2
    >>> put(o, a, 0); o
    ObjectArray([0, 1, 0, 3, 0, 5, 0, 7, 0, 9])
    """
    work = asarray(array)
    if axis == 0:
        work._put((indices,), values)
    elif isinstance(axis, (int, long)):
        if isinstance(indices, (int,long,float)):
            raise ValueError("indices must be a sequence")
        work = _gen.swapaxes(work, 0, axis)
        work._put((indices,), values)
        work = _gen.swapaxes(work, 0, axis)
    else:
        def_axes = range(work.rank)
        for x in axis:
            def_axes.remove(x)
        axis = list(axis) + def_axes
        work = _gen.transpose(work, axis)
        work._put(indices, values)
        work = _gen.transpose(work, axis)
    if work is not array:
        if isinstance(array, _gen.NDArray):
            array[:] = work


# =======================================================================
# Define the ufuncs

def _magicOp(name, f):
    if type(f) != type(_op.add):
        return None

    for i in [1,2,3]:
        args = i
        try:
            apply(f, tuple(["x"]*i))
        except TypeError:
            try:
                apply(f, tuple([1]*i))
            except TypeError:
                continue
        break
        
    if args == 1:
        # print "%s[%d]" % (name,args), 
        return _UnaryObjectUfunc(name, f)
    elif args == 2:
        # print "%s[%d]" % (name,args),
        return _BinaryObjectUfunc(name, f)
    else:
        return None

for name in dir(_op):
    theUfunc = _magicOp(name, _op.__dict__[name])
    if theUfunc is not None:
        globals()[name] = theUfunc

subtract = sub
multiply = mul
divide = div
remainder = mod
equal = eq
not_equal = ne
less = lt
greater = gt
less_equal = le
greater_equal = ge

# =======================================================================

def test():
    import doctest, objects
    return doctest.testmod(objects)
