import numarray.numeric as num
import numarray.convolve
import numarray.convolve._correlate as _correlate
import numarray.linear_algebra.mlab as MLab

def _translate(a, dx, dy, output=None, mode="nearest", cval=0.0):
    """_translate does positive sub-pixel shifts using bilinear interpolation."""
    
    assert 0 <= dx < 1.0
    assert 0 <= dy < 1.0
    
    w = (1-dy) * (1-dx)
    x = (1-dy) * dx
    y = (1-dx) * dy
    z = dx * dy
    
    kernel = num.array([
        [ z, y ],
        [ x, w ],
        ])
    
    return numarray.convolve.correlate2d(a, kernel, output, mode, cval)

def translate(a, sdx, sdy, output=None, mode="nearest", cval=0.0):
    """translate performs a translation of 'a' by (sdx, sdy)
    storing the result in 'output'.

    sdx, sdy  are float values.

    supported 'mode's include:
        'nearest'   elements beyond boundary come from nearest edge pixel.
        'wrap'      elements beyond boundary come from the opposite array edge.
        'reflect'   elements beyond boundary come from reflection on same array edge.
        'constant'  elements beyond boundary are set to 'cval'

    >>> a=num.arange(25, shape=(5,5), type='d')
    >>> translate(a, 0.5, 0.25)
    array([[  0.  ,   0.5 ,   1.5 ,   2.5 ,   3.5 ],
           [  3.75,   4.25,   5.25,   6.25,   7.25],
           [  8.75,   9.25,  10.25,  11.25,  12.25],
           [ 13.75,  14.25,  15.25,  16.25,  17.25],
           [ 18.75,  19.25,  20.25,  21.25,  22.25]])
    """
    a = num.inputarray(a)
    
    sdx, sdy = -sdx, -sdy     # Flip sign to match IRAF sign convention
    
    # _translate works "backwords" due to implementation of 2x2 correlation.
    if sdx >= 0 and sdy >= 0:
        rotation = 2
        dx, dy = abs(sdx), abs(sdy)
    elif sdy < 0 and sdx >= 0:
        rotation = 1
        dx, dy = abs(sdy), abs(sdx)
    elif sdx < 0 and sdy >= 0:
        rotation = 3
        dx, dy = abs(sdy), abs(sdx)
    elif sdx < 0 and sdy < 0:
        rotation = 0
        dx, dy = abs(sdx), abs(sdy)

    b = MLab.rot90(a, rotation)
    c = _correlate.Shift2d(b, int(dx), int(dy),
                           mode=numarray.convolve.pix_modes[mode])
    d = _translate(c, dx % 1, dy % 1, output, mode, cval)
    if output is not None:
        output._copyFrom(MLab.rot90(output, -rotation%4))
    else:
        return MLab.rot90(d, -rotation % 4).astype(a.type())

