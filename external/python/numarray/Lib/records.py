import numarrayall as num
import numerictypes as _nt
import generic as _gen
import memory
import strings as chararray
import sys, os, re, types, stat

__version__ = '$Revision: 1.29 $ $Date: 2006/04/25 19:15:56 $'

class Char:
    """ data type Char class"""
    bytes = 1
    def __repr__(self):
        return "CharType"
    
    def __cmp__(self, other):
        return not isinstance(other,Char)
    
    def __hash__(self):
        """Allow type & name to be used interchangeably
        as dict key"""
        return hash("CharType")

CharType = Char()

# translation table to the data types
# Use new "letter codes" (e.g. 'u1', 'u2', 'u4', etc.)
# u = unsigned, i = integer, f = float, c = complex, b = boolean, and a = char.
# the number follows is the total number of bytes used, so f4 is single 
# precision (4-bytes) float and c8 is single precison complex (since it takes 
# two 4-bytes).
# All Numeric charcodes are removed. "a" means CharArray type
_numfmt = {'a':CharType,
          'Int8':num.Int8,   'UInt8':num.UInt8,
          'Int16':num.Int16, 'UInt16':num.UInt16,
          'Int32':num.Int32, 'UInt32':num.UInt32,
          'Int64':num.Int64, 'UInt64':num.UInt64,
          'Float32':num.Float32,     'Float64':num.Float64,
          'Complex32':num.Complex32, 'Complex64':num.Complex64,
          'Bool':num.Bool}
numfmt = {'i1':num.Int8,  'u1':num.UInt8, 
          'i2':num.Int16, 'u2':num.UInt16,
          'i4':num.Int32, 'u4':num.UInt32, 
          'i8':num.Int64, 'u8':num.UInt64,
          'f4':num.Float32, 'f8':num.Float64,
          'c8':num.Complex32, 'c16':num.Complex64,
          'b1':num.Bool}
numfmt.update(_numfmt)
 
# the reverse translation table of the above
revfmt = {}
for key in _numfmt.keys():
    revfmt[_numfmt[key]]=key

# formats regular expression
# allows multidimension spec with a tuple syntax in front of the letter code
# '(2,3)f4' and ' (  2 ,  3  )  f4  ' are equally allowed
format_re = re.compile(r'(?P<repeat> *[(]?[ ,0-9]*[)]? *)(?P<dtype>[A-Za-z0-9.]*)')

def fromrecords(recList, formats=None, names=None, shape=0, byteorder=sys.byteorder, aligned=0):
    """ create a Record Array from a list of records in text form

        The data in the same field can be heterogeneous, they will be promoted
        to the highest data type.  This method is intended for creating
        smaller record arrays.  If used to create large array e.g.

        r=fromrecords([[2,3.,'abc']]*100000)

        it is slow.

    >>> r=fromrecords([[456,'dbe',1.2],[2,'de',1.3]],names='col1,col2,col3')
    >>> print r[0]
    (456, 'dbe', 1.2)
    >>> r.field('col1')
    array([456,   2])
    >>> r.field('col2')
    CharArray(['dbe', 'de'])
    >>> import cPickle
    >>> print cPickle.loads(cPickle.dumps(r))
    RecArray[ 
    (456, 'dbe', 1.2),
    (2, 'de', 1.3)
    ]
    """

    if shape == 0:
        _shape = len(recList)
    else:
        _shape = shape

    _nfields = len(recList[0])
    for _rec in recList:
        if len(_rec) != _nfields:
            raise ValueError, "inconsistent number of objects in each record"
    arrlist = [0]*_nfields
    for col in range(_nfields):
        tmp = [0]*_shape
        for row in range(_shape):
            tmp[row] = recList[row][col]
        try:
            arrlist[col] = num.array(tmp)
        except:
            try:
                arrlist[col] = chararray.array(tmp)
            except:
                raise ValueError, "inconsistent data at row %d,field %d" % (row, col)
    _array = fromarrays(arrlist, formats=formats, shape=_shape, names=names,
                byteorder=byteorder, aligned=aligned)
    del arrlist
    del tmp
    return _array

