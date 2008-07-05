__all__ = ["solve_linear_equations",
           "inverse",
           "cholesky_decomposition",
           "qr_decomposition",
           "eigenvalues",
           "Heigenvalues",
           "eigenvectors",
           "Heigenvectors",
           "singular_value_decomposition",
           "generalized_inverse",
           "determinant",
           "linear_least_squares",
           "LinearAlgebraError",
           "test"]

# This module is a lite version of LinAlg.py module which contains
# high-level Python interface to the LAPACK library.  The lite versioho
# only accesses the following LAPACK functions: dgesv, zgesv, dgeev,
# zgeev, dgesdd, zgesdd, dgelsd, zgelsd, dsyevd, zheevd, dgetrf, dpotrf.

import numarray.numeric as num
import numarray as na
import copy
import lapack_lite2
import math
import mlab

# import multiarray     # just used for fast 2D tranpose
multiarray = num 

class LinearAlgebraError(Exception):
    pass
LinAlgError = LinearAlgebraError

def __setup__():
    import sys
    sys.float_output_suppress_small = True

__test__ = {
"determiniant" : 
"""
>>> import numarray.linear_algebra as la, numarray.random_array as random_array
>>> A = random_array.random([100,5,5])
>>> _isClose(map(la.determinant, A), determinant(A))
1
>>> _isClose(la.determinant(A[0]), determinant(A[0]))
1
>>> A = random_array.random([100,5,5]) + 1j*random_array.random([100,5,5])
>>> _isClose(map(la.determinant, A), determinant(A))
1
>>> _isClose(la.determinant(A[0]), determinant(A[0]))
1

""",
"inverse" : 
"""
>>> import numarray.linear_algebra as la, numarray.random_array as random_array
>>> A = []
>>> while len(A) < 100:
...     candidate = random_array.random([5,5])
...     if abs(la.determinant(candidate)) > 1e-3:
...         A.append(candidate)
>>> A = na.asarray(A)
>>> _isClose(map(la.inverse, A), inverse(A))
1
>>> _isClose(la.inverse(A[0]), inverse(A[0]))
1
>>> A = []
>>> while len(A) < 100:
...     candidate = random_array.random([5,5]) + 1j*random_array.random([5,5])
...     if abs(la.determinant(candidate)) > 1e-3:
...         A.append(candidate)
>>> A = na.asarray(A)
>>> _isClose(map(la.inverse, A), inverse(A))
1
>>> _isClose(la.inverse(A[0]), inverse(A[0]))
1

""",
"solve_linear_equations" : 
"""
>>> import numarray.linear_algebra as la, numarray.random_array as random_array
>>> A = []
>>> while len(A) < 100:
...     candidate = random_array.random([5,5])
...     if abs(la.determinant(candidate)) > 1e-3:
...         A.append(candidate)
>>> A = na.asarray(A)
>>> B = random_array.random([100,5])
>>> _isClose(map(la.solve_linear_equations, A, B), solve_linear_equations(A, B))
1
>>> _isClose(la.solve_linear_equations(A[0], B[0]), la.solve_linear_equations(A[0], B[0]))
1
   
    
"""
}

# Helper routines
_lapack_type = { num.Float32: 0, num.Float64: 1,
                 num.Complex32: 2, num.Complex64: 3}
_lapack_letter = ['s', 'd', 'c', 'z']

_array_kind = {
    num.Bool:0,
    num.Int8:0,
    num.UInt8:0,
    num.Int16:0,
    num.UInt16:0,    
    num.Int32:0,
    num.UInt32:0,
    num.Int64:0,
    num.UInt64:0,
    num.Float32: 0,
    num.Float64: 0,
    num.Complex32: 1,
    num.Complex64: 1}

_array_precision = {
    num.Bool: 1,
    num.Int8: 1,
    num.UInt8: 1,
    num.Int16: 1,
    num.UInt16: 1,
    num.Int32: 1,
    num.UInt32: 1,
    num.Int64: 1,
    num.UInt64: 1,
    num.Float32: 0,
    num.Float64: 1,
    num.Complex32: 0,
    num.Complex64: 1
    }

