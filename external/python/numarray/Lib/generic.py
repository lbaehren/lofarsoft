"""ndarray: Base multidimensional array class

This is the base multidimensional array class which implements all
structural array operations but treats the array contents as opaque
objects

$Id: generic.py,v 1.84 2006/06/13 11:57:42 jaytmiller Exp $
"""

import types as _types
import math as _math
import operator
import numerictypes as _nt
import sys
import _bytes
import memory
import numinclude
import copy
import copy_reg

from _ndarray import _ndarray, _isIntegerSequence, product, _alignment, ravel

import _ufunc

_PROTOTYPE = 0  # Set to 1 to activate Python prototypes of C code.

_UPDATEDICT = 0x1000  # views update from dict of original
_WRITABLE = 0x400 # array is WRITABLE

def _buffer_reduce(b):
    """Converts a "buffer" object to a pickling reduction tuple."""
    return (memory.memory_from_string, (str(b),))

copy_reg.pickle(memory.MemoryType, _buffer_reduce,
                memory.memory_from_string)

# Register the buffer object.  This approach should work for any object
# supporting str returning a memory image.
# copy_reg.pickle(_types.BufferType, _buffer_reduce, memory.memory_from_string)

NewAxis = None
_IOBLOCKSIZE = 1024**2

def _product(shape):
    n = 1
    for s in shape:
        n *= s
    return n

def getShape(shape, *args):
    """Verifies that this is a legal shape specification and returns tuple

    Shape can be an integer or a sequence of integers.  Also can pass
    several integer arguments.  Raises an exception on problems.
    """
    try:
        if shape is () and not args:
            return ()
        if type(shape) in [_types.IntType, _types.LongType]:
            shape = (shape,) + args
        else:
            if args:
                raise TypeError
            shape = tuple(shape)
        if _isIntegerSequence(shape):
            return shape
    except TypeError:
        pass
    raise TypeError("Shape must be sequence of integers")


def _broadcast(arr, sshape):
    """Return broadcast view of arr, else return None."""
    ashape = arr._shape
    # Just return arr if they have the same shape
    if sshape == ashape:
        return arr
    srank = len(sshape)
    arank = len(ashape)
    if arank > srank:
        return None
    if arank == srank:
        astrides = list(arr._strides)
    else:
        astrides  = [0]*(srank-arank)  + list(arr._strides)
        ashape = sshape[0:srank-arank] + ashape
    if ashape != sshape:
        for i in range(srank):
            if sshape[i] != ashape[i]:
                if ashape[i] == 1:
                    astrides[i] = 0
                else:
                    raise ValueError("Arrays have incompatible shapes")
    tarr = arr.view()
    tarr._shape = sshape
    tarr._strides = tuple(astrides)
    return tarr

def _common_shape(sshape, ashape):
    """Return broadcast shape common to both sshape and ashape."""
    # Just return both if they have the same shape
    if sshape == ashape:
        return sshape
    srank, arank = len(sshape), len(ashape)
    # do a special comparison of all dims with size>1
    if srank > arank:
        newrank = srank
        ashape = sshape[:newrank-arank] + ashape
    else:
        newrank = arank
        sshape = ashape[:newrank-srank] + sshape
    newshape = list(sshape)
    for i in range(newrank):
        if sshape[i] != ashape[i]:
            if sshape[i] == 1:
                newshape[i] = ashape[i]
            elif ashape[i] == 1:
                newshape[i] = sshape[i]
            else:
                raise ValueError("Arrays have incompatible shapes");
    return tuple(newshape)

def _common_shapes(indexArrays):
    """determines the mutual broadcast shape of indexArrays."""
    cshape = ()
    for i in indexArrays:
        if isinstance(i, NDArray) and i._strides is not None:
            cshape = _common_shape(cshape, i._shape)
    return cshape

def _broadcast_all(indexArrays, cshape):
    """returns a list of views of 'indexArrays' broadcast to shape 'cshape'"""
    result = []
    for i in indexArrays:
        if isinstance(i, NDArray) and i._strides is not None:
            result.append(_broadcast(i, cshape))
        else:
            result.append(i)
    return tuple(result)

def _nWayBroadcast( indexArrays ):
    """return views of indexArrays broadcast to their common shape,
    modifying the indexArrays sequence but not the original arrays.
    """
    # original function was impure... so keep side effects.
    cshape = _common_shapes(indexArrays)
    indexArrays[:] = list(_broadcast_all(indexArrays, cshape))
    return indexArrays


