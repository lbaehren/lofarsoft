""" This module provides access to higher level functions and
constants for ieee special values such as Not a Number (nan) and
infinity (inf).

>>> from numarray import *

The special values are designated using lower case as follows:

>> inf
inf
>> plus_inf
inf
>> minus_inf
-inf
>> nan
nan
>> plus_zero
0.0
>> minus_zero
-0.0

Note that the representation of IEEE special values is platform
dependent so your Python might, for instance, say 'Infinity' rather
than 'inf'.  Below, inf is seen to arise as the result of floating
point division by 0 and nan is seen to arise from 0 divided by 0:

>>> a = arange(2.0)
>>> b = a/0
Warning: Encountered invalid numeric result(s)  in divide
Warning: Encountered divide by zero(s)  in divide

Here are the results for linux,  but the repr problem causes this
example to fail for windows:

>> b
array([              nan,               inf])

A curious property of nan is that it does not compare to *itself* as
equal (results also from linux):

>> b == nan
array([0, 0], type=Bool)

The isnan(), isinf(), and isfinite() functions return boolean arrays
which have the value True where the corresponding predicate holds.
These functions detect bit ranges and are therefore more robust than
simple equality checks.

>>> isnan(b)
array([1, 0], type=Bool)
>>> isinf(b)
array([0, 1], type=Bool)
>>> isfinite(b)
array([0, 0], type=Bool)

Array based indexing provides a convenient way to replace special values:

>>> b[isnan(b)] = 999
>>> b[isinf(b)] = 5
>>> b
array([ 999.,    5.])

Here's an easy approach for compressing your data arrays to remove
NaNs:

>>> x, y = arange(10.), arange(10.);  x[5] = nan;  y[6] = nan;
>>> keep = ~isnan(x) & ~isnan(y)
>>> x[keep]
array([ 0.,  1.,  2.,  3.,  4.,  7.,  8.,  9.])
>>> y[keep]
array([ 0.,  1.,  2.,  3.,  4.,  7.,  8.,  9.])

=======================================================================

# >>> inf    # the repr() of inf may vary from platform to platform
# inf
# >>> nan    # the repr() of nan may vary from platform to platform
# nan

# Create a couple inf values in 4,4 array

>>> a=arange(16.0, shape=(4,4))
>>> a[2,3] = 0.0
>>> b = 1/a
Warning: Encountered divide by zero(s)  in divide

# Locate the positions of the inf values

>>> getinf(b)
(array([0, 2]), array([0, 3]))

# Change the inf values to something else

>>> isinf(b)
array([[1, 0, 0, 0],
       [0, 0, 0, 0],
       [0, 0, 0, 1],
       [0, 0, 0, 0]], type=Bool)

>>> isinf(inf)
1
>>> isinf(1)
0
>>> isinf(nan)
0

>>> isfinite(inf)
0
>>> isfinite(1)
1
>>> isfinite(nan)
0

>>> isnan(inf)
0
>>> isnan(1)
0
>>> isnan(nan)
1

>>> isfinite(b)
array([[0, 1, 1, 1],
       [1, 1, 1, 1],
       [1, 1, 1, 0],
       [1, 1, 1, 1]], type=Bool)

>>> a[getinf(b)] = 999
>>> a
array([[ 999.,    1.,    2.,    3.],
       [   4.,    5.,    6.,    7.],
       [   8.,    9.,   10.,  999.],
       [  12.,   13.,   14.,   15.]])

# Set a bunch of locations to a special value

>>> a[0,1] = nan; a[1,2] = nan; a[2,3] = nan
>>> getnan(a)
(array([0, 1, 2]), array([1, 2, 3]))

IEEE Special Value support  32-bit

>>> import ieeespecial
>>> a = arange(5.0, type=Float32)
>>> b = (a*a)/a        
Warning: Encountered invalid numeric result(s)  in divide
>>> ieeemask(b, NAN)
array([1, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, NUMBER)
array([0, 1, 1, 1, 1], type=Bool)
>>> index(b, NAN)
(array([0]),)
>>> getnan(b)
(array([0]),)
>>> setnan(b, 42.0)
>>> b[0]
42.0
>>> a = arange(1.0, 6.0, type=Float32)
>>> b = a/zeros((5,), type=Float32)
Warning: Encountered divide by zero(s)  in divide
>>> ieeemask(b, POS_INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, NEG_INFINITY)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> b = (-a)/zeros((5,), type=Float32)
Warning: Encountered divide by zero(s)  in divide
>>> ieeemask(b, POS_INFINITY)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, NEG_INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, NUMBER)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(array([0], type=Float32),  POS_ZERO)
array([1], type=Bool)
>>> ieeemask(array([0], type=Float32),  NEG_ZERO)
array([0], type=Bool)
>>> ieeemask(array([0], type=Float32),  ZERO)
array([1], type=Bool)
>>> neginf = (array([-1],type=Float32)/array([0], type=Float32))
Warning: Encountered divide by zero(s)  in divide
>>> negzero = array([1], type=Float32)/neginf
>>> ieeemask(negzero, POS_ZERO)
array([0], type=Bool)
>>> ieeemask(negzero, NEG_ZERO)
array([1], type=Bool)
>>> ieeemask(array([-0], type=Float32), ZERO)
array([1], type=Bool)

IEEE Special Value support  64-bit

>>> import ieeespecial
>>> a = arange(5.0, type=Float64)
>>> b = (a*a)/a        
Warning: Encountered invalid numeric result(s)  in divide
>>> ieeemask(b, NAN)
array([1, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, NUMBER)
array([0, 1, 1, 1, 1], type=Bool)
>>> index(b, NAN)
(array([0]),)
>>> getnan(b)
(array([0]),)
>>> setnan(b, 42.0)
>>> b[0]
42.0
>>> a = arange(1.0, 6.0, type=Float64)
>>> b = a/zeros((5,), type=Float64)
Warning: Encountered divide by zero(s)  in divide
>>> ieeemask(b, POS_INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, NEG_INFINITY)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> b = (-a)/zeros((5,), type=Float64)
Warning: Encountered divide by zero(s)  in divide
>>> ieeemask(b, POS_INFINITY)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(b, NEG_INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, INFINITY)
array([1, 1, 1, 1, 1], type=Bool)
>>> ieeemask(b, NUMBER)
array([0, 0, 0, 0, 0], type=Bool)
>>> ieeemask(array([0], type=Float64),  POS_ZERO)
array([1], type=Bool)
>>> ieeemask(array([0], type=Float64),  NEG_ZERO)
array([0], type=Bool)
>>> ieeemask(array([0], type=Float64),  ZERO)
array([1], type=Bool)
>>> neginf = (array([-1],type=Float64)/array([0], type=Float64))
Warning: Encountered divide by zero(s)  in divide
>>> negzero = array([1], type=Float64)/neginf
>>> ieeemask(negzero, POS_ZERO)
array([0], type=Bool)
>>> ieeemask(negzero, NEG_ZERO)
array([1], type=Bool)
>>> ieeemask(array([-0], type=Float64), ZERO)
array([1], type=Bool)
"""