_array_type = [[num.Float32, num.Float64],
               [num.Complex32, num.Complex64]]

def _commonType(*numarray):
    kind = 0
#    precision = 0
#   force higher precision in lite version
    precision = 1
    for a in numarray:
        t = a.type()
        kind = max(kind, _array_kind[t])
        precision = max(precision, _array_precision[t])
    return _array_type[kind][precision]

def _castCopyAndTranspose(type, *numarray, **kargs):
    # This should work as a drop in replacement for LinearAlgegra2._castCopyAndTranspose
    indices = kargs.get("indices")
    cast_numarray = []
    for a in numarray:
        if indices is None:
            a = na.transpose(a)
        else:
            a = na.transpose(a, indices)
        if a.type() == type:
            cast_numarray.append(a.copy())
        else:
            cast_numarray.append(a.astype(type))
    if len(cast_numarray) == 1:
            return cast_numarray[0]
    else:
        return cast_numarray


"""
# _fastCopyAndTranpose is an optimized version of _castCopyAndTranspose.
# It assumes the input is 2D (as all the calls in here are).

_fastCT = multiarray._fastCopyAndTranspose

def _fastCopyAndTranspose(type, *numarray):
    cast_numarray = ()
    for a in numarray:
        if a.type() == type:
            cast_numarray = cast_numarray + (_fastCT(a),)
        else:
            cast_numarray = cast_numarray + (_fastCT(a.astype(type)),)
    if len(cast_numarray) == 1:
            return cast_numarray[0]
    else:
        return cast_numarray
"""

_fastCopyAndTranspose = _castCopyAndTranspose


def _assertRank(rank, *args):
    for a in args:
        if isinstance(rank, int):
            rank = (rank,)
        if len(a.shape) not in rank:
            raise LinAlgError, 'Array must be rank %s' % (' or '.join([str(x) for x in rank]))
   
def _assertRank2(*numarray):
    for a in numarray:
        if len(a.getshape()) != 2:
            raise LinAlgError, 'Array must be two-dimensional'

def _assertSubmatrixSquareness(*args):
    for a in args:
        if a.shape[-1] != a.shape[-2]:
            raise LinAlgError, 'Array (or it submatrices) must be square'


def _isClose(a, b, *args, **kargs):
    return bool(na.allclose(na.ravel(a), na.ravel(b), *args, **kargs))

def _assertSquareness(*numarray):
    for a in numarray:
        if max(a.getshape()) != min(a.getshape()):
            raise LinAlgError, 'Array must be square'
    

# Linear equations
        