def letterCode(key=None):
    """Print 'letter code' in record array."""

    _dict = {'u1': 'unsigned integer of 1 byte (=UInt8)',
             'u2': 'unsigned integer of 2 bytes (=UInt16)',
             'u4': 'unsigned integer of 4 bytes (=UInt32)',
             'u8': 'unsigned integer of 8 bytes (=UInt64)',
             'i1': 'signed integer of 1 byte (=Int8)',
             'i2': 'signed integer of 2 bytes (=Int16)',
             'i4': 'signed integer of 4 bytes (=Int32)',
             'i8': 'signed integer of 8 bytes (=Int64)',
             'f4': '"single precison" floating point number (=Float32)',
             'f8': '"double precison" floating point number (=Float64)',
             'c8': '"single precison" complex number, (=Complex32)',
             'c16': '"double precison" complex number, (=Complex64)',
             'b1': 'Boolean (=Bool)',
             'a': 'character string, e.g. a5 is a string of 5 characters/bytes',
            }
    list =  _dict.keys()

    def _print(key):
        print "%s: %s" % (key, _dict[key])

    if key is None or key == '':
        list.sort()
        for i in list:
            _print(i)
    else:
        if key in list:
            _print(key)
        else:
            print "Undefined letter code %s." % key

def fromarrays(arrayList, formats=None, names=None, shape=0, byteorder=sys.byteorder, aligned=0):
    """ create a Record Array from a list of num/char arrays

    >>> x1=num.array([1,2,3,4])
    >>> x2=chararray.array(['a','dd','xyz','12'])
    >>> x3=num.array([1.1,2,3,4])
    >>> r=fromarrays([x1,x2,x3],names='a,b,c')
    >>> print r[1]
    (2, 'dd', 2.0)
    >>> x1[1]=34
    >>> r.field('a')
    array([1, 2, 3, 4])
    """

    if shape == 0:
        _shape = len(arrayList[0])
    else:
        _shape = shape

    if formats == None:

        # go through each object in the list to see if it is a numarray or
        # chararray and determine the formats
        formats = ''
        for obj in arrayList:
            if len(obj._shape) == 1: 
                _repeat = ''
            elif len(obj._shape) >= 2:
                _repeat = `obj._shape[1:]`
            if isinstance(obj, chararray.RawCharArray):
                formats += _repeat + 'a' + `obj._itemsize` + ','
            elif isinstance(obj, num.NumArray):
                formats += _repeat + revfmt[obj._type] + ','
            else:
                raise ValueError, "item in the array list must be numarray or chararray"
        formats=formats[:-1]

    for obj in arrayList:
        if len(obj) != _shape:
            raise ValueError, "array has different lengths"

    _array = RecArray(None, formats=formats, shape=_shape, names=names,
                byteorder=byteorder, aligned=aligned)

    # populate the record array (make a copy)
    for i in range(len(arrayList)):
        try:
            a = arrayList[i]
            if (isinstance(a, chararray.RawCharArray) and a._itemsize == 0):
                a = "\0x00"
            _array.field(_array._names[i])[:] = a
        except TypeError: # trap only TypeError to get better messages
            raise TypeError, "Incorrect format %s, copy unsuccessful." % _array._formats[i]
    return _array

def fromstring(datastring, formats, shape=0, names=None, byteorder=sys.byteorder, aligned=0):
    """ create a Record Array from binary data contained in a string"""
    _array = RecArray(chararray._stringToBuffer(datastring), formats, shape, names,
                byteorder=byteorder, aligned=aligned)
    if _gen.product(_array._shape)*_array._itemsize > len(datastring):
        raise ValueError("Insufficient input data.")
    else: 
        return _array

