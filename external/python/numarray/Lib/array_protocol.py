"""array_protocol contains self-tests for the scipy newcore array protocol.

Currently array_protcol tests numarray:Numeric exchanges.  If Numeric fails
to import,  array_protocol runs no tests and returns a (0,0) doctest result
tuple.
"""

## doctests for numarray-->Numeric conversions

import sys
import numarray

try:
    import Numeric
except ImportError:
    def test_Numeric():
        """Numeric not installed dummy selftest"""
        pass
else:
    def test_Numeric():
        """
===========================================================================
Numeric interoperability

Test all of the Numeric typecodes with the exception of 'i' which doesn't
"round-trip" consistently for both 32 and 64-bit systems.
        
>>> typecodes = ['b', '1', 's', 'l', 'f', 'd', 'F', 'D']

Checking numarray-->Numeric conversion. Non-strided values. Data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num = Numeric.array(na, copy=1)
...      num2 = Numeric.array([1,2,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1

Checking numarray-->Numeric conversion. Non-strided values. No data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num = Numeric.array(na, copy=0)
...      num2 = Numeric.array([1,2,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1

Checking numarray-->Numeric conversion. Strided values. Data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num = Numeric.array(na[::2], copy=0)
...      num2 = Numeric.array([1,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numarray-->Numeric conversion. Strided values. No data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num = Numeric.array(na[::2], copy=1)
...      num2 = Numeric.array([1,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numarray-->Numeric conversion. Offseted values. Data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num = Numeric.array(na[1:], copy=1)
...      num2 = Numeric.array([2,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numarray-->Numeric conversion. Offseted values. No data copy.

>>> for typecode in typecodes:
...     na = numarray.array([1,2,3], typecode)
...     num = Numeric.zeros(shape=2, typecode=typecode)
...     num = Numeric.array(na[1:], copy=0)
...     num2 = Numeric.array([2,3], typecode)
...     print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

>>> typecodes.append('i')

Checking Numeric<--numarray assignment. Non-strided values. Data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode=typecode)
...      num = Numeric.zeros(shape=3, typecode=typecode)
...      num[...] = na
...      num2 = Numeric.array([1,2,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1
i [1 1 1] 1

Checking Numeric<--numarray assignment. Strided values. Data copy.

>>> for typecode in typecodes:
...      na = numarray.array([1,2,3], typecode)
...      num = Numeric.zeros(shape=2, typecode=typecode)
...      num[...] = na[::2]
...      num2 = Numeric.array([1,3], typecode)
...      print typecode, num == num2, int(num.typecode() == num2.typecode())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1

Checking numarray<--Numeric assignment. Non-strided values. Data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=3, typecode=typecode)
...     na[...] = num
...     nb = numarray.array([1,2,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1
i [1 1 1] 1

Checking numarray<--Numeric assignment. Strided values. Data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na[...] = num[::2]
...     nb = numarray.array([1,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1

Checking Numeric-->numarray conversion. Non-strided values. Data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num, copy=1)
...     nb = numarray.array([1,2,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1
i [1 1 1] 1

Checking Numeric-->numarray conversion. Non-strided values. No data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num, copy=0)
...     nb = numarray.array([1,2,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
1 [1 1 1] 1
s [1 1 1] 1
l [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1
i [1 1 1] 1

Checking Numeric-->numarray conversion. Strided values. Data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num[::2], copy=1)
...     nb = numarray.array([1,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1

Checking Numeric-->numarray conversion. Strided values. No data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num[::2], copy=0)
...     nb = numarray.array([1,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1

Checking Numeric-->numarray conversion. Offseted values. Data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num[1:], copy=1)
...     nb = numarray.array([2,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1

Checking Numeric-->numarray conversion. Offseted values. No data copy.

>>> for typecode in typecodes:
...     num = Numeric.array([1,2,3], typecode)
...     na = numarray.zeros(shape=2, typecode=typecode)
...     na = numarray.array(num[1:], copy=0)
...     nb = numarray.array([2,3], typecode)
...     print typecode, na == nb, int(na.type() == nb.type())
b [1 1] 1
1 [1 1] 1
s [1 1] 1
l [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
i [1 1] 1
    """

try:
    import numpy
except ImportError:
    def test_numpy():
        """numpy not installed dummy selftest"""
        pass
