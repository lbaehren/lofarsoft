"""numarray: The big enchilada numeric module

"""
import sys as _sys
import types, math, os.path
import operator as _operator
import copy as _copy
import warnings as _warnings
from math import pi, e

import memory
import generic as _gen
import _bytes
import _numarray
import _ufunc
import _sort
import numerictypes as _nt
import numinclude as _numinc
import dtype as _dtype

_PROTOTYPE = 0  # Set to 1 to switch to Python prototype code.
                # Set to 0 to inherit C code from C basetype.

# rename built-in function type so not to conflict with keyword
_type = type

MAX_LINE_WIDTH = None
PRECISION = None
SUPPRESS_SMALL = None

PyINT_TYPES = {
    bool: 1,
    int: 1,
    long: 1,
    }

PyREAL_TYPES = {
    bool: 1,
    int: 1,
    long: 1,
    float: 1,
    }

# Python numeric types with values indicating level in type hierarchy
PyNUMERIC_TYPES = {
    bool: 0,
    int: 1,
    long: 2,
    float: 3,
    complex: 4
    }

# Mapping back from level to type
PyLevel2Type = {}
for key, value in PyNUMERIC_TYPES.items():
    PyLevel2Type[value] = key
del key, value

# Mapping from Python to Numeric types
Py2NumType = {
    bool:  _nt.Bool,
    int:   _nt.Long,
    long:  _nt.Int64,
    float: _nt.Float,
    complex: _nt.Complex
    }

_numfmt_to_typestr = {_nt.Int8:'i1',  _nt.UInt8:'u1', 
                      _nt.Int16:'i2', _nt.UInt16:'u2',
                      _nt.Int32:'i4', _nt.UInt32:'u4', 
                      _nt.Int64:'i8', _nt.UInt64:'u8',
                      _nt.Float32:'f4', _nt.Float64:'f8',
                      _nt.Complex32:'c8', _nt.Complex64:'c16',
                      _nt.Bool:'b1'}

class EarlyEOFError(Exception):
    "Raised in fromfile() if EOF unexpectedly occurs."
    pass

class SizeMismatchError(Exception):
    "Raised in fromfile() if file size does not match shape."
    pass

class SizeMismatchWarning(Warning):
    "Issued in fromfile() if file size does not match shape."
    pass

class FileSeekWarning(Warning):
    "Issued in fromfile() if there is unused data and seek() fails"
    pass

##fromfile constants
_BLOCKSIZE=1024

STRICT,SLOPPY,WARN=range(3)

def array2list(arr):
    return arr.tolist()

# array factory functions

def _all_arrays(args):
    for x in args:
        if not isinstance(x, NumArray):
            return 0
    return len(args) > 0

def _maxtype(args):
    """Find the maximum scalar numeric type in the arguments.
    
    An exception is raised if the types are not python numeric types.
    """
    if not len(args):
        return None
    elif isinstance(args, NumArray):
        return args.type()    
    elif _all_arrays(args):
        temp = args[0].type()
        for x in args[1:]:
            if temp < x.type():
                temp = x.type()
        if isinstance(temp, _nt.BooleanType):
            return bool
        elif isinstance(temp, _nt.IntegralType):
            return int
        elif isinstance(temp, _nt.FloatingType):
            return float
        elif isinstance(temp, _nt.ComplexType):
            return complex
    else:
        return PyLevel2Type[_numarray._maxtype(args)]

def _storePyValueInBuffer(buffer, Ctype, index, value):
    """Store a python value in a buffer, index is in element units, not bytes"""
    # Do not use for complex scalars!
    Ctype._conv.fromPyValue(value, buffer._data,
                            index*Ctype.bytes, Ctype.bytes, 0)

def _storePyValueListInBuffer(buffer, Ctype, valuelist):
    # Do not use for complex values!
    for i in xrange(len(valuelist)):
        _storePyValueInBuffer(buffer, Ctype, i, valuelist[i])

def _fillarray(size, start, delta, type=None):
    ptype = _maxtype((start, delta))
    if ptype == long:
        ptype = _nt.Int64
    elif PyINT_TYPES.has_key(ptype):
        ptype = _nt.Long
    elif PyREAL_TYPES.has_key(ptype):
        ptype = _nt.Float
    else:
        ptype = _nt.Complex
    if type:
        outtype = _nt.getType(type)
        if (isinstance(ptype, _nt.ComplexType)
            and not isinstance( outtype, _nt.ComplexType)):
            raise TypeError("outtype must be a complex type")
    else:
        outtype = ptype
        
    if outtype > ptype: # Hack for Int64/UInt64 on 32-bit platforms.
        ptype = outtype
        
    if isinstance(outtype, _nt.ComplexType):
        # Not memory efficient at the moment
        real = _fillarray(size, complex(start).real, complex(delta).real,
                type = _realtype(ptype))
        image = _fillarray(size, complex(start).imag, complex(delta).imag,
                type = _realtype(ptype))
        outarr = NumArray((size,), outtype, real=real, imag=image)
    else:
        # save parameters in a buffer
        parbuffer = ufunc._bufferPool.getBuffer()
        _storePyValueListInBuffer(parbuffer, ptype, [start, delta])
        cfunction = _sort.functionDict[repr((ptype.name, 'fillarray'))]
        outarr = NumArray((size,), outtype)
        if ptype == outtype:
            # no conversion necessary, simple case
            _ufunc.CheckFPErrors()
            cfunction(size, 1, 1, ((outarr._data, 0), (parbuffer._data, 0)))
            errorstatus = _ufunc.CheckFPErrors()
            if errorstatus:
                ufunc.handleError(errorstatus, " in fillarray")
        else:
            # use buffer loop
            convbuffer = ufunc._bufferPool.getBuffer()
            convfunction = ptype._conv.astype[outtype.name]
            bsize = len(convbuffer._data)/ptype.bytes
            iters, lastbsize = divmod(size, bsize)
            _ufunc.CheckFPErrors()

            outoff = 0
            for i in xrange(iters + (lastbsize>0)):
                if i == iters:
                    bsize = lastbsize
                cfunction(bsize, 1, 1,
                          ((convbuffer._data, 0), (parbuffer._data, 0)))
                convfunction(bsize, 1, 1, 
                             ((convbuffer._data, 0), (outarr._data, outoff)))
                outoff += bsize*outtype.bytes
                start += delta * bsize
                _storePyValueListInBuffer(parbuffer, ptype, [start, delta])
            errorstatus = _ufunc.CheckFPErrors()
            if errorstatus:
                ufunc.handleError(errorstatus, " in fillarray")
    return outarr

def _frontseqshape(seq):
    """Find the length of all the first elements, return as a list"""
    if not len(seq):
        return (0,)
    if isinstance(seq, str):
        return (len(seq),)
    try:
        shape = []
        while 1:
            shape.append(len(seq))
            try:
                seq = seq[0]
                if isinstance(seq, str):
                    return shape
            except IndexError:
                return shape
    except TypeError:
        return shape
    except ValueError:
        if isinstance(seq, NumArray) and seq.rank == 0:
            return shape