def _takeShape(scattered, indexArrays):
    """computes the shape of the result of a take/put operation"""
    
    nindexArrays = len(indexArrays)
    arrDims  = len(scattered._shape)

    if nindexArrays > arrDims:
        raise ValueError("Specified too many indices...")

    # Convert indices into suitable index numarray
    for i in range(len(indexArrays)):
        indexArrays[i] = _nc.asarray(indexArrays[i], _nt.MaybeLong)

    indexArrays = _nWayBroadcast(indexArrays) # convert to numarray and broadcast

    # Figure out output array shape and basic blocksize N
    leftOver = arrDims - nindexArrays

    if nindexArrays < arrDims:
        N = scattered._strides[ nindexArrays - 1 ] # size of inner block
        nShape = scattered._shape[-leftOver:]
    else:
        N = scattered._itemsize
        nShape = ()

    impliedShape = indexArrays[0]._shape + nShape
    return impliedShape, N

def info(arr):
    arr.info()

def SuitableBuffer(b):
    """SuitableBuffer(b) determines whether 'b' can be used as an NDArray
    buffer.

    This check is obsolete.
    """
    return ((type(b) is _types.BufferType) or
            (type(b) is memory.MemoryType) or
            ((type(b) is _types.InstanceType)
                and ("__buffer__" in dir(b.__class__))
                and ("resize" in dir(b.__class__))))

def ClassicUnpickler(cls, state):
    self = cls.__new__(cls)
    self.__setstate__(state)
    return self
ClassicUnpickler.__safe_for_unpickling__ = 1