def fromfile(file, formats, shape=-1, names=None, byteorder=sys.byteorder, aligned=0):
    """Create an array from binary file data

    If file is a string then that file is opened, else it is assumed
    to be a file object. No options at the moment, all file positioning
    must be done prior to this function call with a file object

    >>> import testdata, sys
    >>> fd=open(testdata.filename)
    >>> fd.seek(2880*2)
    >>> r=fromfile(fd, formats='f8,i4,a5', shape=3, byteorder='big')
    >>> print r[0]
    (5.1000000000000005, 61, 'abcde')
    >>> r._shape
    (3,)
    """

    if isinstance(shape, types.IntType) or isinstance(shape, types.LongType):
        shape = (shape,)
    name = 0
    if isinstance(file, types.StringType):
        name = 1
        file = open(file, 'rb')
    try:
        size = os.fstat(file.fileno())[stat.ST_SIZE] - file.tell()
    except:
        size = os.path.getsize(file.name) - file.tell()

    dummy = array(None, formats=formats, shape=0, aligned=aligned)
    itemsize = dummy._itemsize

    if shape and itemsize:
        shapesize = _gen.product(shape)*itemsize
        if shapesize < 0:
            shape = list(shape)
            shape[ shape.index(-1) ] = size / -shapesize
            shape = tuple(shape)

    nbytes = _gen.product(shape)*itemsize

    if nbytes > size:
        raise ValueError(
                "Not enough bytes left in file for specified shape and type")

    # create the array
    _array = RecArray(None, formats=formats, shape=shape, names=names,
                byteorder=byteorder, aligned=aligned)
    nbytesread = file.readinto(_array._data)
    if nbytesread != nbytes:
        raise IOError("Didn't read as many bytes as expected")
    if name:
        file.close()
    return _array

