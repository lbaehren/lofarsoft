import numarray.numeric as num
import _correlate
import numarray.ufunc as _ufunc
import numarray.fft as fft
import numarray.numerictypes as _nt
import iraf_frame

VALID = 0
SAME  = 1
FULL  = 2
PASS =  3

convolution_modes = {
    "valid":0,
    "same":1,
    "full":2,
    "pass":3,
    }

def _condition_inputs(data, kernel):
    data, kernel = num.asarray(data), num.asarray(kernel)
    if data.rank == 0:
        data.shape = (1,)
    if kernel.rank == 0:
        kernel.shape = (1,)
    if data.rank > 1 or kernel.rank > 1:
        raise ValueError("arrays must be 1D")
    if len(data) < len(kernel):
        data, kernel = kernel, data
    return data, kernel

def correlate(data, kernel, mode=FULL):
    """correlate(data, kernel, mode=FULL)

    >>> correlate(num.arange(8), [1, 2], mode=VALID)
    array([ 2,  5,  8, 11, 14, 17, 20])
    >>> correlate(num.arange(8), [1, 2], mode=SAME)
    array([ 0,  2,  5,  8, 11, 14, 17, 20])
    >>> correlate(num.arange(8), [1, 2], mode=FULL)
    array([ 0,  2,  5,  8, 11, 14, 17, 20,  7])
    >>> correlate(num.arange(8), [1, 2, 3], mode=VALID)
    array([ 8, 14, 20, 26, 32, 38])
    >>> correlate(num.arange(8), [1, 2, 3], mode=SAME)
    array([ 3,  8, 14, 20, 26, 32, 38, 20])
    >>> correlate(num.arange(8), [1, 2, 3], mode=FULL)
    array([ 0,  3,  8, 14, 20, 26, 32, 38, 20,  7])
    >>> correlate(num.arange(8), [1, 2, 3, 4, 5, 6], mode=VALID)
    array([ 70,  91, 112])
    >>> correlate(num.arange(8), [1, 2, 3, 4, 5, 6], mode=SAME)
    array([ 17,  32,  50,  70,  91, 112,  85,  60])
    >>> correlate(num.arange(8), [1, 2, 3, 4, 5, 6], mode=FULL)
    array([  0,   6,  17,  32,  50,  70,  91, 112,  85,  60,  38,  20,   7])
    >>> correlate(num.arange(8), 1+1j)
    Traceback (innermost last):
    ...
    TypeError: function doesn't support complex arrays.
    """
    data, kernel = _condition_inputs(data, kernel)
    lenk = len(kernel)
    halfk = int(lenk/2)
    even = (lenk % 2 == 0)
    kdata = [0] * lenk

    if mode in convolution_modes.keys():
        mode = convolution_modes[ mode ]

    result_type = max(kernel._type, data._type)
    
    if mode == VALID:
        wdata = num.concatenate((kdata, data, kdata))
        result = wdata.astype(result_type)
        _correlate.Correlate1d(kernel, wdata, result)
        return result[lenk+halfk:-lenk-halfk+even]
    elif mode == SAME:
        wdata = num.concatenate((kdata, data, kdata))
        result = wdata.astype(result_type)
        _correlate.Correlate1d(kernel, wdata, result)
        return result[lenk:-lenk]
    elif mode == FULL:
        wdata = num.concatenate((kdata, data, kdata))
        result = wdata.astype(result_type)
        _correlate.Correlate1d(kernel, wdata, result)
        return result[halfk+1:-halfk-1+even]
    elif mode == PASS:
        result = data.astype(result_type)
        _correlate.Correlate1d(kernel, data, result)
        return result
    else:
        raise ValueError("Invalid convolution mode.") 

cross_correlate = correlate

pix_modes = {
    "nearest" : 0,
    "reflect": 1,
    "wrap" : 2,
    "constant": 3
    }

