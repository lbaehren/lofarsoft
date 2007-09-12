""" This module manages different implementations of matrix multiplication
related routines.
"""

import generic as _gen
import numarrayall as _na

try:
    """
    Provides a BLAS-optimized (i.e. much faster) replacement `dot`
    function for numarray arrays.
    
    BLAS routines exist only for 32 & 64 bit float and complex types;
    if BLAS routines cannot be used the dot defined here defers to the
    standard `numarray.dot`.
    
    This package is integrated with numarray so its improved functions
    are used whenever they're available; they're not always available
    because not everyone needs or wants to install a BLAS.
    """
    
    __author__ = "Richard Everson (R.M.Everson@exeter.ac.uk)"
    __revision__ = "$Revision: 1.2 $"
    __version__  = "1.0"
    
    import numarray._dotblas as _dotblas
    import numarray._numarray as _numarray
    USING_BLAS = 1
except ImportError:
    USING_BLAS = 0

if not USING_BLAS:
    from numarray._numarray import dot, innerproduct
else:
    def dot(a, b):
        """returns matrix-multiplication between a and b.
        The product-sum is over the last dimension of a and the
        second-to-last dimension of b.
        
        NB: No conjugation of complex arguments is performed.
        
        This version uses the BLAS optimized routines where possible.
        """
        try:
            return _dotblas.dot(a, b) 
        except TypeError:
            try:
                return _numarray.dot(a, b)
            except TypeError,detail:
                if _na.shape(a) == () or _na.shape(b) == ():
                    return a*b
                else:
                    raise TypeError, detail or "invalid types for dot"

    def innerproduct(a, b):
        """returns inner product between a and b.
        The product-sum is over the last dimension of a and b.
        
        NB: No conjugation of complex arguments is performed.
        
        This version uses the BLAS optimized routines where possible.
        """
        try:
            return _dotblas.innerproduct(a, b) 
        except TypeError:
            try:
                return _numarray.innerproduct(a, b)
            except TypeError,detail:
                if (_na.shape(a) == () or _na.shape(b) == ()):
                    return a*b
                else:
                    raise TypeError, detail or "invalid types for innerproduct"

def vdot(a, b):
    """Returns the dot product of 2 vectors (or anything that can be made into
    a vector). NB: this is not the same as `dot`, as it takes the conjugate
    of its first argument if complex and always returns a scalar."""
    a, b = _na.ravel(a), _na.ravel(b)
    try:
        return _dotblas.vdot(a, b)
    # in case we get an integer Value
    except TypeError:
        return _numarray.dot(a, b)
    
matrixmultiply = dot

def outerproduct(array1, array2):
    """outerproduct(array1, array2) computes the NxM outerproduct of N vector
    'array1' and M vector 'array2', where result[i,j] = array1[i]*array2[j].
    """
    array1=_gen.reshape(
        _na.asarray(array1), (-1,1))  # ravel array1 into an Nx1
    array2=_gen.reshape(
        _na.asarray(array2), (1,-1))  # ravel array2 into a 1xM
    return matrixmultiply(array1,array2)   # return NxM result

def tensormultiply(array1, array2):
    """tensormultiply returns the product for any rank >=1 arrays, defined as:

    r_{xxx, yyy} = \sum_k array1_{xxx, k} array2_{k, yyyy}

    where xxx, yyy denote the rest of the a and b dimensions.
    """
    array1, array2 = _na.asarray(array1), _na.asarray(array2)
    if array1.shape[-1] != array2.shape[0]:
        raise ValueError, "Unmatched dimensions"
    shape = array1.shape[:-1] + array2.shape[1:]
    return _gen.reshape(dot(_gen.reshape(array1, (-1, array1.shape[-1])),
                            _gen.reshape(array2, (array2.shape[0], -1))), shape)

def kroneckerproduct(a,b):
    '''Computes a otimes b where otimes is the Kronecker product operator.
    
    Note: the Kronecker product is also known as the matrix direct product
    or tensor product.  It is defined as follows for 2D arrays a and b
    where shape(a)=(m,n) and shape(b)=(p,q):
    c = a otimes b  => cij = a[i,j]*b  where cij is the ij-th submatrix of c.
    So shape(c)=(m*p,n*q).
    
    >>> print kroneckerproduct([[1,2]],[[3],[4]])
    [[3 6]
    [4 8]]
    >>> print kroneckerproduct([[1,2]],[[3,4]])
    [ [3 4 6 8]]
    >>> print kroneckerproduct([[1],[2]],[[3],[4]])
    [[3]
    [4]
    [6]
    [8]]
    '''
    a, b = _na.asarray(a), _na.asarray(b)
    if not (len(a.shape)==2 and len(b.shape)==2):
        raise ValueError, 'Input must be 2D arrays.'
    if not a.iscontiguous():
        a = _gen.reshape(a, a.shape)
    if not b.iscontiguous():
        b = _gen.reshape(b, b.shape)
    o = outerproduct(a,b)
    o.shape = a.shape + b.shape
    return _gen.concatenate(_gen.concatenate(o, axis=1), axis=1)
    
