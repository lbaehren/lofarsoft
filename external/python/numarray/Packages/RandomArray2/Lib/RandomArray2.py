import ranlib2 as ranlib
import numarray.numeric as num
import numarray.linear_algebra as linalg
import sys
import math
import types as _types
import numarray.generic as _gen

__doc__ = """Random number array generators.

This module provides functions to generate numarray of random numbers.
"""

# Extended RandomArray to provide more distributions:
# normal, beta, chi square, F, multivariate normal,
# exponential, binomial, multinomial
# Lee Barford, Dec. 1999.

# Extended most functions to accept arrays or sequences as 
# arguments, with broadcasting when necessary.
# Peter J. Verveer, 2003.

ArgumentError = "ArgumentError"

def seed(x=0,y=0):
    """Set the RNG seed using the integers x, y; 
    If |y| == 0 set a random one from clock.
    """
    if type (x) != _types.IntType or type (y) != _types.IntType :
        raise ArgumentError, "seed requires integer arguments."
    if y == 0:
        import time
        t = time.time()*10**4
        ndigits = int(math.log10(t))
        base = 10**(ndigits/2)
        x = int(t/base)
        y = 1 + int(t%base)
    ranlib.set_seeds(x,y)

seed()

def get_seed():
    """Return the current seed pair"""
    return ranlib.get_seeds()

def _broadcast_arguments(args, shape):
    """If there are no arrays/sequences in the args tuple, return args
    and shape unchanged. Otherwise: if shape is empty, broadcast all
    arrays/sequences in the arguments to a common shape, and return
    the result together with the broadcasted shape. If shape is not
    empty, broadcast all arrays/sequences in the arguments to the
    given shape and return these together with the unchanged
    shape. Scalar arguments are never changed.
    """
    if isinstance(shape, _types.IntType): 
        shape = [shape]
    array_args = []
    for arg in args:
        if (isinstance(arg, num.ArrayType) or
            isinstance(arg, _types.ListType) or
            isinstance(arg, _types.TupleType)):
            array_args.append(num.array(arg))
    if len(array_args) > 0:
        if len(shape) == 0:
            array_args = _gen._nWayBroadcast(array_args)
            shape = array_args[0].shape
        else:
            for ii in range(len(array_args)):
                array_args[ii] = _gen._broadcast(num.array(array_args[ii]), shape) 
        new_args = []
        for arg in args:
            if (isinstance(arg, num.ArrayType) or
                isinstance(arg, _types.ListType) or
                isinstance(arg, _types.TupleType)):
                new_args.append(array_args.pop(0))
            else:
                new_args.append(arg)
            args = tuple(new_args)
    return args, shape

def _build_random_array(fun, args, shape=[]):
    """Build an array by applying function |fun| to the arguments in
    |args|, creating an array with the specified shape.
    Allows an integer shape n as a shorthand for (n,).
    """
    args, shape = _broadcast_arguments(args, shape)
    if len(shape) != 0:
        n = num.multiply.reduce(shape)
        s = apply(fun, args + (n,))
        s.setshape(shape)
        return s
    else:
        n = 1
        s = apply(fun, args + (n,))
        return s[0]

def random(shape=[]):
    """random(n) or random([n, m, ...])

    Returns array of random numbers in the range 0.0 to 1.0.
    """
    return _build_random_array(ranlib.sample, (), shape)

def uniform(minimum, maximum, shape=[]):
    """uniform([minimum,], maximum[, shape])

    Return array with shape |shape| of random Floats with all values
    > minimum and < maximum.    
    """
    (minimum, maximum), shape = _broadcast_arguments((minimum, maximum), shape)
    return minimum + (maximum-minimum)*random(shape)

def randint(minimum, maximum=None, shape=[]):
    """randint([minimum,] maximum[, shape])

    Return random integers >= mininimum, < maximum;
    if only one parameter specified, return random integers >= 0, < maximum.
    """
    if maximum is None:
        maximum, minimum = minimum, 0
    a = num.floor(uniform(minimum, maximum, shape))
    if isinstance(a, num.ArrayType):
        return a.astype(num.Int32)
    else:
        return int(a)
     