def array(buffer=None, formats=None, shape=0, names=None, byteorder=sys.byteorder, aligned=0):
    """This function will creates a new instance of a RecArray.

    buffer      specifies the source of the array's initialization data.
                buffer can be: RecArray, list of records in text, list of
                numarray/chararray, None, string, buffer.

    formats     specifies the fromat definitions of the array's records.

    shape       specifies the array dimensions.

    names       specifies the field names.

    aligned     if true, fields are padded to be aligned on word boundaries (as in a C struct.)
                result is machine-dependent since alignment requirements differ.

    >>> r=array([(1,11,'a'),(2,22,'b'),(3,33,'c'),(4,44,'d'),(5,55,'ex'),(6,66,'f'),(7,77,'g')],formats='u1,f4,a1')
    >>> print r
    RecArray[ 
    (1, 11.0, 'a'),
    (2, 22.0, 'b'),
    (3, 33.0, 'c'),
    (4, 44.0, 'd'),
    (5, 55.0, 'e'),
    (6, 66.0, 'f'),
    (7, 77.0, 'g')
    ]
    >>> print r[1]
    (2, 22.0, 'b')
    >>> r.field(1)
    array([ 11.,  22.,  33.,  44.,  55.,  66.,  77.], type=Float32)
    >>> r.field('c1')
    array([1, 2, 3, 4, 5, 6, 7], type=UInt8)
    >>> print r[2].field(1)
    33.0
    >>> print r[1::2]
    RecArray[ 
    (2, 22.0, 'b'),
    (4, 44.0, 'd'),
    (6, 66.0, 'f')
    ]
    >>> print r[1::2][1]
    (4, 44.0, 'd')
    >>> r[1::2].field(0)
    array([2, 4, 6], type=UInt8)
    >>> x=r[1::2].copy()
    >>> print x
    RecArray[ 
    (2, 22.0, 'b'),
    (4, 44.0, 'd'),
    (6, 66.0, 'f')
    ]
    >>> print x[1]
    (4, 44.0, 'd')
    >>> x.field(0)
    array([2, 4, 6], type=UInt8)
    >>> x[1].setfield(0,11)
    >>> print x
    RecArray[ 
    (2, 22.0, 'b'),
    (11, 44.0, 'd'),
    (6, 66.0, 'f')
    ]
    >>> r[1::2][1].setfield(0,17)
    >>> print r
    RecArray[ 
    (1, 11.0, 'a'),
    (2, 22.0, 'b'),
    (3, 33.0, 'c'),
    (17, 44.0, 'd'),
    (5, 55.0, 'e'),
    (6, 66.0, 'f'),
    (7, 77.0, 'g')
    ]
    >>> r[4:6]=r[:2]
    >>> print r
    RecArray[ 
    (1, 11.0, 'a'),
    (2, 22.0, 'b'),
    (3, 33.0, 'c'),
    (17, 44.0, 'd'),
    (1, 11.0, 'a'),
    (2, 22.0, 'b'),
    (7, 77.0, 'g')
    ]
    >>> r[1::2][2]=r[6]
    >>> print r
    RecArray[ 
    (1, 11.0, 'a'),
    (2, 22.0, 'b'),
    (3, 33.0, 'c'),
    (17, 44.0, 'd'),
    (1, 11.0, 'a'),
    (7, 77.0, 'g'),
    (7, 77.0, 'g')
    ]


    >>> r=array('aaaabbbbccccddddeeeeffffgggg'*200, formats='2i2,i4, (2,3) u2, (1,)f4, c8',shape=5)
    
    print r[1::2]  # This test is "commented out" due to FP formatting on Windows.
    RecArray[ 
    (array([25186, 25186], type=Int16), 1667457891, array([[25700, 25700, 25957],
           [25957, 26214, 26214]], type=UInt16), array([  1.09277409e+24], type=Float32), (2.5984589414244182e+20+1.0440133473554483e+21j)),
    (array([25700, 25700], type=Int16), 1701143909, array([[26214, 26214, 26471],
           [26471, 24929, 24929]], type=UInt16), array([  1.04401335e+21], type=Float32), (4.1945724725645174e+21+1.6852366222828967e+22j))
    ]
    r[2:].field(1)  # This test is "commented out" *too* due to 64-bit type formatting issues
    array([1684300900, 1701143909, 1717986918])
    >>> r=array([[456,'dbe',1.2],[2,'de',1.3]],names='col1,col2,col3')
    >>> print r[0]
    (456, 'dbe', 1.2)
    >>> r=array('a'*200,'f4,3i4,a5,i2',3)
    >>> r._bytestride
    23
    >>> r._names
    ['c1', 'c2', 'c3', 'c4']
    >>> r._repeats
    [1, 3, 1, 1]
    >>> r._shape
    (3,)
    """
    if isinstance(buffer, (type(None), str, file)) and (formats is None):
       raise ValueError("Must define formats if buffer is None, a string, or a file pointer.")
    elif buffer is None or _gen.SuitableBuffer(buffer):
        return RecArray(buffer, formats=formats, shape=shape, names=names,
                    byteorder=byteorder, aligned=aligned)
    elif isinstance(buffer, types.StringType):
        return fromstring(buffer, formats=formats, shape=shape, names=names,
                    byteorder=byteorder, aligned=aligned)
    elif isinstance(buffer, types.ListType) or isinstance(buffer, types.TupleType):
        if isinstance(buffer[0], (num.NumArray, chararray.RawCharArray)):
            return fromarrays(buffer, formats=formats, shape=shape, names=names,
                        byteorder=byteorder, aligned=aligned)
        else:
            return fromrecords(buffer, formats=formats, shape=shape, names=names,
                        byteorder=byteorder, aligned=aligned)
    elif isinstance(buffer, RecArray):
        #XXX This should change the fields to match arguments if specified!
        return buffer.copy()
    elif isinstance(buffer, types.FileType):
        return fromfile(buffer, formats=formats, shape=shape, names=names,
                    byteorder=byteorder, aligned=aligned)
    else:
        raise ValueError("Unknown input type")

def _RecGetType(name):
    """Converts a type repr string into a type."""
    if name == "CharType":
        return CharType
    else:
        return _nt.getType(name)