def solve_linear_equations(a, b):
    """solve_linear_equations(a, b) -> x such that dot(a,x) = b
    
    *a* may be either rank-2 or rank-3, in either case, it 
    must be square along the last two axes *b* may either 
    have a rank one lower than *a*, in which case it     
    represents a vector or an array of vectors, or it may be 
    the same rank as *a* in which case it represents a matrix 
    or an array of matrices.
    
    Since that may be a  bit confusing let's look at some examples.
    First the simplest case, a square matrix A, and a vector of 
    results B.
    
    >>> A = [[1,2,3], [3,5,5], [5,6,7]]
    >>> B = [1,1,1]
    >>> x = solve_linear_equations(A, B)
    >>> _isClose(x, num.array([-0.5,  0. ,  0.5]))
    1
    >>> _isClose(na.dot(A, x), # This should give us B
    ...        num.array([ 1.,  1.,  1.]))
    1
    
    The next simplest case is a square matrix A and a matrix B.
    
    >>> B = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    >>> _isClose(solve_linear_equations(A, B),
    ...     num.array([[-0.625, -0.5  ,  0.625],
    ...                [-0.5  ,  1.   , -0.5  ],
    ...                [ 0.875, -0.5  ,  0.125]]))
    1
           
    If *a* is rank-3, then the first dimension of *a* **and** *b* is 
    interpreted as selecting different submatrices or subvectors to operate
    on. In this case, *b* will be rank-2 in the vector case and rank-3 in
    the matrix case. Here is what is looks like in the vector case.
    
    >>> A = [[[1, 3], [2j, 3j]],
    ...      [[2, 4], [4j, 4j]],
    ...      [[3, 5], [6j, 5j]]]
    >>> B = [[1, 0], [0, 1], [1, 1]]
    >>> _isClose(solve_linear_equations(A, B),
    ...      num.array([[-1.        +0.j        ,  0.66666667+0.j        ],
    ...                 [ 0.        -0.5j       ,  0.        +0.25j      ],
    ...                 [-0.33333333-0.33333333j,  0.4       +0.2j       ]]))
    1
           
    The first dimensions of *a* and *b* must either match or one of them must 
    be 1. In the latter case, the length-1 dimension is broadcast in the normal
    way.
    
    >>> B = [[1, 0], [0, 1]]
    >>> solve_linear_equations(A, B)
    Traceback (most recent call last):
       ...
    LinearAlgebraError: first dimensions of a and b must match or be 1
    >>> B = [[1, 0]]
    >>> _isClose(solve_linear_equations(A, B),
    ...          num.array([[-1.        +0.j,  0.66666667+0.j],
    ...                     [-0.5       +0.j,  0.5       +0.j],
    ...                     [-0.33333333+0.j,  0.4       +0.j]]))
    1
    """
    a = na.asarray(a)
    b = na.asarray(b)
    _assertRank((2,3), a)
    _assertSubmatrixSquareness(a)
    rank_a = len(a.shape)
    _assertRank((rank_a-1, rank_a), b)
    stretched = (rank_a == 2)
    if stretched:
        a = a[na.NewAxis,]
        b = b[na.NewAxis,]
    one_eq = (len(b.shape) == len(a.shape)-1)
    if one_eq: 
        b = b[:,:,na.NewAxis]
    broadcast_a = (a.shape[0] == 1)
    broadcast_b = (b.shape[0] == 1)
    if not (broadcast_a or broadcast_b or a.shape[0] == b.shape[0]):
        raise LinAlgError, "first dimensions of a and b must match or be 1"

    # 
    n_cases = max(a.shape[0], b.shape[0])
    n_eq = a.shape[1]
    n_rhs = b.shape[2]
    if n_eq != b.shape[1]:
        raise LinAlgError, 'Incompatible dimensions'
    t = _commonType(a, b)
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zgesv
    else:
        lapack_routine = lapack_lite2.dgesv
    a, b = _castCopyAndTranspose(t, a, b, indices=(0,2,1))
    result = na.zeros([n_cases, n_rhs, n_eq], b.type())
    result[:] = b 
    b = result
    pivots = na.zeros(n_eq, 'l')
    a_stride = n_eq * n_eq * a.itemsize()
    b_stride = n_eq * n_rhs * b.itemsize()
    a_view = a[0]
    b_view = b[0]
    a_i = a_view.copy()
    b_i = b_view.copy()
    for i in range(n_cases):
        if i:
            if not broadcast_a:
                a_i._copyFrom(a_view)
            b_i._copyFrom(b_view)
        outcome = lapack_routine(n_eq, n_rhs, a_i, n_eq, pivots, b_i, n_eq, 0)
        if outcome['info'] > 0:
            raise LinAlgError, 'Singular matrix' 
        b_view._copyFrom(b_i)
        a_view._byteoffset += a_stride
        b_view._byteoffset += b_stride
    b = na.transpose(b, (0,2,1))
    if one_eq:
        b = b[...,0]
    if stretched:
        b = b[0]
    return b


# Matrix inversion

