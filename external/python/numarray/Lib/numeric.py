from numarrayall import *
import libnumeric

numarray_nonzero = nonzero
numarray_put = put
numarray_putmask = putmask
numarray_take = take
numarray_ones = ones
numarray_zeros = zeros

def nonzero(a):
    """nonzero() returns either an index array or a tuple of index
    arrays corresponding to the indices where the values of an array
    are not equal to zero.

    This is the Numeric compatible version of nonzero() supplied by
    numarray.numeric.  It differs from numarray.nonzero() in the
    handling of 1D arrays.  numarray.nonzero() always returns a tuple;
    Numeric.nonzero() returns a tuple when the number of dimensions of
    the array is not one.
    """
    nz = numarray_nonzero(a)
    if len(nz) > 1:
        return nz
    elif len(nz) == 1:
        return nz[0]
    else:
        return nz

def ones(shape, typecode='l', savespace=0):
    return numarray_ones(shape, typecode)

def zeros(shape, typecode='l', savespace=0):
    return numarray_zeros(shape, typecode)

arraytype = NumArray

from libnumeric import take, histogram # , transpose

def repeat(a, repeats, axis=0):
    """repeat elements of a repeats times along axis
       repeats is a sequence of length a.shape[axis]
       telling how many times to repeat each element.
       If repeats is an integer, it is interpreted as
       a tuple of length a.shape[axis] containing repeats.
       The argument a can be anything array(a) will accept.
    """
    a = array(a, copy=0)
    s = a.shape
    if isinstance(repeats, types.IntType):
        repeats = tuple([repeats]*(s[axis]))
    if len(repeats) != s[axis]:
        raise ValueError, "repeat requires second argument integer or of length of a.shape[axis]."
    d = libnumeric.repeat(a, repeats, axis)
    return d

#add extra intelligence to the basic C functions
def concatenate(a, axis=0):
    """concatenate(a, axis=0) joins the tuple of sequences in a into a single
    NumPy array.
    """
    if axis == 0:
        return libnumeric.concatenate(a)
    else:
        new_list = []
        for m in a:
            new_list.append(swapaxes(m, axis, 0))
    return swapaxes(libnumeric.concatenate(new_list), axis, 0)

def sort(a, axis=-1):
    """sort(a,axis=-1) returns array with elements sorted along given axis.
    """
    a = array(a, copy=0)
    n = len(a.shape)
    if axis < 0: axis += n
    if axis < 0 or axis >= n:
        raise ValueError, "sort axis argument out of bounds"
    if axis != n-1: a = swapaxes(a, axis, n-1)
    s = libnumeric.sort(a)
    if axis != n-1: s = swapaxes(s, axis, -1)
    return s

def argsort(a, axis=-1):
    """argsort(a,axis=-1) return the indices into a of the sorted array
    along the given axis, so that take(a,result,axis) is the sorted array.
    """
    a = array(a, copy=0)
    n = len(a.shape)
    if axis < 0: axis += n
    if axis < 0 or axis >= n:
        raise ValueError, "argsort axis argument out of bounds"
    if axis != n-1: a = swapaxes(a, axis, n-1)
    s = libnumeric.argsort(a)
    if axis != n-1: s = swapaxes(s, axis, -1)
    return s

def argmax(a, axis=-1):
    """argmax(a,axis=-1) returns the indices to the maximum value of the
    1-D arrays along the given axis.    
    """
    a = array(a, copy=0)
    n = len(a.shape)
    if axis < 0: axis += n
    if axis < 0 or axis >= n:
        raise ValueError, "argmax axis argument out of bounds"
    if axis != n-1: a = swapaxes(a, axis, n-1)
    s = libnumeric.argmax(a)
    if axis != n-1: s = swapaxes(s, axis, -1)
    return s

def argmin(a, axis=-1):
    """argmin(a,axis=-1) returns the indices to the minimum value of the
    1-D arrays along the given axis.    
    """
    a = -array(a, copy=0)
    n = len(a.shape)
    if axis < 0: axis += n
    if axis < 0 or axis >= n:
        raise ValueError, "argmin axis argument out of bounds" 
    if axis != n-1: a = swapaxes(a, axis, n-1)
    s = libnumeric.argmax(a)
    if axis != n-1: s = swapaxes(s, axis, -1)
    return s

def resize(a, new_shape):
    """resize(a,new_shape) returns a new array with the specified shape.
    The original array's total size can be any size.
    """

    a = ravel(a)
    if not len(a): return zeros(new_shape, a.typecode())
    total_size = multiply.reduce(new_shape)
    n_copies = int(total_size / len(a))
    extra = total_size % len(a)

    if extra != 0: 
        n_copies = n_copies+1
        extra = len(a)-extra

    a = concatenate( (a,)*n_copies)
    if extra > 0:
        a = a[:-extra]

    return reshape(a, new_shape)


def put (a, ind, v):
    """put(a, ind, v) results in a[n] = v[n] for all n in ind
       If v is shorter than mask it will be repeated as necessary.
       In particular v can be a scalar or length 1 array.
       The routine put is the equivalent of the following (although the loop 
       is in C for speed): 

           ind = array(indices, copy=0) 
           v = array(values, copy=0).astype(a, typecode()) 
           for i in ind: a.flat[i] = v[i] 
       a must be a contiguous Numeric array.
    """
    libnumeric.put (a, ind, array(v, copy=0).astype(a.typecode()))

def putmask (a, mask, v):
    """putmask(a, mask, v) results in a = v for all places mask is true.
       If v is shorter than mask it will be repeated as necessary.
       In particular v can be a scalar or length 1 array.
    """
    tc = a.typecode()
    mask = asarray(mask).astype(Int)
    v = array(v, copy=0).astype(tc)
    libnumeric.putmask (a, mask, v)