class NDArray(_ndarray):
    """Multi-dimensional array abstract base class

    This class defines the structural operations common to numarray.
    Subclasses must provide the semantical interpretation of elements,
    including the __str__, __repr__, _getitem, and _setitem methods.


    NDArray(shape=None, itemsize=0, buffer=None, byteoffset=0, bytestride=0, aligned=1)

    shape               The shape of the array.

    itemsize            The size in bytes of a single element.

    buffer              An object meeting the Python buffer protocol which will store the
                        array data.  If None is specified,  a memory oject is created.

    byteoffset          The offset in bytes from the base of buffer to the array data.

    bytestride          The distance in bytes between elements.  Defaults to itemsize.

    aligned             A flag describing whether buffer+offset is aligned for itemsize.


    Given an array index of arr[k,j,i] it is always true that the
    byte offset of the element in the array is computed thusly:

    with shape[0] --> dimension of current view
         strides[0] --> bytestride for k index dimension

    element_byte_offset = byteoffset +
               ( i*strides[2] + j*strides[1] + k*strides[0])

    where 0 <= i < shape[2],
          0 <= j < shape[1],
          0 <= k < shape[0]

    For contiguous numarray strides[i] = shape[i+1]*strides[i+1]

    Summary of attribute meanings:

    _data           buffer with data for the array
    _shape          dimensions of the array
    _byteoffset     The byte offset of the first element from the beginning
                    of the buffer
    _bytestride     The separation between items in bytes.
    _itemsize       The size of items in bytes
    """

    if _PROTOTYPE:
        def __init__(self, shape=(), itemsize=1, buffer=None, byteoffset=0,
                     bytestride=None, byteorder=sys.byteorder, aligned=1):

            _ndarray.__init__(self);

            self._itemsize = itemsize
            self._byteoffset = byteoffset
            self._shape = getShape(shape)
            if bytestride is None:
                self._bytestride = itemsize
            elif bytestride < itemsize:
                raise ValueError('bytestride must be >= itemsize')
            else:
                self._bytestride = bytestride

            if buffer is not None:
                self._data = buffer
            else:
                size = self._bytestride * self.nelements()
                self._data = memory.new_memory(size)
            self._strides = self._stridesFromShape()

        def __len__(self):
            if len(self._shape):
                return int(self._shape[0])
            else:
                raise ValueError("Rank-0 array has no length.")

        def _universalIndexing(self, key, value=None):
            """Handles both getting (value == None) and setting (value != None)"""
            if isinstance(key, int) and len(self._shape) == 1:
                if key < 0: key += self._shape[0];
                if not 0 <= key < self._shape[0]:
                    raise IndexError("Index out of range")
                offset = self._strides[0]*key + self._byteoffset
                if value is None:
                    return self._getitem(offset)
                else:
                    return self._setitem(offset, value)
                
            if isinstance(value, (list,tuple)):
                fvalue = self.factory(value)
            else:
                fvalue = value

            # Make simple types and arrays into a 1-element tuple.
            if isinstance(key, (_types.SliceType,
                                _types.EllipsisType,
                                int, long, _nc.NumArray)):
                tkey = (key,)
            elif isinstance(key, list):
                if isinstance(key[0], _types.SliceType):
                    tkey = tuple(key)
                else:
                    tkey = (key,)
            else:
                tkey = key

            if not isinstance(tkey, tuple):
                raise IndexError("Illegal index")

            tkey2 = list(tkey)
            if _isIntegerSequence(tkey2):
                return self._simpleIndexing(tkey2, fvalue)
            elif self._isSlice(tkey): # i.e., no numarrays...
                return self._slicedIndexing(tkey2, fvalue)
            else:
                return self._arrayIndexing(tkey2, fvalue)

        def _simpleIndexing(self, key, value):
            if len(key) > len(self._shape):
                raise IndexError("Too many indices")
            offset = self._getByteOffset(key)
            if len(key) == len(self._shape):  # single values
                if value is None:
                    return self._getitem(offset)
                else:
                    self._setitem(offset, value)
            else:             # subarray
                retarr = self.view()
                retarr._shape = self._shape[len(key):]
                retarr._strides = self._strides[len(key):]
                retarr._byteoffset = offset
                if value is None:
                    return retarr
                else:
                    retarr._copyFrom(value)

        def _fixSlice(self, slice, shape):
            start, stop, step = slice.start, slice.stop, slice.step

            # print "_fixSlice:", start, stop, step, shape

            if step is None:
                step = 1
            elif step == 0:
                raise IndexError("slice step of zero not allowed")

            if step > 0:
                if start is None:
                    start = 0
                elif start > shape:
                    start = shape
                elif start < 0:
                    start += shape
                    if start < 0:
                        start = 0
                if stop is None:
                    stop = shape
                elif stop > shape:
                    stop = shape                    
                elif stop < 0:
                    stop += shape
                    if stop < 0:
                        stop = 0
            else:
                if start is None:
                    start = shape-1
                elif start > shape:
                    start = shape-1
                elif start < 0:
                    start += shape
                    if start < 0:
                        start = 0                    
                if stop is None:
                    stop = -1
                elif stop > shape:
                    stop = shape-1
                elif stop < 0:
                    stop += shape
                    if stop < 0:
                        stop = -1

            # print "_fixSlice ->", int(start), int(stop), int(step)
            return int(start), int(stop), int(step)

        def _slicedIndexing0(self, key, value, dim):
            if not len(key):
                if value is None:
                    return self
                else:
                    if self.shape is ():
                        self[()] = value
                        return None
                    else:
                        return self._copyFrom(value)
            else:
                slice, rest = key[0], key[1:]
                if isinstance(slice, int):
                    if slice < 0:
                        slice += self._shape[dim]
                    if not (0 <= slice < self._shape[dim]):
                        raise IndexError("Index out of range")
                    self._byteoffset += slice * self._strides[dim]
                    self._strides = self._strides[:dim] + self._strides[dim+1:]
                    self._shape = self._shape[:dim] + self._shape[dim+1:]
                else:
                    start, stop, step = self._fixSlice(slice, self._shape[dim])

                    strided = int(_math.ceil(float(stop - start)/step))
                    if strided < 0:
                        strided = 0

                    self._byteoffset += self._strides[dim] * start

                    self._shape = self._shape[:dim] + (strided,) + \
                                  self._shape[dim+1:]

                    self._strides = self._strides[:dim] + \
                                    (self._strides[dim]*step,) + \
                                    self._strides[dim+1:]            
                    dim += 1

                return self._slicedIndexing0(rest, value, dim)

        def _slicedIndexing(self, key, value=None):
            result = self.view()
            indexed = 0
            for i in range(len(key)):
                if (isinstance(key[i], (int, long)) or 
                    isinstance(key[i], _types.SliceType)):
                    indexed += 1
                elif isinstance(key[i], _types.EllipsisType):
                    non_new = 0
                    for k in key[i+1:]:
                        if k is not NewAxis:
                            non_new += 1
                    key[i:i+1] = [slice(None,None,None)] * \
                                 (len(self._shape)-indexed-non_new)
                    break
            for j in range(i,len(key)):
                if isinstance(key[j], _types.EllipsisType):
                    key[j] = slice(None,None,None)
            keylen = len(key)
            for i in range(keylen):
                if key[i] is NewAxis:
                    key[i] = slice(0,1,1)
                    if i > 0:
                        which = i - 1
                    else:
                        which = 0
                    result._strides = result._strides[:i] + \
                                      (result._strides[which],) + \
                                      result._strides[i:]
                    result._shape = result._shape[:i] + \
                                    (1,) + \
                                    result._shape[i:]
                    keylen = keylen - 1
            if keylen > len(self._shape):
                raise IndexError("too many slices for array shape")
            return result._slicedIndexing0(key, value, 0)

        def _taker(self, indices, result):
            for i in xrange(len(indices[0])):
                index = tuple([ ind[i] for ind in indices])
                result[i] = self[index]
            return result

        def _putter(self, indices, values):
            for i in xrange(len(indices[0])):
                index = tuple([ind[i] for ind in indices])
                self[index] = values[i]

        def _view(self):
            """Return a new array object, with the same reference to the data buffer"""
            arr = self.__class__.__new__(self.__class__)
            arr.__dict__.update(self.__dict__)
            # Handle attributes explicitly
            arr._data = self._data
            arr._shape = self._shape
            arr._byteoffset = self._byteoffset
            arr._bytestride = self._bytestride
            arr._itemsize = self._itemsize
            arr._strides = self._strides
            arr._aligned = self._aligned
            return arr

        def swapaxes(self, axis1, axis2):
            """swapaxes() interchanges axis1 and axis2.
            """
            n = len(self._shape)
            if axis1 < 0: axis1 += n
            if axis2 < 0: axis2 += n
            if n <= 1 or axis1 == axis2:
                return # skip 0D, 1D, and same axis swaps.
            if axis1 not in range(n) or axis2 not in range(n):
                raise ValueError("Specified dimension does not exist")
            if axis1 > axis2: # Make sure that axes are strictly ordered 
                axis1, axis2 = axis2, axis1
            # Just swap the shape and stride elements 
            self._shape = (self._shape[0:axis1] + (self._shape[axis2],) +
                           self._shape[axis1+1:axis2] +
                           (self._shape[axis1],) + self._shape[axis2+1:])
            self._strides = (self._strides[0:axis1] + (self._strides[axis2],) +
                             self._strides[axis1+1:axis2] +
                             (self._strides[axis1],) + self._strides[axis2+1:])

        def ravel(self):
            """ravel(self) setshapes 'self' into an equivalent 1D array.
            """
            self.setshape((self.nelements(),))

    size = _ndarray.nelements  # more portable synonym

    def __getstate__(self):
        """returns state of NDArray for pickling."""
        state = copy.copy(self.__dict__)
        state["_version"]    = numinclude.version
        state["_bytestride"] = self._bytestride
        state["_byteoffset"] = self._byteoffset
        state["_shape"]      = self._shape
        state["_strides"]    = self._strides
        state["_data"]       = self._data
        state["_itemsize"]   = self._itemsize
        state["_flags"]      = self._flags
        return state
    
    def __setstate__(self, state):
        """restores state of NDArray after unpickling."""
        self.__dict__.update(state)
        self._bytestride = state["_bytestride"]
        self._byteoffset = state["_byteoffset"]
        self._shape      = state["_shape"]
        self._strides    = state["_strides"]
        self._data       = state["_data"]
        self._itemsize   = state["_itemsize"]
        try:
            self._flags      = state["_flags"]
        except:
            self._flags |= _UPDATEDICT  

    def __nonzero__(self):
        raise RuntimeError("An array doesn't make sense as a truth value.  Use any(a) or all(a).")
    
    def __copy__(self):
        """support for copy.copy()"""
        return self.copy()

    def __deepcopy__(self, memo):
       """support for copy.deepcopy()"""
       return self.copy()
   
    def __reduce__(self):
        """__reduce__ returns the pickling "reduction tuple" for an NDArray.
        Used for NDArray derived from C basetypes, not classic classes.
        """
        return (ClassicUnpickler, (self.__class__,)+(self.__getstate__(),))

    def __repr__(self):
        name = self.__class__.__name__
        return name + "(" + \
           arrayprint.array2string(self, separator=", ", prefix=name +"(")+ ")"

    def __str__(self):
        return arrayprint.array2string(self, separator=" ", style=str)

    def itemsize(self):
        """Size (in bytes) of an array element"""
        return self._itemsize

    def is_c_array(self):
        """is_c_array() returns 1 iff the array is aligned and contiguous,
        and returns 0 otherwise."""
        return self.isaligned() and self.iscontiguous()

    def _stridesFromShape(self):
        """Compute the strides from shape for a contiguous array"""
        ndim = len(self._shape)
        if ndim:
            strides = [self._bytestride]*ndim
            for i in xrange(ndim-2, -1, -1):
                strides[i] = strides[i+1] * self._shape[i+1]
            return tuple(strides)
        else:
            return ()  # scalar

    def _arrayIndexing(self, key, value):
        for item in key:
            if isinstance(item, (_types.SliceType, _types.EllipsisType)):
                raise IndexError("Cannot mix numarray and slices as indices")
        bool = (isinstance(key[0], _nc.NumArray) and
                key[0].type() is _nt.Bool)
        if bool and len(key) != 1:
            raise ValueError("Invalid boolean key; specify one array only.")
        if value is None:
            if bool:
                return self._take(ufunc.nonzero(key[0]))
            else:
                return self._take(key)
        else:
            if bool:
                putmask( self, key[0], value)
                # self._put( ufunc.nonzero(key[0]), value )
            else:
                self._put( key, value )

    def _isSlice(self, key):
        for item in key:
            if isinstance(item, (_nc.NumArray, list)):
                return 0
            if not isinstance(item, (int, long,
                                     _types.NoneType,
                                     _types.SliceType,
                                     _types.EllipsisType)):
                raise IndexError("index is not of legal form")
        return 1

    def _broadcast(self, arr):
        """Return broadcast view of arr, else return None."""
        return _broadcast(arr, self._shape)

    def _dualbroadcast(self, arr):
        """Return broadcast views both self and arr, else return (None,None)."""
        
        sshape = self._shape
        ashape = arr._shape
        # Just return both if they have the same shape
        if sshape == ashape:
            return (self, arr)
        srank = len(sshape)
        arank = len(ashape)
        # do a special comparison of all dims with size>1
        if srank > arank:
            newrank = srank
            sstrides = list(self._strides)
            ashape = sshape[:newrank-arank] + ashape
            astrides = [0]*(newrank-arank) + list(arr._strides)
        else:
            newrank = arank
            astrides = list(arr._strides)
            sshape = ashape[:newrank-srank] + sshape
            sstrides = [0]*(newrank-srank) + list(self._strides)
        newshape = list(sshape)
        for i in range(newrank):
            if sshape[i] != ashape[i]:
                if sshape[i] == 1:
                    newshape[i] = ashape[i]
                    sstrides[i] = 0
                elif ashape[i] == 1:
                    newshape[i] = sshape[i]
                    astrides[i] = 0
                else:
                    raise ValueError("Arrays have incompatible shapes");
        newshape = tuple(newshape)
        tself, tarr = self, arr
        if self._shape != newshape:
            tself = self.view()
            tself._shape = newshape
            tself._strides = tuple(sstrides)
        if arr._shape != newshape:
            tarr = arr.view()
            tarr._shape = newshape
            tarr._strides = tuple(astrides)
        return tself, tarr

    def _copyFrom(self, arr):
        """Copy elements from another array.

        This is the generic version. Subclasses (such as numarray)
        may override this method
        """
        # Arrays must be shape congruent and have the same itemsize.
        # xxx Don't handle broadcasting yet.
        if (self._shape not in [(1,), (), arr._shape] or
            arr._shape not in [(1,), (), self._shape]):
            raise ValueError("Arrays have inconsistent sizes")
        if arr._itemsize != self._itemsize:
            raise ValueError("Arrays must have the same itemsize")
        cfunc = _bytes.functionDict['copyNbytes']
        sShape = (arr._shape == ()) and () or self._shape
        cfunc(sShape, arr._data,  arr._byteoffset,  arr._strides,
              self._data, self._byteoffset, self._strides, self._itemsize)

    def setshape(self, shape, *args):
        """Change array shape in place.  Call as setshape(i,j,k) or setshape((i,j,k))."""
        if not self.iscontiguous():
            raise ValueError("Can't reshape non-contiguous numarray")
        shape = list(getShape(shape, *args))
        # look for index = -1, which indicates an expandable dimension
        nelements = self.nelements()
        negcount = shape.count(-1)
        if negcount > 1:
            raise ValueError("no more than one dimension can have value -1")
        elif negcount == 1:
            tnelements = abs(product(shape))
            shape[shape.index(-1)] = nelements/tnelements

        newnelements = product(shape)
        if newnelements == nelements:
            self._shape = tuple(shape)
            self._strides = self._stridesFromShape()
        else:
            raise ValueError("New shape is not consistent with the old shape")

    def getshape(self):
        return self._shape

    shape = property(getshape, setshape, doc="tuple of array dimensions")

    def getrank(self): return self.rank  # For backward compat only

    def getflat(self):
        if self.iscontiguous():
            a = self.view()
        else:
            a = self.copy()
        a.ravel()
        return a

    def setflat(self, flat):
        a = self.view()
        a.ravel()
        a[:] = flat
            
    flat = property(getflat, setflat, doc="access to array as 1D")

    def copy(self):
        """Return a new array with the same shape and type, but a copy of the data"""
        arr = self.view()
        arr._data = memory.new_memory(arr._itemsize * arr.nelements())
        arr._byteoffset = 0
        arr._bytestride = arr._itemsize
        arr._strides = arr._stridesFromShape()
        arr._itemsize = self._itemsize
        # now copy data, if possible using larger units
        if product(self._shape):
            fname = "copy"+str(self._itemsize)+"bytes"
            copyfunction = ((self.isaligned() and
                             _bytes.functionDict.get(fname))
                            or _bytes.functionDict["copyNbytes"])
            copyfunction(arr._shape, self._data, self._byteoffset,
                         self._strides, arr._data, 0, arr._strides,
                         arr._itemsize)
        return arr

    def tostring(self):
        """Return a string with a binary copy of the array

        Copies are always contiguous, but no conversions are implied

        """
        if self.rank == 0:
            self = self.view()
            self.shape = (1,)
        return _bytes.copyToString(self._shape, self._data, self._byteoffset,
                                 self._strides, self._itemsize)

    def tofile(self, file):
        """Write the array as a binary image to a file.

        If file is a string, it attempts to open a file with that name,
        otherwise it assumes file is a file object. At the moment if
        special positioning is needed in the file one must do that with
        the file object beforehand. More options may be added to this
        method to allow positioning or appends.

        Note that for numerical data, the system byte order in which
        the data is represented is *not* recorded in the file.  This
        renders the file non-portable because extra information is
        required to interpret it on different machines than the one it
        was created on.
        
        """
        name =  0
        if isinstance(file,(unicode,str)):
            name = 1
            file = open(file, 'wb')
        niter = _IOBLOCKSIZE // self._itemsize
        if niter > 0:
            if self.nelements() != 0: # skip zero length arrays.
                indexlevel, blockingparameters = \
                            _ufunc._getBlockingParameters(self._shape, niter)
                self._tofileByBlocks(file, [], indexlevel, blockingparameters)
            if name:
                file.close()
        else: # very large items 
            v = self.view()
            v._itemsize = 1
            v._shape = self._shape + (self._itemsize,)
            v._strides = self._strides + (1,)
            v.tofile(file)

    def _tofileByBlocks(self, file, dims, indexlevel, blockingparameters):
        """Write the array to a file repeatedly in blocks

        This is done similarly to ufunc._doOverDimensions

        """
        level = len(dims)
        if level == indexlevel:
            nregShapeIters, shape, leftover, leftoverShape, = blockingparameters
            currentIndex = 0
            tshape = shape[:]
            for i in xrange(nregShapeIters + leftover):
                if i==nregShapeIters:
                    tshape = leftoverShape
                tdims = dims + [currentIndex,]
                s = _bytes.copyToString( tshape, self._data,
                                         self._getByteOffset(tdims),
                                         self._strides[-len(tshape):], self._itemsize)
                file.write( s )
                currentIndex += shape[0]
        else:
            # recurse
            for i in xrange(self._shape[level]):
                tdims = dims + [i]
                self._tofileByBlocks(file, tdims, indexlevel, blockingparameters)

    def transpose(self, axes=None):
        """transpose() re-shapes the array by permuting it's
        dimensions as specified by 'axes'.  If 'axes' is none, transpose
        returns the array with it's dimensions reversed.
        """
        slen = len(self._shape)
        if axes == None:
            axes = range(slen)
            axes.reverse()
        if len(axes) != slen:
            raise ValueError("Wrong number of axes in tranpose")
        tax = list(axes[:])
        tax.sort()
        if tax != range(slen):
            raise ValueError("Duplicate or missing transpose axes")
        nshape, nstrides = [],[]
        for i in axes:
            nshape.append(self._shape[i])
            nstrides.append(self._strides[i])
        self._shape = tuple(nshape)
        self._strides = tuple(nstrides)

    def _clone(self, shape):
        """returns an empty array identical to 'self' but with 'shape'.
        """
        c = self.copy()
        c.resize(shape)
        return c

    def _fix_pt_indices(self, indices):
        indices = _nc.array(indices, type=_nt.MaybeLong)
        dt = range(len(indices._shape))
        indices = transpose(indices, dt[1:] + dt[0:1])
        indices = indices.copy() # make contiguous
        return indices

    def _take(self, indices, **keywds):
        indices = list(indices)
        impliedShape, N = _takeShape(self, indices)
        result = self._clone(shape=impliedShape)
        indices = self._fix_pt_indices(indices)
        self._taker(indices, result)
        return result

    def _put(self, indices, values, **keywds):
        indices = list(indices)
        impliedShape, N = _takeShape(self, indices)
        if not isinstance(values, self.__class__):
            values = self.factory( values )
        values = _broadcast( values, impliedShape )
        indices = self._fix_pt_indices(indices)
        self._putter(indices, values)

    def take(self, *indices, **keywords):
        return take(self, *indices, **keywords)

    def put(self, *indices, **keywords):
        return put(self, *indices, **keywords)

    def nonzero(self):
        return ufunc.nonzero(self)

    def resize(self, shape, *args):
        """ resize() shrinks/grows 'self' to new 'shape', possibly
        replacing the underlying buffer object.
        """
        shape = getShape(shape, *args)
        nlen = product(shape)
        if nlen < 0:
            raise ValueError("Negative shape dims don't work with resize")
        olen = self.nelements()

        if (isinstance(self._data, _types.BufferType) or
            isinstance(self._data, memory.MemoryType)):
            if self.iscontiguous():
                oself = self.view()
            else:
                oself = self.copy()
            self._data = memory.new_memory(nlen*self._itemsize)
            self._bytestride = self._itemsize
            self._byteoffset = 0
            blen = min(nlen, olen)
            self.ravel()
            oself.ravel()
            self[:blen] = oself[:blen]
        else: # Memmap
            self._data.resize(nlen*self._itemsize)

        self._shape = (nlen,)
        self._strides = self._stridesFromShape()

        if olen: # use any existing data as a pattern to be repeated
            if nlen > olen:
                offset = olen
                while offset+olen <= nlen:
                    self[offset:offset+olen] = self[0:olen]
                    offset += olen
                self[offset:nlen] = self[0:nlen-offset]
        else:   # zero fill resized zero-length numarray
            self[:] = 0

        self._shape = shape
        self._strides = self._stridesFromShape()
        return self

    def repeat(self, repeats, axis=0):
        """repeat() returns a new array with each element  'a[i]' repeated 'r[i]' times.
        """
        return repeat(self, repeats, axis)

    def factory(self, *args, **keys):
        """factory(...) calls the array(...) factory function defined in the
        source module where the class of 'self' was defined.
        """
        module = sys.modules[self.__class__.__module__]
        return module.array(*args, **keys)

    def astype(self, type=None): #default so numarray.array works for NDArrays
        return self.copy()

    def sinfo(self):
        """sinfo() returns a string describing key attributes of an array."""
        s = ""
        s += "class: " + repr(self.__class__) + "\n"
        s += "shape: " + repr(self._shape) + "\n"
        s += "strides: " + repr(self._strides) + "\n"
        s += "byteoffset: " + repr(self._byteoffset) + "\n"
        s += "bytestride: " + repr(self._bytestride) + "\n"
        s += "itemsize: " + repr(self._itemsize) + "\n"
        s += "aligned: " + repr(self.isaligned()) + "\n"
        s += "contiguous: " + repr(self.iscontiguous()) + "\n"
        s += "buffer: " + repr(self._data) + "\n"
        s += "data pointer: 0x%08x (DEBUG ONLY)\n" % (self._fragile_data,)
        return s

    def info(self):
        """info() prints out the key attributes of an array."""
        sys.stdout.write(self.sinfo())