def inverse(a):
    """inverse(a) -> inverse matrix of a
    
    *a* may be either rank-2 or rank-3. If it is rank-2, it must square.
    
    >>> A = [[1,2,3], [3,5,5], [5,6,7]]
    >>> Ainv = inverse(A)
    >>> _isClose(na.dot(A, Ainv), na.identity(3))
    1
    
    If *a* is rank-3, it is treated as an array of rank-2 matrices and
    must be square along the last 2 axes.
    
    >>> A = [[[1, 3], [2j, 3j]],
    ...      [[2, 4], [4j, 4j]],
    ...      [[3, 5], [6j, 5j]]]
    >>> Ainv = inverse(A)
    >>> _isClose(map(na.dot, A, Ainv), [na.identity(2)]*3)
    1
    
    If *a* is not square along its last two axes, a LinAlgError is raised.
    
    >>> inverse(na.asarray(A)[...,:1])
    Traceback (most recent call last):
       ...
    LinearAlgebraError: Array (or it submatrices) must be square
    
    """
    a = na.asarray(a)
    I = na.identity(a.shape[-2])
    if len(a.shape) == 3:
        I.shape = (1,) + I.shape
    return solve_linear_equations(a, I)


# Cholesky decomposition

def cholesky_decomposition(a):
    _assertRank2(a)
    _assertSquareness(a)
    t =_commonType(a)
    a = _castCopyAndTranspose(t, a)
    m = a.getshape()[0]
    n = a.getshape()[1]
    if _array_kind[t] == 1:
	lapack_routine = lapack_lite2.zpotrf
    else:
	lapack_routine = lapack_lite2.dpotrf
    results = lapack_routine('L', n, a, m, 0)
    if results['info'] > 0:
        raise LinAlgError, 'Matrix is not positive definite - Cholesky decomposition cannot be computed'
    return copy.copy(num.transpose(mlab.triu(a,k=0)))


def qr_decomposition(a, mode='full'):

    """  calculates A=QR, Q orthonormal, R upper triangle matrix.

         mode: 'full'     ==> (Q,R) as return value
	       'r'        ==> (None, R) as return value
	       'economic' ==> (None, A') where the diagonal + upper triangle 
	                      part of A' is R. This is faster if one only requires R.     """
    

    _assertRank2(a)

    t=_commonType(a)

    m = a.getshape()[0]
    n = a.getshape()[1]
    mn = min(m,n)
    tau = num.zeros((mn,), t)

    #  a: convert num storing order to fortran storing order 
    a = _castCopyAndTranspose(t, a)

    if _array_kind[t] == 1:
	lapack_routine = lapack_lite2.zgeqrf
	routine_name='ZGEQRF'
    else:
	lapack_routine = lapack_lite2.dgeqrf
	routine_name='DGEQRF'

    # calculate optimal size of work data 'work'
    lwork = 1
    work = num.zeros((lwork,), t)
    results=lapack_routine(m, n, a, m, tau, work, -1, 0)
    if results['info'] > 0:
        raise LinAlgError, '%s returns %d' % (routine_name, results['info'])

    # do qr decomposition
    lwork = int(abs(work[0]))
    work = num.zeros((lwork,),t)
    results=lapack_routine(m, n, a, m, tau, work, lwork, 0)

    if results['info'] > 0:
        raise LinAlgError, '%s returns %d' % (routine_name, results['info'])


    #  atemp: convert fortrag storing order to num storing order 
    atemp = num.transpose(a)

    #  economic mode
    if mode[0]=='e': return None, atemp

    #  generate r
    r = num.zeros((mn,n), t)
    for i in range(mn):
	    r[i, i:] = atemp[i, i:]

    #  'r'-mode, that is, calculate only r
    if mode[0]=='r': return None, r


    #  from here on: build orthonormal matrix q from a

    if _array_kind[t] == 1:
	lapack_routine = lapack_lite2.zungqr
	routine_name = "ZUNGQR"
    else:
	lapack_routine = lapack_lite2.dorgqr
	routine_name = "DORGQR"


    # determine optimal lwork
    lwork = 1
    work=num.zeros((lwork,), t)
    results=lapack_routine(m,mn,mn, a, m, tau, work, -1, 0)
    if results['info'] > 0:
        raise LinAlgError, '%s returns %d' % (routine_name, results['info'])

    # compute q
    lwork = int(abs(work[0]))
    work=num.zeros((lwork,), t)
    results=lapack_routine(m,mn,mn, a, m, tau, work, lwork, 0)

    if results['info'] > 0:
        raise LinAlgError, '%s returns %d' % (routine_name, results['info'])

    q = num.transpose(a[:mn,:])
    return q,r