def random_integers(maximum, minimum=1, shape=[]):
    """Return array of random integers in interval [minimum, maximum]

    Note that this function has reversed arguments. It is simply a
    redirection through randint, and use of that function (randint) is
    suggested.
    """
    return randint(minimum, maximum+1, shape) 
     
def permutation(n):
    """A permutation of indices range(n)"""
    return num.argsort(random(n))

def standard_normal(shape=[]):
    """standard_normal(n) or standard_normal([n, m, ...])

    Returns array of random numbers normally distributed with mean 0
    and standard deviation 1.
    """
    return _build_random_array(ranlib.standard_normal, (), shape)

def normal(mean, std, shape=[]):
    """normal(mean, std, n) or normal(mean, std, [n, m, ...])

    Returns array of random numbers randomly distributed with
    specified mean and standard deviation
    """
    (mean, std), shape = _broadcast_arguments((mean, std), shape)
    s = standard_normal(shape)
    return s * std + mean

def multivariate_normal(mean, cov, shape=[]):
    """multivariate_normal(mean, cov) or multivariate_normal(mean, cov, [m, n, ...])

    Returns an array containing multivariate normally distributed
    random numbers with specified mean and covariance.

    |mean| must be a one-dimensional array. |cov| must be a square
    two-dimensional array with the same number of rows and columns as
    |mean| has elements.

    The first form returns a single 1-D array containing a
    multivariate normal.

    The second form returns an array of shape (m, n, ...,
    cov.getshape()[0]). In this case, output[i,j,...,:] is a 1-D array
    containing a multivariate normal.
    """
    # Check preconditions on arguments
    mean = num.array(mean)
    cov = num.array(cov)
    if len(mean.getshape()) != 1:
        raise ArgumentError, "mean must be 1 dimensional."
    if (len(cov.getshape()) != 2) or (cov.getshape()[0] != cov.getshape()[1]):
        raise ArgumentError, "cov must be 2 dimensional and square."
    if mean.getshape()[0] != cov.getshape()[0]:
        raise ArgumentError, "mean and cov must have same length."
    # Compute shape of output
    if isinstance(shape, _types.IntType): shape = [shape]
    final_shape = list(shape[:])
    final_shape.append(mean.getshape()[0])
    # Create a matrix of independent standard normally distributed random
    # numbers. The matrix has rows with the same length as mean and as
    # many rows are necessary to form a matrix of shape final_shape.
    x = ranlib.standard_normal(num.multiply.reduce(final_shape))
    x.setshape(num.multiply.reduce(final_shape[0:len(final_shape)-1]),
               mean.getshape()[0])
    # Transform matrix of standard normals into matrix where each row
    # contains multivariate normals with the desired covariance.
    # Compute A such that matrixmultiply(transpose(A),A) == cov.
    # Then the matrix products of the rows of x and A has the desired
    # covariance. Note that sqrt(s)*v where (u,s,v) is the singular value
    # decomposition of cov is such an A.
    (u,s,v) = linalg.singular_value_decomposition(cov)
    x = num.matrixmultiply(x*num.sqrt(s),v)
    # The rows of x now have the correct covariance but mean 0. Add
    # mean to each row. Then each row will have mean mean.
    num.add(mean,x,x)
    x.setshape(final_shape)
    return x

def exponential(mean, shape=[]):
    """exponential(mean, n) or exponential(mean, [n, m, ...])

    Returns array of random numbers exponentially distributed with
    specified mean
    """
    # If U is a random number uniformly distributed on [0,1], then
    #      -ln(U) is exponentially distributed with mean 1, and so
    #      -ln(U)*M is exponentially distributed with mean M.
    (mean,), shape = _broadcast_arguments((mean,), shape)
    x = random(shape)
    num.log(x, x)
    num.subtract(0.0, x, x)
    num.multiply(mean, x, x)
    return x

def beta(a, b, shape=[]):
    """beta(a, b) or beta(a, b, [n, m, ...]) returns array of beta distributed random numbers."""
    return _build_random_array(ranlib.beta, (a, b), shape)