def convolve(data, kernel, mode=FULL):
    """convolve(data, kernel, mode=FULL)
    Returns the discrete, linear convolution of 1-D
    sequences a and v; mode can be 0 (VALID), 1 (SAME), or 2 (FULL)
    to specify size of the resulting sequence.

    >>> convolve(num.arange(8), [1, 2], mode=VALID)
    array([ 1,  4,  7, 10, 13, 16, 19])
    >>> convolve(num.arange(8), [1, 2], mode=SAME)
    array([ 0,  1,  4,  7, 10, 13, 16, 19])
    >>> convolve(num.arange(8), [1, 2], mode=FULL)
    array([ 0,  1,  4,  7, 10, 13, 16, 19, 14])
    >>> convolve(num.arange(8), [1, 2, 3], mode=VALID)
    array([ 4, 10, 16, 22, 28, 34])
    >>> convolve(num.arange(8), [1, 2, 3], mode=SAME)
    array([ 1,  4, 10, 16, 22, 28, 34, 32])
    >>> convolve(num.arange(8), [1, 2, 3], mode=FULL)
    array([ 0,  1,  4, 10, 16, 22, 28, 34, 32, 21])
    >>> convolve(num.arange(8), [1, 2, 3, 4, 5, 6], mode=VALID)
    array([35, 56, 77])
    >>> convolve(num.arange(8), [1, 2, 3, 4, 5, 6], mode=SAME)
    array([ 4, 10, 20, 35, 56, 77, 90, 94])
    >>> convolve(num.arange(8), [1, 2, 3, 4, 5, 6], mode=FULL)
    array([ 0,  1,  4, 10, 20, 35, 56, 77, 90, 94, 88, 71, 42])
    >>> convolve([1,2], num.arange(10.))
    array([  0.,   1.,   4.,   7.,  10.,  13.,  16.,  19.,  22.,  25.,  18.])
    """
    data, kernel = _condition_inputs(data, kernel)
    if len(data) >= len(kernel):
        return correlate(data, kernel[::-1], mode)
    else:
        return correlate(kernel, data[::-1], mode)


def _gaussian(sigma, mew, npoints, sigmas):
    ox = num.arange(mew-sigmas*sigma,
                         mew+sigmas*sigma,
                         2*sigmas*sigma/npoints, type=num.Float64)
    x = ox-mew
    x /= sigma
    x = x * x 
    x *= -1/2
    x = num.exp(x)
    return ox, 1/(sigma * num.sqrt(2*num.pi)) * x

def _correlate2d_fft(data0, kernel0, output=None, mode="nearest", cval=0.0):
    """_correlate2d_fft does 2d correlation of 'data' with 'kernel', storing
    the result in 'output' using the FFT to perform the correlation.

    supported 'mode's include:
        'nearest'   elements beyond boundary come from nearest edge pixel.
        'wrap'      elements beyond boundary come from the opposite array edge.
        'reflect'   elements beyond boundary come from reflection on same array edge.
        'constant'  elements beyond boundary are set to 'cval' 
    """
    shape = data0.shape
    kshape = kernel0.shape
    oversized = (num.array(shape) + num.array(kshape))

    dy = kshape[0] // 2
    dx = kshape[1] // 2

    kernel = num.zeros(oversized, typecode=num.Float64)
    kernel[:kshape[0], :kshape[1]] = kernel0[::-1,::-1]   # convolution <-> correlation
    data = iraf_frame.frame(data0, oversized, mode=mode, cval=cval)

    complex_result = (isinstance(data, _nt.ComplexType) or 
                      isinstance(kernel, _nt.ComplexType))

    Fdata = fft.fft2d(data)
    del data
    
    Fkernel = fft.fft2d(kernel)
    del kernel
    
    num.multiply(Fdata, Fkernel, Fdata)
    del Fkernel

    if complex_result:
        convolved = fft.inverse_fft2d( Fdata, s=oversized)
    else:
        convolved = fft.inverse_real_fft2d( Fdata, s=oversized)
        
    result = convolved[ kshape[0]-1:shape[0]+kshape[0]-1, kshape[1]-1:shape[1]+kshape[1]-1 ]

    if output is not None:
        output._copyFrom( result )
    else:
        return result

                     
def _correlate2d_naive(data, kernel, output=None, mode="nearest", cval=0.0):
    return _correlate.Correlate2d(kernel, data, output, pix_modes[mode], cval)