def reshape(arr, shape, *args):
    """Returns a reshaped *view* of array if possible, else a *copy*. 

    Call either as reshape(i,j,k) or reshape((i,j,k)).
    """
    v = _nc.asarray(arr)
    if v.iscontiguous():
        v = v.view()
    else:
        v = v.copy()
    v.setshape(shape, *args)
    return v

if _PROTOTYPE:
    def ravel(array):
        """Returns a *view* of array reshaped as 1D."""
        array = _nc.asarray(array)
        return reshape(array, (array.nelements(),))

def fromstring(datastring): pass

def resize(array, shape):
    """Returns a *copy* of array, replicated or truncated to match new shape."""
    array = _nc.array(array)   
    array.resize(shape)              # Assume array.resize() resizes in place.
    return array

def transpose(array, axes=None):
    """Returns the transpose of a *view* of array"""
    v = _nc.asarray(array).view()
    v.transpose(axes)
    return v

def sort(array, axis=-1, kind=''):
    """Returns a sorted *copy* of array."""
    array = _nc.array(array)
    array.sort(axis,kind)
    return array

def argsort(array, axis=-1,kind=''):
    """Returns an array of indices which, if "taken" from 'array',
    would sort 'array'.
    """
    array = _nc.asarray(array)
    return array.argsort(axis,kind)