def gamma(a, r, shape=[]):
    """gamma(a, r) or gamma(a, r, [n, m, ...]) returns array of gamma distributed random numbers."""
    return _build_random_array(ranlib.gamma, (a, r), shape)

def F(dfn, dfd, shape=[]):
    """F(dfn, dfd) or F(dfn, dfd, [n, m, ...])

    Returns array of F distributed random numbers with dfn degrees of
    freedom in the numerator and dfd degrees of freedom in the
    denominator.
    """
    (dfn, dfd), shape = _broadcast_arguments((dfn, dfd), shape)
    return ( chi_square(dfn, shape) / dfn) / ( chi_square(dfd, shape) / dfd)

def noncentral_F(dfn, dfd, nconc, shape=[]):
    """noncentral_F(dfn, dfd, nonc) or noncentral_F(dfn, dfd, nonc, [n, m, ...])

    Returns array of noncentral F distributed random numbers with dfn
    degrees of freedom in the numerator and dfd degrees of freedom in
    the denominator, and noncentrality parameter nconc.
    """
    (dfn, dfd, nconc), shape = _broadcast_arguments((dfn, dfd, nconc), shape)
    return ( noncentral_chi_square(dfn, nconc, shape) / dfn ) / ( chi_square(dfd, shape) /dfd )

def chi_square(df, shape=[]):
    """chi_square(df) or chi_square(df, [n, m, ...])

    Returns array of chi squared distributed random numbers with df
    degrees of freedom.
    """
    return _build_random_array(ranlib.chisquare, (df,), shape)

def noncentral_chi_square(df, nconc, shape=[]):
    """noncentral_chi_square(df, nconc) or chi_square(df, nconc, [n, m, ...])

    Returns array of noncentral chi squared distributed random numbers
    with df degrees of freedom and noncentrality parameter.
    """
    return _build_random_array(ranlib.noncentral_chisquare, (df, nconc), shape)

def binomial(trials, p, shape=[]):
    """binomial(trials, p) or binomial(trials, p, [n, m, ...])

    Returns array of binomially distributed random integers.

    |trials| is the number of trials in the binomial distribution.
    |p| is the probability of an event in each trial of the binomial
    distribution.
    """
    return _build_random_array(ranlib.binomial, (trials, p), shape)

def negative_binomial(trials, p, shape=[]):
    """negative_binomial(trials, p) or negative_binomial(trials, p, [n, m, ...])

    Returns array of negative binomially distributed random integers.

    |trials| is the number of trials in the negative binomial
    distribution. |p| is the probability of an event in each trial of
    the negative binomial distribution.
    """
    return _build_random_array(ranlib.negative_binomial, (trials, p), shape)

def multinomial(trials, probs, shape=[]):
    """multinomial(trials, probs) or multinomial(trials, probs, [n, m, ...])

    Returns array of multinomial distributed integer vectors.

    |trials| is the number of trials in each multinomial distribution. 
    |probs| is a one dimensional array. There are len(prob)+1 events. 
    prob[i] is the probability of the i-th event, 0<=i<len(prob). The
    probability of event len(prob) is 1.-num.sum(prob).

    The first form returns a single 1-D array containing one
    multinomially distributed vector.

    The second form returns an array of shape (m, n, ..., len(probs)). 
    In this case, output[i,j,...,:] is a 1-D array containing a
    multinomially distributed integer 1-D array.
    """
    # Check preconditions on arguments
    probs = num.array(probs)
    if len(probs.getshape()) != 1:
        raise ArgumentError, "probs must be 1 dimensional."
        # Compute shape of output
    if type(shape) == type(0): shape = [shape]
    final_shape = shape[:]
    final_shape.append(probs.getshape()[0]+1)
    x = ranlib.multinomial(trials, probs.astype(num.Float32),
                           num.multiply.reduce(shape))
    # Change its shape to the desire one
    x.setshape(final_shape)
    return x

def poisson(mean, shape=[]):
    """poisson(mean) or poisson(mean, [n, m, ...])

    Returns array of poisson distributed random integers with specifed
    mean.
    """
    return _build_random_array(ranlib.poisson, (mean,), shape)


from dtest import test
    
if __name__ == '__main__': 
    test()