def fromlist(seq, type=None, shape=None, check_overflow=0, typecode=None, dtype=None):
    """fromlist creates a NumArray from  the sequence 'seq' which must be
    a list or tuple of python numeric types.  If type is specified, it
    is as the type of  the resulting NumArray.  If shape is specified,
    it becomes the  shape of the result and must  have the same number
    of elements as seq.
    """
    type = _nt._typeFromKeywords(type, typecode, dtype)

    if isinstance(seq, _gen.NDArray):
        arr = seq.copy()
        if arr._type is not type:
            arr = arr.astype(type)
        if shape is not None and arr._shape is not shape:
            arr.shape = shape
        return arr
    
    if not len(seq) and type is None:
        type = _nt.Long

    if type is None:
        highest_type = _maxtype(seq)
        
    tshape = _frontseqshape(seq)
    if shape is not None and _gen.product(shape) != _gen.product(tshape):
        raise ValueError("shape incompatible with sequence")
    ndim = len(tshape)
    if ndim <= 0:
        raise TypeError("Argument must be a sequence")
    if type is None:
        type = Py2NumType.get(highest_type)
    if type is None:
        raise TypeError("Cannot create array of type %s" % highest_type.__name__)
    tshape = tuple(tshape)
    arr = NumArray(shape=tshape, type=type)
    arr._check_overflow = check_overflow
    arr.fromlist(seq)
    arr._check_overflow = 0
    if shape is not None:
        arr.setshape(shape)
    return arr

def getTypeObject(sequence, type):
    """getTypeObject computes the typeObject for 'sequence' if 'type' is
    'unspecified.  Otherwise,  it returns the typeObject specified by
    'type'.
    """
    if type is not None:
        return type
    elif isinstance(sequence, NumArray):  # handle array([])
        return sequence.type()
    elif hasattr(sequence, "typecode"): # for Numeric/MA
        return sequence.typecode()
    elif (isinstance(sequence, (types.ListType, types.TupleType)) and
          len(sequence) == 0):
        return _nt.Long
    else:
        if isinstance(sequence, (types.IntType, types.LongType,
                                 types.FloatType, types.ComplexType)):
            sequence = [sequence]
        try:
            return Py2NumType[ _maxtype(sequence) ]
        except KeyError:
            raise TypeError("Can't determine a reasonable type from sequence")

def array(sequence=None, typecode=None, copy=True, savespace=False,
          type=None, shape=None, dtype=None):
    """Attempt to convert SEQUENCE to an array of specified TYPE and SHAPE.

    SEQUENCE:

      Data sequence. Can be None, an object with an __array__ method,
      a NumArray, a buffer, a string, a (non-string) sequence, a file
      or a number. Unicode objects are currently not handled.
      If sequence is None and shape is None the result is None.
     
    TYPECODE, TYPE:

      Specify only one of TYPE or TYPECODE; they are equivalent. TYPE
      may be a string, in which it case it must be a Numeric or
      numarray typecode, or it may be a numarray type instance. If
      TYPE is None, an attempt is made to infer a type from SEQUENCE; if
      this cannot be done, the type is usually set to Int32, or Int8
      in the case of strings.

    COPY:

      If False, an attempt is made to not copy data.

    SHAPE:

      None, an integer or a sequence of integers specifying the shape
      of the returned NumArray. If SHAPE is None, the shape is
      inferred from SEQUENCE.

    SAVESPACE:

      Ignored; for Numeric compatibility.

    See the numarray docs for a complete description of this
    function's behaviour.
    """

    type=_nt._typeFromKeywords(type,typecode,dtype)

    if not isinstance(sequence, _gen.NDArray):
        a = None
        if hasattr(sequence, "__array_struct__"):
            a = _numarray._array_from_array_struct(sequence)
        elif (hasattr(sequence, "__array_shape__") and
            hasattr(sequence, "__array_typestr__")):
            typestr = sequence.__array_typestr__
            if typestr[0] == "<":
                endian = "little"
            elif typestr[0] == ">":
                endian = "big"
            else:
                raise ValueError("Invalid __array_typestr__:", )
            offset = getattr(sequence, "__array_offset__", 0)
            a = NumArray(buffer=sequence,
                         shape=shape or sequence.__array_shape__,
                         type=typestr[1:],
                         byteoffset=offset,
                         byteorder=endian)
            strides = getattr(sequence, "__array_strides__", a._strides)
            if strides is not None:
                a._strides = strides
        if a is not None: # One of the array interfaces worked
            if copy or (type is not None):
                a = a.astype(type)
            return a

    if sequence is None and shape is None:
        return None

    if shape is not None and not isinstance(shape,tuple):
        try:
            shape=tuple(shape)
        except TypeError:
            shape=(shape,)

    if sequence is None:
        if type is None:
            type=_nt.Int32
        if shape is None:
            shape=(0,)
        return NumArray(buffer=sequence, shape=shape, type=type)

    if hasattr(sequence,"__array__"):
        a=sequence.__array__(type)
        if not isinstance(a,NumArray):
            ##OK if the user knows what he/she/it is doing
            _warnings.warn("__array__ returned non-NumArray instance")
        if shape is not None:
            ##however, if the user specifies a shape, the returned
            ##instance must have a "setshape" method
            a.setshape(shape)
        return a
    
    if isinstance(sequence, _gen.NDArray):
        if (not copy
            and (type is None or type==sequence.type())
            and (shape is None or shape==sequence.getshape())):
            return sequence
        if type is not None:
            a=sequence.astype(type)
        else:
            a=sequence.copy()
        if shape is not None:
            a.setshape(shape)
        return a

    if _gen.SuitableBuffer(sequence):
        return NumArray(buffer=sequence,type=type,shape=shape)

    if isinstance(sequence,str):
        return fromstring(sequence,type,shape)

    if isinstance(sequence,unicode):
        raise ValueError("unicode sequence objects not currently handled")

    if isinstance(sequence,file):
        return fromfile(sequence,type,shape)

    if (hasattr(sequence,'__getitem__')
        and hasattr(sequence,'__len__')):
        return fromlist(sequence,type,shape)

    ##SEQUENCE is a scalar or unhandleable
    ##fromlist will complain in the latter case
    if isinstance(sequence, _nt.scalarTypes):
        if shape is None:
            shape = ()
        sequence = [sequence]
    return fromlist(sequence,type,shape)

def asarray(seq, type=None, typecode=None, dtype=None):
    """converts scalars, lists and tuples to numarray if possible.

    passes NumArrays thru making copies only to convert types.
    """
    if isinstance(seq, _gen.NDArray) and type is None and typecode is None:
        return seq
    return array(seq, type=type, typecode=typecode, copy=0, dtype=dtype)

inputarray = asarray   # Obsolete synonym

