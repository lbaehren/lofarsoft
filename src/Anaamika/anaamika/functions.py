# some functions used to fit data

import numpy as N
import math

def poly(c,x):
    """ y = Sum { c(i)*x^i }, i=0,len(c)"""
    import numpy as N
    y=N.zeros(len(x))
    for i in range(len(c)):
        y += c[i]*(x**i)
    return y

def wenss_fit(c,x):
    """ sqrt(c0*c0 + c1^2/x^2)"""
    import numpy as N
    y = N.sqrt(c[0]*c[0]+c[1]*c[1]/(x*x))
    return y

def shapeletfit(cf, Bset, cfshape):
    """ The function """

    ordermax = Bset.shape[0]
    y = (Bset[0,0,::]).flatten()
    y = N.zeros(y.shape)
    index = [(i,j) for i in range(ordermax) for j in range(ordermax-i)]  # i=0->nmax, j=0-nmax-i
    for coord in index:
	linbasis = (Bset[coord[0], coord[1], ::]).flatten()
        y += cf.reshape(cfshape)[coord]*linbasis

    return y

def func_poly2d(ord,p,x,y):
    """ 2d polynomial.
    ord=0 : z=p[0]
    ord=1 : z=p[0]+p[1]*x+p[2]*y
    ord=2 : z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y 
    ord=3 : z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y+
              p[6]*x*x*x+p[7]*x*x*y+p[8]*x*y*y+p[9]*y*y*y"""

    if ord == 0:
        z=p[0]
    if ord == 1: 
        z=p[0]+p[1]*x+p[2]*y
    if ord == 2: 
        z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y
    if ord == 3: 
        z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y+\
          p[6]*x*x*x+p[7]*x*x*y+p[8]*x*y*y+p[9]*y*y*y
    if ord > 3: 
        print " We do not trust polynomial fits > 3 "
	z = None

    return z

def func_poly2d_ini(ord, av):
    """ Initial guess -- assume flat plane. """

    if ord == 0:
        p0 = N.asarray([av])
    if ord == 1:
        p0 = N.asarray([av] + [0.0]*2)
    if ord == 2:
        p0 = N.asarray([av] + [0.0]*5)
    if ord == 3:
        p0 = N.asarray([av] + [0.0]*9)
    if ord > 3:
        p0 = None

    return p0

def ilist(x):
    """ integer part of a list of floats. """

    fn = lambda x : [int(round(i)) for i in x]
    return fn(x)

def cart2polar(cart, cen):
    """ convert cartesian coordinates to polar coordinates around cen. theta is
    zero for +ve xaxis and goes counter clockwise. cart is a numpy array [x,y] where
    x and y are numpy arrays of all the (>0) values of coordinates."""

    polar = N.zeros(cart.shape)
    pi = math.pi
    rad = 180.0/pi

    cc = N.transpose(cart)
    cc = (cc-cen)*(cc-cen)
    polar[0] = N.sqrt(N.sum(cc,1))
    th = N.arctan2(cart[1]-cen[1],cart[0]-cen[0])*rad
    polar[1] = N.where(th > 0, th, 360+th)

    return polar


def polar2cart(polar, cen):
    """ convert polar coordinates around cen to cartesian coordinates. theta is
    zero for +ve xaxis and goes counter clockwise. polar is a numpy array of [r], [heta]
    and cart is a numpy array [x,y] where x and y are numpy arrays of all the (>0) 
    values of coordinates."""

    cart = N.zeros(polar.shape)
    pi = math.pi
    rad = 180.0/pi

    cart[0]=polar[0]*N.cos(polar[1]/rad)+cen[0]
    cart[1]=polar[0]*N.sin(polar[1]/rad)+cen[1]

    return cart


