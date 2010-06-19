# This is me being nice and writing test programs

import numpy as N
import functions as func
import pylab as pl
import geometry as geom

def test_cartpol():
    """ This tests cart2polar and polar2cart. """

    x = N.random.random(100)*10.0
    y = N.random.random(100)*10.0

    cen=[1.5, 0.3]
    cart = N.array([x, y])

    pol = func.cart2polar(cart, cen)
    cart1 = func.polar2cart(pol, cen)

    res = (cart - cart1)/cart

    print "  Residual mean : ", N.mean(res)*100.0
    print "  Residual std  : ", N.std(res)*100.0


def test_tightboundary():
    """ Tests drawing a tightboundary around a number of points. """

    cen=(0.5, 0.5)
    pl.figure()
    x = N.random.random(50)
    y = N.random.random(50)
    xx, yy = geom.tightboundary(x, y, cen)
    pl.plot(x,y,'*r')
    pl.plot(xx,yy,'-b')




