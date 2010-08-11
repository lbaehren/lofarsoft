# functions to do annoying geometry stuff

import functions as func
import numpy as N
from math import pi,atan2,atan,acos,sqrt
import pylab as pl

def exterior_angle(x,y):
    """ Calculates angle between three points ang(1-2-3) in degrees exterior to the point (0,0). 
        x and y are arrays with the coordinates. """

    x1, x2, x3 = x
    y1, y2, y3 = y
    d1 = sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))
    d2 = sqrt((y2-y3)*(y2-y3)+(x2-x3)*(x2-x3))
    d3 = sqrt((y3-y1)*(y3-y1)+(x3-x1)*(x3-x1))
    theta1 = acos((d1*d1+d2*d2-d3*d3)/(2.0*d1*d2))*180/pi

    x4 = x1 + x3-x2; y4 = y1 + y3-y2
    rall, tall = func.cart2polar(N.array([N.array([x1,x2,x3,x4]), N.array([y1,y2,y3,y4])]), (0.,0.))
    if y1 < 0 or y2 < 0 or y3 < 0: 
      if y1 < 0 and y2 < 0 and y3 < 0:
        tall = tall-180.0
      else:
        if 90 < tall[1] < 270.0: 
          tall = tall-90.0
        else:
          tall = tall-270.0
          tall = N.where(tall > 0, tall, 360+tall)
    [xx1,xx2,xx3,xx4],[yy1,yy2,yy3,yy4] = func.polar2cart(N.array([rall, tall]), (0,0))
    rr, tt = func.cart2polar(N.array([N.array([xx2, xx4]), N.array([yy2, yy4])]), (xx1, yy1))

    if tt[1] > tt[0]:
      theta2 = 360-theta1 
    else:
      theta2 = theta1 

    return theta2

def tightboundary(xo, yo, cen):
    """ Given an array of points x and y, determine successive points, connecting which
    will form a tight boundary of all these points. """

    x = xo - cen[0]
    y = yo - cen[1]
    polar = func.cart2polar(N.array([x, y]), (0.0,0.0))
    r, theta = polar
    index = N.argsort(theta)
    th_sort = theta[index]; r_sort = r[index]  # in ascending order of theta (anticlockwise)
    x_sort = x[index]; y_sort = y[index]

    x_new = N.roll(x_sort, -N.argmax(r_sort))  # shifted so max r is first element
    y_new = N.roll(y_sort, -N.argmax(r_sort))
    x_new = N.append(x_new, [x_new[0], x_new[1]])  # so u dont have to calculate for the edges
    y_new = N.append(y_new, [y_new[0], y_new[1]])

    extang = N.zeros(x_new.shape)
    for i in range(3):   # sufficient
        for i in range(1, len(x_new)-1):
            extang[i] = exterior_angle(x_new[i-1:i+2], y_new[i-1:i+2])
        mask = N.where(extang < 180.0, 1, 0) # 0 => good; 1 => bad; python ishtyle
        ind1 = N.where(mask == 0)
        x_new = N.append(x_new[ind1], [x_new[ind1][0], x_new[ind1][1]])
        y_new = N.append(y_new[ind1], [y_new[ind1][0], y_new[ind1][1]])

    return x_new+cen[0], y_new+cen[1]

def pt_inout_polygon(point, polygon):
    """ Given a point (x,y), and polygon which is an array of x-array and y-array with last 
    point repeated, uses number of horizontal intersections to the right to determine if 
    point is inside (true) or outside (false) the polygon. If ray intersects an edge then dont 
    count unless the two segments on either side of that vertex are on either side of the point."""

    x0, y0 = point
    x1, y1 = polygon
    x = N.append(x1[-1], x1)  # to check for intersection on a vertex
    y = N.append(y1[-1], y1)

    ninter = 0
    for i in range(1,len(x)-1):   # line segment is i->i+1
        xp1, yp1 = x[i], y[i]
        xp2, yp2 = x[i+1], y[i+1]
	yi = y0  		# intersection pt
	if (xp1 != xp2):
          m = (yp1-yp2)/(xp1-xp2)
          c = yp1 - m*xp1
	  xi = (yi-c)/m
  	else:
	  xi = xp1
	if xi > x0: 
	  if min(xp1,xp2) < xi < max(xp1,xp2) and min(yp1,yp2) < yi < max(yp1,yp2): 
  	    ninter += 1
          if xp1 == xi and yp1 == xi:
 	    if (yp1-yi)*(y[i-1]-yi) < 0: ninter += 1
          if xp2 == xi and yp2 == xi:
 	    if (yp2-yi)*(y[i+2]-yi) < 0: ninter += 1

    inside = False
    if (ninter % 2) == 1: inside = True

    return inside
    