def lexsort(keys, axis=-1):
    """Returns index array that lexigraphically sorts on keys.

    This is like taking the first letter from the first key,
    the second from the second key, etc., and forming a 'word.'
    The words are then indirectly sorted on the first key, then
    the second, etc. This will order things into 'alphabetic order'
    for the word *spelled backwards*. To get alphabetic order, the
    sort must start on the last letter and proceed to the first.
    """
    if not isinstance(keys,(_types.ListType, _types.TupleType)) : keys = (keys,)
    if axis == -1 :
        keys = [_nc.asarray(x) for x in keys]
        shps = [x.getshape() for x in keys]
        ashp = shps[0]
        if reduce(operator.__or__, [x != ashp for x in shps]) :
            raise KeyError, 'key arrays must have the same shape'
        w = _nc.array(shape=ashp, type=_nt.Long)
        w[...,:] = _nc.arange(ashp[-1], type=_nt.Long)
        for x in keys :
            ufunc._broadcast_indirect_sort(x, w, 'amergesort')
        return w
    else :
        map(lambda x : x.swapaxes(axis,-1), keys)
        w = lexmergesort(keys)
        map(lambda x : x.swapaxes(axis,-1), keys)
        w.swapaxes(axis,-1)
        return w


def argmin(array, axis=-1):
    """Returns the indices of the minumum elements of 'array' taken
    along 'axis'."""
    array = _nc.asarray(array)
    return array.argmin(axis)