def _split(input):
    """Split the input formats string into field formats without splitting 
       the tuple used to specify multi-dimensional arrays."""

    newlist = []
    hold = ''

    for element in input.split(','):
        if hold != '':
            item = hold + ',' + element
        else:
            item = element
        left = item.count('(')
        right = item.count(')')

        # if the parenthesis is not balanced, hold the string
        if left > right :
            hold = item  

        # when balanced, append to the output list and reset the hold
        elif left == right:
            newlist.append(item)
            hold = ''

        # too many close parenthesis is unacceptable
        else:
            raise SyntaxError, item

    # if there is string left over in hold
    if hold != '':
        raise SyntaxError, hold

    return newlist


class RecArray(_gen.NDArray):
    """Record Array Class"""

    def __init__(self, buffer, formats, shape=0, names=None, byteoffset=0,
                 bytestride=None, byteorder=sys.byteorder, aligned=0):

        # names and formats can be either a string with components separated
        # by commas or a list of string values, e.g. ['i4', 'f4'] and 'i4,f4'
        # are equivalent formats

        self._rec_aligned = aligned
        self._parseFormats(formats)
        self._fieldNames(names)

        itemsize = self._total_itemsize

        if shape != None:
            if type(shape) in [types.IntType, types.LongType]: 
                shape = (shape,)
            elif (type(shape) == types.TupleType and type(shape[0]) in \
                                 [types.IntType, types.LongType]):
                pass
            else: 
                raise NameError, "Illegal shape %s" % `shape`

        #XXX need to check shape*itemsize == len(buffer)?

        self._shape = shape
        _gen.NDArray.__init__(self, self._shape, itemsize, buffer=buffer,
                             byteoffset=byteoffset,
                             bytestride=bytestride,
                             aligned=aligned)
        self._flags |= _gen._UPDATEDICT
        self._byteorder = byteorder

        # Build the column arrays and put in a cache to speed up field access
        self._fields = {}

    def __get_array_typestr__(self):
        return '|V%d' % self._itemsize

    __array_typestr__ = property(__get_array_typestr__, None, "");

    def __add__(self, other):
        """ Add two RecArray objects in a row wise manner """

        # Preconditions
        if not isinstance(other, RecArray):
            raise TypeError, \
                  "unsupported operand types for +: '%s' and '%s'" % \
                  (self.__class__.__name__, other.__class__.__name__)
        
        if not self._formats == other._formats:
            raise TypeError, \
                  "Formats on RecArray operands are not the same"

        # Get a buffer which is the concatenation of the two buffers
        # The copy() ensures a suitable addition of with different strides
        newbuf = buffer(self.copy()._data) + buffer(other.copy()._data)

        # Compute the new shape
        newshape = len(self) + len(other)

        # Build a new recarray from this buffer
        # The names will be those of the first operand. Is that correct?
        return RecArray(newbuf, formats=self._formats,
                        shape=newshape, names=self._names)


    def _parseFormats(self, formats):
        """ Parse the field formats """

        if (type(formats) in [types.ListType, types.TupleType]):
            _fmt = formats[:]
        elif (type(formats) == types.StringType):
            _fmt = _split(formats)
        else:
            raise NameError, "illegal input formats %s" % `formats`

        self._nfields = len(_fmt)
        self._repeats = [1] * self._nfields
        self._itemsizes = [0] * self._nfields
        self._sizes = [0] * self._nfields
        self._stops = [0] * self._nfields

        # preserve the input for future reference
        self._formats = [''] * self._nfields

        sum = 0
        maxalign = 1
        for i in range(self._nfields):

            # parse the formats into repeats and formats
            try:
                (_repeat, _dtype) = format_re.match(_fmt[i].strip()).groups()
            except TypeError, AttributeError: 
                raise ValueError('format %s is not recognized' % _fmt[i])

            # CharType needs special treatment
            _dtype = _dtype.strip()
            if _dtype[0] == 'a':
                self._itemsizes[i] = int(_dtype[1:])
                _dtype = 'a'

            if _repeat == '': 
                _repeat = 1
            else: 
                _repeat = eval(_repeat)
            _fmt[i] = numfmt[_dtype]
            if _dtype != 'a':
                self._itemsizes[i] = _fmt[i].bytes
            self._repeats[i] = _repeat

            if (type(_repeat) in [types.ListType, types.TupleType]):
                self._sizes[i] = self._itemsizes[i] * reduce(lambda x,y: x*y, _repeat)
            else:
                self._sizes[i] = self._itemsizes[i] * _repeat

            sum += self._sizes[i]
            if self._rec_aligned:
                # round sum up to multiple of alignment factor
                align = _gen._alignment[_fmt[i].bytes]
                sum = ((sum + align - 1)/align) * align
                maxalign = max(maxalign, align)
            self._stops[i] = sum - 1

            # Unify the appearance of _format, independent of input formats
            self._formats[i] = `_repeat`+revfmt[_fmt[i]]
            if _dtype == 'a':
                self._formats[i] += `self._itemsizes[i]`

        self._fmt = _fmt
        # This pads record so next record is aligned if self._rec_align is true.
        # Otherwise next the record starts right after the end of the last one.
        self._total_itemsize = (self._stops[-1]/maxalign + 1) * maxalign

    def __getstate__(self):
        """returns pickled state dictionary for RecArray"""

        state = _gen.NDArray.__getstate__(self)
        state["_fmt"] = map(repr, self._fmt)
        return state
    
    def __setstate__(self, state):
        _gen.NDArray.__setstate__(self, state)
        self._fmt = map(_RecGetType, state["_fmt"])

    def _getitem(self, offset):
        row = (offset - self._byteoffset) / self._strides[0]
        return Record(self, row)

    def _setitem(self, offset, value):
        row = (offset - self._byteoffset) / self._strides[0]
        for i in range(self._nfields):
            self.field(self._names[i])[row] = value.field(self._names[i])

    def _copyFrom(self, other):
        """Copy one RecArray (or one Record) to another RecArray.
           Both arrays must have the same field names and presumably the 
same
           (or castable) data types.
        """
        if isinstance(other, (RecArray, Record)):
            for i in self._names:
                self.field(i)[:] = other.field(i)
        else:
            raise TypeError('Not copied from a Record Array or Record.')

    def view(self):
        v = _gen.NDArray.view(self)
        v._fields = {}
        return v

    def getbyteorder(self):
        print "record array: %s" % self._byteorder
        for i in range(self._nfields):
            if isinstance(self.field(i), num.NumArray):
                print "Field %s: %s" % (self._names[i], self.field(i)._byteorder)
            else:
                print "Field %s: (N/A)" % self._names[i]

    def _byteswap(self):
        """Byteswap the data in place.  Does *not* change _byteorder."""

        for indx in range(self._nfields):
            column = self.field(indx)

            # Only deal with array objects which column size is
            # greather than 1
            if isinstance(column, num.NumArray) and column._itemsize != 1:
                column._byteswap()

    def togglebyteorder(self):
        "reverses the state of the _byteorder attribute:  little <-> big."""

        self._byteorder = {"little":"big","big":"little"}[self._byteorder]

        for indx in range(self._nfields):
            column = self.field(indx)

            # Only deal with array objects which column size is
            # greather than 1
            if isinstance(column, num.NumArray) and column._itemsize != 1:
                column._byteorder = \
                    {"little":"big","big":"little"}[column._byteorder]

    def byteswap(self):
        """Byteswap data in place and change the _byteorder attribute."""

        self._byteswap()
        self.togglebyteorder()

    def _fieldNames(self, names=None):
        """convert input field names into a list and assign to the _names
        attribute """

        if (names):
            if (type(names) in [types.ListType, types.TupleType]):
                pass
            elif (type(names) == types.StringType):
                names = names.split(',')
            else:
                raise NameError, "illegal input names %s" % `names`

            self._names = map(lambda n:n.strip(), names)[:self._nfields]
        else: 
            self._names = []

        # if the names are not specified, they will be assigned as "c1, c2,..."
        # if not enough names are specified, they will be assigned as "c[n+1],
        # c[n+2],..." etc. where n is the number of specified names..."
        self._names += map(lambda i: 
            'c'+`i`, range(len(self._names)+1,self._nfields+1))

        # check for redundant names
        _dup = find_duplicate(self._names)
        if _dup:
            raise ValueError, "Duplicate field names: %s" % _dup
        

    def _get_fields(self):
        """Get a dictionary of fields as numeric arrays."""

        # Iterate over all the fields
        fields = {}
        for indx in range(self._nfields):

            # determine the offset within the record
            _start = self._stops[indx] - self._sizes[indx] + 1
            _shape = self._shape
            _type = self._fmt[indx]
            _buffer = self._data
            _offset = self._byteoffset + _start

            # don't use self._itemsize due to possible slicing
            _stride = self._strides[-1]

            _order = self._byteorder

            if isinstance(_type, Char):
                arr = chararray.CharArray(buffer=_buffer, shape=_shape,
                          itemsize=self._itemsizes[indx], byteoffset=_offset,
                          bytestride=_stride)
            else:
                arr = num.NumArray(shape=_shape, type=_type,
                              buffer=_buffer, byteoffset=_offset, 
                              bytestride=_stride, byteorder = _order)

            # modify the _shape and _strides for array elements
            if (self._repeats[indx] != 1):
                if type(self._repeats[indx]) == types.TupleType:
                    arr._shape = self._shape + self._repeats[indx]
                else:
                    arr._shape = self._shape + (self._repeats[indx],)
                nd_stride = num.array(arr._shape[2:]+(1,))[::-1]
                nd_stride = num.cumproduct(nd_stride) * self._itemsizes[indx]
                nd_stride = nd_stride[::-1]
                arr._strides = (self._strides[0],) + tuple(nd_stride)

            # Put this array as a value in dictionary
            # Do both name and index
            fields[indx] = arr
            fields[self._names[indx]] = arr 

        return fields

    def field(self, fieldName):
        """ get the field data as a numeric array """


        # Check if stride or offset has changed from last call.
        # If so, refresh the cache.
        if self._fields == {}:
            self._fields = self._get_fields()  # Refresh the cache
        return self._fields[fieldName]
    
    def sinfo(self):
        """sinfo() returns a string of key attributes of a record array."""
        s = _gen.NDArray.sinfo(self)
        s += "field names: " + repr(self._names) + "\n"
        s += "field formats: " + repr(self._formats) + "\n"
        return s
        
    def __repr__(self):
        outlist = []
        for i in self:
            outlist.append(Record.__str__(i))
        return "array( \n[" + ",\n".join(outlist) + "],\n" + \
               "formats=" + str(self._formats) + ",\n" + \
               "shape=" + str(self.shape[0]) + ",\n" + \
               "names=" + str(self._names) + ")"

    def __str__(self):

        # Byteswap temporarily the byte order for presentation (if needed)
        outlist = []
        for i in self:
            outlist.append(Record.__str__(i))

        # When finished, restore the byte order (if needed)
        return "RecArray[ \n" + ",\n".join(outlist) + "\n]"


    def reshape(*value):
        print "Cannot reshape record array."



class Record:
    """Class for one single row."""

    def __init__(self, input, row=0):
        if isinstance(input, types.ListType) or isinstance(input, types.TupleType):
            input = fromrecords([input])
        if isinstance(input, RecArray):
            self.array = input
            self.row = row


    def field(self, fieldName):
        """Get the field data of the record."""

        return self.array.field(fieldName)[self.row]

    def setfield(self, fieldName, value):
        """Set the field data of the record."""

        self.array.field(fieldName)[self.row] = value

    def __str__(self):
        """Print one row."""

        outlist = []
        for i in range(self.array._nfields):
            outlist.append(`self.array.field(i)[self.row]`)
        return "(" + ", ".join(outlist) + ")"


def find_duplicate(list):
    """Find duplication in a list, return a list of dupicated elements"""
    dup = []
    for i in range(len(list)):
        if (list[i] in list[i+1:]):
            if (list[i] not in dup):
                dup.append(list[i])
    return dup

def test():
    import doctest, records
    return doctest.testmod(records)

if __name__ == "__main__":
    test()
