TEST1 = """
Test of basic array creation and arithmetic.

>>> x=Numeric.array([1.,1.,1.,-2., pi/2.0, 4., 5., -10., 10., 1., 2., 3.])
>>> y=Numeric.array([5.,0.,3., 2., -1., -4., 0., -10., 10., 1., 0., 3.])
>>> a10 = 10.
>>> m1 = [1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0]
>>> m2 = [0, 0, 1, 0, 0, 1, 1, 0, 0, 0 ,0, 1]
>>> xm = array(x, mask=m1)
>>> ym = array(y, mask=m2)
>>> z = Numeric.array([-.5, 0., .5, .8])
>>> zm = array(z, mask=[0,1,0,0])
>>> xf = Numeric.where(m1, 1.e+20, x)
>>> xm.set_fill_value(1.e+20)
>>> for item in [x, y, xm, ym, xf]:
...     item.shape = s

>>> assert isMaskedArray(x) == 0
>>> assert isMaskedArray(xm) == 1
>>> assert shape(xm) == s
>>> assert xm.shape == s
>>> assert size(xm) == reduce(lambda x,y:x*y, s)
>>> assert xm.size() == size(xm)
>>> assert size(xm,0) == s[0]
>>> assert xm.size(0) == size(xm,0)
>>> assert count(xm) == len(m1) - reduce(lambda x,y:x+y, m1)
>>> if rank(xm) > 1:
...     assert count(xm,0) == size(xm,0) - reduce(lambda x,y:x+y, xm.mask()[0])

>>> alltest(xm, xf)
>>> alltest(filled(xm, 1.e20), xf)
>>> alltest(x, xm)
>>> alltest(-x, -xm)
>>> alltest(x + y, xm + ym)
>>> alltest(x - y, xm - ym)
>>> alltest(x * y, xm * ym)
    
>>> numarray.Error.pushMode(dividebyzero="ignore")
>>> alltest(x / y, xm / ym)
>>> dont_care = numarray.Error.popMode()
    
>>> alltest(a10 + y, a10 + ym)
>>> alltest(a10 - y, a10 - ym)
>>> alltest(a10 * y, a10 * ym)
    
>>> numarray.Error.pushMode(dividebyzero="ignore")
>>> alltest(a10 / y, a10 / ym)
>>> dont_care = numarray.Error.popMode()
    
>>> alltest(x + a10, xm + a10)
>>> alltest(x - a10, xm - a10)
>>> alltest(x * a10, xm * a10)
>>> alltest(x / a10, xm / a10)
>>> a2d = array([[1,2],[0,4]], Float)
>>> a2dm = masked_array(a2d, [[0,0],[1,0]])
>>> alltest (a2d * a2d, a2d * a2dm)
>>> alltest (a2d + a2d, a2d + a2dm)
>>> alltest (a2d - a2d, a2d - a2dm)
>>> alltest(x**2, xm**2)
>>> alltest(abs(x)**2.5, abs(xm) **2.5)
>>> alltest(x**y, xm**ym)
>>> alltest(Numeric.add(x,y), add(xm, ym))
>>> alltest(Numeric.subtract(x,y), subtract(xm, ym))
>>> alltest(Numeric.multiply(x,y), multiply(xm, ym))

>>> numarray.Error.pushMode(dividebyzero="ignore")
>>> alltest(Numeric.divide(x,y), divide(xm, ym))
>>> dont_care = numarray.Error.popMode()
    
>>> alltest(Numeric.cos(x), cos(xm))
>>> alltest(Numeric.cosh(x), cosh(xm))
>>> alltest(Numeric.sin(x), sin(xm))
>>> alltest(Numeric.sinh(x), sinh(xm))
>>> alltest(Numeric.tan(x), tan(xm))
>>> alltest(Numeric.tanh(x), tanh(xm))
>>> alltest(Numeric.sqrt(abs(x)), sqrt(xm))
>>> alltest(Numeric.log(abs(x)), log(xm))
>>> alltest(Numeric.log10(abs(x)), log10(xm))
>>> alltest(Numeric.exp(x), exp(xm))
>>> alltest(Numeric.arcsin(z), arcsin(zm))
>>> alltest(Numeric.arccos(z), arccos(zm))
>>> alltest(Numeric.arctan(z), arctan(zm))
>>> alltest(Numeric.arctan2(x, y), arctan2(xm, ym))
>>> alltest(Numeric.absolute(x), absolute(xm))
>>> alltest(Numeric.equal(x,y), equal(xm, ym))
>>> alltest(Numeric.not_equal(x,y), not_equal(xm, ym))
>>> alltest(Numeric.less(x,y), less(xm, ym))
>>> alltest(Numeric.greater(x,y), greater(xm, ym))
>>> alltest(Numeric.less_equal(x,y), less_equal(xm, ym))
>>> alltest(Numeric.greater_equal(x,y), greater_equal(xm, ym))
>>> alltest(Numeric.conjugate(x), conjugate(xm))
>>> alltest(Numeric.concatenate((x,y)), concatenate((xm,ym)))
>>> alltest(Numeric.concatenate((x,y)), concatenate((x,y)))
>>> alltest(Numeric.concatenate((x,y)), concatenate((xm,y)))
>>> alltest(Numeric.concatenate((x,y,x)), concatenate((x,ym,x)))

>>> ott = array([0.,1.,2.,3.], mask=[1,0,0,0])
>>> assert isinstance(count(ott), types.IntType)
>>> alltest(3, count(ott))
>>> alltest(1, count(1))
>>> alltest(0, array(1,mask=[1]))
>>> ott.shape = (2,2)
>>> assert isMaskedArray(count(ott,0))
>>> assert isinstance(count(ott), types.IntType)
>>> alltest(3, count(ott))
>>> assert getmask(count(ott,0)) is None
>>> alltest([1,2],count(ott,0))

>>> xr = Numeric.ravel(x) #max doesn't work if shaped
>>> xmr = ravel(xm)
>>> alltest(max(xr), maximum(xmr)) #true because of careful selection of data
>>> alltest(min(xr), minimum(xmr)) #true because of careful selection of data
>>> alltest(Numeric.add.reduce(x), add.reduce(x))
>>> alltest(Numeric.add.accumulate(x), add.accumulate(x))
>>> alltest(4, sum(array(4)))
>>> alltest(4, sum(array(4), axis=0))
>>> alltest(Numeric.sum(x), sum(x))
>>> alltest(Numeric.sum(filled(xm,0)), sum(xm))
>>> alltest(Numeric.sum(x,0), sum(x,0))
>>> alltest(Numeric.product(x), product(x))
>>> alltest(Numeric.product(x,0), product(x,0))
>>> alltest(Numeric.product(filled(xm,1)), product(xm))
>>> if len(s) > 1:
...     alltest(Numeric.concatenate((x,y),1), concatenate((xm,ym),1))
...     alltest(Numeric.add.reduce(x,1), add.reduce(x,1))
...     alltest(Numeric.sum(x,1), sum(x,1))
...     alltest(Numeric.product(x,1), product(x,1))

"""