def argmax(array, axis=-1):
    """Returns the indices of the maximum elements of 'array' taken
    along 'axis'."""
    array = _nc.asarray(array)
    return array.argmax(axis)

def swapaxes(array, axis1, axis2):
    """Returns a *view* of array with axis1 and axis2 interchanged."""
    if array is None:
        return array
    v = _nc.asarray(array).view()
    v.swapaxes(axis1, axis2)
    return v

def where(condition, x=None, y=None, out=None):
    """where() returns an array shaped like 'condition' with
    elements selected from 'x' or 'y' by the 1 or 0 value of each condition
    element, respectively.

    If neither 'x' nor 'y' is specified, where acts as a synonym for
    nonzero().
    """
    if x is None and y is None:
        if out is None:
            return ufunc.nonzero(condition)
        else:
            raise ValueError("single parameter where() does not support output array")
    else:
        if x is None or y is None:
            raise ValueError("Invalid parameter list")
        return choose(ufunc.not_equal(condition, 0), (y,x), out)

def clip(m, m_min, m_max):
    """clip()  returns a new array with every entry in m that is less than m_min 
    replaced by m_min, and every entry greater than m_max replaced by m_max.
    """
    selector = ufunc.less(m, m_min)+2*ufunc.greater(m, m_max)
    return choose(selector, (m, m_min, m_max))

