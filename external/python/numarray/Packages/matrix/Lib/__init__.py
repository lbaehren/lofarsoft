""" numarray.matrix ports/re-implements the Matrix module from Numeric.

The basic goal of numarray.matrix is to provide a quick and dirty matrix
class which is backward compatible with Numeric's Matrix class.

In numarray, Matrix() is a factory function for _Matrix, which is a
subclass of NumArray.  _Matrix is not yet itself subclassable.
Because a _Matrix is a subclass of NumArray, it can be used where a
NumArray can be used without conversion, particularly in C.

A _Matrix can be constructed from a 2D nested sequence:

>>> m = Matrix( [[1,2,3],[11,12,13],[21,22,23]]); m
_Matrix([[ 1,  2,  3],
       [11, 12, 13],
       [21, 22, 23]])

Or,  a _Matrix can be constructed from an abbreviated string description:

>>> m = Matrix("1 2 3; 6 5 4; 9 20 2"); m
_Matrix([[ 1,  2,  3],
       [ 6,  5,  4],
       [ 9, 20,  2]])

Because a _Matrix is a subclass of numarray,  most NumArray methods and
behavior are identical.  In particular,  a _Matrix can be indexed as a 2D
array:

>>> m[1:2,1:2]
_Matrix([[5]])

_Matrix overloads the * operator to perform matrix multiplication rather
than elementwise multiplication:
       
>>> m*m
_Matrix([[ 40,  72,  17],
       [ 72, 117,  46],
       [147, 158, 111]])

_Matrix defines an "array" attribute to return the contents of the matrix
as a 2D NumArray.  Here,  NumArray multiplication is shown to be elementwise
multiplication:

>>> m.array*m.array
array([[  1,   4,   9],
       [ 36,  25,  16],
       [ 81, 400,   4]])

Checking if the determinant is close to zero can determine if a matrix
is singular or not.

>>> EPS = 1e-13            # fudge an epsilon... your mileage may vary
>>> m.determinant() < EPS  # singular?
0

_Matrix overloads the __pow__ special method to perform matrix
multiplication and inversion.  Since m is non-singular, we can invert
it.  _Matrix inversion is represented using **-1 notation:

>>> m**-1                  
_Matrix([[-0.34482759,  0.27586207, -0.03448276],
       [ 0.1182266 , -0.12315271,  0.06896552],
       [ 0.36945813, -0.00985222, -0.03448276]])

Of course multiplying by the inverse should give something close to identity.

>>> numarray.allclose(m*m**-1, numarray.identity(3), atol=EPS)
1

You can use numarray's transpose() function to compute the transpose of
a _Matrix:

>>> transpose(m)
_Matrix([[ 1,  6,  9],
       [ 2,  5, 20],
       [ 3,  4,  2]])

Raising a matrix to a positive power results in matrix multiplication
by itself as illustrated in the _fibonacci function:

>>> _fibonacci(10)
Fibonacci numbers: 1 1 2 3 5 8 13 21 34 55

"""

import string
import types

import numarray
from numarray import asarray, dot as _dot, identity, conjugate, NumArray, shape
from numarray import transpose as transpose
import numarray.linear_algebra as LinearAlgebra

def _fibonacci(n=10):
    """A self test demo function from Numeric's Matrix."""
    print "Fibonacci numbers:",
    m = Matrix([[1,1],[1,0]])
    for i in range(n):
        mm=m**i
        print mm[0][0],
    print

# make translation table
_table = [None]*256
for k in range(256):
    _table[k] = chr(k)
_table = ''.join(_table)

_numchars = string.digits + ".-+jeEL"
_todelete = []
for k in _table:
    if k not in _numchars:
        _todelete.append(k)
_todelete = ''.join(_todelete)

def _eval(astr):
    return eval(astr.translate(_table,_todelete))

def _convert_from_string(data):
    data.find
    rows = data.split(';')
    newdata = []
    count = 0
    for row in rows:
        trow = row.split(',')
        newrow = []
        for col in trow:
            temp = col.split()
            newrow.extend(map(_eval,temp))
        if count == 0:
            Ncols = len(newrow)
        elif len(newrow) != Ncols:
            raise ValueError, "Rows not the same size."
        count += 1
        newdata.append(newrow)
    return newdata


_lkup = {'0':'000',
         '1':'001',
         '2':'010',
         '3':'011',
         '4':'100',
         '5':'101',
         '6':'110',
         '7':'111'}

def _binary(num):
    ostr = oct(num)
    bin = ''
    for ch in ostr[1:]:
        bin += _lkup[ch]
    ind = 0
    while bin[ind] == '0':
        ind += 1
    return bin[ind:]

class _Matrix(NumArray):
    def _rc(self, a):
        if len(shape(a)) == 0:
            return a
        else:
            return Matrix(a)
            
    def __mul__(self, other):
        aother = asarray(other)
        if len(aother.shape) == 0:
            return self._rc(self*aother)
        else:
            return self._rc(_dot(self, aother))

    def __rmul__(self, other):
        aother = asarray(other)
        if len(aother.shape) == 0:
            return self._rc(aother*self)
        else:
            return self._rc(_dot(aother, self))

    def __imul__(self,other):
        aother = asarray(other)
        self[:] = _dot(self, aother)
        return self

    def __pow__(self, other):
        shape = self.shape
        if len(shape)!=2 or shape[0]!=shape[1]:
            raise TypeError, "matrix is not square"
        if type(other) not in (type(1), type(1L)):
            raise TypeError, "exponent must be an integer"        
        if other==0:
            return Matrix(identity(shape[0]))
        if other<0:
            result=Matrix(LinearAlgebra.inverse(self))
            x=Matrix(result)
            other=-other
        else:
            result=self
            x=result
        if other <= 3:
            while(other>1):
                result=result*x
                other=other-1
            return result
        # binary decomposition to reduce the number of Matrix
        #  Multiplies for other > 3.
        beta = _binary(other)            
        t = len(beta)
        Z,q = x.copy(),0
        while beta[t-q-1] == '0':
            Z *= Z
            q += 1
        result = Z.copy()
        for k in range(q+1,t):
            Z *= Z
            if beta[t-k-1] == '1':
                result *= Z
        return result

    def __rpow__(self, other):
        raise TypeError, "x**y not implemented for matrices y"

    def __invert__(self):
        return Matrix(conjugate(self))

    def determinant(self):
        """Computes the determinant of the matrix."""
        return LinearAlgebra.determinant(self)

    def getarray(self):
        v = self.view()
        v.__class__ = NumArray
        return v

    array = property(getarray, None, "NumArray form of a Matrix")


def Matrix(data, typecode=None, copy=1, savespace=0):
    if type(data) is types.StringType:
        data = _convert_from_string(data)
    a = numarray.fromlist(data, type=typecode)
    a.__class__ = _Matrix
    return a


def test():
    import doctest, __init__
    return doctest.testmod(__init__)

if __name__ == '__main__':
    print test()