def fromstring(datastring, type=None, shape=None, typecode=None, dtype=None):
    """Create an array from binary data contained in a string (by copying)

    If type is None (the default), the returned array will be of type Int8.

    If shape is None (the default), the returned shape will be
    (len(datastring),).
    """
    type = _nt._typeFromKeywords(type, typecode, dtype)
    if type is None:
        type=_nt.Int8
    if not shape:
        size, rem = divmod(len(datastring), type.bytes)
        if rem:
            raise ValueError("Type size inconsistent with string length")
        else:
            shape = (size,) # default to a 1-d array
    elif _type(shape) is types.IntType:
        shape = (shape,)
    if len(datastring) != (_gen.product(shape)*type.bytes):
        raise ValueError("Specified shape and type inconsistent with string length")
    arr = NumArray(shape=shape, type=type)
    strbuff = buffer(datastring)
    nelements = arr.nelements()
    # Currently uses only the byte-by-byte copy, should be optimized to use
    # larger element copies when possible.
    cfunc = _bytes.functionDict["copyNbytes"]
    cfunc((nelements,), strbuff, 0, (type.bytes,),
          arr._data, 0, (type.bytes,), type.bytes)
    if arr._type is _nt.Bool:
        ufunc.not_equal(arr, 0, arr)
    return arr

def fromfile(infile,type=None,shape=None,sizing=STRICT,
             typecode=None,dtype=None):
    """Read in an array from INFILE, a file-like object or a filename.

    INFILE must have a read() method. If it also has the tell() and
    seek() methods memory allocation will be more efficient when the
    SHAPE is unspecified or incomplete, and the filesize is
    unchanging.

    If INFILE is a string or unicode object, a file with that name is
    opened for reading.

    INFILE should be in binary mode on systems where that makes sense
    (e.g., Win32). read() must be blocking, and if len(read(size)) <
    size, it is assumed that end of file has been reached, and no
    further read()s are attempted.

    If TYPE is None, TYPE is set to Int32.

    If SHAPE is None, it is set to (-1,).

    Shape completely specified
    --------------------------

    When SHAPE is completely specified, exactly
    TYPE.bytes*product(SHAPE) bytes will be read from INFILE, from the
    current position. In this case SIZING must be STRICT. If
    end-of-file is reached before the required amount of data has been
    read, an EarlyEOFError is raised.

    Shape incompletely specified
    ----------------------------

    If SHAPE is incompletely specified (contains a -1), data will be
    read from INFILE until EOF is reached.

    The amount of data used to form the returned array will be a
    multiple of `record size', where record size is the product of
    TYPE.bytes and the specified elements of SHAPE. If the amount of
    data read when end-of-file is reached is not an exact multiple of
    the record size, one of the following will occur:

    1) if SIZING==STRICT, a SizeMismatchError is raised (default),
       
    2) if SIZING==SLOPPY, the extra data will be silently disregarded,

    3) if SIZING==WARN, a SizeMismatchWarning is issued.

    In cases (2) and (3), if there is extra data, an attempt is made
    to position the file cursor to just after the last byte actually
    used; this will only succeed if the file has a seek() method. If
    the seek() fails, a FileSeekWarning is issued, but no error is
    raised.
    """

    type = _nt._typeFromKeywords(type, typecode, dtype)

    if isinstance(infile,(str,unicode)):
        infile=open(infile,"rb")

    if type is None:
        type=_nt.Int32

    type = _nt.getType(type)  # allow for typenames and Numeric charcodes

    if shape is None:
        shape=(-1,)

    if(list(shape).count(-1)>1):
        raise ValueError("At most one unspecified dimension in shape")

    if -1 not in shape:
        if not sizing == STRICT:
            raise ValueError("sizing must be STRICT if size complete")
        arr=NumArray(type=type,shape=shape)
        bytesleft=type.bytes*_gen.product(shape)
        bytesread=0
        while bytesleft > _BLOCKSIZE:
            data=infile.read(_BLOCKSIZE)
            if len(data) != _BLOCKSIZE:
                raise EarlyEOFError("Unexpected EOF reading data for size complete array")
            arr._data[bytesread:bytesread+_BLOCKSIZE]=data
            bytesread += _BLOCKSIZE
            bytesleft -= _BLOCKSIZE
        if bytesleft > 0:
            data = infile.read(bytesleft)
            if len(data) != bytesleft:
                raise EarlyEOFError("Unexpected EOF reading data for size complete array")
            arr._data[bytesread:bytesread+bytesleft]=data
        return arr

    ##shape is incompletely specified
    ##read until EOF
    ##implementation 1: naively use memory blocks
    ##problematic because memory allocation can be double what is
    ##necessary (!)

    ##the most common case, namely reading in data from an unchanging
    ##file whose size may be determined before allocation, should be
    ##quick -- only one allocation will be needed.
    
    recsize = type.bytes * _gen.product([i for i in shape if i != -1])
    blocksize = max(_BLOCKSIZE/recsize, 1)*recsize

    ##try to estimate file size
    try:
        curpos=infile.tell()
        infile.seek(0,2)
        endpos=infile.tell()
        infile.seek(curpos)
    except (AttributeError, IOError):
        initsize=blocksize
    else:
        initsize=max(1,(endpos-curpos)/recsize)*recsize

    buf = memory.new_memory(initsize)

    bytesread=0
    while 1:
        data=infile.read(blocksize)
        if len(data) != blocksize: ##eof
            break
        ##do we have space?
        if len(buf) < bytesread+blocksize:
            buf=_resizebuf(buf,len(buf)+blocksize)
            ## or rather a=resizebuf(a,2*len(a)) ?
        assert len(buf) >= bytesread+blocksize
        buf[bytesread:bytesread+blocksize]=data
        bytesread += blocksize

    if len(data) % recsize != 0:
        if sizing == STRICT:
            raise SizeMismatchError("Filesize does not match specified shape")
        if sizing == WARN:
            _warnings.warn("Filesize does not match specified shape",
                           SizeMismatchWarning)
        try:
            infile.seek(-(len(data) % recsize),1)
        except AttributeError:
            _warnings.warn("Could not rewind (no seek support)",
                           FileSeekWarning)
        except IOError:
            _warnings.warn("Could not rewind (IOError in seek)",
                           FileSeekWarning)
    datasize = (len(data)/recsize) * recsize
    if len(buf) != bytesread+datasize:
        buf=_resizebuf(buf,bytesread+datasize)
    buf[bytesread:bytesread+datasize]=data[:datasize]
    ##deduce shape from len(buf)
    shape = list(shape)
    uidx = shape.index(-1)
    shape[uidx]=len(buf) / recsize

    a = NumArray(buffer=buf,shape=shape,type=type)
    if a._type is _nt.Bool:
        ufunc.not_equal(a, 0, a)
    return a
    