import numarrayall as _na
from numarray.ufunc import isnan
# Define *ieee special values*
_na.Error.pushMode(all="ignore")

plus_inf = inf = (_na.array(1.0)/_na.array(0.0))[()]
minus_inf = (_na.array(-1.0)/_na.array(0.0))[()]
nan = (_na.array(0.0)/_na.array(0.0))[()]
plus_zero = zero = 0.0
minus_zero = (_na.array(-1.0)*0.0)[()]

_na.Error.popMode()

# Define *mask condition bits*
class _IeeeMaskBit(_na.NumArray):
    pass

def _BIT(x):
    a = _na.array((1 << x), type=_na.Int32)
    a.__class__ = _IeeeMaskBit
    return a

POS_QUIET_NAN     = _BIT(0)
NEG_QUIET_NAN     = _BIT(1)
POS_SIGNAL_NAN    = _BIT(2)
NEG_SIGNAL_NAN    = _BIT(3)
POS_INFINITY      = _BIT(4)
NEG_INFINITY      = _BIT(5)
POS_DENORMALIZED  = _BIT(6)
NEG_DENORMALIZED  = _BIT(7)
POS_NORMALIZED    = _BIT(8)
NEG_NORMALIZED    = _BIT(9)
POS_ZERO          = _BIT(10)
NEG_ZERO          = _BIT(11)
INDETERM          = _BIT(12)
BUG               = _BIT(15)

