r"""

Import modules

>>> import numpy as N
>>> import num_test_ext as nt
>>> import sys

Setup

>>> fred = N.array([[1,2,3],[4,5,6]], N.dtype(N.float64).char)

nt.refcount() gives 1 more than sys.getrefcount(), which makes sense because its a wrapped method

>>> nt.refcount(fred)
3
>>> sys.getrefcount(fred)
2

Copy Constructor

>>> joe = nt.test_copy_construct(fred)
>>> nt.refcount(fred)
4
>>> nt.refcount(joe)
4
>>> sys.getrefcount(fred)
3
>>> sys.getrefcount(joe)
3
>>> fred[0][0] = 99
>>> joe
array([[ 99.,   2.,   3.],
       [  4.,   5.,   6.]])
>>> fred
array([[ 99.,   2.,   3.],
       [  4.,   5.,   6.]])

After an array is printed, its refcount is 1 too high until you count it once (either way)

>>> sys.getrefcount(fred)
4
>>> sys.getrefcount(fred)
3

1d array from pointer constructor

>>> foo = N.array([1,2,3,4,5], N.float32)
>>> bar = nt.test_1d_float_construct(foo)
>>> bar
array([ 1.,  2.,  3.,  4.,  5.], dtype=float32)
>>> bar[0] = 99
>>> foo
array([ 1.,  2.,  3.,  4.,  5.], dtype=float32)
>>> bar
array([ 99.,   2.,   3.,   4.,   5.], dtype=float32)
>>> sys.getrefcount(foo)
2
>>> sys.getrefcount(bar)
2

nd array from pointer constructor

>>> fred = N.array([[1,2,3],[4,5,6]], N.dtype(N.float64).char)
>>> bar = nt.test_nd_double_construct(fred)
>>> bar
array([[ 1.,  2.,  3.],
       [ 4.,  5.,  6.]])
>>> fred[1][1] = 77
>>> fred
array([[  1.,   2.,   3.],
       [  4.,  77.,   6.]])
>>> bar
array([[ 1.,  2.,  3.],
       [ 4.,  5.,  6.]])
>>> sys.getrefcount(foo)
2
>>> sys.getrefcount(bar)
2

Integer + Type constructor

>>> joe = nt.test_int_type_construct(5, N.dtype(N.float64).char)
>>> joe
array([ 0.,  0.,  0.,  0.,  0.])

Dimensions + Type constructor

>>> joe = nt.test_dims_type_construct((2,2,3), N.dtype(N.int32).char)
>>> joe
array([[[0, 0, 0],
        [0, 0, 0]],
<BLANKLINE>
       [[0, 0, 0],
        [0, 0, 0]]])
  
Clone method
>>> fred[1,0] = 33
>>> joe = 1
>>> joe = nt.test_clone(fred)
>>> sys.getrefcount(fred)
2
>>> sys.getrefcount(joe)
2
>>> joe
array([[  1.,   2.,   3.],
       [ 33.,  77.,   6.]])
>>> joe[1][1] = 55
>>> joe
array([[  1.,   2.,   3.],
       [ 33.,  55.,   6.]])
>>> fred
array([[  1.,   2.,   3.],
       [ 33.,  77.,   6.]])
>>> fred[0][0] = 99
>>> fred
array([[ 99.,   2.,   3.],
       [ 33.,  77.,   6.]])
>>> joe
array([[  1.,   2.,   3.],
       [ 33.,  55.,   6.]])

Astype method

>>> joe = nt.test_astype(fred, N.dtype(N.int32).char)
>>> sys.getrefcount(fred)
2
>>> sys.getrefcount(joe)
2
>>> fred[0][0] = 11
>>> fred
array([[ 11.,   2.,   3.],
       [ 33.,  77.,   6.]])
>>> joe
array([[99,  2,  3],
       [33, 77,  6]])
>>> joe[0][0] = 33
>>> fred
array([[ 11.,   2.,   3.],
       [ 33.,  77.,   6.]])
>>> joe
array([[33,  2,  3],
       [33, 77,  6]])

Spacesaver, type, rank, size, dims, iscontiguous, refcount methods
The refcount is +2 because it's using a wrapped method

>>> foo = N.array([[[1,2],[2,3]], [[3,4],[4,5]]], N.dtype(N.float64).char)
>>> bar = N.array([[1,2,3],[4,5,6]], N.dtype(N.int32).char)
>>> nt.test_stats(foo)
['type', 'PyArray_DOUBLE', 'rank', 3, 'size', 8, 'refcount', 4, 'dimension sizes', 1, 2, 2, 2, 3, 2, 'contiguous', 1, 'strides', 1, 8, 2, 16, 3, 32]
>>> nt.test_stats(bar)
['type', 'PyArray_LONG', 'rank', 2, 'size', 6, 'refcount', 4, 'dimension sizes', 1, 3, 2, 2, 'contiguous', 1, 'strides', 1, 4, 2, 12]
>>> x = N.array([[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]], N.dtype(N.int32).char)
>>> y = x[:,1:2]
>>> y
array([[ 2],
       [ 6],
       [10],
       [14]])
>>> nt.test_stats(y)
['type', 'PyArray_LONG', 'rank', 2, 'size', 4, 'refcount', 5, 'dimension sizes', 1, 1, 2, 4, 'contiguous', 0, 'strides', 1, 4, 2, 16]


>>> nt.test_stats(foo)
['type', 'PyArray_DOUBLE', 'rank', 3, 'size', 8, 'refcount', 4, 'dimension sizes', 1, 2, 2, 2, 3, 2, 'contiguous', 1, 'strides', 1, 8, 2, 16, 3, 32]

Check_rank, check_size, check_contiguous, check_dims, check_type methods

>>> bar
array([[1, 2, 3],
       [4, 5, 6]])
>>> nt.test_checks(bar, (2,3), 2, 6, N.dtype(N.int32).char)
>>> nt.test_checks(bar, (1,2), 2, 6, N.dtype(N.int32).char)
Traceback (most recent call last):
  File "<stdin>", line 1, in ?
RuntimeError: expected dimensions (1, 2), found dimensions (2, 3)
>>> nt.test_checks(bar, (2,3), 3, 6, N.dtype(N.int32).char)
Traceback (most recent call last):
  File "<stdin>", line 1, in ?
RuntimeError: expected rank 3, found rank 2
>>> nt.test_checks(bar, (2,3), 2, 0, N.dtype(N.int32).char)
Traceback (most recent call last):
  File "<stdin>", line 1, in ?
RuntimeError: expected size 0, found size 6
>>> nt.test_checks(bar, (2,3), 2, 6, N.dtype(N.float64).char)
Traceback (most recent call last):
  File "<stdin>", line 1, in ?
TypeError: expected Numeric type PyArray_DOUBLE, found Numeric type PyArray_LONG

Data method

>>> bar = N.array([[1,2,3],[4,5,6]], N.dtype(N.float64).char)
>>> nt.test_data_float(bar)
[1.0, 2.0, 3.0, 4.0, 5.0, 6.0]
"""
def _test():
  import doctest, num_py_test
  return doctest.testmod(num_py_test)
    
if __name__ == "__main__":
  _test()