def _resizebuf(buf,newsize):
    "Return a copy of BUF of size NEWSIZE."
    newbuf=memory.new_memory(newsize)
    if newsize > len(buf):
        newbuf[:len(buf)]=buf
    else:
        newbuf[:]=buf[:len(newbuf)]
    return newbuf

class UsesOpPriority(object):
    """Classes can subclass from UsesOpPriority to signal to numarray
    that perhaps the class' r-operator hook (e.g. __radd__) should be
    given preference over NumArray's l-operator hook (e.g. __add__).
    This would be done so that when different object types are used in
    an operation (e.g. NumArray + MaskedArray) the type of the result
    is well defined and independent of the order of the operands
    (e.g. MaskedArray).

    Before altering the "normal" behavior of an operator, this scheme
    (implemented in the operator hook functions of NumArray) first
    checks to see if the other operand subclasses UsesOpPriority.  If
    it does, the op priorities of both operands are compared, and an
    appropriate hook function from the one with the highest priority
    is called.

    Thus, a subclass of NumArray which wants to ensure that its type
    dominates in mixed type operations should define a class level
    op_priority > 0.  If several subclasses wind up doing this,
    op_priority will determine how they relate to one another as well.
    """
    op_priority = 0.0


class NumArray(_numarray._numarray, _gen.NDArray, UsesOpPriority):
    """Fundamental numerical array class.
    
    NumArray(shape=None, type=None, buffer=None, byteoffset=0, bytestride=None,
             byteorder=sys.byteorder, aligned=1, real=None, imag=None)

    shape      The shape of the resulting array.
    
    type       The type of each data element, e.g. Int32.

    buffer     An object which supports the Python buffer protocol in C.

    byteoffset Offset in bytes from the start of 'buffer' to the array data.

    bytestride Distance in bytes between single elements.

    byteorder  The actual ordering of bytes in buffer: "big" or "little".

    aligned    Flag declaring whether array data is aligned, or not.  legacy.

    real       Initializer for real component of complex arrays.

    imag       Initializer for imaginary component of complex arrays.
    
    """
    if _PROTOTYPE:
        def __init__(self, shape=None, type=None, buffer=None,
                     byteoffset=0, bytestride=None, byteorder=_sys.byteorder,
                     aligned=1, real=None, imag=None):
            type = _nt.getType(type)
            itemsize = type.bytes
            _gen.NDArray.__init__(self, shape, itemsize, buffer,
                                  byteoffset, bytestride)
            self._type = type
            if byteorder in ["little", "big"]:
                self._byteorder = byteorder
            else:
                raise ValueError("byteorder must be 'little' or 'big'")
            if real is not None:
                self.real = real
            if imag is not None:
                self.imag = imag

        def _copyFrom(self, arr):
            """Copy elements from another array.

            This overrides the _generic NDArray version
            """
            # Test for simple case first
            if isinstance(arr, NumArray):
                if (arr.nelements() == 0 and self.nelements() == 0):
                    return
                if (arr._type == self._type and
                    self._shape == arr._shape and
                    arr._byteorder == self._byteorder and
                    _gen.product(arr._strides) != 0 and
                    arr.isaligned() and self.isaligned()):
                    name = 'copy'+`self._itemsize`+'bytes'
                    cfunc = ( _bytes.functionDict.get(name) or
                              _bytes.functionDict["copyNbytes"])
                    cfunc(self._shape, arr._data,  arr._byteoffset,  arr._strides,
                          self._data, self._byteoffset, self._strides,
                          self._itemsize)
                    return
            else:
                arr = array(arr)
            if self.rank == 0 and arr.nelements() == 1:
                barr = arr.view()
                barr._shape = ()
            else:
                barr = self._broadcast(arr)
                if barr is None:
                    raise ValueError('array sizes must be consistent')
            ufunc._copyFromAndConvert(barr, self)

        def view(self):
            """Returns a new array object which refers to the same data as the
            original array.  The new array object can be manipulated (reshaped,
            restrided, new attributes, etc.) without affecting the original array.
            Modifications of the array data *do* affect the original array.
            """
            v = _gen.NDArray.view(self)
            v._type = self._type
            v._byteorder = self._byteorder
            return v

    def __getstate__(self):
        """returns state of NumArray for pickling."""
        # assert not hasattr(self, "_shadows") # Not a good idea for pickling.
        state = _gen.NDArray.__getstate__(self)
        state["_byteorder"] = self._byteorder
        state["_type"]      = self._type.name
        return state

    def __setstate__(self, state):
        """sets state of NumArray after unpickling."""
        _gen.NDArray.__setstate__(self, state)
        self._byteorder = state["_byteorder"]
        self._type      = _nt.getType(state["_type"])

    def _put(self, indices, values, **keywds):
        ufunc._put(self, indices, values, **keywds)

    def _take(self, indices, **keywds):
        return ufunc._take(self, indices, **keywds)

    def getreal(self):
        if isinstance(self._type, _nt.ComplexType):
            t = _realtype(self._type)
            arr = NumArray(self._shape, t, buffer=self._data,
                           byteoffset=self._byteoffset,
                           bytestride=self._bytestride,
                           byteorder=self._byteorder)
            arr._strides = self._strides[:]
            return arr
        elif isinstance(self._type, _nt.FloatingType):
            return self
        else:
            return self.astype(_nt.Float64)            

    def setreal(self, value):
        if isinstance(self._type, (_nt.ComplexType, _nt.FloatingType)):
            self.getreal()[:] = value
        else:
            raise TypeError("Can't setreal() on a non-floating-point array")
                    
    real = property(getreal, setreal,
                    doc="real component of a non-complex numarray")

    def getimag(self):
        if isinstance(self._type, _nt.ComplexType):
            t = _realtype(self._type)
            arr = NumArray(self._shape, t, buffer=self._data,
                           byteoffset=self._byteoffset+t.bytes,
                           bytestride=self._bytestride,
                           byteorder=self._byteorder)
            arr._strides = self._strides[:]
            return arr
        else:
            zeros = self.new(_nt.Float64)
            zeros[:] = 0.0
            return zeros

    def setimag(self, value):
        if isinstance(self._type, _nt.ComplexType):
            self.getimag()[:] = value
        else:
            raise TypeError("Can't set imaginary component of a non-comlex type")

    imag = property(getimag, setimag,
                        doc="imaginary component of complex array")

    real = property(getreal, setreal,
                        doc="real component of complex array")

    setimaginary = setimag
    getimaginary = getimag
    imaginary = imag

    def conjugate(self):
        """Returns the conjugate a - bj of complex array a + bj."""
        a = self.copy()
        ufunc.minus(a.getimag(), a.getimag())
        return a

    def togglebyteorder(self):
        """reverses the state of the _byteorder attribute:  little <-> big."""
        self._byteorder = {"little":"big","big":"little"}[self._byteorder]

    def byteswap(self):
        """Byteswap data in place, leaving the _byteorder attribute untouched.
        """
        if self._itemsize == 1:
            return
        if isinstance(self._type, _nt.ComplexType):
            fname = "byteswap" + self._type.name
        else:
            fname = "byteswap"+str(self._itemsize)+"bytes"
            
        cfunc = _bytes.functionDict[fname]
        cfunc(self._shape, 
              self._data, self._byteoffset, self._strides,
              self._data, self._byteoffset, self._strides)
        
    _byteswap = byteswap  # alias to keep old code working.

    def byteswapped(self):
        """returns a byteswapped copy of self."""
        b = self.copy()
        b._byteswap()
        return b
        
    def sinfo(self):
        """info() prints out the key attributes of a numarray."""
        s = _gen.NDArray.sinfo(self)
        s += "byteorder: " + repr(self._byteorder) + "\n"
        s += "byteswap: " + repr(self.isbyteswapped()) + "\n"
        s += "type: " + repr(self._type) + "\n"
        return s

    def astype(self, type=None, typecode=None, dtype=None):
        """Return a copy of the array converted to the given type"""
        type = _nt._typeFromKeywords(type, typecode, dtype)
        if type is None:
            type = self._type
        if type == self._type:
            # always return a copy even if type is same
            return self.copy()
        if type._conv:
            retarr = self.__class__(buffer=None, shape=self._shape, type=type)
            if retarr.nelements() == 0:
                return retarr
            if self.is_c_array():
                _ufunc.CheckFPErrors()
                cfunc = self._type._conv.astype[type.name]
                cfunc(self.nelements(), 1, 1,
                      ((self._data, self._byteoffset), (retarr._data, 0)))
                errorstatus = _ufunc.CheckFPErrors()
                if errorstatus:
                    ufunc.handleError(errorstatus, " during type conversion")
            else:
                ufunc._copyFromAndConvert(self, retarr)
        elif type.fromtype:
            retarr = type.fromtype(self)
        else:
            raise TypeError("Don't know how to convert from %s to %s" %
                            (self._type.name, type.name))
        return retarr

    def __tonumtype__(self, type):
        """__tonumtype__ supports C-API NA_setFromPythonScalar permitting a rank-0
        array to be used in lieu of a numerical scalar value."""
        if self.rank != 0:
            raise ValueError("Can't use non-rank-0 array as a scalar.")
        if type is not self.type():
            s = self.astype(type)
        else:
            s = self
        return s[()]

    def is_c_array(self):
        """returns 1 iff an array is c-contiguous, aligned, and not
        byteswapped."""
        return (self.iscontiguous() and self.isaligned() and not
                self.isbyteswapped()) != 0

    def is_f_array(self):
        """returns 1 iff an array is fortan-contiguous, aligned, and not
        byteswapped."""
        return (self.is_fortran_contiguous() and self.isaligned() and not
                self.isbyteswapped())

    def new(self, type=None):
        """Return a new array of given type with same shape as this array

        Note this only creates the array; it does not copy the data.
        """
        if type is None:
            type = self._type
        return self.__class__(shape=self._shape, type=type)
        
    def type(self):
        """Return the type object for the array"""
        return self._type

    def copy(self):
        """Returns a native byte order copy of the array."""
        c = _gen.NDArray.copy(self)
        c._byteorder = self._byteorder
        c._type = self._type
        if self.isbyteswapped():
            c.byteswap()
            c.togglebyteorder()
        return c

    def __str__(self):
        return array_str(self)

    def __repr__(self):
        return array_repr(self)
        try:
            return array_repr(self)
        except:
            return "<<DEBUG NumArray.__repr__(): " + self.sinfo().rstrip() + ">>"

    if _PROTOTYPE:
        def __int__(self):
            if self.rank is 0:
                return int(self[()])
            elif self._shape == (1,):
                return int(self[0])
            else:
                raise TypeError, "Only rank-0 numarray can be cast to integers."

        def __float__(self):
            if self.rank is 0:
                return float(self[()])
            elif self._shape == (1,):
                return float(self[0])
            else:
                raise TypeError, "Only rank-0 or shape=(1,) numarray can be cast to floats."
        def __complex__(self):
            if self.rank is 0:
                return complex(self[()])
            elif self._shape == (1,):
                return complex(self[0])
            else:
                raise TypeError, "Only rank-0 or shape=(1,) numarray can be cast to complex."
        def __abs__(self): return ufunc.abs(self)
        def __neg__(self): return ufunc.minus(self)
        def __invert__(self): return ufunc.bitwise_not(self)
        def __pos__(self): return self

        def __add__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__radd__(self)
            else:
                return ufunc.add(self, operand)

        def __radd__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__add__(self)
            else:
                r = ufunc.add(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __sub__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rsub__(self)
            else:
                return ufunc.subtract(self, operand)

        def __rsub__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__sub__(self)
            else:
                r = ufunc.subtract(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __mul__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rmul__(self)
            else:
                return ufunc.multiply(self, operand)

        def __rmul__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__mul__(self)
            else:
                r = ufunc.multiply(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __div__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rdiv__(self)
            else:
                return ufunc.divide(self, operand)

        def __truediv__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rtruediv__(self)
            else:
                return ufunc.true_divide(self, operand)

        def __floordiv__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rfloordiv__(self)
            else:
                return ufunc.floor_divide(self, operand)

        def __rdiv__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__div__(self)
            else:
                r = ufunc.divide(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __rtruediv__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__truediv__(self)
            else:
                r = ufunc.true_divide(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __rfloordiv__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__floordiv__(self)
            else:
                r = ufunc.floor_divide(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __mod__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rmod__(self)
            else:
                return ufunc.remainder(self, operand)

        def __rmod__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__mod__(self)
            else:
                r = ufunc.remainder(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __divmod__(self,operand):
            """returns the tuple (self/operand, self%operand)."""
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rdivmod__(self)
            else:
                return ufunc.divide_remainder(self, operand)

        def __rdivmod__(self,operand):
            """returns the tuple (operand/self, operand%self)."""
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__divmod__(self)
            else:
                r = ufunc.divide_remainder(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __pow__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rpow__(self)
            else:
                return ufunc.power(self, operand)

        def __rpow__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__pow__(self)
            else:
                r = ufunc.power(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __and__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rand__(self)
            else:
                return ufunc.bitwise_and(self, operand)

        def __rand__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__and__(self)
            else:
                r = ufunc.bitwise_and(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __or__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__ror__(self)
            else:
                return ufunc.bitwise_or(self, operand)

        def __ror__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__or__(self)
            else:
                r = ufunc.bitwise_or(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __xor__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rxor__(self)
            else:
                return ufunc.bitwise_xor(self, operand)

        def __rxor__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__xor__(self)
            else:
                r = ufunc.bitwise_xor(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __rshift__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rrshift__(self)
            else:
                return ufunc.rshift(self, operand)

        def __rrshift__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rshift__(self)
            else:
                r = ufunc.rshift(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        def __lshift__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__rlshift__(self)
            else:
                return ufunc.lshift(self, operand)

        def __rlshift__(self, operand):
            if (isinstance(operand, UsesOpPriority) and
                self.op_priority < operand.op_priority):
                return operand.__lshift__(self)
            else:
                r = ufunc.lshift(operand, self)
                if isinstance(r, NumArray):
                    r.__class__ = self.__class__
                return r

        # augmented assignment operators

        def __iadd__(self, operand):
            ufunc.add(self, operand, self)
            return self

        def __isub__(self, operand):
            ufunc.subtract(self, operand, self)
            return self

        def __imul__(self, operand):
            ufunc.multiply(self, operand, self)
            return self

        def __idiv__(self, operand):
            ufunc.divide(self, operand, self)
            return self

        def __ifloordiv__(self, operand):
            ufunc.floor_divide(self, operand, self)
            return self

        def __itruediv__(self, operand):
            ufunc.true_divide(self, operand, self)
            return self

        def __imod__(self, operand):
            ufunc.remainder(self, operand, self)
            return self

        def __ipow__(self, operand):
            ufunc.power(self, operand, self)
            return self

        def __iand__(self, operand):
            ufunc.bitwise_and(self, operand, self)
            return self

        def __ior__(self, operand):
            ufunc.bitwise_or(self, operand, self)
            return self

        def __ixor__(self, operand):
            ufunc.bitwise_xor(self, operand, self)
            return self

        def __irshift__(self, operand):
            ufunc.rshift(self, operand, self)
            return self

        def __ilshift__(self, operand):
            ufunc.lshift(self, operand, self)
            return self

        # rich comparisons (only works in Python 2.1 and later)

        def __lt__(self, operand):
            if isinstance(self._type, _nt.ComplexType):
                raise TypeError("Complex NumArrays don't support >, >=, <, <= operators")
            else:
                return ufunc.less(self, operand)

        def __gt__(self, operand):
            if isinstance(self._type, _nt.ComplexType):
                raise TypeError("Complex NumArrays don't support >, >=, <, <= operators")
            else:
                return ufunc.greater(self, operand)

        def __le__(self, operand):
            if isinstance(self._type, _nt.ComplexType):
                raise TypeError("Complex NumArrays don't support >, >=, <, <= operators")
            else:
                return ufunc.less_equal(self, operand)

        def __ge__(self, operand):
            if isinstance(self._type, _nt.ComplexType):
                raise TypeError("Complex NumArrays don't support >, >=, <, <= operators")
            else:
                return ufunc.greater_equal(self, operand)

        def __eq__(self, operand):
            if operand is None:
                return 0
            else:
                return ufunc.equal(self, operand)
        def __ne__(self, operand):
            if operand is None:
                return 1
            else:
                return ufunc.not_equal(self, operand)

    def sort(self, axis=-1, kind='quicksort'):
        """sorts an array in-place along the specified axis, returning None.

        kind can be one of '', 'quicksort', 'heapsort', 'mergesort'
        """
        if not kind :
            if axis==-1:
                ufunc._sortN(self)
            else:
                self.swapaxes(axis,-1)
                ufunc._sortN(self)
                self.swapaxes(axis,-1)
        else :
            ufunc._direct_sort(self, kind, axis)

    def _argsort(self, axis=-1):
            if axis==-1:
                ashape = self.getshape()
                w = array(shape=ashape, type=_nt.Long)
                w[...,:] = arange(ashape[-1], type=_nt.Long)
                ufunc._argsortN(self,w)
                return w
            else:
                self.swapaxes(axis,-1)
                w = self._argsort()
                w.swapaxes(axis, -1)
                return w

    def argsort(self, axis=-1, kind='quicksort'):
        """returns the indices of 'self' which if taken from self would return
        a copy of 'self' sorted along the specified 'axis'.

        kind can be one of '', 'quicksort', 'heapsort', 'mergesort'
        """
        if not kind :
            return self.copy()._argsort(axis)
        else :
            return ufunc._indirect_sort(self, kind, axis)
            
    def argmax(self, axis=-1):
        """returns the index(es) of the greatest element(s) of 'self' along the
        specified 'axis'."""
        import numeric
        return numeric.argmax(self, axis)

    def argmin(self, axis=-1):
        """returns the index(es) of the least element(s) of 'self' along the
        specified 'axis'."""
        import numeric
        return numeric.argmin(self, axis)

    def diagonal(self, *args, **keywords):
        """returns the diagonal elements of the array."""
        return diagonal(self, *args, **keywords)

    def trace(self, *args, **keywords):
        """returns the sum of the diagonal elements of the array."""
        return trace(self, *args, **keywords)

    def typecode(self):
        """returns the Numeric typecode of the array."""
        t = _nt.typecode[self._type]
        if _numinc.LP64 and t == "N":
            t = "l"   # Long is Int64 on LP64 platforms.  'l' is compatible
                      # with Numeric 
        return t

    def _getdtype(self):
        """NumPy compatible dtype syntax."""
        return _dtype._dtypes[self._type]
    dtype = property(_getdtype, None, "numpy type syntax")

    def min(self):
        """Returns the minimum element in the array."""
        return ufunc.minimum.reduce(ufunc.minimum.areduce(self).flat)

    def max(self):
        """Returns the maximum element in the array."""
        return ufunc.maximum.reduce(ufunc.maximum.areduce(self).flat)

    def sum(self, type=None):
        """Returns the sum of all elements in the array."""
        if type is None:
            type = _nt.MaximumType(self._type)
        return ufunc.add.reduce(ufunc.add.areduce(self, type=type).flat, type=type)

    def mean(self):
        """Returns the average of all elements in the array."""
        return self.sum()/(self.nelements()*1.0)

    def stddev(self):
        """Returns the standard deviation of the array."""
        m = self.mean()
        N = self.nelements()
        return math.sqrt(((self - m)**2).sum()/(N-1))

    def spacesaver(self):
        """Always False.  Supported for Numeric compatibility."""
        return 0

class ComplexArray(NumArray):   # Deprecated
    pass

def Complex32_fromtype(arr):
    """Used for converting other types to Complex32.

    This is used to set an fromtype attribute of the ComplexType objects"""
    rarr = arr.astype(Float32)
    retarr = ComplexArray(arr._shape, _nt.Complex32)
    retarr.getreal()[:] = rarr
    retarr.getimag()[:] = 0.
    return retarr

def Complex64_fromtype(arr):
    """Used for converting other types to Complex64.

    This is used to set an fromtype attribute of the ComplexType objects"""
    rarr = arr.astype(Float64)
    retarr = ComplexArray(arr._shape, _nt.Complex64)
    retarr.getreal()[:] = rarr
    retarr.getimag()[:] = 0.
    return retarr

# Check whether byte order is big endian or little endian.

from sys import byteorder
isBigEndian = (byteorder == "big")
del byteorder

# Add fromtype function to Complex types

_nt.Complex32.fromtype = Complex32_fromtype
_nt.Complex64.fromtype = Complex64_fromtype

# Return type of complex type components

def _isComplexType(type):  # Deprecated
    return type in [_nt.Complex32, _nt.Complex64]

def _realtype(complextype):
    if complextype == _nt.Complex32:
        return _nt.Float32
    else:
        return _nt.Float64

def conjugate(a):
    """conjugate(a) returns the complex conjugate of 'a'"""
    a = asarray(a)
    if not isinstance(a._type, _nt.ComplexType):
        a = a.astype(_nt.Complex64)
    return a.conjugate()

def zeros(shape, type=None, typecode=None, dtype=None):
    """Constructs a zero filled array of the specified shape and type."""
    type = _nt._typeFromKeywords(type, typecode, dtype)
    if type is None:
        type = _nt.Long
    retarr = NumArray(shape=shape, type=type)
    retarr._data.clear()
    return retarr

def ones(shape, type=None, typecode=None, dtype=None):
    """Constructs an array of the specified shape and type filled with ones."""
    type = _nt._typeFromKeywords(type, typecode, dtype)
    shape = _gen.getShape(shape)
    retarr = _fillarray(_gen.product(shape), 1, 0, type)
    retarr.setshape(shape)
    return retarr

def arange(a1, a2=None, stride=1, type=None, shape=None, typecode=None, dtype=None):
    """Returns an array of numbers in sequence over the specified range."""
    # Return empty range of correct type for single negative paramter.
    type = _nt._typeFromKeywords(type, typecode, dtype)
    if not isinstance(a1, types.ComplexType) and a1 < 0 and a2 is None:
        t = __builtins__["type"](a1)
        return zeros(shape=(0,), type=Py2NumType[t])
    if a2 == None:
        start = 0 + (0*a1) # to make it same type as stop
        stop  = a1
    else:
        start = a1 +(0*a2)
        stop  = a2
    delta = (stop-start)/stride ## xxx int divide issue
    if _type(delta) == types.ComplexType:
        # xxx What make sense here?
        size = int(math.ceil(delta.real))
    else:
        size = int(math.ceil((stop-start)/float(stride)))
    if size < 0:
        size = 0
    r = _fillarray(size, start, stride, type)
    if shape is not None:
        r.setshape(shape)
    return r

arrayrange = arange  # alias arange as arrayrange.

def identity(n, type=None, typecode=None, dtype=None):
    """Returns an array resembling and identity matrix."""
    type = _nt._typeFromKeywords(type, typecode, dtype)
    a = zeros(shape=(n,n), type=type)
    i = arange(n)
    a.put((i, i), 1, axis=(0,))
    return a

def allclose (array1, array2, rtol=1.e-5, atol=1.e-8): # From Numeric 20.3
    """allclose returns true if all components of array1 and array2 are
    equal subject to given tolerances.  The relative error rtol must be
    positive and << 1.0.  The absolute error atol comes into play for those
    elements of y that are very small or zero; it says how small x must be
    also.
    """
    x, y = asarray(array1), asarray(array2)
    d = ufunc.less(ufunc.abs(x-y), atol + rtol * ufunc.abs(y))
    return bool(alltrue(_gen.ravel(d)))

# JC's revised diagonal for supporting dimensions > 2 correctly.
def diagonal(a, offset= 0, axis1=0, axis2=1):
    """diagonal(a, offset=0, axis1=0, axis2=1) returns the given diagonals
    defined by the last two dimensions of the array.
    """
    a = inputarray(a)
    ###  do not swap axis1 and axis2, so the offset sign is meaningful
    ###if axis2 < axis1: axis1, axis2 = axis2, axis1
    ###if axis2 > 1:
    if 1: ###
        new_axes = range(len(a._shape))
        ###del new_axes[axis2]; del new_axes[axis1]
        try: ###
            new_axes.remove(axis1)  ###
            new_axes.remove(axis2)  ###
        except: ###
            raise ValueError, "axis1(=%d) and axis2(=%d) must be different and within range." % (axis1, axis2) ###
        ###new_axes [0:0] = [axis1, axis2]
        new_axes = new_axes + [axis1, axis2] ### insert at the end, not the beginning
        a = _gen.transpose(a, new_axes)
    s = a._shape
    rank = len(s) ###
    if rank == 2: ###
        n1 = s[0]
        n2 = s[1]
        n = n1 * n2
        s = (n,)
        a = _gen.reshape(a, s)
        if offset < 0:
            return _gen.take(a, range(- n2 * offset, min(n2, n1+offset) *
                                      (n2+1) - n2 * offset, n2+1), axis=0)
        else:
            return _gen.take(a, range(offset, min(n1, n2-offset) *
                                      (n2+1) + offset, n2+1), axis=0)
    else :
        my_diagonal = []
        for i in range(s[0]):
            my_diagonal.append(diagonal(a[i], offset, rank-3, rank-2)) ###
        return array(my_diagonal)

# From Numeric-21.0
def trace(array, offset=0, axis1=0, axis2=1):
    """trace returns the sum along diagonals (defined by the last
    two dimenions) of the array.
    """
    return ufunc.add.reduce(diagonal(array, offset, axis1, axis2))

# From Numeric-23.6
def average (a, axis=0, weights=None, returned = 0):
    """average(a, axis=0, weights=None)
       Computes average along indicated axis.
       If axis is None, average over the entire array.
       Inputs can be integer or floating types; result is type Float.

       If weights are given, result is:
           sum(a*weights)/(sum(weights))
       weights must have a's shape or be the 1-d with length the size
       of a in the given axis. Integer weights are converted to Float.

       Not supplying weights is equivalent to supply weights that are
       all 1.

       If returned, return a tuple: the result and the sum of the weights
       or count of values. The shape of these two results will be the same.

       raises ZeroDivisionError if appropriate when result is scalar.
       (The version in MA does not -- it returns masked values).
    """
    if axis is None:
        a = array(a).flat
        if weights is None:
            n = ufunc.add.reduce(a)
            d = len(a) * 1.0
        else:
            w = array(weights).flat * 1.0
            n = ufunc.add.reduce(a*w)
            d = ufunc.add.reduce(w)
    else:
        a = array(a)
        ash = a.shape
        if ash == ():
            a.shape = (1,)
        if weights is None:
            n = ufunc.add.reduce(a, axis)
            d = ash[axis] * 1.0
            if returned:
                d = ones(shape(n)) * d
        else:
            w = array(weights, copy=0) * 1.0
            wsh = w.shape
            if wsh == ():
                wsh = (1,)
            if wsh == ash:
                n = ufunc.add.reduce(a*w, axis)
                d = ufunc.add.reduce(w, axis)
            elif wsh == (ash[axis],):
                ni = ash[axis]
                r = [NewAxis]*ni
                r[axis] = slice(None,None,1)
                w1 = eval("w["+repr(tuple(r))+"]*ones(ash, Float)")
                n = ufunc.add.reduce(a*w1, axis)
                d = ufunc.add.reduce(w1, axis)
            else:
                raise ValueError, 'average: weights wrong shape.'

    if not isinstance(d, ArrayType):
        if d == 0.0:
            raise ZeroDivisionError, 'Numeric.average, zero denominator'
    if returned:
        return n/d, d
    else:
        return n/d

def rank(array):
    """rank returns the number of dimensions in an array."""
    return len(shape(array))

def shape(array):
    """shape returns the shape tuple of an array."""
    try:
        return array.shape
    except AttributeError:
        return asarray(array).getshape()

def size(array, axis=None):
    """size  returns the number of elements in an array or
    along the specified axis."""
    array = asarray(array)
    if axis is None:
        return array.nelements()
    else:
        s = array.shape
        if axis < 0:
            axis += len(s)
        return s[axis]

def array_str(array, max_line_width=MAX_LINE_WIDTH, precision=PRECISION,
              suppress_small=SUPPRESS_SMALL):
    """Formats and array as a string."""
    array = asarray(array)
    return arrayprint.array2string(
        array, max_line_width, precision, suppress_small, ' ', "")

def array_repr(array, max_line_width=MAX_LINE_WIDTH, precision=PRECISION,
               suppress_small=SUPPRESS_SMALL):
    """Returns the repr string of an array."""
    array = asarray(array)
    if array.nelements() > 0 or array.shape==(0,):
        lst = arrayprint.array2string(
            array, max_line_width, precision, suppress_small, ', ', "array(")
    else: # show zero-length shape unless it is (0,)
        lst = "[], shape=%s" % (repr(array.shape),)
    typeless = array._type in [_nt.Long, _nt.Float, _nt.Complex] and \
               not hasattr(array, "_explicit_type")
    if array.__class__ is not NumArray:
        cName= array.__class__.__name__
    else:
        cName = "array"
    if typeless and array.nelements():
        return cName + "(%s)" % lst
    else:
        if typeless:
            typename = { _nt.Long: "Long",
                         _nt.Float: "Float",
                         _nt.Complex: "Complex" }[array._type]
        else:
            typename = array._type.name
        return cName + "(%s, type=%s)" % (lst, typename)

def around(array, digits=0, output=None):
    """rounds 'array'  to 'digits' of precision, storing the result in
    'output', or returning the result as new array if output=None"""
    array = asarray(array)
    scale = 10.0**digits
    if output is None:
        wout = array.copy()
    else:
        wout = output
    if digits != 0:
        wout *= scale  # bug in 2.2.1 and earlier causes fail as bad sequence op
    ufunc._round(wout, wout)
    if digits != 0:
        wout /= scale
    if output is None:
        return wout

def round(*args, **keys):
    _warnings.warn("round() is deprecated.  Switch to around().",
                   DeprecationWarning)
    return ufunc._round(*args, **keys)

def explicit_type(x):
    """explicit_type(x) returns a view of x which will always show it's type in it's repr.
    This is useful when the same test is run in two places, one where the type used *is* the
    default and hence not normally repr'ed, and one where the type used *is not* the default
    and so is displayed.
    """
    y = x.view()
    y._explicit_type = 1
    return y

ArrayType = NumArray  # Alias for backwards compatability with Numeric

def array_equiv(array1, array2):

    """array_equiv returns True if 'a' and 'b' are shape consistent
    (mutually broadcastable) and have all elements equal and False
    otherwise."""
    
    try:
        array1, array2 = asarray(array1), asarray(array2)
    except TypeError:
        return 0
    if not isinstance(array1, NumArray) or not isinstance(array2, NumArray):
        return 0
    try:
        array1, array2 = array1._dualbroadcast(array2)
    except ValueError:
        return 0
    return ufunc.logical_and.reduce(_gen.ravel(array1 == array2))

def array_equal(array1, array2):
    
    """array_equal returns True if array1 and array2 have identical shapes
    and all elements equal and False otherwise."""

    try:
        array1, array2 = asarray(array1), asarray(array2)
    except TypeError:
        return 0
    if not isinstance(array1, NumArray) or not isinstance(array2, NumArray):
        return 0
    if array1._shape != array2._shape:
        return 0
    return ufunc.logical_and.reduce(_gen.ravel(array1 == array2))


class _UBuffer(NumArray):
    """_UBuffer is used to hold a single "block" of ufunc data during
    the block-wise processing of all elements in an array.

    Subclassing the buffer object from numnumarray simplifies (and speeds!)
    their usage at the C level.  They are not intended to be used as
    public array objects, hence they are private!
    """

    def __init__(self, pybuffer):
        NumArray.__init__(self, (len(pybuffer),), _nt.Int8, pybuffer)
        self._strides    = None   # how it is distinguished from a real array
        self._buffersize = len(pybuffer)

    def isbyteswapped(self):           return 0    
    def isaligned(self):               return 1
    def iscontiguous(self):            return 1
    def is_c_array(self):              return 1
    def _getByteOffset(self, shape):   return 0
    
    def __del__(self):
        """On deletion return the data to the buffer pool"""
        if self._data is not None:
            if ufunc is not None and ufunc._bufferPool is not None:
                ufunc._bufferPool.buffers.append(self._data)

    def __repr__(self):
        return "<_UBuffer of size:%d>" % self._buffersize

import ufunc
import arrayprint

sum = ufunc.add.reduce
cumsum = ufunc.add.accumulate
product = ufunc.multiply.reduce
cumproduct = ufunc.multiply.accumulate
absolute = ufunc.abs  
negative = ufunc.minus
fmod = ufunc.remainder

def sign(m): # From Numeric
    """sign(m) gives an array with shape of m with elements defined by sign
    function:  where m is less than 0 return -1, where m greater than 0, a=1,
    elsewhere a=0.
    """
    m = asarray(m)
    return zeros(m.shape)-ufunc.less(m,0)+ufunc.greater(m,0)

def alltrue(array, axis=0):
    """For 1D arrays, returns True IFF all the elements of the array are nonzero.
    For higher dimensional arrays, returns a reduction.
    """
    return ufunc.logical_and.reduce(array, axis=axis)
    
def sometrue(array, axis=0):
    """For 1D arrays, returns True IFF any one of the elements of the array is nonzero.
    For higher dimensional arrays, returns a reduction.
    """
    return ufunc.logical_or.reduce(array, axis=axis)

NewArray = NumArray  # unnecessary following merger of real/complex Arrays 

if _PROTOTYPE:
    # From SciPy.  Too critical not to have duplicated here.
    def any(x):
        """Return true if any elements of x are true:  sometrue(ravel(x))
        """
        return sometrue(_gen.ravel(x))


    def all(x):
        """Return true if all elements of x are true:  alltrue(ravel(x))
        """
        return alltrue(_gen.ravel(x))
else:
    from numarray._numarray import any, all