# Eigenvalues

def eigenvalues(a):
    _assertRank2(a)
    _assertSquareness(a)
    t =_commonType(a)
    real_t = _array_type[0][_array_precision[t]]
    a = _fastCopyAndTranspose(t, a)
    n = a.getshape()[0]
    dummy = num.zeros((1,), t)
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zgeev
        w = num.zeros((n,), t)
        rwork = num.zeros((n,),real_t)
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', 'N', n, a, n, w,
                                 dummy, 1, dummy, 1, work, -1, rwork, 0)
        lwork = int(abs(work[0]))
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', 'N', n, a, n, w,
                                 dummy, 1, dummy, 1, work, lwork, rwork, 0)
    else:
        lapack_routine = lapack_lite2.dgeev
        wr = num.zeros((n,), t)
        wi = num.zeros((n,), t)
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', 'N', n, a, n, wr, wi,
                                 dummy, 1, dummy, 1, work, -1, 0)
        lwork = int(work[0])
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', 'N', n, a, n, wr, wi,
                                 dummy, 1, dummy, 1, work, lwork, 0)
        if num.logical_and.reduce(num.equal(wi, 0.)):
            w = wr
        else:
            w = wr+1j*wi
    if results['info'] > 0:
        raise LinAlgError, 'Eigenvalues did not converge'
    return w


def Heigenvalues(a, UPLO='L'):
    _assertRank2(a)
    _assertSquareness(a)
    t =_commonType(a)
    real_t = _array_type[0][_array_precision[t]]
    a = _castCopyAndTranspose(t, a)
    n = a.getshape()[0]
    liwork = 5*n+3 
    iwork = num.zeros((liwork,),'l')
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zheevd
        w = num.zeros((n,), real_t)
        lwork = 1
        work = num.zeros((lwork,), t)
        lrwork = 1
        rwork = num.zeros((lrwork,),real_t)
        results = lapack_routine('N', UPLO, n, a, n,w, work, -1, rwork, -1, iwork, liwork,  0)
        lwork = int(abs(work[0]))
        work = num.zeros((lwork,), t)
        lrwork = int(rwork[0])
        rwork = num.zeros((lrwork,),real_t)
        results = lapack_routine('N', UPLO, n, a, n,w, work, lwork, rwork, lrwork, iwork, liwork,  0)
    else:
        lapack_routine = lapack_lite2.dsyevd
        w = num.zeros((n,), t)
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', UPLO, n, a, n,w, work, -1, iwork, liwork, 0)
        lwork = int(work[0])
        work = num.zeros((lwork,), t)
        results = lapack_routine('N', UPLO, n, a, n,w, work, lwork, iwork, liwork, 0)
    if results['info'] > 0:
        raise LinAlgError, 'Eigenvalues did not converge'
    return w

# Eigenvectors