def _shape0(a):
    if a.rank == 0:
        return 1
    else:
        return a._shape[0]

def _concat(arrs):
    """_concat handles the simplest case of concatenating numarray along the
    zero-th axis.
    """
    combinedLength = reduce(operator.add, [ _shape0(a) for a in arrs ])
    rShape = arrs[0]._shape[1:]
    destShape = (combinedLength,) + tuple(rShape)

    try:
        convType = ufunc._maxPopType(arrs)
    except TypeError:
        dest = arrs[0]._clone(shape=destShape)
    else:
        try:
            dest  = arrs[0].__class__(shape=destShape, type=convType)
        except TypeError:
            dest  = arrs[0].__class__(shape=destShape)        
    ix = 0
    for a in arrs:
        if a._shape[1:] != rShape:
            raise ValueError("_concat array shapes must match except 1st dimension")
        dest[ix:ix+_shape0(a)]._copyFrom(a)
        ix += _shape0(a)
    return dest

def concatenate(arrs, axis=0):
    """concatenate() joins the sequence of numarray 'arrs' in a into a single array
    along the specified 'axis'.
    """
    arrs = map(_nc.asarray, arrs)
    if axis == 0:
        return _concat(arrs)
    else:
        return swapaxes(_concat([swapaxes(m,axis,0) for m in arrs]), axis, 0)