NAN = POS_QUIET_NAN | NEG_QUIET_NAN | POS_SIGNAL_NAN | NEG_SIGNAL_NAN | INDETERM
INFINITY = POS_INFINITY | NEG_INFINITY
SPECIAL = NAN | INFINITY

NORMALIZED = POS_NORMALIZED | NEG_NORMALIZED
DENORMALIZED = POS_DENORMALIZED | NEG_DENORMALIZED
ZERO = POS_ZERO | NEG_ZERO
NUMBER = NORMALIZED | DENORMALIZED | ZERO
FINITE = NUMBER

def mask(a, m):
    """mask(a, m) returns the values of 'a' satisfying category 'm'.
    mask does a parallel check for values which are not classifyable
    by the categorization code, raising a RuntimeError exception if
    any are found.
    """
    a = _na.asarray(a)
    if isinstance(a.type(), _na.IntegralType):
        a = a.astype('Float64')
    if isinstance(a.type(), _na.ComplexType):
        f = _na.ieeemask(a.real, m) | _na.ieeemask(a.imag, m)
        g = _na.ieeemask(a.real, BUG) | _na.ieeemask(a.imag, BUG)
    else:
        f = _na.ieeemask(a, m)
        g = _na.ieeemask(a, BUG)
    if _na.bitwise_or.reduce(_na.ravel(g)) != 0:
        raise RuntimeError("Unclassifyable floating point values.")
    if f.rank == 0:
        f = f[()]
    return f

def index(a, msk):
    """index returns the tuple of indices where the values satisfy 'mask'"""
    return _na.nonzero(mask(a, msk))

def getinf(a):
    """getinf returns a tuple of indices of 'a' where the values are infinite."""
    return index(a, INFINITY)

def setinf(a, value):
    """setinf sets elements of 'a' which are infinite to 'value' instead.
    DEPRECATED:  use 'a[getinf(a)] = value'  instead.
    """
    _na.put(a, getinf(a), value)

def isinf(a):
    """Idenitfies elements of 'a' which are infinity.
    """
    return mask(a, INFINITY)

def getposinf(a):
    """getposinf returns a tuple of indices of 'a' where the values are +inf."""
    return index(a, POS_INFINITY)

def getneginf(a):
    """getneginf returns a tuple of indices of 'a' where the values are -inf."""
    return index(a, NEG_INFINITY)

def getnan(a):
    """getnan returns a tuple of indices of 'a' where the values are not-a-numbers"""
    return _na.nonzero(isnan(a))

def setnan(a, value):
    """setnan sets elements of 'a' which are NANs to 'value' instead.
    DEPRECATED:  use 'a[getnan(a)] = value'  instead.
    """
    a[isnan(a)]= value

#def isnan(a):
#    """Idenitfies elements of 'a' which are NANs, not a number.
#    """
#    return _na.isnan(a)
#
#   This function has been replaced by isnan macro added to the numarray.ufunc module.

def isfinite(a):
    """Identifies elements of an array which are neither nan nor infinity."""
    return _na.logical_not(isinf(a)| isnan(a))

def getbug(a):
    """getbug returns a tuple of indices of 'a' where the values are not classifyable."""
    return index(a, BUG)

def test():
    import doctest, ieeespecial
    return doctest.testmod(ieeespecial)