def eigenvectors(a):
    """eigenvectors(a) returns u,v  where u is the eigenvalues and
v is a matrix of eigenvectors with vector v[i] corresponds to 
eigenvalue u[i].  Satisfies the equation dot(a, v[i]) = u[i]*v[i]
"""
    _assertRank2(a)
    _assertSquareness(a)
    t =_commonType(a)
    real_t = _array_type[0][_array_precision[t]]
    a = _fastCopyAndTranspose(t, a)
    n = a.getshape()[0]
    dummy = num.zeros((1,), t)
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zgeev
        w = num.zeros((n,), t)
        v = num.zeros((n,n), t)
        lwork = 1
        work = num.zeros((lwork,),t)
        rwork = num.zeros((2*n,),real_t)
        results = lapack_routine('N', 'V', n, a, n, w,
                                  dummy, 1, v, n, work, -1, rwork, 0)
        lwork = int(abs(work[0]))
        work = num.zeros((lwork,),t)
        results = lapack_routine('N', 'V', n, a, n, w,
                                  dummy, 1, v, n, work, lwork, rwork, 0)
    else:
        lapack_routine = lapack_lite2.dgeev
        wr = num.zeros((n,), t)
        wi = num.zeros((n,), t)
        vr = num.zeros((n,n), t)
        lwork = 1
        work = num.zeros((lwork,),t)
        results = lapack_routine('N', 'V', n, a, n, wr, wi,
                                  dummy, 1, vr, n, work, -1, 0)
        lwork = int(work[0])
        work = num.zeros((lwork,),t)
        results = lapack_routine('N', 'V', n, a, n, wr, wi,
                                  dummy, 1, vr, n, work, lwork, 0)
        if num.logical_and.reduce(num.equal(wi, 0.)):
            w = wr
            v = vr
        else:
            w = wr+1j*wi
            v = num.array(vr,type=num.Complex)
            ind = num.nonzero(
                          num.equal(
                              num.equal(wi,0.0) # true for real e-vals
                                       ,0)          # true for complex e-vals
                                 )                  # indices of complex e-vals
            for i in range(len(ind)/2):
                v[ind[2*i]] = vr[ind[2*i]] + 1j*vr[ind[2*i+1]]
                v[ind[2*i+1]] = vr[ind[2*i]] - 1j*vr[ind[2*i+1]]
    if results['info'] > 0:
        raise LinAlgError, 'Eigenvalues did not converge'
    return w,v


def Heigenvectors(a, UPLO='L'):
    _assertRank2(a)
    _assertSquareness(a)
    t =_commonType(a)
    real_t = _array_type[0][_array_precision[t]]
    a = _castCopyAndTranspose(t, a)
    n = a.getshape()[0]
    liwork = 5*n+3
    iwork = num.zeros((liwork,),'l')
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zheevd
        w = num.zeros((n,), real_t)
        lwork = 1
        work = num.zeros((lwork,), t)
        lrwork = 1
        rwork = num.zeros((lrwork,),real_t)
        results = lapack_routine('V', UPLO, n, a, n,w, work, -1, rwork, -1, iwork, liwork,  0)
        lwork = int(abs(work[0]))
        work = num.zeros((lwork,), t)
        lrwork = int(rwork[0])
        rwork = num.zeros((lrwork,),real_t)
        results = lapack_routine('V', UPLO, n, a, n,w, work, lwork, rwork, lrwork, iwork, liwork,  0)
    else:
        lapack_routine = lapack_lite2.dsyevd
        w = num.zeros((n,), t)
        lwork = 1
        work = num.zeros((lwork,),t)
        results = lapack_routine('V', UPLO, n, a, n,w, work, -1, iwork, liwork, 0)
        lwork = int(work[0])
        work = num.zeros((lwork,),t)
        results = lapack_routine('V', UPLO, n, a, n,w, work, lwork, iwork, liwork, 0)
    if results['info'] > 0:
        raise LinAlgError, 'Eigenvalues did not converge'
    return (w,a)

   
# Singular value decomposition