def _fix_data_kernel(data, kernel):
    """The _correlate.Correlate2d C-code can only handle kernels which
    fit inside the data array.  Since convolution and correlation are
    commutative, _fix_data_kernel reverses kernel and data if necessary
    and panics if there's no good order.
    """
    data, kernel = map(num.inputarray, [data, kernel])
    if data.rank == 0:
        data.shape = (1,1)
    elif data.rank == 1:
        data.shape = (1,) + data.shape
    if kernel.rank == 0:
        kernel.shape = (1,1)
    elif kernel.rank == 1:
        kernel.shape = (1,) + kernel.shape
    if (kernel.getshape()[0] > data.getshape()[0] and
        kernel.getshape()[1] > data.getshape()[1]):
        kernel, data = data, kernel
    elif (kernel.getshape()[0] <= data.getshape()[0] and
          kernel.getshape()[1] <= data.getshape()[1]):
        pass
    return data, kernel

def correlate2d(data, kernel, output=None, mode="nearest", cval=0.0, fft=0):
    """correlate2d does 2d correlation of 'data' with 'kernel', storing
    the result in 'output'.

    supported 'mode's include:
        'nearest'   elements beyond boundary come from nearest edge pixel.
        'wrap'      elements beyond boundary come from the opposite array edge.
        'reflect'   elements beyond boundary come from reflection on same array edge.
        'constant'  elements beyond boundary are set to 'cval'

    If fft is True,  the correlation is performed using the FFT, else the
    correlation is performed using the naive approach.

    >>> a = num.arange(20*20., shape=(20,20))
    >>> b = num.ones((5,5), typecode='Float64')
    >>> rn = correlate2d(a, b, fft=0)
    >>> rf = correlate2d(a, b, fft=1)
    >>> num.alltrue(num.ravel(rn-rf<1e-10))
    1
    """
    data, kernel = _fix_data_kernel(data, kernel)
    if fft:
        return _correlate2d_fft(data, kernel, output, mode, cval)
    else:
        return _correlate2d_naive(data, kernel, output, mode, cval)

def convolve2d(data, kernel, output=None, mode="nearest", cval=0.0, fft=0):
    """convolve2d does 2d convolution of 'data' with 'kernel', storing
    the result in 'output'.

    supported 'mode's include:
        'nearest'   elements beyond boundary come from nearest edge pixel.
        'wrap'      elements beyond boundary come from the opposite array edge.
        'reflect'   elements beyond boundary come from reflection on same array edge.
        'constant'  elements beyond boundary are set to 'cval'

    >>> a = num.arange(20*20., shape=(20,20))
    >>> b = num.ones((5,5), typecode='Float64')
    >>> rn = convolve2d(a, b, fft=0)
    >>> rf = convolve2d(a, b, fft=1)
    >>> num.alltrue(num.ravel(rn-rf<1e-10))
    1
    """
    data, kernel = _fix_data_kernel(data, kernel)    
    kernel = kernel[::-1,::-1] # convolution -> correlation
    if fft:
        return _correlate2d_fft(data, kernel, output, mode, cval)
    else:
        return _correlate2d_naive(data, kernel, output, mode, cval)

def _boxcar(data, output, boxshape, mode, cval):
    if len(boxshape) == 1:
        _correlate.Boxcar2d(data[num.NewAxis,...], 1, boxshape[0],
                            output[num.NewAxis,...], mode, cval)
    elif len(boxshape) == 2:
        _correlate.Boxcar2d(data, boxshape[0], boxshape[1], output, mode, cval)
    else:
        raise ValueError("boxshape must be a 1D or 2D shape.")