else:
    def test_numpy():
        """
=============================================================================
numpy interoperability

>>> dtypes = ['b','B','h', 'H', 'i','I', 'q','Q', 'f', 'd','F', 'D' ]

Checking numpy<--numarray assignment. Non-strided values. Data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=3, dtype=dtype)
...      num[...] = na
...      num2 = numpy.array([1,2,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True True] 1
B [True True True] 1
h [True True True] 1
H [True True True] 1
i [True True True] 1
I [True True True] 1
q [True True True] 1
Q [True True True] 1
f [True True True] 1
d [True True True] 1
F [True True True] 1
D [True True True] 1

Checking numpy<--numarray assignment. Strided values. Data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num[...] = na[::2]
...      num2 = numpy.array([1,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True] 1
B [True True] 1
h [True True] 1
H [True True] 1
i [True True] 1
I [True True] 1
q [True True] 1
Q [True True] 1
f [True True] 1
d [True True] 1
F [True True] 1
D [True True] 1

Checking numarray-->numpy conversion. Non-strided values. Data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num = numpy.array(na, copy=1)
...      num2 = numpy.array([1,2,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True True] 1
B [True True True] 1
h [True True True] 1
H [True True True] 1
i [True True True] 1
I [True True True] 1
q [True True True] 1
Q [True True True] 1
f [True True True] 1
d [True True True] 1
F [True True True] 1
D [True True True] 1

Checking numarray-->numpy conversion. Non-strided values. No data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num = numpy.array(na, copy=0)
...      num2 = numpy.array([1,2,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True True] 1
B [True True True] 1
h [True True True] 1
H [True True True] 1
i [True True True] 1
I [True True True] 1
q [True True True] 1
Q [True True True] 1
f [True True True] 1
d [True True True] 1
F [True True True] 1
D [True True True] 1

Checking numarray-->numpy conversion. Strided values. Data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num = numpy.array(na[::2], copy=0)
...      num2 = numpy.array([1,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True] 1
B [True True] 1
h [True True] 1
H [True True] 1
i [True True] 1
I [True True] 1
q [True True] 1
Q [True True] 1
f [True True] 1
d [True True] 1
F [True True] 1
D [True True] 1

Checking numarray-->numpy conversion. Strided values. No data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num = numpy.array(na[::2], copy=1)
...      num2 = numpy.array([1,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True] 1
B [True True] 1
h [True True] 1
H [True True] 1
i [True True] 1
I [True True] 1
q [True True] 1
Q [True True] 1
f [True True] 1
d [True True] 1
F [True True] 1
D [True True] 1

Checking numarray-->numpy conversion. Offseted values. Data copy.

>>> for dtype in dtypes:
...      na = numarray.array([1,2,3], dtype=dtype)
...      num = numpy.zeros(shape=2, dtype=dtype)
...      num = numpy.array(na[1:], copy=1)
...      num2 = numpy.array([2,3], dtype=dtype)
...      print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True] 1
B [True True] 1
h [True True] 1
H [True True] 1
i [True True] 1
I [True True] 1
q [True True] 1
Q [True True] 1
f [True True] 1
d [True True] 1
F [True True] 1
D [True True] 1

Checking numarray-->numpy conversion. Offseted values. No data copy.

>>> for dtype in dtypes:
...     na = numarray.array([1,2,3], dtype=dtype)
...     num = numpy.zeros(shape=2, dtype=dtype)
...     num = numpy.array(na[1:], copy=0)
...     num2 = numpy.array([2,3], dtype=dtype)
...     print dtype, num == num2, int(num.dtype == num2.dtype)
b [True True] 1
B [True True] 1
h [True True] 1
H [True True] 1
i [True True] 1
I [True True] 1
q [True True] 1
Q [True True] 1
f [True True] 1
d [True True] 1
F [True True] 1
D [True True] 1

Checking numarray<--numpy assignment. Non-strided values. Data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=3, dtype=dtype)
...     na[...] = num
...     nb = numarray.array([1,2,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
B [1 1 1] 1
h [1 1 1] 1
H [1 1 1] 1
i [1 1 1] 1
I [1 1 1] 1
q [1 1 1] 1
Q [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1

Checking numarray<--numpy assignment. Strided values. Data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na[...] = num[::2]
...     nb = numarray.array([1,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1] 1
B [1 1] 1
h [1 1] 1
H [1 1] 1
i [1 1] 1
I [1 1] 1
q [1 1] 1
Q [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numpy-->numarray conversion. Non-strided values. Data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num, copy=1)
...     nb = numarray.array([1,2,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
B [1 1 1] 1
h [1 1 1] 1
H [1 1 1] 1
i [1 1 1] 1
I [1 1 1] 1
q [1 1 1] 1
Q [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1

Checking numpy-->numarray conversion. Non-strided values. No data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num, copy=0)
...     nb = numarray.array([1,2,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1 1] 1
B [1 1 1] 1
h [1 1 1] 1
H [1 1 1] 1
i [1 1 1] 1
I [1 1 1] 1
q [1 1 1] 1
Q [1 1 1] 1
f [1 1 1] 1
d [1 1 1] 1
F [1 1 1] 1
D [1 1 1] 1

Checking numpy-->numarray conversion. Strided values. Data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num[::2], copy=1)
...     nb = numarray.array([1,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1] 1
B [1 1] 1
h [1 1] 1
H [1 1] 1
i [1 1] 1
I [1 1] 1
q [1 1] 1
Q [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numpy-->numarray conversion. Strided values. No data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num[::2], copy=0)
...     nb = numarray.array([1,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1] 1
B [1 1] 1
h [1 1] 1
H [1 1] 1
i [1 1] 1
I [1 1] 1
q [1 1] 1
Q [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numpy-->numarray conversion. Offseted values. Data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num[1:], copy=1)
...     nb = numarray.array([2,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1] 1
B [1 1] 1
h [1 1] 1
H [1 1] 1
i [1 1] 1
I [1 1] 1
q [1 1] 1
Q [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1

Checking numpy-->numarray conversion. Offseted values. No data copy.

>>> for dtype in dtypes:
...     num = numpy.array([1,2,3], dtype=dtype)
...     na = numarray.zeros(shape=2, dtype=dtype)
...     na = numarray.array(num[1:], copy=0)
...     nb = numarray.array([2,3], dtype=dtype)
...     print dtype, na == nb, int(na.type() == nb.type())
b [1 1] 1
B [1 1] 1
h [1 1] 1
H [1 1] 1
i [1 1] 1
I [1 1] 1
q [1 1] 1
Q [1 1] 1
f [1 1] 1
d [1 1] 1
F [1 1] 1
D [1 1] 1
    """

def test():
    import doctest, array_protocol
    return doctest.testmod(array_protocol)

if __name__ == "__main__":
    test()