def singular_value_decomposition(a, full_matrices = 0):
    _assertRank2(a)
    n = a.getshape()[1]
    m = a.getshape()[0]
    t =_commonType(a)
    real_t = _array_type[0][_array_precision[t]]
    a = _fastCopyAndTranspose(t, a)
    if full_matrices:
	nu = m
	nvt = n
	option = 'A'
    else:
	nu = min(n,m)
	nvt = min(n,m)
	option = 'S'
    s = num.zeros((min(n,m),), real_t)
    u = num.zeros((nu, m), t)
    vt = num.zeros((n, nvt), t)
    iwork = num.zeros((8*min(m,n),), 'l')
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zgesdd
        rwork = num.zeros((5*min(m,n)*min(m,n) + 5*min(m,n),), real_t)
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine(option, m, n, a, m, s, u, m, vt, nvt,
                                 work, -1, rwork, iwork, 0)
        lwork = int(abs(work[0]))
        work = num.zeros((lwork,), t)
        results = lapack_routine(option, m, n, a, m, s, u, m, vt, nvt,
                                 work, lwork, rwork, iwork, 0)
    else:
        lapack_routine = lapack_lite2.dgesdd
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine(option, m, n, a, m, s, u, m, vt, nvt,
                                 work, -1, iwork, 0)
        lwork = int(work[0])
        work = num.zeros((lwork,), t)
        results = lapack_routine(option, m, n, a, m, s, u, m, vt, nvt,
                                 work, lwork, iwork, 0)
    if results['info'] > 0:
        raise LinAlgError, 'SVD did not converge'
    return multiarray.transpose(u), s, multiarray.transpose(vt) # why copy here?


# Generalized inverse

def generalized_inverse(a, rcond = 1.e-10):
    u, s, vt = singular_value_decomposition(a, 0)
    m = u.getshape()[0]
    n = vt.getshape()[1]
    cutoff = rcond*num.maximum.reduce(s)
    for i in range(min(n,m)):
        if s[i] > cutoff:
            s[i] = 1./s[i]
	else:
	    s[i] = 0.;
    return num.dot(num.transpose(vt),
		       s[:, num.NewAxis]*num.transpose(u))

# Determinant

def determinant(a):
    """determinant(a) -> ||a||
 
    *a* may be either rank-2 or rank-3. If it is rank-2, it must square. 
    
    >>> A = [[1,2,3], [3,4,5], [5,6,7]]
    >>> _isClose(determinant(A), 0)
    1
    
    If *a* is rank-3, it is treated as an array of rank-2 matrices and
    must be square along the last 2 axes.
 
    >>> A = [[[1, 3], [2j, 3j]], [[2, 4], [4j, 4j]], [[3, 5], [6j, 5j]]]
    >>> _isClose(determinant(A), [-3j, -8j, -15j])
    1

    If *a* is not square along its last two axes, a LinAlgError is raised.
    
    >>> determinant(na.asarray(A)[...,:1])
    Traceback (most recent call last):
       ...
    LinearAlgebraError: Array (or it submatrices) must be square
    
    """
    a = na.asarray(a)
    _assertRank((2,3), a)
    _assertSubmatrixSquareness(a)
    stretched = (len(a.shape) == 2)
    if stretched:
        a = a[na.NewAxis,]
    t = _commonType(a)
    a = _castCopyAndTranspose(t, a, indices=(0,2,1))
    n_cases, n = a.shape[:2]
    if _array_kind[t] == 1:
        lapack_routine = lapack_lite2.zgetrf
    else:
        lapack_routine = lapack_lite2.dgetrf
    no_pivoting = na.arrayrange(1, n+1)
    pivots = na.zeros((n,), 'l')
    all_pivots = na.zeros((n_cases, n,), 'l')
    sum , not_equal = na.sum, na.not_equal
    stride = n * n * a.itemsize()
    pivots_stride = n * pivots.itemsize()
    view = a[0].view()
    view_pivots = all_pivots[0]
    a_i = view.copy()
    for i in range(n_cases):
        if i:
            a_i._copyFrom(view)
        outcome = lapack_routine(n, n, a_i, n, pivots, 0)
        view_pivots._copyFrom(pivots)
        view._copyFrom(a_i)
        view._byteoffset += stride
        view_pivots._byteoffset += pivots_stride
    signs = na.where(sum(not_equal(all_pivots, no_pivoting), 1) % 2, -1, 1).astype(t)
    for i in range(n):
        signs *= a[:,i,i]
    if stretched:
        signs = signs[0]
    return signs


