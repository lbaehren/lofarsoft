"""
Test module for NumArray

>>> import numinclude
>>> import generic
>>> import ufunc as uf

>>> Error.setMode()  # Ensure default error mode

normalize_results() does standard return value handling for
a given set of ufunc actual parameters and results.  Ufunc inputs
are assumed to be arrays or scalars, ufunc outputs can be arrays
or unspecified, and ufunc "preliminary results" must be arrays.

>>> from numarray.ufunc import _normalize_results

The standard processing assumptions and rules are summarized as
follows:

1. All ufunc inputs must be specified with *some* value.

2. If any outputs were specified, all outputs must be specified.
If no outputs were specified, outputs is None or ().  Specified
outputs are always arrays.

3. If any outputs were specified, the return value is supressed
(is None).

>>> _normalize_results( (array(1),), (zeros(1),), (array(3),))

4. If no outputs were specified, at least one array value will be
returned.  All results are assumed to have the same shape.

a. If len(results)==1, the tuple is discarded and the return value
is a single array or scalar.

>>> _normalize_results( (array(1), array(2)), (), (array(3),), False)
array(3)

b. If len(results) > 1,  the return value is a tuple.

>>> _normalize_results( (array(1), array(2)), (), (array(3),array(4)), False)
(array(3), array(4))

5. If the result array(s) are rank-0, either a scalar, rank-0, or
rank-1 array is returned.

a. If at least 1 input is an array, the result will be an array.

>>> _normalize_results( (array(1), 2), (), (array(3),), False)
array(3)

b. If all inputs are scalars, the result(s) is(are) a scalar.

>>> _normalize_results( (1, 2), (), (array(3),), False)
3

>>> _normalize_results( (1, 2), (), (array(3), array(4)), False)
(3, 4)

c. If rank1 is True, rank-0 results are converted to rank-1-len-1.

>>> _normalize_results( (array(1), 2), (), (array(3),), True)
array([3])

>>> _normalize_results( (array(1), 2), (), (array(3),array(4)), True)
(array([3]), array([4]))

6. Arrays of dimension > 0 have no special shape handling.

>>> _normalize_results( ([1], [2]), (), (array([3]), array([4])))
(array([3]), array([4]))

Tests for the array() factory function

>>> repr(array(None))        # compatible with Numeric-23.6
'None'

>>> x = arange(10)
>>> x
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
>>> x = arange(5,35)
>>> x
array([ 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
       21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34])
>>> x = arange(5,100,3)
>>> x
array([ 5,  8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47, 50,
       53, 56, 59, 62, 65, 68, 71, 74, 77, 80, 83, 86, 89, 92, 95, 98])
>>> zeros((10,))
array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
>>> ones((20,))
array([1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
>>> zeros(32600, Int16)[32499:32501]
array([0, 0], type=Int16)
>>> array([1,7,9,5,1])
array([1, 7, 9, 5, 1])
>>> x = arange(20)
>>> x[3]
3
>>> x[22]
Traceback (innermost last):
...
IndexError: Index out of range
>>> x[9] = 1000
>>> x
array([   0,    1,    2,    3,    4,    5,    6,    7,    8, 1000,
         10,   11,   12,   13,   14,   15,   16,   17,   18,   19])
>>> x[10] = 11.9
>>> x
array([   0,    1,    2,    3,    4,    5,    6,    7,    8, 1000,
         11,   11,   12,   13,   14,   15,   16,   17,   18,   19])
>>> x[10] = -7.9
>>> x
array([   0,    1,    2,    3,    4,    5,    6,    7,    8, 1000,
         -7,   11,   12,   13,   14,   15,   16,   17,   18,   19])
>>> x = array([1, 0, 2])
>>> y = array([3.5, 2,1])
>>> y
array([ 3.5,  2. ,  1. ])
>>> x+y
array([ 4.5,  2. ,  3. ])
>>> y+y
array([ 7.,  4.,  2.])
>>> y.astype(Int16)
array([3, 2, 1], type=Int16)
>>> x.astype(Float32)
array([ 1.,  0.,  2.], type=Float32)
>>> x = arange(10, type=Bool)
>>> x
array([0, 1, 1, 1, 1, 1, 1, 1, 1, 1], type=Bool)
>>> x[3]
1
>>> x[3] = 111
>>> x
array([0, 1, 1, 1, 1, 1, 1, 1, 1, 1], type=Bool)
>>> x[3] = 0.0
>>> x
array([0, 1, 1, 0, 1, 1, 1, 1, 1, 1], type=Bool)
>>> x[:] = 0
>>> x
array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0], type=Bool)
>>> x[:] = 0.1
>>> y = x.astype(Float64)
>>> y
array([ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.])
>>> y[1] = 1.111111111111
>>> y[1] # platform dependent?
1.1111111111109999
>>> y = y.astype(Float32)
>>> y[1] # platform dependent?
1.1111111640930176
>>> x = arange(300, type=Int8)
>>> x[::10]
array([   0,   10,   20,   30,   40,   50,   60,   70,   80,   90,
        100,  110,  120, -126, -116, -106,  -96,  -86,  -76,  -66,
        -56,  -46,  -36,  -26,  -16,   -6,    4,   14,   24,   34], type=Int8)
>>> x[0] = -100
>>> x[0]
-100
>>> x[0] = -3.7
>>> x[0]
-3
>>> x[0] = 300
>>> x[0]
44
>>> x[0] = 200
>>> x[0]
-56
>>> x = arange(300, type=UInt8)
>>> x[::10]
array([  0,  10,  20,  30,  40,  50,  60,  70,  80,  90, 100, 110, 120,
       130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250,
         4,  14,  24,  34], type=UInt8)
         
>>> x[0] = -100
>>> x[0]
156

Conversion of negative FP value to an unsigned integer type is undefined
Harbison & Steele 5e, p191

# >>> x[0] = -3.7
# >>> x[0]
# 253

>>> x[0] = 300
>>> x[0]
44
>>> x = arange(10, type=Int16)
>>> x
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], type=Int16)
>>> x = arange(10, type=UInt16)
>>> x
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], type=UInt16)

>>> x[0] = -2
>>> x[0]
65534

>>> x = array([1,256,256*256,256**3], type=Int32); x.byteswap(); explicit_type(x)
array([16777216,    65536,      256,        1], type=Int32)
>>> y = ones((4,), type=Int32)
>>> explicit_type(x + y)
array([16777217,    65537,      257,        2], type=Int32)
>>> explicit_type(x + x)
array([33554432,   131072,      512,        2], type=Int32)
>>> z = array([10., 0.5, 0., 0.])
>>> z + x
array([  1.67772260e+07,   6.55365000e+04,   2.56000000e+02,
         1.00000000e+00])
>>> x + z
array([  1.67772260e+07,   6.55365000e+04,   2.56000000e+02,
         1.00000000e+00])
>>> a = arange(10, type=Int32)    # xxx replace when strides supported
>>> a = a[::2]; explicit_type(a)
array([0, 2, 4, 6, 8], type=Int32)
>>> explicit_type(a + a)
array([ 0,  4,  8, 12, 16], type=Int32)
>>> b = arange(5, type=Int32)
>>> explicit_type(a + b)
array([ 0,  3,  6,  9, 12], type=Int32)
>>> explicit_type(b + a)
array([ 0,  3,  6,  9, 12], type=Int32)
>>> a.togglebyteorder()
>>> explicit_type(a + b)
array([        0,  33554433,  67108866, 100663299, 134217732], type=Int32)
>>> c = arange(15, type=Float32)
>>> c._strides = (12,)
>>> c._shape = (5,)
>>> a.byteswap()
>>> a + c
array([  0.,   5.,  10.,  15.,  20.], type=Float32)
>>> cc = c.copy()
>>> cc[0] = 25.5
>>> cc
array([ 25.5,   3. ,   6. ,   9. ,  12. ], type=Float32)
>>> c._copyFrom(a); c
array([ 0.,  2.,  4.,  6.,  8.], type=Float32)
>>> # test of non-aligned data (different for big endian, little endian machines)
>>> if isBigEndian:
...     d = array([0, 2**24, 2**16, 2**8, 1], type=Int32)
... else:
...     d = array([2**8, 2**16, 2**24, 0, 1], type=Int32)
>>> d._shape = (4,)
>>> d._byteoffset = 1    # misalign it
>>> d._bytestride = 5
>>> d._strides = (5,)
>>> e = zeros((4,), type=Int32)
>>> explicit_type(d + e)
array([1, 1, 1, 1], type=Int32)
>>> dc = d.copy()
>>> explicit_type(dc)
array([1, 1, 1, 1], type=Int32)
>>> int(dc.isaligned())
1
>>> int(dc.iscontiguous())
1
>>> f = arange(8, type=Int32)
>>> f._strides = (8,)
>>> f._shape = (4,)
>>> explicit_type(d + f)
array([1, 3, 5, 7], type=Int32)
>>> g = array([2**24,2**24,2**24,2**24], type=Int32)
>>> g.togglebyteorder()
>>> explicit_type(d + g)
array([2, 2, 2, 2], type=Int32)
>>> h = arange(10000, type=Int32) # xxx test of blocking
>>> h._shape=(5000,)
>>> h._strides=(8,)
>>> t = h + h
>>> t[4000]
16000
>>> p = arange(5000, type=Float32)
>>> t = h + p
>>> t[4000]
12000.0
>>> h._shape=(4577,)
>>> t = h + h
>>> t[4500]
18000
>>> x = _misaligned(arange(10000))
>>> x.isaligned()
0
>>> x[:] = arange(10000)+10
>>> x[2495:2505]
array([2505, 2506, 2507, 2508, 2509, 2510, 2511, 2512, 2513, 2514])
>>> r = arange(64) # test n-dimensional looping of computation
>>> r.setshape((4,4,4))
>>> t = r + r # test fast computation of multidimensional numarray
>>> t.setshape((64,))
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> r.swapaxes(0, 1); r.iscontiguous()  # swapaxes for noncontiguous
0
>>> t = r + r
>>> r.swapaxes(0,1)         # swap back
>>> t.swapaxes(0,1); t.flat
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> oldblocksize = uf._bufferPool.setBufferSize(64*4) # same size as array
>>> t = r + r
>>> t.setshape((64,))
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> o = uf._bufferPool.setBufferSize(32*4) # two blocks, no leftover
>>> t = r + r
>>> t.setshape((64,))
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> o = uf._bufferPool.setBufferSize(48*4) # one block with leftover
>>> t = r + r
>>> t.setshape((64,))
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> o = uf._bufferPool.setBufferSize(48*4-8) # should force smaller block
>>> t = r + r
>>> t.setshape((64,))
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])
>>> o = uf._bufferPool.setBufferSize(3*4) # blocked in last dimension
>>> t = r + r
>>> t.setshape((64,))
>>> o = uf._bufferPool.setBufferSize(oldblocksize)
>>> t
array([  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,
        26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,
        52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102,
       104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126])

# Test Nary ufuncs with a 3 input 1 output function, _distance3d

>>> uf._distance3d(1,2,3)
3.7416574954986572
>>> oldblocksize = uf._bufferPool.setBufferSize(4*4)
>>> uf._distance3d(arange(10,type='i4'), arange(10,type='i4'), arange(10,type='i4'))
array([  0.        ,   1.73205081,   3.46410162,   5.19615242,
         6.92820323,   8.66025404,  10.39230485,  12.12435565,
        13.85640646,  15.58845727])
>>> uf._distance3d(arange(10.), arange(10.), arange(10.))
array([  0.        ,   1.73205081,   3.46410162,   5.19615242,
         6.92820323,   8.66025404,  10.39230485,  12.12435565,
        13.85640646,  15.58845727])
>>> out = arange(10.)
>>> uf._distance3d(arange(10.), arange(10.), arange(10.), out)
>>> out
array([  0.        ,   1.73205081,   3.46410162,   5.19615242,
         6.92820323,   8.66025404,  10.39230485,  12.12435565,
        13.85640646,  15.58845727])
>>> out = arange(10)
>>> uf._distance3d(arange(10.), arange(10.), arange(10.), out)
>>> out
array([ 0,  1,  3,  5,  6,  8, 10, 12, 13, 15])
>>> uf._distance3d(arange(10.), arange(10.), arange(10.), out, out)
Traceback (most recent call last):
...
ValueError: Ufunc _distance3d takes 3 inputs and 1 outputs but 5 parameters given.
>>> uf._distance3d(arange(10.), out)
Traceback (most recent call last):
...
ValueError: Ufunc _distance3d takes 3 inputs and 1 outputs but 2 parameters given.
>>> o = uf._bufferPool.setBufferSize(oldblocksize)

>>> a = arange(10.)
>>> b = arange(11.)
>>> uf._distance3d(a, 1.0, 3.0)  # vss match
array([ 3.16227766,  3.31662479,  3.74165739,  4.35889894,  5.09901951,
        5.91607978,  6.78232998,  7.68114575,  8.60232527,  9.53939201])
>>> uf._distance3d(a, a*2, a*4)  # vvv match
array([  0.        ,   4.58257569,   9.16515139,  13.74772708,
        18.33030278,  22.91287847,  27.49545417,  32.07802986,
        36.66060556,  41.24318125])
>>> uf._distance3d(a, a+1j, a*4) # unsupported types
Traceback (most recent call last):
...
TypeError: Couldn't find a function which matches the inputs
>>> uf._distance3d(a, b, 3.0)    # mismatched shapes
Traceback (most recent call last):
...
ValueError: Arrays have incompatible shapes


>>> x = arange(4, type=Int32) # test of broadcasting through 0 strides mechanism
>>> x._shape = (4,3)
>>> x._strides = (4,0)
>>> y = arange(3, type=Int32)
>>> y._shape = (4,3)
>>> y._strides = (0,4)
>>> z = x + y # both x and y are being broadcast into the other's dimension!
>>> z._shape = (12,)
>>> z._strides = (4,)
>>> explicit_type(z)
array([0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5], type=Int32)
>>> zeros(10)
array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
>>> zeros(10, type=Float64)
array([ 0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.])
>>> ones(10)
array([1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
>>> ones(10, type=Float64)
array([ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.])
>>> x = ones((10,10))
>>> x._shape
(10, 10)
>>> arange(10)
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
>>> arange(10.)
array([ 0.,  1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.])
>>> arange(10, type=Float64)
array([ 0.,  1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.])
>>> arange(2,12)
array([ 2,  3,  4,  5,  6,  7,  8,  9, 10, 11])
>>> arange(2,12,2)
array([ 2,  4,  6,  8, 10])
>>> arange(2,12,0.5)
array([  2. ,   2.5,   3. ,   3.5,   4. ,   4.5,   5. ,   5.5,   6. ,
         6.5,   7. ,   7.5,   8. ,   8.5,   9. ,   9.5,  10. ,  10.5,
        11. ,  11.5])
>>> x = arange(100000)
>>> x[99999]
99999
>>> x = array([[1, 2, 3, 4], [0, 0, 0, 0], [9, 8, 7, 6], [-1, -2, -3, -4]])
>>> print x
[[ 1  2  3  4]
 [ 0  0  0  0]
 [ 9  8  7  6]
 [-1 -2 -3 -4]]
>>> print x[2]
[9 8 7 6]
>>> int(x[2].iscontiguous())
1
>>> print x[2][:2]
[9 8]
>>> y = reshape(x, (-1,))
>>> y
array([ 1,  2,  3,  4,  0,  0,  0,  0,  9,  8,  7,  6, -1, -2, -3, -4])
>>> print y[::2]
[ 1  3  0  0  9  7 -1 -3]
>>> print y[...]
[ 1  2  3  4  0  0  0  0  9  8  7  6 -1 -2 -3 -4]
>>> int(y[::2].iscontiguous())
0
>>> print y[3:9:3]
[4 0]
>>> print y[10:2:-2]
[7 9 0 0]
>>> print y[-7:]
[ 8  7  6 -1 -2 -3 -4]
>>> print y[:-1:20]
[1]
>>> print y[-1:-10:-1]
[-4 -3 -2 -1  6  7  8  9  0]
>>> x = arange(128)
>>> x.setshape((2,2,2,2,2,4))
>>> # doctest doesn't like blank lines so prefix with periods
>>> dtp(x)
. array([[[[[[  0,   1,   2,   3],
.            [  4,   5,   6,   7]],
.
.           [[  8,   9,  10,  11],
.            [ 12,  13,  14,  15]]],
.
.
.          [[[ 16,  17,  18,  19],
.            [ 20,  21,  22,  23]],
.
.           [[ 24,  25,  26,  27],
.            [ 28,  29,  30,  31]]]],
.
.
.
.         [[[[ 32,  33,  34,  35],
.            [ 36,  37,  38,  39]],
.
.           [[ 40,  41,  42,  43],
.            [ 44,  45,  46,  47]]],
.
.
.          [[[ 48,  49,  50,  51],
.            [ 52,  53,  54,  55]],
.
.           [[ 56,  57,  58,  59],
.            [ 60,  61,  62,  63]]]]],
.
.
.
.
.        [[[[[ 64,  65,  66,  67],
.            [ 68,  69,  70,  71]],
.
.           [[ 72,  73,  74,  75],
.            [ 76,  77,  78,  79]]],
.
.
.          [[[ 80,  81,  82,  83],
.            [ 84,  85,  86,  87]],
.
.           [[ 88,  89,  90,  91],
.            [ 92,  93,  94,  95]]]],
.
.
.
.         [[[[ 96,  97,  98,  99],
.            [100, 101, 102, 103]],
.
.           [[104, 105, 106, 107],
.            [108, 109, 110, 111]]],
.
.
.          [[[112, 113, 114, 115],
.            [116, 117, 118, 119]],
.
.           [[120, 121, 122, 123],
.            [124, 125, 126, 127]]]]]])

>>> t = x[1,...,0,...,2]
>>> dtp(t)
. array([[[ 66,  70],
.         [ 82,  86]],
.
.        [[ 98, 102],
.         [114, 118]]])

>>> t = x[1,...,0,2,NewAxis]
>>> dtp(t)
. array([[[[ 66],
.          [ 74]],
.
.         [[ 82],
.          [ 90]]],
.
.
.        [[[ 98],
.          [106]],
.
.         [[114],
.          [122]]]])

>>> t = x[None,:,1,...,0,1:,None]
>>> dtp(t)
. array([[[[[[ 33],
.            [ 34],
.            [ 35]],
.
.           [[ 41],
.            [ 42],
.            [ 43]]],
.
.
.          [[[ 49],
.            [ 50],
.            [ 51]],
.
.           [[ 57],
.            [ 58],
.            [ 59]]]],
.
.
.
.         [[[[ 97],
.            [ 98],
.            [ 99]],
.
.           [[105],
.            [106],
.            [107]]],
.
.
.          [[[113],
.            [114],
.            [115]],
.
.           [[121],
.            [122],
.            [123]]]]]])

>>> s = zeros((5,5,5,5))
>>> print s._shape
(5, 5, 5, 5)
>>> print s[2,...,2]._shape
(5, 5)
>>> print s[2:4,...,2:5]._shape
(2, 5, 5, 3)
>>> s = zeros((5,5))
>>> print s[2:4,...,2:5]._shape
(2, 3)
>>> print s[NewAxis,2:4,...,2:5]._shape
(1, 2, 3)
>>> print s[2:4,...,NewAxis,2:5]._shape
(2, 1, 3)
>>> print s[2:4,...,2:5,NewAxis]._shape
(2, 3, 1)

Check slicing with strides

>>> a = arange(10)
>>> a[::]
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
>>> a[::-1]
array([9, 8, 7, 6, 5, 4, 3, 2, 1, 0])
>>> a[5:3:-1]
array([5, 4])
>>> a[5:3:2]
array([], type=Long)
>>> a[3:5:-1]
array([], type=Long)
>>> a[3:3:-1]
array([], type=Long)
>>> a[5::-1]
array([5, 4, 3, 2, 1, 0])
>>> a[5:0:-1]
array([5, 4, 3, 2, 1])
>>> a[5:0:-2]
array([5, 3, 1])
>>> a[25:0:-2]
array([9, 7, 5, 3, 1])
>>> a[25::-2]
array([9, 7, 5, 3, 1])
>>> a[25::-20]
array([9])
>>> a[25::2]
array([], type=Long)
>>> a[1::2]
array([1, 3, 5, 7, 9])
>>> a[:5:2]
array([0, 2, 4])
>>> a[:15:2]
array([0, 2, 4, 6, 8])
>>> a[:5:-2]
array([9, 7])
>>> a[::0]
Traceback (most recent call last):
...
IndexError: slice step of zero not allowed

Check empty slices in 2D array

>>> a = arange(10)
>>> a.setshape(2,5)
>>> a[:,3:3]
array([], shape=(2, 0), type=Long)
>>> print a[:,3:3]._shape
(2, 0)
>>> print a[:,3:3][1]._shape
(0,)
>>> print a[:,3:3][2]._shape
Traceback (most recent call last):
...
IndexError: Index out of range
>>> print a[:,3:3][:,0]._shape
Traceback (most recent call last):
...
IndexError: Index out of range

Check setitem slicing

>>> x = arange(10)
>>> x[2:4] = array([10, 11])
>>> x
array([ 0,  1, 10, 11,  4,  5,  6,  7,  8,  9])
>>> x[2:4] = array([10, 11, 12])
Traceback (innermost last):
...
ValueError: Arrays have incompatible shapes
>>> x[:] = -1
>>> x
array([-1, -1, -1, -1, -1, -1, -1, -1, -1, -1])
>>> x[::-1] = arange(10)
>>> x
array([9, 8, 7, 6, 5, 4, 3, 2, 1, 0])
>>> x[1::2] = arange(100,110,2)
>>> x
array([  9, 100,   7, 102,   5, 104,   3, 106,   1, 108])
>>> x = arange(36)
>>> x.setshape((6,6))
>>> x[2] = 99
>>> x
array([[ 0,  1,  2,  3,  4,  5],
       [ 6,  7,  8,  9, 10, 11],
       [99, 99, 99, 99, 99, 99],
       [18, 19, 20, 21, 22, 23],
       [24, 25, 26, 27, 28, 29],
       [30, 31, 32, 33, 34, 35]])
>>> x[-1:1:-2] = ones((6,))
>>> x
array([[ 0,  1,  2,  3,  4,  5],
       [ 6,  7,  8,  9, 10, 11],
       [99, 99, 99, 99, 99, 99],
       [ 1,  1,  1,  1,  1,  1],
       [24, 25, 26, 27, 28, 29],
       [ 1,  1,  1,  1,  1,  1]])
>>> x[2:5,2:4] = -11
>>> x
array([[  0,   1,   2,   3,   4,   5],
       [  6,   7,   8,   9,  10,  11],
       [ 99,  99, -11, -11,  99,  99],
       [  1,   1, -11, -11,   1,   1],
       [ 24,  25, -11, -11,  28,  29],
       [  1,   1,   1,   1,   1,   1]])
>>> x[2:5,2:4] = array([[33, 34],[41, 42],[59, 58]])
>>> x
array([[ 0,  1,  2,  3,  4,  5],
       [ 6,  7,  8,  9, 10, 11],
       [99, 99, 33, 34, 99, 99],
       [ 1,  1, 41, 42,  1,  1],
       [24, 25, 59, 58, 28, 29],
       [ 1,  1,  1,  1,  1,  1]])

Check backwards overlapping slice assign

>>> a = arange(1,6)
>>> a[-1:0:-1] = a[-2::-1]
>>> a
array([1, 1, 2, 3, 4])

Check ufunc argument handling

>>> add([3,4,5], [1,5,9])
array([ 4,  9, 14])
>>> add([3,4,5], 10)
array([13, 14, 15])
>>> add([3,4,5], 10.5)
array([ 13.5,  14.5,  15.5])
>>> add(2,2)
4
>>> add(10, [3,4,5.])
array([ 13.,  14.,  15.])
>>> print '%.6f' % cos(1)  # Sometimes Int32, sometimes Int64
0.540302

Check ufunc broadcasting

>>> x = arange(4)
>>> y = arange(0, 6, 2)
>>> x+y
Traceback (innermost last):
...
ValueError: Arrays have incompatible shapes
>>> x[:,NewAxis]+y
array([[0, 2, 4],
       [1, 3, 5],
       [2, 4, 6],
       [3, 5, 7]])
>>> x = arange(18)
>>> x.setshape((3,3,2))
>>> y = arange(100,106)
>>> y.setshape((3,2))
>>> dtp(x+y)
. array([[[100, 102],
.         [104, 106],
.         [108, 110]],
.
.        [[106, 108],
.         [110, 112],
.         [114, 116]],
.
.        [[112, 114],
.         [116, 118],
.         [120, 122]]])

>>> y.setshape((3,1,2))
>>> dtp(x+y)
. array([[[100, 102],
.         [102, 104],
.         [104, 106]],
.
.        [[108, 110],
.         [110, 112],
.         [112, 114]],
.
.        [[116, 118],
.         [118, 120],
.         [120, 122]]])

>>> y.setshape((3,2,1))
>>> x+y
Traceback (innermost last):
...
ValueError: Arrays have incompatible shapes

Check use of output numarray in ufuncs

>>> x = arange(10)
>>> y = ones((10,))
>>> add(x,y,x)
>>> x
array([ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10])
>>> z = arange(20, type=Float64)
>>> add(x,y,z[::2])
>>> z
array([  2.,   1.,   3.,   3.,   4.,   5.,   5.,   7.,   6.,   9.,
         7.,  11.,   8.,  13.,   9.,  15.,  10.,  17.,  11.,  19.])
>>> x = arange(10)
>>> cos(x,x)
>>> x
array([1, 0, 0, 0, 0, 0, 0, 0, 0, 0])
>>> y = arange(10).astype(Float32)
>>> cos(y, y)
>>> comparr(y,
...    array([ 1.        ,  0.54030228, -0.41614684, -0.9899925 , -0.65364361,
...            0.2836622 ,  0.96017027,  0.75390226, -0.14550003, -0.91113025], type=Float32),
...    10.**-7)

# previous answer before changing arrayprint to use numeric functions
array([  1.00000000e+00,   5.40302277e-01,  -4.16146845e-01,
        -9.89992499e-01,  -6.53643608e-01,   2.83662200e-01,
         9.60170269e-01,   7.53902256e-01,  -1.45500034e-01,
        -9.11130250e-01], type=Float32)

Check astype from strides

>>> x = arange(20)
>>> x[::2].astype(Float64)
array([  0.,   2.,   4.,   6.,   8.,  10.,  12.,  14.,  16.,  18.])

Check reduce method

>>> add.reduce(arange(10.))
45.0
>>> x = arange(10000)
>>> add.reduce(x)
49995000
>>> x.setshape((100,100))
>>> add.reduce(x)
array([495000, 495100, 495200, 495300, 495400, 495500, 495600, 495700,
       495800, 495900, 496000, 496100, 496200, 496300, 496400, 496500,
       496600, 496700, 496800, 496900, 497000, 497100, 497200, 497300,
       497400, 497500, 497600, 497700, 497800, 497900, 498000, 498100,
       498200, 498300, 498400, 498500, 498600, 498700, 498800, 498900,
       499000, 499100, 499200, 499300, 499400, 499500, 499600, 499700,
       499800, 499900, 500000, 500100, 500200, 500300, 500400, 500500,
       500600, 500700, 500800, 500900, 501000, 501100, 501200, 501300,
       501400, 501500, 501600, 501700, 501800, 501900, 502000, 502100,
       502200, 502300, 502400, 502500, 502600, 502700, 502800, 502900,
       503000, 503100, 503200, 503300, 503400, 503500, 503600, 503700,
       503800, 503900, 504000, 504100, 504200, 504300, 504400, 504500,
       504600, 504700, 504800, 504900])
>>> add.reduce(x,1)
array([  4950,  14950,  24950,  34950,  44950,  54950,  64950,  74950,
        84950,  94950, 104950, 114950, 124950, 134950, 144950, 154950,
       164950, 174950, 184950, 194950, 204950, 214950, 224950, 234950,
       244950, 254950, 264950, 274950, 284950, 294950, 304950, 314950,
       324950, 334950, 344950, 354950, 364950, 374950, 384950, 394950,
       404950, 414950, 424950, 434950, 444950, 454950, 464950, 474950,
       484950, 494950, 504950, 514950, 524950, 534950, 544950, 554950,
       564950, 574950, 584950, 594950, 604950, 614950, 624950, 634950,
       644950, 654950, 664950, 674950, 684950, 694950, 704950, 714950,
       724950, 734950, 744950, 754950, 764950, 774950, 784950, 794950,
       804950, 814950, 824950, 834950, 844950, 854950, 864950, 874950,
       884950, 894950, 904950, 914950, 924950, 934950, 944950, 954950,
       964950, 974950, 984950, 994950])
>>> x.togglebyteorder(); x.byteswap()  # byteswapped --> slow mode
>>> add.reduce(x)
array([495000, 495100, 495200, 495300, 495400, 495500, 495600, 495700,
       495800, 495900, 496000, 496100, 496200, 496300, 496400, 496500,
       496600, 496700, 496800, 496900, 497000, 497100, 497200, 497300,
       497400, 497500, 497600, 497700, 497800, 497900, 498000, 498100,
       498200, 498300, 498400, 498500, 498600, 498700, 498800, 498900,
       499000, 499100, 499200, 499300, 499400, 499500, 499600, 499700,
       499800, 499900, 500000, 500100, 500200, 500300, 500400, 500500,
       500600, 500700, 500800, 500900, 501000, 501100, 501200, 501300,
       501400, 501500, 501600, 501700, 501800, 501900, 502000, 502100,
       502200, 502300, 502400, 502500, 502600, 502700, 502800, 502900,
       503000, 503100, 503200, 503300, 503400, 503500, 503600, 503700,
       503800, 503900, 504000, 504100, 504200, 504300, 504400, 504500,
       504600, 504700, 504800, 504900])
>>> add.reduce(x,1)
array([  4950,  14950,  24950,  34950,  44950,  54950,  64950,  74950,
        84950,  94950, 104950, 114950, 124950, 134950, 144950, 154950,
       164950, 174950, 184950, 194950, 204950, 214950, 224950, 234950,
       244950, 254950, 264950, 274950, 284950, 294950, 304950, 314950,
       324950, 334950, 344950, 354950, 364950, 374950, 384950, 394950,
       404950, 414950, 424950, 434950, 444950, 454950, 464950, 474950,
       484950, 494950, 504950, 514950, 524950, 534950, 544950, 554950,
       564950, 574950, 584950, 594950, 604950, 614950, 624950, 634950,
       644950, 654950, 664950, 674950, 684950, 694950, 704950, 714950,
       724950, 734950, 744950, 754950, 764950, 774950, 784950, 794950,
       804950, 814950, 824950, 834950, 844950, 854950, 864950, 874950,
       884950, 894950, 904950, 914950, 924950, 934950, 944950, 954950,
       964950, 974950, 984950, 994950])
>>> w = zeros((100,))
>>> add.reduce(x,1,w)
>>> w
array([  4950,  14950,  24950,  34950,  44950,  54950,  64950,  74950,
        84950,  94950, 104950, 114950, 124950, 134950, 144950, 154950,
       164950, 174950, 184950, 194950, 204950, 214950, 224950, 234950,
       244950, 254950, 264950, 274950, 284950, 294950, 304950, 314950,
       324950, 334950, 344950, 354950, 364950, 374950, 384950, 394950,
       404950, 414950, 424950, 434950, 444950, 454950, 464950, 474950,
       484950, 494950, 504950, 514950, 524950, 534950, 544950, 554950,
       564950, 574950, 584950, 594950, 604950, 614950, 624950, 634950,
       644950, 654950, 664950, 674950, 684950, 694950, 704950, 714950,
       724950, 734950, 744950, 754950, 764950, 774950, 784950, 794950,
       804950, 814950, 824950, 834950, 844950, 854950, 864950, 874950,
       884950, 894950, 904950, 914950, 924950, 934950, 944950, 954950,
       964950, 974950, 984950, 994950])

# Matching output type, >=2D input
>>> w = array([-1,-1,-1,-1])
>>> x = arange(16)
>>> x.setshape((4,4))
>>> add.reduce(x,0,w)
>>> w
array([24, 28, 32, 36])

# Different output type, >=2D input
>>> w = array([-1,-1,-1,-1], type=Float32)
>>> x = arange(16)
>>> x.setshape((4,4))
>>> add.reduce(x,0,w)
>>> w
array([ 24.,  28.,  32.,  36.], type=Float32)

# Matching output type, 1D input
>>> w = zeros((1,))
>>> x = arange(16)
>>> add.reduce(x,0,w)
>>> w
array([120])

# Different output type, 1D input
>>> w = zeros((1,), type=Float32)
>>> x = arange(16)
>>> add.reduce(x,0,w)
>>> w
array([ 120.], type=Float32)

# Byteswapped reduce
>>> x = arange(16)
>>> x.byteswap()
>>> x.togglebyteorder()
>>> add.reduce(x, 0)
120
>>> add.areduce(x, 0)
array([120])

# Discontiguous reduce
>>> x = arange(16, shape=(4,4)); x.swapaxes(0, 1) # make discontiguous
>>> add.reduce(add.reduce(x, 0), 0)
120

# Mismatched output type
>>> x = arange(16)
>>> y = zeros((1,), type=Float64)
>>> add.reduce(x, 0, y)
>>> y
array([ 120.])

Check accumulate method

>>> add.accumulate(arange(10))
array([ 0,  1,  3,  6, 10, 15, 21, 28, 36, 45])
>>> x = arange(16)
>>> x.setshape((4,4))
>>> add.accumulate(x)
array([[ 0,  1,  2,  3],
       [ 4,  6,  8, 10],
       [12, 15, 18, 21],
       [24, 28, 32, 36]])
>>> add.accumulate(x,1)
array([[ 0,  1,  3,  6],
       [ 4,  9, 15, 22],
       [ 8, 17, 27, 38],
       [12, 25, 39, 54]])
>>> x = arange(27)
>>> x.setshape((3,3,3))
>>> dtp(add.accumulate(x))
. array([[[ 0,  1,  2],
.         [ 3,  4,  5],
.         [ 6,  7,  8]],
.
.        [[ 9, 11, 13],
.         [15, 17, 19],
.         [21, 23, 25]],
.
.        [[27, 30, 33],
.         [36, 39, 42],
.         [45, 48, 51]]])

>>> dtp(add.accumulate(x,1))
. array([[[ 0,  1,  2],
.         [ 3,  5,  7],
.         [ 9, 12, 15]],
.
.        [[ 9, 10, 11],
.         [21, 23, 25],
.         [36, 39, 42]],
.
.        [[18, 19, 20],
.         [39, 41, 43],
.         [63, 66, 69]]])

>>> dtp(add.accumulate(x,2))
. array([[[ 0,  1,  3],
.         [ 3,  7, 12],
.         [ 6, 13, 21]],
.
.        [[ 9, 19, 30],
.         [12, 25, 39],
.         [15, 31, 48]],
.
.        [[18, 37, 57],
.         [21, 43, 66],
.         [24, 49, 75]]])

>>> add.accumulate(x,3)
Traceback (innermost last):
...
ValueError: Specified dimension does not exist

>>> x = arange(10000) # test blocking version
>>> x.setshape((100,100))
>>> x.byteswap(); x.togglebyteorder() # byteswapped --> blocking
>>> z = add.accumulate(x)
>>> z[:,0]
array([     0,    100,    300,    600,   1000,   1500,   2100,   2800,
         3600,   4500,   5500,   6600,   7800,   9100,  10500,  12000,
        13600,  15300,  17100,  19000,  21000,  23100,  25300,  27600,
        30000,  32500,  35100,  37800,  40600,  43500,  46500,  49600,
        52800,  56100,  59500,  63000,  66600,  70300,  74100,  78000,
        82000,  86100,  90300,  94600,  99000, 103500, 108100, 112800,
       117600, 122500, 127500, 132600, 137800, 143100, 148500, 154000,
       159600, 165300, 171100, 177000, 183000, 189100, 195300, 201600,
       208000, 214500, 221100, 227800, 234600, 241500, 248500, 255600,
       262800, 270100, 277500, 285000, 292600, 300300, 308100, 316000,
       324000, 332100, 340300, 348600, 357000, 365500, 374100, 382800,
       391600, 400500, 409500, 418600, 427800, 437100, 446500, 456000,
       465600, 475300, 485100, 495000])
>>> z = add.accumulate(x,1)
>>> z[0]
array([   0,    1,    3,    6,   10,   15,   21,   28,   36,   45,
         55,   66,   78,   91,  105,  120,  136,  153,  171,  190,
        210,  231,  253,  276,  300,  325,  351,  378,  406,  435,
        465,  496,  528,  561,  595,  630,  666,  703,  741,  780,
        820,  861,  903,  946,  990, 1035, 1081, 1128, 1176, 1225,
       1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770,
       1830, 1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415,
       2485, 2556, 2628, 2701, 2775, 2850, 2926, 3003, 3081, 3160,
       3240, 3321, 3403, 3486, 3570, 3655, 3741, 3828, 3916, 4005,
       4095, 4186, 4278, 4371, 4465, 4560, 4656, 4753, 4851, 4950])
>>> z[:,0]
array([   0,  100,  200,  300,  400,  500,  600,  700,  800,  900,
       1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
       2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900,
       3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900,
       4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900,
       5000, 5100, 5200, 5300, 5400, 5500, 5600, 5700, 5800, 5900,
       6000, 6100, 6200, 6300, 6400, 6500, 6600, 6700, 6800, 6900,
       7000, 7100, 7200, 7300, 7400, 7500, 7600, 7700, 7800, 7900,
       8000, 8100, 8200, 8300, 8400, 8500, 8600, 8700, 8800, 8900,
       9000, 9100, 9200, 9300, 9400, 9500, 9600, 9700, 9800, 9900])
>>> x = arange(10000)
>>> z = add.accumulate(x)
>>> z[-10:]
array([49905045, 49915036, 49925028, 49935021, 49945015, 49955010,
       49965006, 49975003, 49985001, 49995000])
>>> w = zeros((4,4))
>>> x = arange(16)
>>> x.setshape((4,4))
>>> add.accumulate(x, 0, w)
>>> w
array([[ 0,  1,  2,  3],
       [ 4,  6,  8, 10],
       [12, 15, 18, 21],
       [24, 28, 32, 36]])
>>> w = ones((4,4))
>>> add.accumulate(x[::2,::2], 0, w[::2,::2])
>>> w
array([[ 0,  1,  2,  1],
       [ 1,  1,  1,  1],
       [ 8,  1, 12,  1],
       [ 1,  1,  1,  1]])
>>> add.accumulate(x[:3,:2],1,w[1::,::2])
>>> w
array([[ 0,  1,  2,  1],
       [ 0,  1,  1,  1],
       [ 4,  1,  9,  1],
       [ 8,  1, 17,  1]])

Check outer method

>>> add.outer([1,10],[20,50])
array([[21, 51],
       [30, 60]])
>>> w = zeros((4,4), type=Float64)
>>> add.outer([1,10],[20,50], w[::2,::2])
>>> w
array([[ 21.,   0.,  51.,   0.],
       [  0.,   0.,   0.,   0.],
       [ 30.,   0.,  60.,   0.],
       [  0.,   0.,   0.,   0.]])

Check to and from string

>>> x = arange(6, type=Int32)
>>> if isBigEndian:
...     x.byteswap()
>>> s = x.tostring()
>>> map(ord,s)
[0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0]
>>> b = fromstring(s, Int32)
>>> if isBigEndian:
...     b.byteswap()
>>> b.astype(Long)
array([0, 1, 2, 3, 4, 5])
>>> b = fromstring(s[0:18], Int32)
Traceback (innermost last):
...
ValueError: Type size inconsistent with string length
>>> b = fromstring(s, Int32, (4,2))
Traceback (innermost last):
...
ValueError: Specified shape and type inconsistent with string length
>>> b = fromstring(s, Int32, (3,2))
>>> if isBigEndian:
...     b.byteswap()
>>> b.astype(Long)
array([[0, 1],
       [2, 3],
       [4, 5]])

Check various forms of type specification

>>> print zeros(10, type=Int32)._type
Int32
>>> print zeros(10, type='Int32')._type
Int32
>>> print zeros(10, type='i4')._type
Int32
>>> print zeros(10, type='Int')._type
Int32
>>> print zeros(10, type=Float32)._type
Float32
>>> print zeros(10, type='Float32')._type
Float32
>>> print zeros(10, type='f4')._type
Float32
>>> print zeros(10, type='Float')._type
Float64

------------------------------------------------------------------------------
Check transpose

>>> a=reshape(arange(16),(4,4))
>>> dtp(a)
. array([[ 0,  1,  2,  3],
.        [ 4,  5,  6,  7],
.        [ 8,  9, 10, 11],
.        [12, 13, 14, 15]])
>>> dtp(transpose(a))
. array([[ 0,  4,  8, 12],
.        [ 1,  5,  9, 13],
.        [ 2,  6, 10, 14],
.        [ 3,  7, 11, 15]])
>>> a=reshape(arange(27),(3,3,3))
>>> dtp(a)
. array([[[ 0,  1,  2],
.         [ 3,  4,  5],
.         [ 6,  7,  8]],
.
.        [[ 9, 10, 11],
.         [12, 13, 14],
.         [15, 16, 17]],
.
.        [[18, 19, 20],
.         [21, 22, 23],
.         [24, 25, 26]]])
>>> a.transpose((1,2,0))
>>> dtp(a)
. array([[[ 0,  9, 18],
.         [ 1, 10, 19],
.         [ 2, 11, 20]],
.
.        [[ 3, 12, 21],
.         [ 4, 13, 22],
.         [ 5, 14, 23]],
.
.        [[ 6, 15, 24],
.         [ 7, 16, 25],
.         [ 8, 17, 26]]])

# Ensure that tranpose(transpose()) is contiguous...

>>> a=reshape(arange(100),(10,10))
>>> a.transpose()
>>> int(a.iscontiguous())
0
>>> a.transpose()
>>> int(a.iscontiguous())
1

------------------------------------------------------------------------------
Check swapaxes

>>> a=reshape(arange(27),(3,3,3)) # (0,1,2)
>>> dtp(a)
. array([[[ 0,  1,  2],
.         [ 3,  4,  5],
.         [ 6,  7,  8]],
.
.        [[ 9, 10, 11],
.         [12, 13, 14],
.         [15, 16, 17]],
.
.        [[18, 19, 20],
.         [21, 22, 23],
.         [24, 25, 26]]])
>>> int(a.iscontiguous())
1
>>> a.swapaxes(0,1)
>>> dtp(a)        #  (1,0,2)
. array([[[ 0,  1,  2],
.         [ 9, 10, 11],
.         [18, 19, 20]],
.
.        [[ 3,  4,  5],
.         [12, 13, 14],
.         [21, 22, 23]],
.
.        [[ 6,  7,  8],
.         [15, 16, 17],
.         [24, 25, 26]]])
>>> int(a.iscontiguous())
0
>>> a.swapaxes(1,2)
>>> dtp(a)        #  (1,2,0)
. array([[[ 0,  9, 18],
.         [ 1, 10, 19],
.         [ 2, 11, 20]],
.
.        [[ 3, 12, 21],
.         [ 4, 13, 22],
.         [ 5, 14, 23]],
.
.        [[ 6, 15, 24],
.         [ 7, 16, 25],
.         [ 8, 17, 26]]])
>>> int(a.iscontiguous())
0
>>> a.swapaxes(2,0)
>>> dtp(a)        # (0,2,1)
. array([[[ 0,  3,  6],
.         [ 1,  4,  7],
.         [ 2,  5,  8]],
.
.        [[ 9, 12, 15],
.         [10, 13, 16],
.         [11, 14, 17]],
.
.        [[18, 21, 24],
.         [19, 22, 25],
.         [20, 23, 26]]])
>>> int(a.iscontiguous())
0
>>> a.swapaxes(2,1)
>>> dtp(a)        # (0,1,2)
. array([[[ 0,  1,  2],
.         [ 3,  4,  5],
.         [ 6,  7,  8]],
.
.        [[ 9, 10, 11],
.         [12, 13, 14],
.         [15, 16, 17]],
.
.        [[18, 19, 20],
.         [21, 22, 23],
.         [24, 25, 26]]])
>>> int(a.iscontiguous())
1

------------------------------------------------------------------------------
Check choose

>>> a=arange(50,60)
>>> choose(a%3,(a,100),None,WRAP)
array([ 50,  51, 100,  53,  54, 100,  56,  57, 100,  59])
>>> choose(a%3,(a,100),None,CLIP)
array([100,  51, 100, 100,  54, 100, 100,  57, 100, 100])
>>> a=arange(50,150)
>>> choose(greater(a,100),(a,100))
array([ 50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
        63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
        76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,
        89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 100,
       100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
       100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
       100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
       100, 100, 100, 100, 100, 100, 100, 100, 100])

Check choose output array and output array coercion

>>> c=a.astype(Float64)
>>> choose(greater(a,100),(a,100),c)
>>> c
array([  50.,   51.,   52.,   53.,   54.,   55.,   56.,   57.,   58.,
         59.,   60.,   61.,   62.,   63.,   64.,   65.,   66.,   67.,
         68.,   69.,   70.,   71.,   72.,   73.,   74.,   75.,   76.,
         77.,   78.,   79.,   80.,   81.,   82.,   83.,   84.,   85.,
         86.,   87.,   88.,   89.,   90.,   91.,   92.,   93.,   94.,
         95.,   96.,   97.,   98.,   99.,  100.,  100.,  100.,  100.,
        100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,
        100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,
        100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,
        100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,
        100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,
        100.])

Check choose input array type conversion

>>> a=arange(-10,10,1,Int8)
>>> b=arange(0,20,1,UInt8)
>>> choose(greater(a,0),(a,b))
array([-10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,  11,  12,
        13,  14,  15,  16,  17,  18,  19], type=Int16)
>>> c=arange(100,120,1,Float64)
>>> choose(greater(a,0)+greater(a,5),(a,b,c))
array([ -10.,   -9.,   -8.,   -7.,   -6.,   -5.,   -4.,   -3.,   -2.,
         -1.,    0.,   11.,   12.,   13.,   14.,   15.,  116.,  117.,
        118.,  119.])

Check choose array broadcasts

>>> a=reshape(arange(16),(2,4,2))
>>> b=reshape(arange(100,108,1), (4,2))
>>> c=reshape(arange(200,202,1), (2,))
>>> dtp(choose(greater(a,8)+greater(a,13),(a,b,c)))
. array([[[  0,   1],
.         [  2,   3],
.         [  4,   5],
.         [  6,   7]],
.
.        [[  8, 101],
.         [102, 103],
.         [104, 105],
.         [200, 201]]])

------------------------------------------------------------------------------
Check where

>>> a=arange(5,15)
>>> where(greater(a,10),a,0)
array([ 0,  0,  0,  0,  0,  0, 11, 12, 13, 14])
>>> where(range(10),100,200)
array([200, 100, 100, 100, 100, 100, 100, 100, 100, 100])

Show array promotion to higher kind
>>> where(equal(a, 10), a, 1.0)
array([  1.,   1.,   1.,   1.,   1.,  10.,   1.,   1.,   1.,   1.])

Show array override within kind
>>> a=arange(10, type=Int8)
>>> where(equal(a,5), a, -1)
array([-1, -1, -1, -1, -1,  5, -1, -1, -1, -1], type=Int8)

------------------------------------------------------------------------------
Check clip

>>> a = arange(10)
>>> clip(a, 5, 8)
array([5, 5, 5, 5, 5, 5, 6, 7, 8, 8])
>>> a.setshape(5,2)
>>> clip(a, 3, 7)
array([[3, 3],
       [3, 3],
       [4, 5],
       [6, 7],
       [7, 7]])

------------------------------------------------------------------------------
Check concatenate

>>> a=reshape(arange(16),(4,4))
>>> b=reshape(arange(8,type=Float64), (2,4))
>>> concatenate((a,b))
array([[  0.,   1.,   2.,   3.],
       [  4.,   5.,   6.,   7.],
       [  8.,   9.,  10.,  11.],
       [ 12.,  13.,  14.,  15.],
       [  0.,   1.,   2.,   3.],
       [  4.,   5.,   6.,   7.]])
>>> c=concatenate((a,b,a))
>>> c
array([[  0.,   1.,   2.,   3.],
       [  4.,   5.,   6.,   7.],
       [  8.,   9.,  10.,  11.],
       [ 12.,  13.,  14.,  15.],
       [  0.,   1.,   2.,   3.],
       [  4.,   5.,   6.,   7.],
       [  0.,   1.,   2.,   3.],
       [  4.,   5.,   6.,   7.],
       [  8.,   9.,  10.,  11.],
       [ 12.,  13.,  14.,  15.]])

Make sure unique copies are made by concatenation...

>>> c[0][0]=100
>>> c[6][0]=101
>>> c
array([[ 100.,    1.,    2.,    3.],
       [   4.,    5.,    6.,    7.],
       [   8.,    9.,   10.,   11.],
       [  12.,   13.,   14.,   15.],
       [   0.,    1.,    2.,    3.],
       [   4.,    5.,    6.,    7.],
       [ 101.,    1.,    2.,    3.],
       [   4.,    5.,    6.,    7.],
       [   8.,    9.,   10.,   11.],
       [  12.,   13.,   14.,   15.]])

Check non-zero axes concatenations

>>> a=reshape(arange(27),(3,3,3))
>>> dtp(a)
. array([[[ 0,  1,  2],
.         [ 3,  4,  5],
.         [ 6,  7,  8]],
.
.        [[ 9, 10, 11],
.         [12, 13, 14],
.         [15, 16, 17]],
.
.        [[18, 19, 20],
.         [21, 22, 23],
.         [24, 25, 26]]])
>>> dtp(concatenate((a,a),2))
. array([[[ 0,  1,  2,  0,  1,  2],
.         [ 3,  4,  5,  3,  4,  5],
.         [ 6,  7,  8,  6,  7,  8]],
.
.        [[ 9, 10, 11,  9, 10, 11],
.         [12, 13, 14, 12, 13, 14],
.         [15, 16, 17, 15, 16, 17]],
.
.        [[18, 19, 20, 18, 19, 20],
.         [21, 22, 23, 21, 22, 23],
.         [24, 25, 26, 24, 25, 26]]])

Check array syntax for concatenation

>>> a = arange(5)
>>> array((a,a))
array([[0, 1, 2, 3, 4],
       [0, 1, 2, 3, 4]])

>>> array((a,a), type=Float64)
array([[ 0.,  1.,  2.,  3.,  4.],
       [ 0.,  1.,  2.,  3.,  4.]])

>>> array((a,a), shape=(10))
array([0, 1, 2, 3, 4, 0, 1, 2, 3, 4])

Check take & put

>>> a=arange(10)
>>> b=arange(5)
>>> take(a,b)
array([0, 1, 2, 3, 4])
>>> a.take(b)
array([0, 1, 2, 3, 4])
>>>

>>> a=reshape(arange(20),(4,5))
>>> a
array([[ 0,  1,  2,  3,  4],
       [ 5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14],
       [15, 16, 17, 18, 19]])
>>> take(a,(-1,),clipmode=CLIP)
array([[0, 1, 2, 3, 4]])
>>> take(a,(3,),clipmode=WRAP)
array([[15, 16, 17, 18, 19]])
>>> take(a, (0,1))
array([[0, 1, 2, 3, 4],
       [5, 6, 7, 8, 9]])
>>> take(a, (0,-1),clipmode=CLIP)
array([[0, 1, 2, 3, 4],
       [0, 1, 2, 3, 4]])
>>> take(a, (arange(4),0), axis=(0,))
array([ 0,  5, 10, 15])
>>> x=arange(10.)*100
>>> take(x,[[2,4],[1,2]])
array([[ 200.,  400.],
       [ 100.,  200.]])
>>> a0=arange(100, shape=(10,10))
>>> take(a0, 1000, clipmode=RAISE)
Traceback (most recent call last):
...
IndexError: Index out of range

>>> b=arange(5)
>>> put(a,(3,3),1,axis=(0,))
>>> a
array([[ 0,  1,  2,  3,  4],
       [ 5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14],
       [15, 16, 17,  1, 19]])

>>> # Check clipping
>>> put(a, (4, b), 0, axis=(0,), clipmode=CLIP)
>>> a
array([[ 0,  1,  2,  3,  4],
       [ 5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14],
       [ 0,  0,  0,  0,  0]])
>>> put(a, (4, b), 0, axis=(0,), clipmode=WRAP)
>>> a
array([[ 0,  0,  0,  0,  0],
       [ 5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14],
       [ 0,  0,  0,  0,  0]])
>>> a0=arange(100, shape=(10,10))
>>> put(a0, 1000, 1, clipmode=RAISE)
Traceback (most recent call last):
...
IndexError: Index out of range
>>> a=reshape(arange(12*2, type=Int16),(4,3,2))
>>> dtp(a)
. array([[[ 0,  1],
.         [ 2,  3],
.         [ 4,  5]],
.
.        [[ 6,  7],
.         [ 8,  9],
.         [10, 11]],
.
.        [[12, 13],
.         [14, 15],
.         [16, 17]],
.
.        [[18, 19],
.         [20, 21],
.         [22, 23]]], type=Int16)
>>> b=take(a, (((1,1),(2,2)), ((1,2),(1,2))), axis=(0,) )
>>> dtp(b)
. array([[[ 8,  9],
.         [10, 11]],
.
.        [[14, 15],
.         [16, 17]]], type=Int16)
>>> b = sqrt(b)
>>> put(a, (((1,1),(2,2)), ((1,2),(1,2))), b, axis=(0,))
>>> dtp(a)
. array([[[ 0,  1],
.         [ 2,  3],
.         [ 4,  5]],
.
.        [[ 6,  7],
.         [ 2,  3],
.         [ 3,  3]],
.
.        [[12, 13],
.         [ 3,  3],
.         [ 4,  4]],
.
.        [[18, 19],
.         [20, 21],
.         [22, 23]]], type=Int16)

Put buffer limit check tests

>>> a._data = memory.new_memory(len(a._data) - 1)
>>> put(a, (((1,1),(2,2)), ((1,2),(1,2))), b, axis=(0,))
Traceback (most recent call last):
...
error: putNBytes: access beyond buffer. offset=47 buffersize=47

>>> b=take(a, (((1,1),(2,2)), ((1,2),(1,2))), axis=(0,) )
Traceback (most recent call last):
...
error: takeNBytes: access beyond buffer. offset=47 buffersize=47

Check array based indexing

>>> a=arange(25,shape=(5,5)); a[arange(5),1]
array([ 1,  6, 11, 16, 21])
>>> a=arange(25,shape=(5,5)); a[arange(5),0] = a[arange(5),1]; a
array([[ 1,  1,  2,  3,  4],
       [ 6,  6,  7,  8,  9],
       [11, 11, 12, 13, 14],
       [16, 16, 17, 18, 19],
       [21, 21, 22, 23, 24]])
>>> a=arange(25,shape=(5,5)); a[ range(3) ]
array([[ 0,  1,  2,  3,  4],
       [ 5,  6,  7,  8,  9],
       [10, 11, 12, 13, 14]])
>>> a=arange(25,shape=(5,5)); a[ range(3) ] = range(5); a
array([[ 0,  1,  2,  3,  4],
       [ 0,  1,  2,  3,  4],
       [ 0,  1,  2,  3,  4],
       [15, 16, 17, 18, 19],
       [20, 21, 22, 23, 24]])

Check boolean array based indexing (compress / choose)

>>> a=arange(25,shape=(5,5)); a[ a < 11  ]
array([ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10])
>>> a=arange(25,shape=(5,5)); a[ a < 11 ] = 99; a
array([[99, 99, 99, 99, 99],
       [99, 99, 99, 99, 99],
       [99, 11, 12, 13, 14],
       [15, 16, 17, 18, 19],
       [20, 21, 22, 23, 24]])


Exercise multi-buffer put and take.

>>> bsize = uf._bufferPool.setBufferSize(100)
>>> a=reshape(arange(100*2),(100,2))
>>> a[arange(100),1] = a[arange(100),0] % 5
>>> a[arange(50,100),1]
array([0, 2, 4, 1, 3, 0, 2, 4, 1, 3, 0, 2, 4, 1, 3, 0, 2, 4, 1, 3, 0,
       2, 4, 1, 3, 0, 2, 4, 1, 3, 0, 2, 4, 1, 3, 0, 2, 4, 1, 3, 0, 2,
       4, 1, 3, 0, 2, 4, 1, 3])
>>> o = uf._bufferPool.setBufferSize(bsize)

Exercise non-contiguous put, take, choose

>>> a = arange(12., shape=(4,3))[:,0]; a
array([ 0.,  3.,  6.,  9.])
>>> int(a.iscontiguous())
0
>>> choose(a%6 != 0, (a, 3*a))
array([  0.,   9.,   6.,  27.])
>>> b = nonzero(a==6); take(a, *b)
array([ 6.])
>>> a[ a == 6 ]
array([ 6.])
>>> put(a, *(nonzero(a==6) + (99,))); a
array([  0.,   3.,  99.,   9.])
>>> a[ a == 99. ] = 66; a
array([  0.,   3.,  66.,   9.])
>>> a = arange(12., shape=(4,3)); a.transpose(); int(a.iscontiguous()); a
0
array([[  0.,   3.,   6.,   9.],
       [  1.,   4.,   7.,  10.],
       [  2.,   5.,   8.,  11.]])
>>> a.transpose(); a.byteswap(); a.togglebyteorder(); int(a.iscontiguous()); int(a.isbyteswapped())
1
1

SF #[ 856611 ] takeNBytes fails

>>> a = arange(25, 50, shape=(5,5))
>>> a[ argmax(a) ]
array([[45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49]])

take output array specification

>>> b = zeros((5,5))
>>> take(a, argmax(a), outarr=b); b
array([[45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49],
       [45, 46, 47, 48, 49]])

------------------------------------------------------------------------------
Check compress

>>> x=arange(4)
>>> x
array([0, 1, 2, 3])
>>> c=greater(x,2)
>>> c
array([0, 0, 0, 1], type=Bool)
>>> compress(c, x)
array([3])

Or the shorthand compression notation:
>>> x[x > 2]
array([3])

------------------------------------------------------------------------------
Check Nonzero

>>> a=arange(20)
>>> nonzero(a%2)
(array([ 1,  3,  5,  7,  9, 11, 13, 15, 17, 19]),)
>>> a.setshape(2,10)
>>> nonzero(a)
(array([0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]), array([1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]))

------------------------------------------------------------------------------
Check whole cast of ufuncs

>>> eps = 1.e-5

Arithmetic operators

>>> x = ((arange(20.)-10)/3).astype(Float64)
>>> y = zeros((20,))+15.333
>>> comparr(-x,
...         array([  3.33333325e+00,   3.00000000e+00,   2.66666675e+00,
...                  2.33333325e+00,   2.00000000e+00,   1.66666663e+00,
...                  1.33333337e+00,   1.00000000e+00,   6.66666687e-01,
...                  3.33333343e-01,   0.00000000e+00,  -3.33333343e-01,
...                 -6.66666687e-01,  -1.00000000e+00,  -1.33333337e+00,
...                 -1.66666663e+00,  -2.00000000e+00,  -2.33333325e+00,
...                 -2.66666675e+00,  -3.00000000e+00]), eps)
>>> comparr(x/y,
...         array([ -2.17396021e-01,  -1.95656419e-01,  -1.73916832e-01,
...         -1.52177215e-01,  -1.30437613e-01,  -1.08698010e-01,
...         -8.69584158e-02,  -6.52188063e-02,  -4.34792079e-02,
...         -2.17396040e-02,   0.00000000e+00,   2.17396040e-02,
...          4.34792079e-02,   6.52188063e-02,   8.69584158e-02,
...          1.08698010e-01,   1.30437613e-01,   1.52177215e-01,
...          1.73916832e-01,   1.95656419e-01]), eps)
>>> comparr(x*y,
...         array([-51.11000061, -45.99900055, -40.88800049, -35.77700043,
...         -30.66600037, -25.55500031, -20.44400024, -15.33300018,
...         -10.22200012,  -5.11100006,   0.        ,   5.11100006,
...          10.22200012,  15.33300018,  20.44400024,  25.55500031,
...          30.66600037,  35.77700043,  40.88800049,  45.99900055]), eps)
>>> comparr(x-y,
...         array([-18.66633415, -18.33300018, -17.99966621, -17.66633415,
...         -17.33300018, -16.99966621, -16.66633415, -16.33300018,
...         -15.99966717, -15.6663332 , -15.33300018, -14.99966717,
...         -14.6663332 , -14.33300018, -13.99966717, -13.6663332 ,
...         -13.33300018, -12.99966717, -12.6663332 , -12.33300018]), eps)
>>> comparr(x % 0.31,
...         array([ -2.33333230e-01,  -2.09999979e-01,  -1.86666727e-01,
...         -1.63333237e-01,  -1.39999986e-01,  -1.16666615e-01,
...         -9.33333635e-02,  -6.99999928e-02,  -4.66666818e-02,
...         -2.33333409e-02,   0.00000000e+00,   2.33333409e-02,
...          4.66666818e-02,   6.99999928e-02,   9.33333635e-02,
...          1.16666615e-01,   1.39999986e-01,   1.63333237e-01,
...          1.86666727e-01,   2.09999979e-01]), eps)

Trig functions

>>> vcos = cos(x)
>>> vcosc = array([ -9.81674016e-01,  -9.89992499e-01,  -8.89326632e-01,
...                 -6.90758109e-01,  -4.16146845e-01,  -9.57235098e-02,
...                  2.35237539e-01,   5.40302277e-01,   7.85887241e-01,
...                  9.44956958e-01,   1.00000000e+00,   9.44956958e-01,
...                  7.85887241e-01,   5.40302277e-01,   2.35237539e-01,
...                 -9.57235098e-02,  -4.16146845e-01,  -6.90758109e-01,
...                 -8.89326632e-01,  -9.89992499e-01])
>>> comparr(vcos, vcosc, eps)
>>> vsin = sin(x)
>>> vsinc = array([  1.90567881e-01,  -1.41120002e-01,  -4.57272559e-01,
...                 -7.23085940e-01,  -9.09297407e-01,  -9.95407939e-01,
...                 -9.71937895e-01,  -8.41470957e-01,  -6.18369818e-01,
...                 -3.27194721e-01,   0.00000000e+00,   3.27194721e-01,
...                  6.18369818e-01,   8.41470957e-01,   9.71937895e-01,
...                  9.95407939e-01,   9.09297407e-01,   7.23085940e-01,
...                  4.57272559e-01,   1.41120002e-01])
>>> comparr(vsin, vsinc, eps)
>>> comparr(vsin**2+vcos**2, ones((20,)), eps)
>>> vtan = tan(x)
>>> vtanc = array([ -1.94125429e-01,   1.42546549e-01,   5.14178455e-01,
...                  1.04680049e+00,   2.18503976e+00,   1.03987827e+01,
...                 -4.13172960e+00,  -1.55740774e+00,  -7.86842942e-01,
...                 -3.46253574e-01,   0.00000000e+00,   3.46253574e-01,
...                  7.86842942e-01,   1.55740774e+00,   4.13172960e+00,
...                 -1.03987827e+01,  -2.18503976e+00,  -1.04680049e+00,
...                 -5.14178455e-01,  -1.42546549e-01])
>>> comparr(vtan, vtanc, eps)
>>> varcsinc = array([  1.91740602e-01,  -1.41592652e-01,  -4.74925905e-01,
...                    -8.08259428e-01,  -1.14159262e+00,  -1.47492576e+00,
...                    -1.33333325e+00,  -9.99999940e-01,  -6.66666687e-01,
...                    -3.33333373e-01,   0.00000000e+00,   3.33333373e-01,
...                     6.66666687e-01,   9.99999940e-01,   1.33333325e+00,
...                     1.47492576e+00,   1.14159262e+00,   8.08259428e-01,
...                     4.74925905e-01,   1.41592652e-01])
>>> comparr(arcsin(vsin), varcsinc, eps)
>>> varccosc = array([  2.94985199e+00,   3.00000000e+00,   2.66666675e+00,
...                     2.33333325e+00,   2.00000000e+00,   1.66666663e+00,
...                     1.33333337e+00,   1.00000000e+00,   6.66666687e-01,
...                     3.33333284e-01,   0.00000000e+00,   3.33333284e-01,
...                     6.66666687e-01,   1.00000000e+00,   1.33333337e+00,
...                     1.66666663e+00,   2.00000000e+00,   2.33333325e+00,
...                     2.66666675e+00,   3.00000000e+00])
>>> comparr(arccos(vcos), varccosc, eps)
>>> varctanc = array([ -1.91740602e-01,   1.41592667e-01,   4.74925935e-01,
...                     8.08259368e-01,   1.14159262e+00,   1.47492599e+00,
...                    -1.33333337e+00,  -1.00000000e+00,  -6.66666687e-01,
...                    -3.33333343e-01,   0.00000000e+00,   3.33333343e-01,
...                     6.66666687e-01,   1.00000000e+00,   1.33333337e+00,
...                    -1.47492599e+00,  -1.14159262e+00,  -8.08259368e-01,
...                    -4.74925935e-01,  -1.41592667e-01])
>>> comparr(arctan(vtan), varctanc, eps)

Exponential and log related functions

>>> xplus = ((arange(20.)+1)/2).astype(Float64)
>>> x = ((arange(40.)-20)/2).astype(Float64)
>>> vlog = log(xplus)
>>> vlogc = array([ -6.93147182e-01,   0.00000000e+00,   4.05465096e-01,
...                  6.93147182e-01,   9.16290760e-01,   1.09861231e+00,
...                  1.25276291e+00,   1.38629436e+00,   1.50407743e+00,
...                  1.60943794e+00,   1.70474803e+00,   1.79175949e+00,
...                  1.87180221e+00,   1.94591010e+00,   2.01490307e+00,
...                  2.07944155e+00,   2.14006615e+00,   2.19722462e+00,
...                  2.25129175e+00,   2.30258512e+00])
>>> comparr(vlog, vlogc, eps)
>>> vlog10 = log10(xplus)
>>> vlog10c = array([ -3.01030010e-01,   0.00000000e+00,   1.76091254e-01,
...                    3.01030010e-01,   3.97940010e-01,   4.77121264e-01,
...                    5.44068038e-01,   6.02060020e-01,   6.53212488e-01,
...                    6.98970020e-01,   7.40362704e-01,   7.78151274e-01,
...                    8.12913358e-01,   8.45098019e-01,   8.75061274e-01,
...                    9.03090000e-01,   9.29418921e-01,   9.54242527e-01,
...                    9.77723598e-01,   1.00000000e+00])
>>> comparr(vlog10, vlog10c, eps)
>>> vexp = exp(x)
>>> vexpc = array([  4.53999310e-05,   7.48518287e-05,   1.23409802e-04,
...                  2.03468371e-04,   3.35462624e-04,   5.53084363e-04,
...                  9.11881973e-04,   1.50343915e-03,   2.47875229e-03,
...                  4.08677151e-03,   6.73794700e-03,   1.11089963e-02,
...                  1.83156393e-02,   3.01973838e-02,   4.97870669e-02,
...                  8.20849985e-02,   1.35335281e-01,   2.23130167e-01,
...                  3.67879450e-01,   6.06530666e-01,   1.00000000e+00,
...                  1.64872122e+00,   2.71828175e+00,   4.48168898e+00,
...                  7.38905621e+00,   1.21824942e+01,   2.00855370e+01,
...                  3.31154518e+01,   5.45981483e+01,   9.00171280e+01,
...                  1.48413162e+02,   2.44691925e+02,   4.03428802e+02,
...                  6.65141663e+02,   1.09663318e+03,   1.80804236e+03,
...                  2.98095801e+03,   4.91476904e+03,   8.10308398e+03,
...                  1.33597266e+04])
>>> ratioarr(vexp, vexpc, eps)
>>> vexp10 = 10.**x[::3]
>>> vexp10c = array([  1.00000001e-10,   3.16227755e-09,   1.00000001e-07,
...                    3.16227761e-06,   9.99999975e-05,   3.16227763e-03,
...                    1.00000001e-01,   3.16227770e+00,   1.00000000e+02,
...                    3.16227759e+03,   1.00000000e+05,   3.16227775e+06,
...                    1.00000000e+08,   3.16227763e+09])
>>> ratioarr(vexp10, vexp10c, eps)
>>> vsinh = sinh(x[::3])
>>> vsinhc = array([ -1.10132324e+04,  -2.45738428e+03,  -5.48316101e+02,
...                  -1.22343925e+02,  -2.72899170e+01,  -6.05020428e+00,
...                  -1.17520118e+00,   5.21095276e-01,   3.62686038e+00,
...                   1.65426273e+01,   7.42032089e+01,   3.32570068e+02,
...                   1.49047888e+03,   6.67986328e+03])
>>> ratioarr(vsinh, vsinhc, eps)
>>> vcosh = cosh(x[::3])
>>> vcoshc = array([  1.10132334e+04,   2.45738452e+03,   5.48317017e+02,
...                   1.22348007e+02,   2.73082333e+01,   6.13228941e+00,
...                   1.54308069e+00,   1.12762594e+00,   3.76219559e+00,
...                   1.65728245e+01,   7.42099457e+01,   3.32571564e+02,
...                   1.49047913e+03,   6.67986328e+03])
>>> ratioarr(vcosh, vcoshc, eps)
>>> vtanh = tanh(x[::3])
>>> vtanhc = array([ -1.00000000e+00,  -9.99999940e-01,  -9.99998331e-01,
...                  -9.99966621e-01,  -9.99329329e-01,  -9.86614287e-01,
...                  -7.61594176e-01,   4.62117165e-01,   9.64027584e-01,
...                   9.98177886e-01,   9.99909222e-01,   9.99995470e-01,
...                   9.99999762e-01,   1.00000000e+00])
>>> comparr(vtanh, vtanhc, eps)
>>> vsqrt = sqrt(xplus)
>>> vsqrtc = array([  7.07106769e-01,   1.00000000e+00,   1.22474492e+00,
...                   1.41421354e+00,   1.58113885e+00,   1.73205078e+00,
...                   1.87082875e+00,   2.00000000e+00,   2.12132025e+00,
...                   2.23606801e+00,   2.34520793e+00,   2.44948983e+00,
...                   2.54950976e+00,   2.64575124e+00,   2.73861289e+00,
...                   2.82842708e+00,   2.91547585e+00,   3.00000000e+00,
...                   3.08220696e+00,   3.16227770e+00])
>>> comparr(vsqrt, vsqrtc, eps)

Comparison operators

>>> xint1 = array([2, 8, 100, -100, -50, 44, 32, 2, 5, 19])
>>> xint2 = arange(10)*5
>>> xdouble1 = vcos.astype(Float64)
>>> xdouble2 = vtan
>>> comparr(greater(xint1, xint2), array([1, 1, 1, 0, 0, 1, 1, 0, 0, 0], type=Bool),0)
>>> comparr(greater(xint1, xint2), less_equal(xint2, xint1), 0)
>>> comparr(greater(xint1, 5), array([0, 1, 1, 0, 0, 1, 1, 0, 0, 1], type=Bool), 0)
>>> comparr(greater(xdouble1, xdouble2),
...         array([0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0], type=Bool), 0)
>>> comparr(equal(xint2, 20), array([0, 0, 0, 0, 1, 0, 0, 0, 0, 0], type=Bool), 0)
>>> comparr(equal(xint2, 20), logical_not(not_equal(20, xint2)), 0)
>>> comparr(less(xint1, 0.), array([0, 0, 0, 1, 1, 0, 0, 0, 0, 0], type=Bool), 0)
>>> comparr(less(xdouble1, xdouble2), 
...         array([1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1], type=Bool), 0)
>>> comparr(less(xdouble1, xdouble2), greater_equal(xdouble2, xdouble1), 0)

Logical operators

>>> xint1 = array([2,  4,  5, -1, 0, 1, 0, 0])
>>> xint2 = array([0, -2,  1,  0, 0, 1, 0, 9])
>>> comparr(logical_and(xint1, xint2),  array([0, 1, 1, 0, 0, 1, 0, 0], type=Bool), 0)
>>> comparr(logical_or(xint1, xint2), array([1, 1, 1, 1, 0, 1, 0, 1], type=Bool), 0)
>>> comparr(logical_xor(xint1, xint2), array([1, 0, 0, 1, 0, 0, 0, 1], type=Bool), 0)
>>> comparr(logical_not(xint1), array([0, 0, 0, 0, 1, 0, 1, 1], type=Bool), 0)
>>> comparr(logical_and(xint1.astype(Float64), xint2),
...         array([0, 1, 1, 0, 0, 1, 0, 0], type=Bool), 0)
>>> comparr(logical_or(xint1, xint2.astype(Float64)),
...         array([1, 1, 1, 1, 0, 1, 0, 1], type=Bool), 0)
>>> comparr(logical_xor(xint1.astype(Bool), xint2),
...         array([1, 0, 0, 1, 0, 0, 0, 1], type=Bool), 0)

Bitwise operators

>>> xint1 = array([7878, 34234, 4234423, 437880, 11111, 0, -1])
>>> xint2 = array([-1,   -287,  789789,  23,      89089, 9, -1000])
>>> comparr(bitwise_and(xint1, xint2),
...         array([ 7878, 33952,  3093,    16,  2049,     0, -1000]), 0)
>>> comparr(bitwise_or(xint1, xint2),
...         array([     -1,      -5, 5021119,  437887,   98151,       9,      -1]), 0)
>>> comparr(bitwise_xor(xint1, xint2),
...         array([  -7879,  -33957, 5018026,  437871,   96102,       9,     999]), 0)
>>> comparr(bitwise_not(xint1),
...         array([   -7879,   -34235, -4234424,  -437881,   -11112,       -1,        0]), 0)
>>> comparr(rshift(xint2, 3),
...         array([   -1,   -36, 98723,     2, 11136,     1,  -125]), 0)
>>> comparr(lshift(xint2, 3),
...         array([     -8,   -2296, 6318312,     184,  712712,      72,   -8000]), 0)

>>> a = array([1, 2, 4, 8])
>>> b = array([8, 4, 2, 8])
>>> a & b
array([0, 0, 0, 8])
>>> a | b
array([9, 6, 6, 8])
>>> a ^ b
array([9, 6, 6, 0])

Ceil and Floor

>>> xfloat = arange(-3.,3,0.31)
>>> comparr(ceil(xfloat),
...         array([-3., -2., -2., -2., -1., -1., -1.,  0.,  0.,  0.,  1.,  1.,  1.,
...                 2.,  2.,  2.,  2.,  3.,  3.,  3.]), 0.)
>>> comparr(floor(xfloat),
...         array([-3., -3., -3., -3., -2., -2., -2., -1., -1., -1.,  0.,  0.,
...                 0.,  1.,  1.,  1.,  1.,  2.,  2.,  2.]), 0.)
>>> comparr(ceil(xint1),
...         array([   7878,   34234, 4234423,  437880,   11111,       0,      -1]), 0)
>>> comparr(floor(xint1),
...         array([   7878,   34234, 4234423,  437880,   11111,       0,      -1]), 0)

Minium and Maximum

>>> maximum(arange(10),5)
array([5, 5, 5, 5, 5, 5, 6, 7, 8, 9])
>>> minimum(arange(10)-3,0.)
array([-3., -2., -1.,  0.,  0.,  0.,  0.,  0.,  0.,  0.])
>>> maximum.reduce(arange(10)-12)
-3

Now test each ufunc sig type against various types
(And test exception handling and limits for integers too)

>>> array([1, 8, 100, 100], type=Int8) * array([1, 8, 100, -100], type=Int8)
Warning: Encountered overflow(s)  in multiply
array([   1,   64,  127, -128], type=Int8)
>>> array([1, 8, 100, 100], type=UInt8) * array([1, 8, 100, -100], type=UInt8)
Warning: Encountered overflow(s)  in multiply
array([  1,  64, 255, 255], type=UInt8)
>>> array([1, 100, 15000, 15000], type=Int16) * array([1, 100, 15000, -15000], type=Int16)
Warning: Encountered overflow(s)  in multiply
array([     1,  10000,  32767, -32768], type=Int16)
>>> array([1, 100, 15000, 15000], type=UInt16) * array([1, 100, 15000, -15000], type=UInt16)
Warning: Encountered overflow(s)  in multiply
array([    1, 10000, 65535, 65535], type=UInt16)

>>> a = array([1, 15000, 2000000, 2000000], type=Int32)
>>> b = array([1, 15000, 2000000, -2000000], type=Int32)
>>> explicit_type(a * b)
Warning: Encountered overflow(s)  in multiply
array([          1,   225000000,  2147483647, -2147483648], type=Int32)
>>> r = array([1.e20, 1.e-20], type=Float32) * array([1.e20, 1.e-20], type=Float32)
Warning: Encountered overflow(s)  in multiply

>>> r = array([1.e200, 1.e-200], type=Float64) * array([1.e200, 1.e-200], type=Float64)
Warning: Encountered overflow(s)  in multiply


>>> array([1, 8, 100, 100], type=Int8) / array([0, 3, 101, -100], type=Int8)
Warning: Encountered divide by zero(s)  in divide
array([ 0,  2,  0, -1], type=Int8)
>>> array([1, 8, 100, 100], type=UInt8) / array([0, 3, 101, 50], type=UInt8)
Warning: Encountered divide by zero(s)  in divide
array([0, 2, 0, 2], type=UInt8)
>>> array([1, 100, 15000, 15000], type=Int16) / array([0, 16, 15001, -15000], type=Int16)
Warning: Encountered divide by zero(s)  in divide
array([ 0,  6,  0, -1], type=Int16)
>>> array([1, 100, 15000, 15000], type=UInt16) / array([0, 16, 15001, 5000], type=UInt16)
Warning: Encountered divide by zero(s)  in divide
array([0, 6, 0, 3], type=UInt16)
>>> array([1, 15000, 2000000, 2000000])/array([0, 15001, 2000000, -2000000])
Warning: Encountered divide by zero(s)  in divide
array([ 0,  0,  1, -1])
>>> r = array([0., 1.], type=Float32)/0.
Warning: Encountered invalid numeric result(s)  in divide
Warning: Encountered divide by zero(s)  in divide
>>> int(r._type == Float32)
1
>>> r = array([0., 1.], type=Float64)/0.
Warning: Encountered invalid numeric result(s)  in divide
Warning: Encountered divide by zero(s)  in divide
>>> int(r._type == Float64)
1

>>> array([100, 0], type=Int8) - 50
array([ 50, -50], type=Int8)
>>> array([100, 0], type=UInt8) - 50
array([ 50, 206], type=UInt8)
>>> array([100, 0], type=Int16) - 50
array([ 50, -50], type=Int16)
>>> array([100, 0], type=UInt16) - 50
array([   50, 65486], type=UInt16)
>>> array([100, 0]) - 50
array([ 50, -50])
>>> array([100, 0]) - 50.
array([ 50., -50.])
>>> array([100, 0], type=Float32) - 50.
array([ 50., -50.], type=Float32)
>>> array([100, 0], type=Float64) - 50
array([ 50., -50.])
>>> array([50, 50], type=UInt8) - array([50, -50], type=Int8)
array([  0, 100], type=Int16)

Test for other domain errors (platform dependent?)

>>> r = sqrt(array([-100]))
Warning: Encountered invalid numeric result(s)  in sqrt
>>> r = log(array([0., -1.]))
Warning: Encountered invalid numeric result(s)  in log
Warning: Encountered divide by zero(s)  in log
>>> r = arccos(array([2.]))
Warning: Encountered invalid numeric result(s)  in arccos

>>> r = bitwise_and(array([15.]), array([7]))
Traceback (most recent call last):
...
TypeError: Couldn't find a function which matches the inputs

>>> comparr(sin(array([10], type=UInt8)), array([ -5.44021130e-01]), eps)

>>> x = array([0., -1.e300, 1.e-300], type=Float64)
>>> Error.setMode(all="ignore")
>>> r = sqrt(x) + (x+1)/x + x*x
>>> Error.setMode(all="warn")
>>> r = sqrt(x) + (x+1)/x + x*x
Warning: Encountered invalid numeric result(s)  in sqrt
Warning: Encountered divide by zero(s)  in divide
Warning: Encountered overflow(s)  in multiply
Warning: Encountered underflow(s)  in multiply
>>> Error.setMode(all="raise")
>>> r = sqrt(x) + (x+1)/x + x*x
Traceback (most recent call last):
...
MathDomainError:  in sqrt
>>> Error.setMode(underflow="ignore", overflow="warn", invalid="warn", dividebyzero="raise")
>>> r = sqrt(x) + x*x
Warning: Encountered invalid numeric result(s)  in sqrt
Warning: Encountered overflow(s)  in multiply
>>> r = (x+1)/x
Traceback (most recent call last):
...
ZeroDivisionError:  in divide

>>> Error.pushMode(all="warn")
>>> Error.getMode()
_NumErrorMode(overflow='warn', underflow='warn', dividebyzero='warn', invalid='warn')
>>> x = Error.popMode()
>>> Error.getMode()
_NumErrorMode(overflow='warn', underflow='ignore', dividebyzero='raise', invalid='warn')


------------------------------------------------------------------------------
Check sort, argsort & searchsorted

>>> a=arange(10,0,-1)
>>> a.sort()
>>> a
array([ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10])
>>> searchsorted(a,a)
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
>>> bins = arange(0,1.0,0.1)
>>> data = array([0.303, 0.795, 0.827, 0.780, 0.551, 0.760, 0.285])
>>> searchsorted(bins, data)
array([4, 8, 9, 8, 6, 8, 3])

>>> for kind in ['','quicksort','mergesort','heapsort'] :
...     a = arange(23,0,-1)
...     a.sort(kind=kind)
...     print a
...
[ 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23]
[ 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23]
[ 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23]
[ 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23]


>>> a=arange(10,0,-1)
>>> a.argsort()
array([9, 8, 7, 6, 5, 4, 3, 2, 1, 0])

>>> for kind in ['','quicksort','mergesort','heapsort'] :
...     a = arange(23,0,-1)
...     print a.argsort(kind=kind)
...
[22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0]
[22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0]
[22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0]
[22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0]

>>> sort(arange(10,0,-1))
array([ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10])
>>> argsort(arange(20,10,-1))
array([9, 8, 7, 6, 5, 4, 3, 2, 1, 0])

>>> for kind in ['','quicksort','mergesort','heapsort'] : print sort(arange(10,0,-1),kind=kind)
...
[ 1  2  3  4  5  6  7  8  9 10]
[ 1  2  3  4  5  6  7  8  9 10]
[ 1  2  3  4  5  6  7  8  9 10]
[ 1  2  3  4  5  6  7  8  9 10]

>>> for kind in ['','quicksort','mergesort','heapsort'] : print argsort(arange(20,10,-1),kind=kind)
...
[9 8 7 6 5 4 3 2 1 0]
[9 8 7 6 5 4 3 2 1 0]
[9 8 7 6 5 4 3 2 1 0]
[9 8 7 6 5 4 3 2 1 0]

Check lexical sort

>>> key1 = arange(23,0,-1)%3
>>> key2 = arange(23,0,-1)//3
>>> print lexsort((key1,key2))
[22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0]

Check multi-dim sort

>>> a=array([[5, 0, 1, 9, 8],
...          [2, 5, 8, 3, 2],
...          [8, 0, 3, 7, 0],
...          [9, 6, 9, 5, 0],
...          [9, 0, 9, 7, 7]])

>>> print sort(a)
[[0 1 5 8 9]
 [2 2 3 5 8]
 [0 0 3 7 8]
 [0 5 6 9 9]
 [0 7 7 9 9]]

>>> for kind in ['','quicksort','mergesort','heapsort'] : print sort(a,kind=kind)
...
[[0 1 5 8 9]
 [2 2 3 5 8]
 [0 0 3 7 8]
 [0 5 6 9 9]
 [0 7 7 9 9]]
[[0 1 5 8 9]
 [2 2 3 5 8]
 [0 0 3 7 8]
 [0 5 6 9 9]
 [0 7 7 9 9]]
[[0 1 5 8 9]
 [2 2 3 5 8]
 [0 0 3 7 8]
 [0 5 6 9 9]
 [0 7 7 9 9]]
[[0 1 5 8 9]
 [2 2 3 5 8]
 [0 0 3 7 8]
 [0 5 6 9 9]
 [0 7 7 9 9]]

>>> print sort(a, 0)
[[2 0 1 3 0]
 [5 0 3 5 0]
 [8 0 8 7 2]
 [9 5 9 7 7]
 [9 6 9 9 8]]

>>> for kind in ['','quicksort','mergesort','heapsort'] : print sort(a,0,kind=kind)
...
[[2 0 1 3 0]
 [5 0 3 5 0]
 [8 0 8 7 2]
 [9 5 9 7 7]
 [9 6 9 9 8]]
[[2 0 1 3 0]
 [5 0 3 5 0]
 [8 0 8 7 2]
 [9 5 9 7 7]
 [9 6 9 9 8]]
[[2 0 1 3 0]
 [5 0 3 5 0]
 [8 0 8 7 2]
 [9 5 9 7 7]
 [9 6 9 9 8]]
[[2 0 1 3 0]
 [5 0 3 5 0]
 [8 0 8 7 2]
 [9 5 9 7 7]
 [9 6 9 9 8]]

argmax,argmin testing

>>> a = array([
... [8, 6, 1, 3, 0],
... [0, 1, 8, 9, 1],
... [7, 4, 5, 4, 2],
... [5, 2, 6, 7, 1],
... [9, 8, 7, 8, 6]])

>>> a.argmax()
array([0, 3, 0, 3, 0])
>>> argmax(a)
array([0, 3, 0, 3, 0])
>>> a.argmax(0)
array([4, 4, 1, 1, 4])
>>> a.argmin()
array([4, 0, 4, 4, 4])
>>> a.argmin(0)
array([1, 1, 0, 0, 0])
>>> argmin(a, 0)
array([1, 1, 0, 0, 0])

------------------------------------------------------------------------------
Check diagonal & trace

>>> a = reshape(arange(100),(10,10))
>>> a.diagonal(0)
array([ 0, 11, 22, 33, 44, 55, 66, 77, 88, 99])
>>> a.diagonal(1)
array([ 1, 12, 23, 34, 45, 56, 67, 78, 89])
>>> diagonal(a,-1)
array([10, 21, 32, 43, 54, 65, 76, 87, 98])
>>> x = reshape(arange(25),(5,5))
>>> x.trace()
60
>>> x.trace(-1)
56
>>> trace(x, 1)
40
>>> arange(24, shape=(3,8)).diagonal(0)
array([ 0,  9, 18])
>>> arange(24, shape=(3,8)).diagonal(1)
array([ 1, 10, 19])
>>> arange(24, shape=(3,8)).diagonal(-1)
array([ 8, 17])
>>> arange(24, shape=(3,8)).diagonal(-2)
array([16])
>>> arange(24, shape=(3,8)).diagonal(-3)
array([], type=Long)
>>> arange(24, shape=(3,8)).trace(-3)
0

------------------------------------------------------------------------------
Check repeat & resize

>>> a=arange(4)
>>> repeat(a,a)
array([1, 2, 2, 3, 3, 3])
>>> repeat(a,a*0)
array([], type=Long)

>>> a = arange(10)
>>> y = resize(x, (2,2))
>>> y
array([[0, 1],
       [2, 3]])
>>> resize(array((0,1)), (3,3))
array([[0, 1, 0],
       [1, 0, 1],
       [0, 1, 0]])
>>> y = arange(1)
>>> _junk = y.resize(5)
>>> y
array([0, 0, 0, 0, 0])
>>> y[2] = 5
>>> y[2]
5

------------------------------------------------------------------------------
Check matrixmultiply

>>> a=array([[1,2,-1],[3,1,4]])
>>> b=array([[-2,5],[4,-3],[2,1]])
>>> matrixmultiply(a,b)
array([[ 4, -2],
       [ 6, 16]])
>>> matrixmultiply(a,identity(3))
array([[ 1,  2, -1],
       [ 3,  1,  4]])

>>> Error.setMode(all="warn")
>>> a = identity(1,Float64)*10.0**300
>>> x=dot(a,a)
Warning: Encountered overflow(s)  in dot

>>> a = identity(1,Float64)*10.0**-300
>>> x=dot(a,a)
Warning: Encountered underflow(s)  in dot

Check for memory leaks

>>> a = arange(25., shape=(5,5))
>>> b = a.copy()
>>> x, y = sys.getrefcount(a), sys.getrefcount(b)
>>> c = matrixmultiply(a,b)
>>> assert x == sys.getrefcount(a)
>>> assert y == sys.getrefcount(b)
>>> assert sys.getrefcount(c) == 2

------------------------------------------------------------------------------
Check indices

>>> dtp(indices((2,2)))
. array([[[0, 0],
.         [1, 1]],
.
.        [[0, 1],
.         [0, 1]]])

Check fromfunction

>>> fromfunction(lambda x,y: x**2+y**2, (3,3))
array([[0, 1, 4],
       [1, 2, 5],
       [4, 5, 8]])

Check c-interface buffer exception handling

>>> b = buffer(chr(0)*100)
>>> a=NumArray(buffer=b, shape=25, type=Int32)
>>> a[:] = 42
Traceback (most recent call last):
...
error: copy4bytes: Problem with output buffer (read only?)

>>> a += 42
Traceback (most recent call last):
...
error: add_iixi_vsxv: Problem with write buffer[2].

Check array buffer protocol support

>>> a = arange(10)
>>> b = NumArray(buffer=a, shape=10, type='Int32')
>>> b[0] = 999
>>> a
array([999,   1,   2,   3,   4,   5,   6,   7,   8,   9])


Check array super factory

# No buffer
>>> a=array(sequence=None, shape=(10,10), type=Int32)
>>> a.getshape()
(10, 10)
>>> int(a.type() is Int32)
1

# fromlist already tested.

# fromstring
>>> array("\1\2\3", shape=3, type=UInt8)
array([1, 2, 3], type=UInt8)

# Check properties for python versions >= (2,2,0)
>>> a = arange(4)
>>> print a.shape
(4,)
>>> a.shape = (2,2)
>>> print repr(a)
array([[0, 1],
       [2, 3]])
>>> print repr(a.flat)
array([0, 1, 2, 3])
>>> a.flat = [2, 3, 4, 5]
>>> print repr(a)
array([[2, 3],
       [4, 5]])
>>> c = arange(0+10j, 4+50j, 1+10j)
>>> print repr(c)
array([ 0.+10.j,  1.+20.j,  2.+30.j,  3.+40.j])
>>> c.real = [4, 3, 2, 1]
>>> print repr(c.real)
array([ 4.,  3.,  2.,  1.])
>>> c.imag = [10, 9, 8, 7]
>>> print repr(c.imag)
array([ 10.,   9.,   8.,   7.])
>>> c.imaginary = [15, 14, 13, 12]
>>> print repr(c.imaginary)
array([ 15.,  14.,  13.,  12.])
>>> print repr(c)
array([ 4.+15.j,  3.+14.j,  2.+13.j,  1.+12.j])

Bugfix nonzero typecast to Int8
>>> nonzero([0.1, 1.2, -1.3, 0])
(array([0, 1, 2]),)

Bugfix output array offset
>>> a=arange(5)
>>> a[1:3] += 1
>>> a
array([0, 2, 3, 3, 4])

Bugfix array of empty sequence
>>> array([], type=Float32)
array([], type=Float32)

NumPy Typecode compatability

>>> arange(10.0).type()
Float64
>>> int(arange(10.0).type() == "d")
1
>>> int(arange(10.0).type() != "d")
0
>>> int(arange(10.0).type() == "f")
0
>>> int(arange(10.0).type() != "f")
1
>>> arange(10, type=Int32).type()
Int32
>>> int(arange(10).type() == "l")
1
>>> int(arange(10).type() == "b")
0
>>> int(arange(10, type=Int16).type() == "s")
1
>>> int(arange(10, type=Int16).type() != "s")
0

Safety code ufunc offset check
>>> a = arange(10, type=Int32)
>>> a._byteoffset = 12
>>> a+a
Traceback (most recent call last):
...
error: add_iixi_vvxv: access out of buffer. niter=10 typesize=4 bsize=28

>>> a._byteoffset = -4
>>> a+a
Traceback (most recent call last):
...
error: add_iixi_vvxv: invalid negative offset:-4 for buffer[0]

# Safety code ufunc alignment check
#        can't be tested without _aligned hack.
# >>> a = arange(10, type=Int32)
# >>> b = _misaligned(a)
# >>> b+b
# Traceback (most recent call last):
# ...
# error: add_iixi_vvxv: buffer not aligned on 4 byte boundary.

Safety code striding offset check
>>> a = arange(10, type=Int32)
>>> a._byteoffset = 4
>>> add.reduce(a)
Traceback (most recent call last):
...
error: add_ixi_R: access beyond buffer. offset=43 buffersize=40

>>> a._byteoffset = -4
>>> add.reduce(a)
Traceback (most recent call last):
...
error: copy4bytes: access before buffer. offset=-4 buffersize=40

Safety code negative striding check

>>> a = arange(10, type=Int32)
>>> b=a[::-1]; explicit_type(b)
array([9, 8, 7, 6, 5, 4, 3, 2, 1, 0], type=Int32)
>>> b._byteoffset
36
>>> b._byteoffset = 32
>>> c = b+a
Traceback (most recent call last):
...
error: copy4bytes: access before buffer. offset=-4 buffersize=40

Bugfix dot(1D, 1D) --> scalar
>>> dot([1,2,3],[4,5,6])
32

Bugfix bad transpose of second argument to dot
>>> a = array([[1., 0., 0.], [0., 1., 0.], [0., 0., 1.]])
>>> b = array([[0.5, 0.8, 0.], [-0.8, 0.5, 0.], [0., 0., 1.]])
>>> dot(a, b)
array([[ 0.5,  0.8,  0. ],
       [-0.8,  0.5,  0. ],
       [ 0. ,  0. ,  1. ]])
>>> a
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
>>> b
array([[ 0.5,  0.8,  0. ],
       [-0.8,  0.5,  0. ],
       [ 0. ,  0. ,  1. ]])

Bugfix SF# [ 1386811 ] innerproduct output array stride error

>>> a = array([[1,2,3],[3,4,5]], type=Float64)
>>> innerproduct(a,a)
array([[ 14.,  26.],
       [ 26.,  50.]])

Bugfix SF# [ 1151892 ] Bug in matrixmultiply with zero size arrays

>>> A=zeros( (0,0), Float64)
>>> b=zeros( (0,), Float64)
>>> x=matrixmultiply(A,b)

Bugfixes SF# [ 1057584 ] Refcounting in innerproduct
a. rank-0 refcounting 
>>> innerproduct(array(1), array(2))
2
>>> a=array(1)
>>> innerproduct(a,a)
1
>>> sys.getrefcount(a)
2

b. refcounting for mismatch exception
>>> a=array([1,2])
>>> b=array([1,2,3])
>>> innerproduct(a,b)
Traceback (most recent call last):
...
ValueError: innerproduct: last sequence dimensions must match.
>>> raise ValueError("foo!")  # Todd's ingenious exc_clear() hack
Traceback (most recent call last):
...
ValueError: foo!
>>> sys.getrefcount(a), sys.getrefcount(b)
(2, 2)

Rank 0 numarray, i.e. empty-shape --> scalar
>>> zeros(())
array(0)

Bugfix:  Assignment of a really big Long to an array of ints should overflow
>>> a = arange(10.0)
>>> try:
...     a[0] = 111111111111111111111111111111111L
... except:
...     pass
... else:
...     raise "Overflow exception missing"

Putmask testing

>>> x = arange(5)
>>> putmask(x, [1, 0, 1, 0, 1], [10, 20, 30, 40, 50])
>>> print x
[10  1 30  3 50]
>>> putmask(x, [1, 0, 1, 0, 1], [-1, -2])
>>> print x
[-1  1 -1  3 -1]

Implementation of Complex32 and Complex64 using C-ufuncs

>>> a=arange(10)
>>> b=a.astype(Complex64)
>>> b
array([ 0.+0.j,  1.+0.j,  2.+0.j,  3.+0.j,  4.+0.j,  5.+0.j,  6.+0.j,
        7.+0.j,  8.+0.j,  9.+0.j])
>>> b += 10
>>> b
array([ 10.+0.j,  11.+0.j,  12.+0.j,  13.+0.j,  14.+0.j,  15.+0.j,
        16.+0.j,  17.+0.j,  18.+0.j,  19.+0.j])
>>> 10 + b
array([ 20.+0.j,  21.+0.j,  22.+0.j,  23.+0.j,  24.+0.j,  25.+0.j,
        26.+0.j,  27.+0.j,  28.+0.j,  29.+0.j])
>>> b + 10
array([ 20.+0.j,  21.+0.j,  22.+0.j,  23.+0.j,  24.+0.j,  25.+0.j,
        26.+0.j,  27.+0.j,  28.+0.j,  29.+0.j])
>>> b - 10j
array([ 10.-10.j,  11.-10.j,  12.-10.j,  13.-10.j,  14.-10.j,
        15.-10.j,  16.-10.j,  17.-10.j,  18.-10.j,  19.-10.j])
>>> b*b
array([ 100.+0.j,  121.+0.j,  144.+0.j,  169.+0.j,  196.+0.j,
        225.+0.j,  256.+0.j,  289.+0.j,  324.+0.j,  361.+0.j])
>>> b += 20j
>>> b
array([ 10.+20.j,  11.+20.j,  12.+20.j,  13.+20.j,  14.+20.j,
        15.+20.j,  16.+20.j,  17.+20.j,  18.+20.j,  19.+20.j])
>>> b*b
array([-300.+400.j, -279.+440.j, -256.+480.j, -231.+520.j, -204.+560.j,
       -175.+600.j, -144.+640.j, -111.+680.j,  -76.+720.j,  -39.+760.j])
>>> b.astype(Float64)
array([ 10.,  11.,  12.,  13.,  14.,  15.,  16.,  17.,  18.,  19.])
>>> b/b
array([ 1.+0.j,  1.+0.j,  1.+0.j,  1.+0.j,  1.+0.j,  1.+0.j,  1.+0.j,
        1.+0.j,  1.+0.j,  1.+0.j])
>>> c = b*b
>>> c
array([-300.+400.j, -279.+440.j, -256.+480.j, -231.+520.j, -204.+560.j,
       -175.+600.j, -144.+640.j, -111.+680.j,  -76.+720.j,  -39.+760.j])
>>> c/b
array([ 10.+20.j,  11.+20.j,  12.+20.j,  13.+20.j,  14.+20.j,
        15.+20.j,  16.+20.j,  17.+20.j,  18.+20.j,  19.+20.j])
>>> c.setshape(2,5)
>>> c
array([[-300.+400.j, -279.+440.j, -256.+480.j, -231.+520.j, -204.+560.j],
       [-175.+600.j, -144.+640.j, -111.+680.j,  -76.+720.j,  -39.+760.j]])
>>> b.astype(Bool)
array([1, 1, 1, 1, 1, 1, 1, 1, 1, 1], type=Bool)
>>> d=zeros(shape=(10,), type=Complex32)
>>> d
array([ 0.+0.j,  0.+0.j,  0.+0.j,  0.+0.j,  0.+0.j,  0.+0.j,  0.+0.j,
        0.+0.j,  0.+0.j,  0.+0.j], type=Complex32)
>>> d += 1j
>>> d
array([ 0.+1.j,  0.+1.j,  0.+1.j,  0.+1.j,  0.+1.j,  0.+1.j,  0.+1.j,
        0.+1.j,  0.+1.j,  0.+1.j], type=Complex32)
>>> d.astype(Bool)
array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0], type=Bool)
>>> minus([1+1j])
array([-1.-1.j])
>>> abs(arange(10)*(1+1j))
array([  0.        ,   1.41421356,   2.82842712,   4.24264069,
         5.65685425,   7.07106781,   8.48528137,   9.89949494,
        11.3137085 ,  12.72792206])
>>> remainder(arange(10)+1j,1.5+2j)
array([ 0. +1.j,  1. +1.j,  2. +1.j,  1.5-1.j,  2.5-1.j,  3.5-1.j,
        4.5-1.j,  4. -3.j,  5. -3.j,  6. -3.j])
>>> exp([1+1j])
array([ 1.46869394+2.28735529j])
>>> log([1+1j])
array([ 0.34657359+0.78539816j])
>>> log10([1+1j])
array([ 0.150515+0.34109409j])
>>> power([1+1j], [2+2j])
array([-0.265654+0.31981811j])
>>> sqrt([1+1j])
array([ 1.09868411+0.45508986j])
>>> sin([1+1j])
array([ 1.29845758+0.63496391j])
>>> cos([1+1j])
array([ 0.83373003-0.98889771j])
>>> tan([1+1j])
array([ 0.27175259+1.08392333j])
>>> sinh([1+1j])
array([ 0.63496391+1.29845758j])
>>> cosh([1+1j])
array([ 0.83373003+0.98889771j])
>>> tanh([1+1j])
array([ 1.08392333+0.27175259j])
>>> arcsin(arange(2)+(1+1j))
array([ 0.66623943+1.06127506j,  1.06344002+1.46935174j])
>>> arccos(arange(2)+(1+1j))
array([ 0.90455689-1.06127506j,  0.5073563 -1.46935174j])
>>> arctan(arange(2)+(1+1j))
array([ 1.01722197+0.40235948j,  1.17809725+0.1732868j ])
>>> arcsinh(arange(2)+(1+1j))
array([ 1.06127506+0.66623943j,  1.52857092+0.42707859j])
>>> arccosh(arange(2)+(1+1j))
array([ 1.06127506+0.90455689j,  1.46935174+0.5073563j ])
>>> arctanh(arange(2)+(1+1j))
array([ 0.40235948+1.01722197j,  0.40235948+1.33897252j])

>>> ceil(arange(0,2,0.5)+1j)
array([ 0.+1.j,  1.+1.j,  1.+1.j,  2.+1.j])
>>> floor(arange(0,2,0.5)+1j)
array([ 0.+1.j,  0.+1.j,  1.+1.j,  1.+1.j])
 
>>> logical_and([ 0+1j, 2+2j], [1+0j, 0])
array([1, 0], type=Bool)
>>> logical_or([ 0+1j, 2+2j], [1+0j, 0])
array([1, 1], type=Bool)
>>> logical_xor([ 0+1j, 2+2j], [1+0j, 0])
array([0, 1], type=Bool)
>>> logical_not([ 0+1j, 2+2j, 0])
array([0, 0, 1], type=Bool)

>>> minimum( [1+1j,2+2j,3+3j,4+4j],[1+0j,2+3j,4+4j,3+3j] )
array([ 1.+1.j,  2.+2.j,  3.+3.j,  3.+3.j])
>>> maximum( [1+1j,2+2j,3+3j,4+4j],[1+0j,2+3j,4+4j,3+3j] )
array([ 1.+1.j,  2.+2.j,  4.+4.j,  4.+4.j])

Test complex comparison operators.  Return value is a NumArray
while at least one input was a ComplexArray.

>>> equal(array([1+1j]), array([1+1j]))
array([1], type=Bool)
>>> equal(array([1+2j]), array([1+1j]))
array([0], type=Bool)
>>> not_equal(array([1+2j]), array([1+1j]))
array([1], type=Bool)
>>> not_equal(array([1+1j]), array([1+1j]))
array([0], type=Bool)
>>> arange(5) > 3+3j
array([0, 0, 0, 0, 1], type=Bool)
>>> arange(5j) > 3+3j
Traceback (most recent call last):
...
TypeError: Complex NumArrays don't support >, >=, <, <= operators
>>> arange(5j) >= 3+3j
Traceback (most recent call last):
...
TypeError: Complex NumArrays don't support >, >=, <, <= operators
>>> arange(5j) < 3+3j
Traceback (most recent call last):
...
TypeError: Complex NumArrays don't support >, >=, <, <= operators
>>> arange(5j) <= 3+3j
Traceback (most recent call last):
...
TypeError: Complex NumArrays don't support >, >=, <, <= operators



Test complex sort, argsort, searchsorted

>>> a=arange(4,0,-1) * [1+0.5j]
>>> sort(a)
array([ 1.+0.5j,  2.+1.j ,  3.+1.5j,  4.+2.j ])
>>> argsort(a)
array([3, 2, 1, 0])
>>> bins = arange(0,1.0,0.1, type=Complex32) * [1+1j]
>>> data = array([0.3+2j, 0.795, 0.8+2j, 0.780, 0.551, 0.760, 0.285], type=Complex32)
>>> searchsorted(bins, data)
array([3, 8, 8, 8, 6, 8, 3])

Test complex byte swapping

input converters

>>> a=arange(4, type=Int32) + array([1+1j])
>>> a.byteswap()
>>> a.togglebyteorder()
>>> a + a
array([ 2.+2.j,  4.+2.j,  6.+2.j,  8.+2.j])

>>> a=arange(4, type=Int32) + array([1+1j])
>>> a.byteswap()
>>> a.togglebyteorder()
>>> a + a
array([ 2.+2.j,  4.+2.j,  6.+2.j,  8.+2.j])

output converters

>>> b=arange(4, type=Int32) + array([1+1j])
>>> a=zeros((4,), type=Complex)
>>> a.togglebyteorder()
>>> int(a.isbyteswapped())
1
>>> add(b,b,a)
>>> a
array([ 2.+2.j,  4.+2.j,  6.+2.j,  8.+2.j])

>>> b=arange(4, type=Int32) + array([1+1j])
>>> a=zeros((4,), type=Complex)
>>> a.togglebyteorder()
>>> add(b,b,a)
>>> a
array([ 2.+2.j,  4.+2.j,  6.+2.j,  8.+2.j])
>>> comparr(conjugate(arange(4)+1j),
...         array([ 0.-1.j,  1.-1.j,  2.-1.j,  3.-1.j]),
...         eps)
>>> comparr(conjugate(arange(4)),
...         array([ 0.+0.j,  1.+0.j,  2.+0.j,  3.+0.j]),
...         eps)
>>> fabs(arange(-2, 2, type=Int32))
array([ 2.,  1.,  0.,  1.])
>>> fabs(arange(-2, 2, type=Int32)+1j)
array([ 2.+1.j,  1.+1.j,  0.+1.j,  1.+1.j])

>>> negative(arange(-2,2))
array([ 2,  1,  0, -1])
>>> comparr(negative(arange(-2,2)+1.j), 
...	array([ 2.-1.j,  1.-1.j, -0.-1.j, -1.-1.j]), 
...	eps)
>>> comparr(around(arange(-2,2,0.4)), 
...         array([-2., -2., -1., -1., -0., -0.,  0.,  1.,  1.,  2.]),
...         eps)
>>> comparr(around(arange(-2,2,0.4)+1.j),
...         array([-2.+1.j, -2.+1.j, -1.+1.j, -1.+1.j, -0.+1.j,
...            -0.+1.j,  0.+1.j,  1.+1.j,  1.+1.j,  2.+1.j]),
...         eps)
>>> comparr(hypot(arange(3),arange(3)),
...         array([ 0.        ,  1.41421354,  2.82842708], type=Float32),
...         eps)
>>> comparr( hypot(arange(3)+1j,arange(3)+1j),
...          array([ 8.65927457e-17+1.41421356j, 1.41421356e+00+1.41421356j,
...                   2.82842712e+00+1.41421356j]),
...          eps)
>>> rank([[0,1],[2,3],[4,5]])
2
>>> shape([[0,1],[2,3],[4,5]])
(3, 2)
>>> size([[0,1],[2,3],[4,5]])
6
>>> print array_repr([[0,1],[2,3],[4,5]])
array([[0, 1],
       [2, 3],
       [4, 5]])
>>> print array_str([[0,1],[2,3],[4,5]])
[[0 1]
 [2 3]
 [4 5]]

>>> array(1)
array(1)
>>> generic._broadcast(array(1), (2,))
array([1, 1])
>>> a = array([1,2])
>>> b = array(a)
>>> int(a is b)             
0
>>> b = array(a, copy=0)
>>> int(a is b)             
1

Test numeric pickling support

>>> import cPickle, cStringIO
>>> cPickle.loads(cPickle.dumps(arange(5)+1j))
array([ 0.+1.j,  1.+1.j,  2.+1.j,  3.+1.j,  4.+1.j])
>>> a = arange(5)+1j
>>> b = a[1:3]
>>> f = cStringIO.StringIO()
>>> p = cPickle.Pickler(f)
>>> p = p.dump(a)
>>> p = p.dump(b)
>>> del p
>>> f.seek(0)
>>> up = cPickle.Unpickler(f)
>>> c = up.load()
>>> d = up.load()
>>> int(c._data is d._data)              # was view data represented only once?
1
>>> int(logical_and.reduce(a.__eq__(c)))
1
>>> int(logical_and.reduce(b.__eq__(d)))
1

>>> array([])
array([], type=Long)
>>> array(())
array([], type=Long)

Exotic type coercions

>>> a = arange(5, type=Int8)
>>> b = arange(5, type=UInt16)
>>> (a+b).type()
Int32
>>> (b+a).type()
Int32

>>> a = arange(5, type=Int16)
>>> b = arange(5, type=UInt32)
>>> (a+b).type()
Int64
>>> (b+a).type()
Int64

>>> a = arange(5, type=Float64)
>>> b = arange(5, type=Complex32)
>>> (a+b).type()
Complex64
>>> (b+a).type()
Complex64

# Type equality checks

>>> int(Int32 == 'Int32')
1
>>> int(Int32 == 'foo')
0
>>> int(IsType(Int32))
1
>>> int(IsType('Int32'))
1
>>> int(IsType('bar'))
0

array_equal() tests array both contents *and* shape for equality

>>> int(array_equal(arange(2), arange(4)))
0
>>> int(array_equal(array([1,2,3]), array([1,2,0])))
0
>>> int(array_equal( arange(2), None ))
0
>>> int(array_equal( arange(2), "foobar"))
0
>>> int(array_equal(array([[1,2,3],[1,2,3]]), array([1,2,3])))
0

array_equiv() is similar to array_equal, but broadcasts if necessary

>>> int(array_equiv(array([[1,2,3],[1,2,3]]), array([1,2,3])))
1

.real property for non-complex numarray.

>>> a=arange(10)
>>> a.real
array([ 0.,  1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.])
>>> a.real = 45
Traceback (most recent call last):
...
TypeError: Can't setreal() on a non-floating-point array
>>> a=arange(10.)
>>> a.real = 45
>>> a
array([ 45.,  45.,  45.,  45.,  45.,  45.,  45.,  45.,  45.,  45.])

From bug #709956 (Error summing over large boolean numarray)
>>> sum(arange(200000) == 101000)
1

From bug #690640 (irregular list bug)
>>> array([3, [4, 5]])
Traceback (most recent call last):
...
ValueError: Nested sequences with different lengths.
>>> array([[1,2,3], [4, 5]])
Traceback (most recent call last):
...
ValueError: Nested sequences with different lengths.

Fortran version of contiguity method

>>> a = arange(4, shape=(2,2))
>>> a.iscontiguous()
1
>>> a.is_fortran_contiguous()
0
>>> int(a.is_f_array())
0
>>> a.transpose()
>>> a.iscontiguous()
0
>>> a.is_fortran_contiguous()
1
>>> a.is_c_array()
0
>>> int(a.is_f_array())
1

NDArray.tofile() fromfile() tests

>>> import os, tempfile; name = tempfile.mktemp()
>>> a = arange(100.); a.tofile(name); b = fromfile(name, type=Float64);
>>> logical_and.reduce(a == b)
1
>>> os.remove(name)

Logical reduction on non-boolean

>>> logical_and.reduce(arange(10))
0
>>> logical_and.reduce(arange(1,10))
1

# [ 733534 ] Broadcasts of non-array sequences fails
>>> a=arange(10,11);  b=[1,2];  a+b
array([11, 12])

# Hook for conversion of arbitrary objects to numerical scalars

>>> class friendly:
...    def __tonumtype__(self, type): print type; return -1
>>> class nasty:
...    def __tonumtype__(self, type): print type; return nasty()
>>> a = arange(5, type=Int32)
>>> a[1] = friendly(); explicit_type(a)
Int32
array([ 0, -1,  2,  3,  4], type=Int32)
>>> a[2] = nasty()
Traceback (most recent call last):
...
RuntimeError: NA_setFromPythonScalar: __tonumtype__ conversion chain too long

# overflow checking for array assignment and fromlist

>>> a=arange(1, type=Bool)    
>>> a[0] = -129
>>> a[0] = 128
>>> a._check_overflow=1  # By design, Bool never raises, even when checking
>>> a[0] = -129
>>> a[0] = 128

>>> a=arange(1, type=Int8)
>>> a[0] = -129;
>>> a[0] = 128
>>> a._check_overflow=1
>>> a[0] = -129
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 128
Traceback (most recent call last):
...
OverflowError: value out of range for array

>>> a=arange(1, type=Int16)
>>> a[0] = -32769;
>>> a[0] = 32768
>>> a._check_overflow=1
>>> a[0] = -32769
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = -32768
>>> a[0] = 32768
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 32767

>>> a=arange(1, type=UInt16)
>>> a[0] = -1;
>>> a[0] = 65536
>>> a._check_overflow=1
>>> a[0] = -1
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 0
>>> a[0] = 65536
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 65535

>>> a=arange(1, type=Int32)
>>> a[0] = -2147483649.
>>> a[0] =  2147483648.
>>> a._check_overflow=1
>>> a[0] = -2147483648.
>>> a[0] = -2147483649.
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 2147483647.
>>> a[0] = 2147483648.
Traceback (most recent call last):
...
OverflowError: value out of range for array

>>> a=arange(1, type=UInt32)
>>> a[0] = -1;
>>> a[0] = 4294967296.
>>> a._check_overflow=1
>>> a[0] = 0
>>> a[0] = -1
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 4294967295.
>>> a[0] = 4294967296.
Traceback (most recent call last):
...
OverflowError: value out of range for array

>>> a=arange(1, type=Int64)
>>> a[0] = -9223372036854775809.
>>> a[0] =  9223372036854775808.
>>> a._check_overflow=1
>>> a[0] = -9223372036854775808.
>>> a[0] = -9223372036854778809.
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 9223372036854775807.
>>> a[0] = 9223372036854778808. # delta needs to be ~2-3000 due to 53-bit rep
Traceback (most recent call last):
...
OverflowError: value out of range for array

# Since UInt64 doesn't work under Win32, commented out
# >>> a=arange(1, type=UInt64)
# >>> a[0] = -1;
# >>> a[0] = 18446744073709551616.
# >>> a._check_overflow=1
# >>> a[0] = 0
# >>> a[0] = -1
# Traceback (most recent call last):
# ...
# OverflowError: value out of range for array
# >>> a[0] = 18446744073709551615.
# >>> a[0] = 18446744073709551616
# Traceback (most recent call last):
# ...
# OverflowError: long too big to convert

>>> a=arange(1, type=Float32)
>>> a[0] = -1.e38
>>> a[0] = -1.e40
>>> a._check_overflow=1
>>> a[0] = -1.e38
>>> a[0] = -1.e40
Traceback (most recent call last):
...
OverflowError: value out of range for array
>>> a[0] = 1.e38
>>> a[0] = 1.e40
Traceback (most recent call last):
...
OverflowError: value out of range for array

# Fromlist overflow checking

>>> fromlist([1, -2, 3000, 4.6], type=Int8)
array([  1,  -2, -72,   4], type=Int8)
>>> fromlist([1, -2, 3000, 4.6], type=Int8, check_overflow=1)
Traceback (most recent call last):
...
OverflowError: value out of range for array

# Scalar multiplication overflow

>>> a=arange(5, type=Int8) * 32
Warning: Encountered overflow(s)  in multiply
>>> a=arange(5, type=Int16) * 8192
Warning: Encountered overflow(s)  in multiply
>>> a=arange(5, type=Int32) * 2**29
Warning: Encountered overflow(s)  in multiply

# >>> a=arange(5, type=Int64) * 2**61
# Warning: Encountered invalid numeric result(s)  in multiply
# Warning: Encountered overflow(s)  in multiply

>>> a=arange(5, type=UInt8) * 64
Warning: Encountered overflow(s)  in multiply
>>> a=arange(5, type=UInt16) * 16384
Warning: Encountered overflow(s)  in multiply

# >>> a=arange(5, type=UInt32) * 2**30
# Warning: Encountered invalid numeric result(s)  in multiply
# Warning: Encountered overflow(s)  in multiply

# Commented out since unsupported on win32
# >>> a=arange(5, type=UInt64) * 2**62
# Warning: Encountered invalid numeric result(s)  in multiply
# Warning: Encountered overflow(s)  in multiply

>>> (arange(5, type='Complex32')+arange(5, type='Int64')).type()
Complex64
>>> (arange(5, type='Int64')+arange(5, type='Complex32')).type()
Complex64

# >>> (arange(5, type='Complex32')+arange(5, type='UInt64')).type()
# Complex64
# >>> (arange(5, type='UInt64')+arange(5, type='Complex32')).type()
# Complex64


Higher precision reductions and accumulations

>>> a = arange(100, shape=(10,10), type='Int8')
>>> add.areduce(a, type='Int16')
array([450, 460, 470, 480, 490, 500, 510, 520, 530, 540], type=Int16)
>>> add.reduce(a, type='Int16')
array([450, 460, 470, 480, 490, 500, 510, 520, 530, 540], type=Int16)
>>> add.accumulate(a, type='Int16')
array([[  0,   1,   2,   3,   4,   5,   6,   7,   8,   9],
       [ 10,  12,  14,  16,  18,  20,  22,  24,  26,  28],
       [ 30,  33,  36,  39,  42,  45,  48,  51,  54,  57],
       [ 60,  64,  68,  72,  76,  80,  84,  88,  92,  96],
       [100, 105, 110, 115, 120, 125, 130, 135, 140, 145],
       [150, 156, 162, 168, 174, 180, 186, 192, 198, 204],
       [210, 217, 224, 231, 238, 245, 252, 259, 266, 273],
       [280, 288, 296, 304, 312, 320, 328, 336, 344, 352],
       [360, 369, 378, 387, 396, 405, 414, 423, 432, 441],
       [450, 460, 470, 480, 490, 500, 510, 520, 530, 540]], type=Int16)
       
>>> a.mean()
49.5

>>> int(a.sum())
4950

Tests for buffer write protection exceptions

>>> import numarray.libnumarray as libnumarray
>>> a = NumArray(buffer="\\0"*10**4, shape=(2500,), type='i')
>>> a[0] = 0
Traceback (most recent call last):
...
ValueError: NA_setFromPythonScalar: assigment to readonly array buffer
>>> a[0:1000] = 0
Traceback (most recent call last):
...
error: copy4bytes: Problem with output buffer (read only?)

SF#[ 824840 ] add.reduce return array with wrong shape

>>> a = arange(36, shape=(3,3,2,2))
>>> add.reduce(a,0).shape
(3, 2, 2)

>>> import copy, numarray as na
>>> a = na.arange(3)
>>> b = copy.copy(a)
>>> c = copy.deepcopy(a)
>>> a[0] = 99
>>> a, b, c
(array([99,  1,  2]), array([0, 1, 2]), array([0, 1, 2]))

SF#[ 894788 ] A bug in repeat function?

>>> a=arange(1000)
>>> b=ones(1000,type=Bool)
>>> c=repeat(a,b)              # used to raise exception

SF# [ 683957 ] Adding certain arrays fails in Numarray

>>> a = zeros((0,10))
>>> add.reduce(a)
array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
>>> add.accumulate(a)
array([], shape=(0, 10), type=Long)
>>> a+a
array([], shape=(0, 10), type=Long)
>>> cos(a)
array([], shape=(0, 10), type=Float)

SF# 927737  Modify integer promotions to Float64
>>> int(cos(1) == cos(1.0))
1

SF# 955314 strings.num2char bug?
>>> a = array([], shape=(0,32))
>>> a[:] = a

SF# 979123 A[n:n] = x no longer works

>>> a = arange(10)
>>> a[4:4] = 1
>>> a
array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
>>> a[4:4]
array([], type=Long)

SF # [ 979712 ] No exception for invalid axis

>>> na.sum( na.zeros((2,6)) , 2)
Traceback (most recent call last):
...
ValueError: Specified dimension does not exist
>>> na.sum( na.zeros((2,6)) , -3)
Traceback (most recent call last):
...
ValueError: Specified dimension does not exist

SF # [ 979702 ]	too many slices fails silently

>>> arange(10)[:,:]
Traceback (most recent call last):
...
IndexError: too many slices for array shape

>>> a = arange(10)
>>> b = array(a, copy=0)
>>> a is b
1
>>> b = array(a, copy=1)
>>> a is b
0
>>> b = array(a, 'l', copy=0)
>>> a is b
1
>>> b = array(a, 'f', copy=0)
>>> a is b
0
>>> b = array(a, 'i', copy=1)
>>> a is b
0
>>> b = array(a, 'f', copy=1)
>>> a is b
0

SF# [ 995287 ] float * array fails?

>>> array(0.0)+4.0*array(1.0)
array(4.0)

SF# [ 980750 ] argmax fails on on slices of larger arrays

>>> a = array([[0,1,2], [3,2,1]])
>>> argmax(a[0]) # OK
2
>>> argmax(a[1]) # Should be 0
0
>>> argmin(a[1])
2
>>> argmin(array(a[1]))
2

SF# [ 1008462 ] searchsorted bug and fix

>>> a = ones(100)
>>> searchsorted(a, 1)
0

SF#

>>> print kroneckerproduct([[1,2]],[[3],[4]])
[[3 6]
 [4 8]]
>>> print kroneckerproduct([[1,2]],[[3,4]])
[[3 4 6 8]]
>>> print kroneckerproduct([[1],[2]],[[3],[4]])
[[3]
 [4]
 [6]
 [8]]


SF # [ 1099974 ] .sum() gives wrong results

Ensure no over or under counts occur at both "left over" and "no
leftover" slow mode ufunc blocking parameter cases.

>>> assert uf._bufferPool.getBufferSize() == 100000L
>>> for i in range(-10,10):
...    a = ones(100000L - i)
...    b = a.sum()   # default integer sum promotes to Int64 => slow mode
...    assert b == (100000L - i)

>>> for i in range(-10,10):
...    a = ones(100000L - i)
...    b = add(a,a.astype('Int64'))   # force add type conversion => slow mode
...    assert all(b == 2)

Test for zero-length reduction to identity value

>>> a = ones(0)
>>> int(a.sum())
0

Test rank-0 reduction using "fast and slow mode" cases.

>>> a = array(1, type='Int16')
>>> add.reduce(a, type='Long')
1
>>> add.reduce(a)
1
>>> add.accumulate(a)
array(1, type=Int16)
>>> multiply.reduce(a)
1

Several "slow mode" sums with various shapes and blocking parameterizations.
Assume blocksize == 100K bytes.

>>> int(ones((1000,100,1)).sum())
100000
>>> int(ones((10,1000,100,1)).sum())
1000000
>>> int(ones((1000,100,1,10)).sum())
1000000
>>> int(ones((100000,10)).sum())
1000000
>>> int(ones((10,100000)).sum())
1000000

# Large array check for blocking

>>> a = arange(1001**2, shape=(1001,1001))
>>> add.reduce(a)
array([501000500, 501001501, 501002502, ..., 501999498, 502000499,
       502001500])
>>> a.byteswap()
>>> a.togglebyteorder()
>>> add.reduce(a)
array([501000500, 501001501, 501002502, ..., 501999498, 502000499,
       502001500])

# Test any() and all()

SciPy introduced the functions any() and all() which are critical for
cleanly and correctly evaluating a number of different kinds of
expressions.  I thought Numeric's sometrue() and alltrue() already
had this covered,  but it turns out that they only do a reduction
along a single axis, thus they return an array result for rank>1
reductions.

Unlike Numeric which interprets A.__nonzero__() as any(A), numarray's
A.__nonzero__() raises an exception.  numarray does this because
whether nonzero should mean any() or all() depends on the usage, so
neither is correct in all cases and therefore doing either is unsafe.

>>> a = arange(100, shape=(10,10))
>>> if a:          # Doesn't work because a is an array.
...    print "foo!"
Traceback (most recent call last):
...
RuntimeError: An array doesn't make sense as a truth value.  Use any(a) or all(a).

>>> if a < 0:      # Doesn't work because a<0 is also an array.
...    print "foo!"
Traceback (most recent call last):
...
RuntimeError: An array doesn't make sense as a truth value.  Use any(a) or all(a).

>>> if not all(a > 0): # Works w/o exeception because expr is a scalar.
...    print "bar!"
bar!

>>> if any(a > 0):
...    print "foo!"
foo!

>>> a = zeros((10,10))
>>> if not any(a):
...    print "bar!"
bar!

>>> a = ones((10,10))
>>> if all(a):
...    print "foo!"
foo!


SF# [ 1001547 ] anarray[[out_of_range]] error message obscure

>>> a = arange(10)
>>> a[[20]]
Traceback (most recent call last):
...
IndexError: Index out of range

 
SF# [ 1008373 ] Boolean array weirdness

>>> b = ones(shape=2,type=Bool)
>>> (b & False).astype('Bool')   # astype() required for Python-2.2
array([0, 0], type=Bool)

SF # [ 1014104 ] iscontiguous can return wrong answer

>>> ones(shape=(2,0)).iscontiguous()
1
>>> ones(shape=(2,0)).flat
array([], type=Long)

SF # [ 1045518 ] A disconnected numarray rant

>>> ones((-5,))
Traceback (innermost last):
...
ValueError: invalid shape tuple

numarray-1.2.2 Numeric interoperability failure

>>> try:
...    import Numeric
... except ImportError:
...    pass
... else:
...    a = arange(5)
...    b = Numeric.arange(5)
...    c = a + b


SF # [ 1179355 ] average() broken in numarray 1.2.3
>>> average([1.,2,3])
2.0

SF# [ 1363723 ] my_array = +my_other_array uncovers a bug
>>> +arange(5)
array([0, 1, 2, 3, 4])

>>> arange(5).rank
1
>>> arange(25, shape=(5,5)).rank
2
>>> arange(5).ndim
1
>>> arange(25, shape=(5,5)).ndim
2

"""
from numarrayall import *
import sys, string, cPickle, doctest