REST = """

Test of conversions and indexing

>>> x1 = Numeric.array([1,2,4,3])
>>> x2 = array(x1, mask = [1,0,0,0])
>>> x3 = array(x1, mask = [0,1,0,1])
>>> x4 = array(x1)

test conversion to strings

>>> junk, garbage = str(x2), repr(x2)
>>> alltest(Numeric.sort(x1),sort(x2, fill_value=0))

tests of indexing

>>> assert type(x2[1]) is type(x1[1])
>>> assert x1[1] == x2[1]
>>> assert x2[0] is masked
>>> alltest(x1[2],x2[2])
>>> alltest(x1[2:5],x2[2:5])
>>> alltest(x1[:],x2[:])
>>> alltest(x1[1:], x3[1:])
>>> x1[2]=9
>>> x2[2]=9
>>> alltest(x1,x2)
>>> x1[1:3] = 99
>>> x2[1:3] = 99
>>> alltest(x1,x2)
>>> x2[1] = masked
>>> alltest(x1,x2)
>>> x2[1:3]=masked
>>> alltest(x1,x2)
>>> x2[:] = x1
>>> x2[1] = masked
>>> assert allequal(getmask(x2),array([0,1,0,0]))
>>> x3[:] = masked_array([1,2,3,4],[0,1,1,0])
>>> assert allequal(getmask(x3), array([0,1,1,0]))
>>> x4[:] = masked_array([1,2,3,4],[0,1,1,0])
>>> assert allequal(getmask(x4), array([0,1,1,0]))
>>> assert allequal(x4, array([1,2,3,4]))
>>> x1 = Numeric.arange(5)*1.0
>>> x2 = masked_values(x1, 3.0)
>>> alltest(x1,x2)
>>> assert allequal(array([0,0,0,1,0],MaskType), x2.mask())
>>> alltest(3.0, x2.fill_value())
    
PyObject tests disabled for now, maybe forever

    x1 = array([1,2,3],PyObject)
    x2 = Numeric.array([1,2,3],PyObject)
    x1[1] = 'hello'
    x2[1] = 'hello'
    assert x2[1] == 'hello'
    assert x1[1:1].shape == (0,)

Tests of some subtle points of copying and sizing.
     
>>> n = [0,0,1,0,0]
>>> m = make_mask(n)
>>> m2 = make_mask(m)
>>> assert m is m2
>>> m3 = make_mask(m, copy=1)
>>> assert m is not m3
    
>>> x1 = Numeric.arange(5)
>>> y1 = array(x1, mask=m)
>>> assert y1.raw_data() is not x1
>>> assert allequal(x1,y1.raw_data())
>>> assert y1.mask() is m

>>> y1a = array(y1, copy=0)
>>> assert y1a.raw_data() is y1.raw_data()
>>> assert y1a.mask() is y1.mask()
    
>>> y2 = array(x1, mask=m, copy=0)
>>> assert y2.raw_data() is x1
>>> assert y2.mask() is m
>>> assert y2[2] is masked
>>> y2[2]=9
>>> assert y2[2] is not masked
>>> assert y2.mask() is not m
>>> assert allequal(y2.mask(), 0)

>>> y3 = array(x1*1.0, mask=m, savespace=1).astype(Float32)
>>> assert filled(y3).typecode() == Float32

    assert y3.spacesaver()

>>> x4 = arange(4)
>>> x4[2] = masked
>>> y4 = resize(x4, (8,))
>>> alltest(concatenate([x4,x4]), y4)
>>> alltest(getmask(y4),[0,0,1,0,0,0,1,0])
>>> y5 = repeat(x4, (2,2,2,2))
>>> alltest(y5, [0,0,1,1,2,2,3,3])
>>> y6 = repeat(x4, 2)
>>> alltest(y5, y6)

    Test of put

>>> x=arange(5)
>>> put (x, [1,4],[10,40])
>>> alltest(x, [0,10,2,3,40])

>>> x=arange(5) * 1.0
>>> put (x, [1,4], [10.,40.])
>>> alltest(x, [0,10,2,3,40])

>>> x = arange(5).astype(Float32)
>>> put (x, [1,4],[10.,40.])
>>> alltest(x, [0,10,2,3,40])

>>> x=arange(6)*1.0
>>> x.shape=(2,3)
>>> put(x, [1,4],[10,40])
>>> alltest(x, [[0,10,2],[3,40,5]])

>>> d = arange(5)
>>> n = [0,0,0,1,1]
>>> m = make_mask(n)
>>> x = array(d, mask = m)
>>> assert x[3] is masked
>>> assert x[4] is masked
>>> put(x, [1,4], [10,40])
>>> assert x.mask() is not m
>>> assert x[3] is masked
>>> assert x[4] is not masked
>>> alltest(x, [0,10,2,-1,40])

>>> x = array(d, mask = m) 
>>> assert x.mask() is m
>>> x.put([-1,100,200])
>>> alltest(x, [-1,100,200,0,0])
>>> assert x[3] is masked
>>> assert x[4] is masked
>>> assert x.mask() is m

>>> x = array(d, mask = m) 
>>> x.putmask([30,40])
>>> alltest(x, [0,1,2,30,40])
>>> assert x.mask() is None

>>> x = array(d, mask = m) 
>>> y = x.compressed()
>>> z = array(x, mask = m)
>>> z.put(y)
>>> alltest (x, z)

Test of other odd features

>>> x = arange(20); x.shape=(4,5)
>>> x.flat[5] = 12
>>> assert x[1,0] == 12
>>> z = x + 10j * x
>>> alltest(z.real, x)
>>> alltest(z.imaginary, 10*x)
>>> alltest((z*conjugate(z)).real, 101*x*x)
>>> z.imaginary[...] = 0.0

>>> x = arange(10)
>>> x[3] = masked
>>> assert str(x[3]) == str(masked)
>>> c = x >= 8
>>> assert count(where(c,masked,masked)) == 0
>>> assert shape(where(c,masked,masked)) == c.shape
>>> z = where(c , x, masked)
>>> assert z.typecode() == x.typecode()
>>> assert z[3] is masked
>>> assert z[4] is masked
>>> assert z[7] is masked
>>> assert z[8] is not masked
>>> assert z[9] is not masked
>>> alltest(x,z)
>>> z = where(c , masked, x)
>>> assert z.typecode() == x.typecode()
>>> assert z[3] is masked
>>> assert z[4] is not masked
>>> assert z[7] is not masked
>>> assert z[8] is masked
>>> assert z[9] is masked
>>> z = masked_where(c, x)
>>> assert z.typecode() == x.typecode()
>>> assert z[3] is masked
>>> assert z[4] is not masked
>>> assert z[7] is not masked
>>> assert z[8] is masked
>>> assert z[9] is masked
>>> alltest(x,z)
>>> x = array([1.,2.,3.,4.,5.])
>>> c = array([1,1,1,0,0])
>>> x[2] = masked
>>> z = where(c, x, -x)
>>> alltest(z, [1.,2.,0., -4., -5])
>>> c[0] = masked
>>> z = where(c, x, -x)
>>> alltest(z, [1.,2.,0., -4., -5])
>>> assert z[0] is masked
>>> assert z[1] is not masked
>>> assert z[2] is masked
>>> alltest(masked_where(greater(x, 2), x), masked_greater(x,2))
>>> alltest(masked_where(greater_equal(x, 2), x), masked_greater_equal(x,2))
>>> alltest(masked_where(less(x, 2), x), masked_less(x,2))
>>> alltest(masked_where(less_equal(x, 2), x), masked_less_equal(x,2))
>>> alltest(masked_where(not_equal(x, 2), x), masked_not_equal(x,2))
>>> alltest(masked_where(equal(x, 2), x), masked_equal(x,2))
>>> alltest(masked_where(not_equal(x,2), x), masked_not_equal(x,2))
>>> alltest(masked_inside(range(5), 1, 3), [0, 199, 199, 199, 4])
>>> alltest(masked_outside(range(5), 1, 3),[199,1,2,3,199])
>>> alltest(masked_inside(array(range(5), mask=[1,0,0,0,0]), 1, 3).mask(), [1,1,1,1,0])
>>> alltest(masked_outside(array(range(5), mask=[0,1,0,0,0]), 1, 3).mask(), [1,1,0,0,1])
>>> alltest(masked_equal(array(range(5), mask=[1,0,0,0,0]), 2).mask(), [1,0,1,0,0])
>>> alltest(masked_not_equal(array([2,2,1,2,1], mask=[1,0,0,0,0]), 2).mask(), [1,0,1,0,1])
>>> alltest(masked_where([1,1,0,0,0], [1,2,3,4,5]), [99,99,3,4,5])
>>> atest = ones((10,10,10), typecode=Float32)
>>> atest.savespace(1)
>>> btest = MA.zeros(atest.shape, MaskType)
>>> ctest = MA.masked_where(btest,atest)
>>> alltest(atest,ctest)
>>> z = choose(c, (-x, x))
>>> alltest(z, [1.,2.,0., -4., -5])
>>> assert z[0] is masked
>>> assert z[1] is not masked
>>> assert z[2] is masked
>>> x = arange(6)
>>> x[5] = masked
>>> y = arange(6)*10
>>> y[2]= masked
>>> c = array([1,1,1,0,0,0], mask=[1,0,0,0,0,0])
>>> cm = c.filled(1)
>>> z = where(c,x,y)
>>> zm = where(cm,x,y)
>>> alltest(z, zm)
>>> assert getmask(zm) is None
>>> alltest(zm, [0,1,2,30,40,50])
>>> z = where(c, masked, 1)
>>> alltest(z, [99,99,99,1,1,1])
>>> z = where(c, 1, masked)
>>> alltest(z, [99, 1, 1, 99, 99, 99])

    Test of minumum, maximum.
    
>>> alltest(minimum([1,2,3],[4,0,9]), [1,0,3])
>>> alltest(maximum([1,2,3],[4,0,9]), [4,2,9])
>>> x = arange(5)
>>> y = arange(5) - 2
>>> x[3] = masked
>>> y[0] = masked
>>> alltest(minimum(x,y), where(less(x,y), x, y))
>>> alltest(maximum(x,y), where(greater(x,y), x, y))
>>> assert minimum(x) == 0
>>> assert maximum(x) == 4

    Test of take, transpose, inner, outer products
    
>>> x = arange(24)
>>> y = Numeric.arange(24)
>>> x[5:6] = masked
>>> x.shape=(2,3,4)
>>> y.shape=(2,3,4)
>>> alltest(Numeric.transpose(y,(2,0,1)), transpose(x,(2,0,1)))
>>> alltest(Numeric.take(y, (2,0,1), 1), take(x, (2,0,1), 1))
>>> alltest(Numeric.innerproduct(filled(x),filled(y)),
...                 innerproduct(x, y))
>>> alltest(Numeric.outerproduct(filled(x),filled(y)),
...                 outerproduct(x, y))

y = array(['abc', 1, 'def', 2, 3], PyObject)
y[2] = masked
t = take(y,[0,3,4])
assert t[0] == 'abc'
assert t[1] == 2
assert t[2] == 3

Test of inplace operations and rich comparisons
    
>>> y = arange(10)

>>> x = arange(10)
>>> xm = arange(10)
>>> xm[2] = masked
>>> x += 1
>>> alltest(x, y+1)
>>> xm += 1
>>> alltest(x, y+1)

>>> x = arange(10)
>>> xm = arange(10)
>>> xm[2] = masked
>>> x -= 1
>>> alltest(x, y-1)
>>> xm -= 1
>>> alltest(xm, y-1)

>>> x = arange(10)*1.0
>>> xm = arange(10)*1.0
>>> xm[2] = masked
>>> x *= 2.0
>>> alltest(x, y*2)
>>> xm *= 2.0
>>> alltest(xm, y*2)

>>> x = arange(10)*2
>>> xm = arange(10)
>>> xm[2] = masked
>>> x /= 2
>>> alltest(x, y)
>>> xm /= 2
>>> alltest(x, y)

>>> x = arange(10)*1.0
>>> xm = arange(10)*1.0
>>> xm[2] = masked
>>> x /= 2.0
>>> alltest(x, y/2.0)
>>> xm /= arange(10)
>>> alltest(xm, ones((10,)))

>>> x = arange(10).astype(Float32)
>>> xm = arange(10)
>>> xm[2] = masked
>>> id1 = id(x.raw_data())
>>> x += 1.
>>> assert id1 == id(x.raw_data())
>>> alltest(x, y+1.)

Test of pickling
    
>>> x = arange(12)
>>> x[4:10:2] = masked
>>> x.shape=(4,3)
>>> f = open('test9.pik','wb')
>>> import pickle
>>> pickle.dump(x, f)
>>> f.close()
>>> f = open('test9.pik', 'rb')
>>> y = pickle.load(f)
>>> alltest(x,y) 


Test of masked element

>>> assert masked.shape == ()
>>> assert count(masked) == 0
>>> x=arange(6)
>>> x[1] = masked
>>> assert x[1] is masked
>>> assert masked + 2 is masked
>>> assert masked + masked is masked
>>> assert count(x + masked) == 0
>>> assert size(x+masked) == size(x)
>>> assert filled(masked,1.e20).typecode() == Float
>>> try:
...     masked.shape=(1,)
...     raise RuntimeError, 'should not have been able to set shape'
... except AttributeError:
...     pass

Test of average

>>> ott = array([0.,1.,2.,3.], mask=[1,0,0,0])
>>> alltest(2.0, average(ott))
>>> alltest(2.0, average(ott, weights=[1., 1., 2., 1.]))
>>> result, wts = average(ott, weights=[1.,1.,2.,1.], returned=1)
>>> alltest(2.0, result)
>>> assert wts == 4.0
>>> ott[:] = masked
>>> assert average(ott) is masked
>>> ott = array([0.,1.,2.,3.], mask=[1,0,0,0])
>>> ott.shape=(2,2)
>>> ott[:,1] = masked
>>> alltest(average(ott), [2.0, 0.0])
>>> assert average(ott,axis=1)[0] is masked
>>> alltest([2.,0.], average(ott))
>>> result, wts = average(ott, returned=1)
>>> alltest(wts, [1., 0.])
>>> w1 = [0,1,1,1,1,0]
>>> w2 = [[0,1,1,1,1,0],[1,0,0,0,0,1]]
>>> x=arange(6)
>>> assert allclose(average(x), 2.5)
>>> assert allclose(average(x, weights=w1), 2.5)
>>> y=array([arange(6), 2.0*arange(6)])
>>> assert allclose(average(y, None), Numeric.add.reduce(Numeric.arange(6))*3./12.)
>>> assert allclose(average(y, axis=0), Numeric.arange(6) * 3./2.)
>>> assert allclose(average(y, axis=1), [average(x), average(x) * 2.0])
>>> assert allclose(average(y, None, weights=w2), 20./6.)
>>> assert allclose(average(y, axis=0, weights=w2), [0.,1.,2.,3.,4.,10.])
>>> assert allclose(average(y, axis=1), [average(x), average(x) * 2.0])

>>> m1 = zeros(6)
>>> m2 = [0,0,1,1,0,0]
>>> m3 = [[0,0,1,1,0,0],[0,1,1,1,1,0]]
>>> m4 = ones(6)
>>> m5 = [0, 1, 1, 1, 1, 1]
>>> assert allclose(average(masked_array(x, m1)), 2.5)
>>> assert allclose(average(masked_array(x, m2)), 2.5)
>>> assert allclose(average(masked_array(x, m4)), 1200000)
>>> assert average(masked_array(x, m4)) is masked
>>> assert average(masked_array(x, m5)) == 0.0
>>> assert count(average(masked_array(x, m4))) == 0
>>> z = masked_array(y, m3)
>>> assert allclose(average(z, None), 20./6.)
>>> assert allclose(average(z, axis=0), [0.,1.,99.,99.,4.0, 7.5])
>>> assert allclose(average(z, axis=1), [2.5, 5.0])
>>> assert allclose( average(z,weights=w2), [0.,1., 99., 99., 4.0, 10.0])

>>> a = arange(6)
>>> b = arange(6) * 3
>>> r1, w1 = average([[a,b],[b,a]], axis=1, returned=1)
>>> assert shape(r1) == shape(w1)
>>> assert r1.shape == w1.shape
>>> r2, w2 = average(ones((2,2,3)), axis=0, weights=[3,1], returned=1)
>>> assert shape(w2) == shape(r2)
>>> r2, w2 = average(ones((2,2,3)), returned=1)
>>> assert shape(w2) == shape(r2)
>>> r2, w2 = average(ones((2,2,3)), weights=ones((2,2,3)), returned=1)
>>> assert shape(w2) == shape(r2)
>>> a2d = array([[1,2],[0,4]], Float)
>>> a2dm = masked_array(a2d, [[0,0],[1,0]])
>>> a2da = average(a2d)
>>> alltest (a2da, [0.5, 3.0])
>>> a2dma = average(a2dm)
>>> alltest( a2dma, [1.0, 3.0])
>>> a2dma = average(a2dm, axis=None)
>>> alltest(a2dma, 7./3.)
>>> a2dma = average(a2dm, axis=1)
>>> alltest(a2dma, [1.5, 4.0])


SF# [ 1364215 ] Cannot combine array and masked array (e.g. via divide)

>>> a = numarray.array([1,2,3])
>>> ma = array([4,5,6],mask=[0,1,0])
>>> print a/ma
[0 -- 0]
"""

