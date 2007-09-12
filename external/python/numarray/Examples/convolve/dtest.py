"""Unconverted data, unconverted output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10.), num.zeros((10,), type=Float64))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

Converted data, unconverted output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10), num.zeros((10,), type=Long))
array([ 0,  4,  8, 12, 16, 20, 24, 28, 32,  9])

Unconverted data, converted output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10.), num.zeros((10,), type=Long))
array([ 0,  4,  8, 12, 16, 20, 24, 28, 32,  9])

Converted data, converted output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10.), num.zeros((10,), type=Long))
array([ 0,  4,  8, 12, 16, 20, 24, 28, 32,  9])

Unonverted data, no output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10.))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

Converted data, no output.

>>> conv1(num.array([1.,2.,1.]), num.arange(10))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

List data, no output.

>>> conv1([1.,2.,1.], num.arange(10))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

Byteswapped data, no output.

>>> a = num.array([1.,2.,1.])
>>> a.byteswap()
>>> a.togglebyteorder()
>>> conv1(a, range(10))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

Misaligned data, no output.

>>> a = num.array([1.,2.,1.])
>>> a._aligned = 1
>>> conv1(a, range(10))
array([  0.,   4.,   8.,  12.,  16.,  20.,  24.,  28.,  32.,   9.])

2d convolve 10x10 by 5,5.

>>> x = num.zeros((10,10), num.Float64) + 1
>>> k = num.zeros((5,5), num.Float64) + 0.25
>>> k[1:4,1:4] = 0.5
>>> k[2,2] = 1
>>> z = num.zeros((10,10), num.Float64)
>>> conv2(k, x, z)
array([[ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.],
       [ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  9.,  9.,  9.,  9.,  9.,  9.,  1.,  1.],
       [ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.],
       [ 1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.,  1.]])


"""

VARIANTS = ["numarray.examples.convolve.high_level",
            "numarray.examples.convolve.one_dimensional",
            "numarray.examples.convolve.element_wise",
            "numarray.examples.convolve.numpy_compat"]

import numarray as num
import sys

array = num.array
Float64 = num.Float64
Int32 = num.Int32
Long = num.Long

# some helper functions used in the tests
def dotest(f, k, x, z=None):
    kref = sys.getrefcount(k)
    xref = sys.getrefcount(x)
    zref = sys.getrefcount(z)
    if z is None:
        c = f(k, x)
    else:
        c = f(k, x, z)
    assert(sys.getrefcount(k) == kref)
    assert(sys.getrefcount(x) == xref)
    if z is None:
        assert(sys.getrefcount(c) == 2)
    else:
        assert(sys.getrefcount(z) == zref)
    if c is not None:
        return c
    else:
        return z

def conv1(k, x, z=None):
    return dotest(convolve.Convolve1d, k, x, z)
    
def conv2(k, x, z=None):
    return dotest(convolve.Convolve2d, k, x, z)

def test():
    import numarray.numtest as nt
    import dtest

    results = []
    for v in VARIANTS:
        exec("import " + v + " as convolve\n", globals())
        t = nt.Tester(globs=globals())
        t.rundoc(dtest, v)
        results.append(t.summarize())
    return tuple(results)

