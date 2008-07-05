"""This module partially implements a numpy-like dtype class.

My intent is to make it easier for people to migrate to NumPy, not to
reimplement NumPy, so numarray.dtype only implements a dtype "veneer"
which can be used for declaring numerical arrays in a numpy-like
style.

Only basic usages are supported:

numerical arrays
simple attributes

NOT supported:

string and unicode arrays
record arrays
object arrays
byteswapping and alignment info
shaped numerical arrays
field names

>>> import numarray as na
>>> import numarray.dtype

Most of the numarray factory functions support a dtype parameter.  I
think this generally works best specified in one of the string forms
but dtype instances also work.

>>> x = na.arange(10, dtype='int32')
>>> x = na.zeros((10,), dtype='=i4')
>>> x = na.arange(10, dtype=numarray.dtype.int32)
>>> x = na.array([1,2,3], dtype=x.dtype)
>>> a = na.zeros((10,), dtype='q')

isbuiltin defines a type as having a primitive C implementation.

>>> int(a.dtype.isbuiltin)
1

isnative defines a type as being in the native system byte order.

>>> int(a.dtype.isnative)
1

dtype's kind character identifies it is boolean, int, unsigned int,
float, or complex:

>>> a.dtype.kind
'i'

dtype's char attribute fully specifies the type in native byte order
and alignment.

>>> a.dtype.char
'q'

dtype's type attribute refers to the native numarray NumericType object.

>>> a.dtype.type
Int64

dtype's name attribute gives the numpy name for the type/dtype.

>>> a.dtype.name
'int64'

dtype's itemsize attribute gives the size of the dtype in bytes.

>>> a.dtype.itemsize
8

dtype's str attribute gives a terse string specification for the
dtype, including byteorder.

>> a.dtype.str  # byteorder char is non-portable
'<i8'

dtype's fields attribute is stubbed as None.
>>> a.dtype.fields

dtype's arrdescr is fairly useless in numarray:
>> a.dtype.arrdescr
[('', '<i4')]
"""

pass

"""
Below are working notes which define more of how the dtype works for
numpy:

A dtype is defined by a descriptor which may be describe a single
field or a sequence of fields:

array_descr --> field
array_descr -->  [ field+ ]

Each field is either a simple type, a tuple naming the field and
defining it's type, or a tuple defining a named type which is repeated
over a shape:

field --> type
field --> fieldname, type
field --> fieldname, type, shape
field --> array_descr

'<i4' and ('foo', '>i2') and ('bar', '<i4', (10, 10)) are all valid
field descriptors.

Each fieldname is either a simple string or a tuple consisting of an
arbitrary "fullname" and a basicname which must be useable as a Python
identifier.

fieldname --> str
fieldname --> (fullname, basicname)

fullname --> str
basicname --> [A-Za-z_]+[A-Za-z_0-9]*  # Python identifier

type --> endian_char kind bytes

endian_char -->  > or < or | or =

>            big endian
<            little endian
|            unordered
=            system byte order

type kinds are defined by single character codes as follows:

t             bitfield
b             boolean
i             integer
u             unsigned integer
f             float
c             complex
O             object
S             string
U             unicode
V             void

bytes defines the number of bytes of storage for the type

bytes --> [0-9]+

"""

import sys
import numerictypes as _nt

_numpy_descr_chars = "tbiufcOSUV"

_endian_specs = {
    "big": ">",
    "little": "<",
    "unordered" : "|"
    }

class dtype(object):
    """Partial implementation of numpy-like dtype class."""
    
    def __new__(subclass, typestr, name=None, char=None, type=None):

        assert typestr[1] in _numpy_descr_chars

        self = object.__new__(subclass)

        self.arrdescr = [('', typestr)]
        self.byteorder = typestr[0]
        self.kind = typestr[1]
        self.itemsize = int(typestr[2:])
        self.name = name
        self.type = type
        self.char = char
        self.fields = None

        return self

    def __repr__(self):
        typestr = self.byteorder + self.kind + str(self.itemsize)
        return "dtype('" + typestr + "')"

    def _get_str(self):
        return self.typestr
    str = property(_get_str, None, "")

    def _get_isnative(self):
        return ((sys.byteorder == "little" and self.byteorder == "<") or
                (sys.byteorder == "big" and self.byteorder == ">") or
                (self.byteorder == "|"))
    isnative = property(_get_isnative, None, "type is in native byte order.")

    def _get_isbuiltin(self):
        return self.kind in "biufc"

    isbuiltin = property(_get_isbuiltin, None, "is a primitive numeric type.")

    """
    def _get_alignment(self):
        return self.itemsize
    alignment = property(_get_alignment, None, "buffer address should be a multiple of")

    def newbyteorder(self):
        neworder = {">":"<","<":">","|":"|" }[ self.byteorder ]
        descr = neworder + self.kind + str(self.itemsize)
        return self.__class__(descr) #, self.name, self.char, self.type)
    """

_dtypes = {}

def get_dtype(d):
    if isinstance(d, dtype):
        return d
    else:
        return _dtypes[d]

def _dtype(spec, name, char, type):
    order = _endian_specs[sys.byteorder]
    t = dtype(order+spec, name, char, type)
    _dtypes[ spec ] = t
    _dtypes[ "=" + spec ] = t    
    _dtypes[ order + spec ] = t
    _dtypes[ name ] = t
    _dtypes[ char ] = t
    _dtypes[ type ] = t
    return t

bool_ = _dtype("b1", "bool_", "?", _nt.Bool)
bool8 = _dtype("b1", "bool8", "?", _nt.Bool)

int8   = _dtype("i1", "int8", "b", _nt.Int8)
uint8  = _dtype("u1", "uint8", "B", _nt.UInt8)

int16  = _dtype("i2", "int16", "h", _nt.Int16)
uint16 = _dtype("u2", "uint16", "H", _nt.UInt16)

int32  = _dtype("i4", "int32", "i", _nt.Int32)
uint32 = _dtype("u4", "uint32", "I", _nt.UInt32)

int64  = _dtype("i8", "int64", "q", _nt.Int64)
uint64 = _dtype("u8", "uint64", "Q", _nt.UInt64)

float32 = _dtype("f4", "float32", "f", _nt.Float32)
float64 = _dtype("f8", "float64", "d", _nt.Float64)

complex64 = _dtype("c8", "complex64", "F", _nt.Complex32)
complex128 = _dtype("c16", "complex128", "D", _nt.Complex64)

def test():
    import doctest, numarray.dtype
    return doctest.testmod(numarray.dtype)