# Linear Least Squares 
        
def linear_least_squares(a, b, rcond=1.e-10):
    """solveLinearLeastSquares(a,b) returns x,resids,rank,s 
where x minimizes 2-norm(|b - Ax|) 
      resids is the sum square residuals
      rank is the rank of A
      s is an rank of the singual values of A in desending order

If b is a matrix then x is also a matrix with corresponding columns.
If the rank of A is less than the number of columns of A or greater than
the numer of rows, then residuals will be returned as an empty array
otherwise resids = sum((b-dot(A,x)**2).
Singular values less than s[0]*rcond are treated as zero.
"""
    one_eq = len(b.getshape()) == 1
    if one_eq:
        b = b[:, num.NewAxis]
    _assertRank2(a, b)
    m  = a.getshape()[0]
    n  = a.getshape()[1]
    n_rhs = b.getshape()[1]
    ldb = max(n,m)
    if m != b.getshape()[0]:
        raise LinAlgError, 'Incompatible dimensions'
    t =_commonType(a, b)
    real_t = _array_type[0][_array_precision[t]]
    bstar = num.zeros((ldb,n_rhs),t)
    bstar[:b.getshape()[0],:n_rhs] = copy.copy(b)
    a,bstar = _castCopyAndTranspose(t, a, bstar)
    s = num.zeros((min(m,n),),real_t)
    nlvl = max( 0, int( math.log( float(min( m,n ))/2. ) ) + 1 )
    iwork = num.zeros((3*min(m,n)*nlvl+11*min(m,n),), 'l')
    if _array_kind[t] == 1: # Complex routines take different arguments
        lapack_routine = lapack_lite2.zgelsd
        lwork = 1
        rwork = num.zeros((lwork,), real_t)
        work = num.zeros((lwork,),t)
        results = lapack_routine( m, n, n_rhs, a, m, bstar,ldb , s, rcond,
                        0,work,-1,rwork,iwork,0 )
        lwork = int(abs(work[0]))
        rwork = num.zeros((lwork,),real_t)
        a_real = num.zeros((m,n),real_t)
        bstar_real = num.zeros((ldb,n_rhs,),real_t)
        results = lapack_lite2.dgelsd( m, n, n_rhs, a_real, m, bstar_real,ldb , s, rcond,
                        0,rwork,-1,iwork,0 )
        lrwork = int(rwork[0])
        work = num.zeros((lwork,), t)
        rwork = num.zeros((lrwork,), real_t)
        results = lapack_routine( m, n, n_rhs, a, m, bstar,ldb , s, rcond,
                        0,work,lwork,rwork,iwork,0 )
    else:
        lapack_routine = lapack_lite2.dgelsd
        lwork = 1
        work = num.zeros((lwork,), t)
        results = lapack_routine( m, n, n_rhs, a, m, bstar,ldb , s, rcond,
                        0,work,-1,iwork,0 )
        lwork = int(work[0])
        work = num.zeros((lwork,), t)
        results = lapack_routine( m, n, n_rhs, a, m, bstar,ldb , s, rcond,
                        0,work,lwork,iwork,0 )
    if results['info'] > 0:
        raise LinAlgError, 'SVD did not converge in Linear Least Squares'
    resids = num.array([],type=t)
    if one_eq:
        x = copy.copy(num.ravel(bstar)[:n])
        if (results['rank']==n) and (m>n):
            resids = num.array([num.sum((num.ravel(bstar)[n:])**2)])
    else:
        x = copy.copy(num.transpose(bstar)[:n,:])
        if (results['rank']==n) and (m>n):
            resids = copy.copy(num.sum((num.transpose(bstar)[n:,:])**2))
    return x,resids,results['rank'],copy.copy(s[:min(n,m)]) 


def test():
	import doctest, dtest, LinearAlgebra2
	return doctest.testmod(dtest), doctest.testmod(LinearAlgebra2)

if __name__ == '__main__':
    test()