def dummy(): pass

if sys.version_info < (2,4):
    Tester=doctest.Tester
else:
    class Tester:
        def __init__(self, globs, isprivate=None):
            self._runner = doctest.DocTestRunner()
            self._globs = globs
            
        def rundoc(self, object, id_str=None):
            self._finder = doctest.DocTestFinder(False)
            for t in self._finder.find(object, globs=self._globs, name=id_str):
                self._runner.run(t)
                
        def summarize(self):
            return self._runner.summarize()
        
def test():
    import numtest
    t1 = Tester(globs=globals())
    t2 = Tester(globs=globals())
    t1.rundoc(numtest, "first pass")
    r1 = t1.summarize()
    t2.rundoc(numtest, "cache pass")
    r2 = t2.summarize()
    return r1, r2

def dtp(array, prefix=". "):
    """dtp == DocTestPrint

    Prepends a leading ". " to each line of repr(self) so that blank
    lines in the repr will work correctly with DocTest.  Since dtp(x)
    has no quoted characters, it's easy to insert into a DocTest.

    """
    s = prefix + repr(array).replace("\n","\n"+prefix)
    lines = map(string.strip, string.split(s,"\n"))
    print string.join(lines,"\n")

def comparr(arr1, arr2, epsilon):
    """Check to see that all elements of arr1 and arr2 are within epsilon.

    This is to eliminate platform dependencies of float calculations.
    We are only checking to see if the values are close, and not checking
    if they are of expected precision. Assumes 1-d numarray for now.

    """
    adiff = abs(arr1-arr2)
    mask = where(greater(adiff, epsilon), 1, 0)
    if add.reduce(mask) >  0:
        print add.reduce(mask), \
            " elements of comparison differ by more than ", epsilon

def ratioarr(arr1, arr2, epsilon):
    """Check to see that all elements of arr1 and arr2 ratios within epsilon of 1.

    This is to eliminate platform dependencies of float calculations.
    We are only checking to see if the values are close, and not checking
    if they are of expected precision. Assumes 1-d numarray for now.

    """
    aratio = abs(arr1/arr2  - 1)
    mask = where(greater(aratio, epsilon), 1, 0)
    if add.reduce(mask) >  0:
        print add.reduce(mask), \
            " elements of comparison differ by more than ", epsilon

def _misaligned(a):
    b = arange(a.nelements()+1, type=a.type())
    b._shape = a._shape
    b._strides = a._strides
    b._byteoffset = 1
    b._copyFrom(a)
    return b

if __name__ == "__main__":
    test()
    sys.exit(0)