def boxcar(data, boxshape, output=None, mode="nearest", cval=0.0):
    """boxcar computes a 1D or 2D boxcar filter on every 1D or 2D subarray of data.
    
    'boxshape' is a tuple of integers specifying the dimensions of the filter: e.g. (3,3)

    if 'output' is specified, it should be the same shape as 'data' and 
    None will be returned.

    supported 'mode's include:
        'nearest'   elements beyond boundary come from nearest edge pixel.
        'wrap'      elements beyond boundary come from the opposite array edge.
        'reflect'   elements beyond boundary come from reflection on same array edge.
        'constant'  elements beyond boundary are set to 'cval'

    >>> boxcar(num.array([10, 0, 0, 0, 0, 0, 1000]), (3,), mode="nearest").astype('Long')
    array([  6,   3,   0,   0,   0, 333, 666])
    >>> boxcar(num.array([10, 0, 0, 0, 0, 0, 1000]), (3,), mode="wrap").astype('Long')
    array([336,   3,   0,   0,   0, 333, 336])
    >>> boxcar(num.array([10, 0, 0, 0, 0, 0, 1000]), (3,), mode="reflect").astype('Long')
    array([  6,   3,   0,   0,   0, 333, 666])
    >>> boxcar(num.array([10, 0, 0, 0, 0, 0, 1000]), (3,), mode="constant").astype('Long')
    array([  3,   3,   0,   0,   0, 333, 333])
    >>> a = num.zeros((10,10))
    >>> a[0,0] = 100
    >>> a[5,5] = 1000
    >>> a[9,9] = 10000
    >>> boxcar(a, (3,3)).astype('Long')
    array([[  44,   22,    0,    0,    0,    0,    0,    0,    0,    0],
           [  22,   11,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0, 1111, 2222],
           [   0,    0,    0,    0,    0,    0,    0,    0, 2222, 4444]])
    >>> boxcar(a, (3,3), mode="wrap").astype('Long')
    array([[1122,   11,    0,    0,    0,    0,    0,    0, 1111, 1122],
           [  11,   11,    0,    0,    0,    0,    0,    0,    0,   11],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [1111,    0,    0,    0,    0,    0,    0,    0, 1111, 1111],
           [1122,   11,    0,    0,    0,    0,    0,    0, 1111, 1122]])
    >>> boxcar(a, (3,3), mode="reflect").astype('Long')
    array([[  44,   22,    0,    0,    0,    0,    0,    0,    0,    0],
           [  22,   11,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
           [   0,    0,    0,    0,    0,    0,    0,    0, 1111, 2222],
           [   0,    0,    0,    0,    0,    0,    0,    0, 2222, 4444]])
   >>> boxcar(a, (3,3), mode="constant").astype('Long')
   array([[  11,   11,    0,    0,    0,    0,    0,    0,    0,    0],
          [  11,   11,    0,    0,    0,    0,    0,    0,    0,    0],
          [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
          [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
          [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
          [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
          [   0,    0,    0,    0,  111,  111,  111,    0,    0,    0],
          [   0,    0,    0,    0,    0,    0,    0,    0,    0,    0],
          [   0,    0,    0,    0,    0,    0,    0,    0, 1111, 1111],
          [   0,    0,    0,    0,    0,    0,    0,    0, 1111, 1111]])

    >>> a = num.zeros((10,10))
    >>> a[3:6,3:6] = 111
    >>> boxcar(a, (3,3)).astype('Long')
    array([[  0,   0,   0,   0,   0,   0,   0,   0,   0,   0],
           [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0],
           [  0,   0,  12,  24,  37,  24,  12,   0,   0,   0],
           [  0,   0,  24,  49,  74,  49,  24,   0,   0,   0],
           [  0,   0,  37,  74, 111,  74,  37,   0,   0,   0],
           [  0,   0,  24,  49,  74,  49,  24,   0,   0,   0],
           [  0,   0,  12,  24,  37,  24,  12,   0,   0,   0],
           [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0],
           [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0],
           [  0,   0,   0,   0,   0,   0,   0,   0,   0,   0]])
    """
    mode = pix_modes[ mode ]
    if output is None:
        woutput = data.astype(num.Float64)
    else:
        woutput = output
    _ufunc._fbroadcast(_boxcar, len(boxshape), data.getshape(),
                      (data, woutput), (boxshape, mode, cval))
    if output is None:
        return woutput

def test():
    import doctest, Convolve
    return doctest.testmod(Convolve)

if __name__ == "__main__":
    print test()