# ------------------------------------------------------------------------
# import these last so module dependencies don't cause problems
import numarraycore as _nc
import ufunc
from ufunc import choose, _take, take, _put, put
import arrayprint

# ------------------------------------------------------------------------

def _compress(condition, a):
    return _take(a, ufunc.nonzero(condition))

def compress(condition, a, axis=0):
    """compress selects members of array 'a' along 'axis' which correspond to
    non-zero members of 'condition'.
    """
    if axis == 0:
        return _compress(condition, a)
    else:
        return swapaxes( _compress(condition, swapaxes(a, 0, axis)), 0, axis)

def _repeat(array, repeats):
    if ufunc._isScalar(repeats):
        repeats = (repeats,)*len(array)
    else:
        repeats = _nc.asarray(repeats, type=_nt.MaybeLong)
    total = ufunc.add.reduce(repeats)
    newshape = (total,)+array._shape[1:]
    newarray = array.__class__(shape=newshape, type=array._type)
    newi = 0;
    for i in range(len(repeats)):
        limit = repeats[i]
        for j in range(limit):
            newarray[newi+j] = array[i]            
        newi += limit
    return newarray

def repeat(array, repeats, axis=0):
    """repeat() returns a new array with each element  'a[i]' repeated 'r[i]' times.
    """
    if axis == 0:
        return _repeat(_nc.asarray(array), repeats)
    else:
        return swapaxes( _repeat(swapaxes(array, 0, axis), repeats), 0, axis)

def indices(shape, type=None):
    """indices(shape, type=None) returns an array representing a grid
    of indices with row-only, and column-only variation.
    """
    shape = tuple(shape)
    a = concatenate(ufunc.nonzero(_nc.ones(shape)))
    a.setshape((len(shape),)+shape)
    if type is not None:
        a = a.astype(type)
    return a

def fromfunction(function, dimensions, type=None): # from Numeric
    """fromfunction() returns an array constructed by calling function
    on a tuple of number grids.  The function should accept as many
    arguments as there are dimensions which is a list of numbers
    indicating the length of the desired output for each axis.
    """
    return apply(function, tuple(indices(dimensions, type)))

def _broadcast_or_resize(a, b):
    try:
        r = a._broadcast(b)
    except ValueError:
        r = resize(b, a.nelements())
        r.setshape(a.getshape())
    return r
        
def putmask(array, mask, values):
    """putmask() sets elements of 'array' for which 'mask' is non-zero to
    the corresponding element in 'values'.  'array' must be an array.
    """
    bmask = _nc.asarray(mask)
    bvalues = _nc.asarray(values)
    if bmask.nelements() == array.nelements():
        if bmask._shape != array._shape:
            bmask = bmask.view()
            bmask.setshape(array.getshape())
    else:
        bmask = _broadcast_or_resize(array, bmask)
    bvalues = _broadcast_or_resize(array, bvalues)
    choose(bmask != 0, (array, bvalues), array)