__doc__ = ">>> s = (12,)\n" + TEST1 + \
          ">>> s = (4,3)\n" + TEST1 + \
          ">>> s = (2,3,2)\n" + TEST1 + \
          REST

import numarray as Numeric
import numarray
import types, time

from numarray.ma import *

def alltest(v,w):
    result = allclose(v,w)
    if not result:
        print "These two values did not compare:"
        print v
        print "----"
        print w
        raise RuntimeError

def testt():
    for f in [testf, testinplace]:
        for n in [1000,10000,50000]:
            t = testta(n, f)
            t1 = testtb(n, f)
            t2 = testtc(n, f)
            print f.test_name
            print """\
n = %7d 
Numeric time (ms) %6.1f 
MA maskless ratio %6.1f
MA masked ratio %6.1f
""" % (n, t*1000.0, t1/t, t2/t)

def testta(n, f):
    x=Numeric.arange(n) + 1.0
    tn0 = time.time()
    z = f(x)
    return time.time() - tn0

def testtb(n, f):
    x=arange(n) + 1.0
    tn0 = time.time()
    z = f(x)
    return time.time() - tn0

def testtc(n, f):
    x=arange(n) + 1.0
    x[0] = masked
    tn0 = time.time()
    z = f(x)
    return time.time() - tn0

def testf(x):
    for i in range(25):
        y = x **2 +  2.0 * x - 1.0
        w = x **2 +  1.0
        z = (y / w) ** 2
    return z
testf.test_name = 'Simple arithmetic'

def testinplace(x):
    for i in range(25):
        y = x**2
        y += 2.0*x
        y -= 1.0
        y /= x
    return y
testinplace.test_name = 'Inplace operations'

def test(option=0):
    import doctest, dtest
    rval = doctest.testmod(dtest)
    if option: testt()
    return rval

if __name__ == "__main__":
    import sys, gc
    test(len(sys.argv)>=2)
    gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    assert gc.collect() == 0
    print "Test of MA completed o.k."
