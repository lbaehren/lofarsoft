"""
>>> case(fft.fft( (0,1)*4 ),
...      Numeric.array([ 4.+0.j,  0.+0.j,  0.+0.j,  0.+0.j,
...                     -4.+0.j, 0.+0.j,  0.+0.j, 0.+0.j]))
OK

>>> case(fft.inverse_fft( fft.fft((0,1)*4)),
...      Numeric.array([ 0.+0.j,  1.+0.j,  0.+0.j,  1.+0.j,
...                      0.+0.j,  1.+0.j,  0.+0.j,  1.+0.j]))
OK

>>> case(fft.fft( (0,1)*4, n=16),
...      Numeric.array([
...     4.00000000e+00+0.j        ,  -1.11022302e-15-2.61312593j,
...     0.00000000e+00+0.j        ,  -1.55431223e-15-1.0823922j ,
...     0.00000000e+00+0.j        ,  -4.44089210e-16-1.0823922j ,
...     0.00000000e+00+0.j        ,  -3.55271368e-15-2.61312593j,
...     -4.00000000e+00+0.j        ,   1.11022302e-15+2.61312593j,
...     0.00000000e+00+0.j        ,   1.55431223e-15+1.0823922j ,
...     0.00000000e+00+0.j        ,   4.44089210e-16+1.0823922j ,
...     0.00000000e+00+0.j        ,   3.55271368e-15+2.61312593j]))
OK
    
>>> case(fft.fft( (0,1)*4, n=4 ),
...      Numeric.array([ 2.+0.j,  0.+0.j, -2.+0.j,  0.+0.j]))
OK
    
>>> case(fft.fft2d( [(0,1),(1,0)] ),
...      Numeric.array([[ 2.+0.j,  0.+0.j],
...                     [ 0.+0.j, -2.+0.j]]))
OK

>>> case(fft.inverse_fft2d (fft.fft2d( [(0, 1), (1, 0)] ) ),
...      Numeric.array([[ 0.+0.j,  1.+0.j],
...                     [ 1.+0.j,  0.+0.j]]))
OK
    
>>> case(fft.real_fft2d([(0,1),(1,0)]),
...      Numeric.array([[ 2.+0.j,  0.+0.j],
...                     [ 0.+0.j, -2.+0.j]]))
OK

>>> case(fft.real_fft2d([(1,1),(1,1)] ),
...      Numeric.array([[ 4.+0.j,  0.+0.j],
...                     [ 0.+0.j,  0.+0.j]]))
OK

    Tests for correctness.
    Somewhat limited since
    p (and thus q also) is real and hermite, and the dimension we're
    testing is a power of 2.  If someone can cook up more general data
    in their head or with another program/library, splice it in!

>>> toler = 1.e-10

>>> oosq2 = 1.0/Numeric.sqrt(2.0)

>>> p = Numeric.array(((1, 1, 1, 1, 1, 1, 1, 1),
...                    (1, oosq2, 0, -oosq2, -1, -oosq2, 0, oosq2),
...                    (1, 0, -1, 0, 1, 0, -1, 0),
...                    (1, -oosq2, 0, oosq2, -1, oosq2, 0, -oosq2),
...                    (1, -1, 1, -1, 1, -1, 1, -1),
...                    (1, 0, 0, 0, 0, 0, 0, 0),
...                    (0, 0, 0, 0, 0, 0, 0, 0)))
>>> q = Numeric.array(((8,0,0,0,0,0,0,0),
...                    (0,4,0,0,0,0,0,4),
...                    (0,0,4,0,0,0,4,0),
...                    (0,0,0,4,0,4,0,0),
...                    (0,0,0,0,8,0,0,0),
...                    (1,1,1,1,1,1,1,1),
...                    (0,0,0,0,0,0,0,0)))

    Correctness testing dimension -1.

>>> P = fft.fft(p)
>>> cndns(P-q) / cndns(q) > toler
0

>>> RP = fft.real_fft(p)
>>> npt = p.getshape()[-1]; rpt = npt/2 + 1
>>> qr = q[:,:rpt];  cndns(RP-qr) / cndns(qr) > toler
0

>>> I = fft.inverse_real_fft(q, npt)
>>> cndns(I-p) / cndns(p) > toler
0

Consistency testing dimension 0, length 7.

>>> dim = 0

FFT

>>> P = fft.fft(p, None, dim)
>>> Q = fft.inverse_fft(P, None, dim)
>>> cndns(Q-p) / cndns(p) > toler
0

Real FFT

>>> RP = fft.real_fft(p, None, dim)
>>> npt = p.getshape()[dim]; rpt = npt/2 + 1
>>> P = Numeric.take(P, range(rpt), axis=dim)
>>> cndns(RP-P) / cndns(RP) > toler
0

Inverse FFT

>>> P = fft.fft(p, None, dim)
>>> Q = fft.inverse_fft(P, None, dim)
>>> cndns(Q-p) / cndns(p) > toler
0

Inverse Hermite FFT

>>> hp = fft.inverse_hermite_fft(q, npt, dim)
>>> Q = fft.inverse_fft(q, None, dim)
>>> Q = Numeric.take(Q, range(rpt), axis=dim)
>>> cndns(hp-Q) / cndns(hp) > toler
0


Consistency testing dimension 1, length 8.

>>> dim = 1

FFT

>>> P = fft.fft(p, None, dim)
>>> Q = fft.inverse_fft(P, None, dim)
>>> cndns(Q-p) / cndns(p) > toler
0

Real FFT

>>> RP = fft.real_fft(p, None, dim)
>>> npt = p.getshape()[dim]; rpt = npt/2 + 1
>>> P = Numeric.take(P, range(rpt), axis=dim)
>>> cndns(RP-P) / cndns(RP) > toler
0

Inverse FFT

>>> P = fft.fft(p, None, dim)
>>> Q = fft.inverse_fft(P, None, dim)
>>> cndns(Q-p) / cndns(p) > toler
0

Inverse Hermite FFT

>>> hp = fft.inverse_hermite_fft(q, npt, dim)
>>> Q = fft.inverse_fft(q, None, dim)
>>> Q = Numeric.take(Q, range(rpt), axis=dim)
>>> cndns(hp-Q) / cndns(hp) > toler
0

Multi-dimensional tests

>>> tee = Numeric.array(((2.0, 0, 2, 0),
...                      (0, 2, 0, 2),
...                      (2, 0, 2, 0),
...                      (0, 2, 0, 2)))
>>> eff = Numeric.array(((16.0, 0, 0, 0),
...                      (0, 0, 0, 0),
...                      (0, 0, 16, 0),
...                      (0, 0, 0, 0)))

FFT N-dimensional

>>> ftest = fft.fftnd(tee)
>>> cndns(ftest - eff) / cndns(eff) > toler
0

Inverse FFT N-Dimensional

>>> cndns(fft.inverse_fftnd(ftest) - tee) / cndns(tee) > toler
0

Real FFT N-dimensional

>>> fred = fft.real_fftnd(p)
>>> npts = p.getshape()[-1]; rpts = npts/2 + 1
>>> actual = fft.fftnd(p);  ract = actual[..., :rpts]
>>> cndns(fred-ract) / cndns(ract) > toler
0

Inverse Real FFT N-dimensional

>>> ethel = fft.inverse_real_fftnd(fred)
>>> cndns(p-ethel) / cndns(p) > toler
0

FFT 2D shape test:

>>> axes = (0,1); shape = (7,4); barney = Numeric.zeros(shape,'d')
>>> betty = fft.fft2d(barney); betty.shape == barney.shape
1
>>> betty = fft.fft2d(barney, None, axes); betty.shape == barney.shape
1
>>> betty = fft.fft2d(barney, shape); betty.shape == barney.shape
1
>>> betty = fft.fft2d(barney, shape, axes); betty.shape == barney.shape
1
>>> betty = fft.real_fft2d(barney); wilma = fft.inverse_real_fft2d(betty)
>>> wilma.shape == barney.shape
1
>>> wilma = fft.inverse_real_fft2d(betty, shape); wilma.shape == barney.shape
1
>>> wilma = fft.inverse_real_fft2d(betty, None, axes); wilma.shape == barney.shape
1
>>> wilma = fft.inverse_real_fft2d(betty, shape, axes); wilma.shape == barney.shape
1

Codelet order test:

>>> for size in range (1,25): 
...     for i in range(3): 
...         a = random_array.random(size) 
...         b = fft.real_fft(a) 
...         c = fft.inverse_real_fft(b,size) 
...         if cndns(c-a) / cndns(a) > toler:
...             print "real transforms failed for size %d" % size
...         a = a + random_array.random(size) * 1j
...         b = fft.fft(a) 
...         c = fft.inverse_fft(b,size) 
...         if cndns(c-a) / cndns(a) > toler:
...             print "complex transforms failed for size %d" % size

>>> x = Numeric.cos(Numeric.arange(30.0)/30.0*2*Numeric.pi) 
>>> y = fft.real_fft(x) 
>>> z = fft.inverse_real_fft(y,30)
>>> cndns(x-z)/cndns(x) > toler
0
"""

import numarray.fft as fft
import numarray.numeric as Numeric
import numarray.random_array as random_array
import sys

class DoctestError(Exception):
    pass

def cndns(m):
    return Numeric.maximum.reduce(Numeric.abs(m).getflat())

def case(expr, ans, eps=1e-9):
    if cndns(ans-expr) < eps:
        print "OK"
    else:
        raise DoctestError, "failed"
    
def test():
    import dtest
    import doctest
    return doctest.testmod(dtest)

if __name__ == '__main__':
    test()

