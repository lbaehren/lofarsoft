#
#  
#  
#
#  Created by Arthur Corstanje on 1/20/10.
#  Copyright (c) 2010, Arhtur Corstanje.  All rights reserved.
#

import numpy as np
from numpy import sin, cos, pi
import sys
import math
#import time

c = 299792458.0 # speed of ligth in m/s
twopi = 2 * pi
halfpi = 0.5 * pi
rad2deg = 360.0 / twopi

def directionFromThreeAntennas(positions, times):
    # Get a direction of arrival (az, el) assuming a source at infinity (plane wave).
    # From three antennas we get one solution for the direction, 
    # unless the arrival times are out of bounds, i.e. larger than the light-time between two antennas.
    # In: positions array (x1, y1, z1, x2, y2, z2, x3, y3, z3); times array (t1, t2, t3).
    
    p1 = np.array(positions[0:3]) 
    p2 = np.array(positions[3:6]) 
    p3 = np.array(positions[6:9]) # yes, for 10 antennas we'd write it differently...
    t1 = times[0]; t2 = times[1]; t3 = times[2]

    # First take by definition p1 = 0, t1 = 0, use translation invariance of the result
    t2 -= t1; t3 -= t1
    p2 -= p1; p3 -= p1
    t1 = 0; p1 = np.array(np.zeros(3))
    t2 *= c; t3 *= c; # t -> ct
    
    # Now do a coordinate rotation such that z-coordinates z2, z3 are 0, and p2 is on the x-axis (i.e. y2 = 0).
    xx = p2 / np.linalg.norm(p2) # make x-axis by normalizing p2
    yy = p3 - p2 * (np.dot(p2, p3) / np.dot(p2, p2)) # make y-axis by taking the part of p3 that is perpendicular to p2
    yy = yy / np.linalg.norm(yy)
    zz = np.cross(xx, yy); zz = zz / np.linalg.norm(zz)   # and make z-axis by the vector perpendicular to both p2 and p3.
    # We use the fact that we preserve lengths and angles in the new coord. system.
    
    # We now have to solve for A and B in:
    # t2 = A x2
    # t3 = A x3 + B y3
    
    a = np.dot(p2, p3) / np.linalg.norm(p2) 
    b = np.sqrt(np.dot(p3, p3) - a*a)      # a and b are x3 and y3 resp.
    
    A = - t2 / np.linalg.norm(p2)
    B = (1/b) * (- t3 - A * a)
    
    # Which gives the incoming-signal vector in the rotated coord. frame as (A, B, C). 
    # C is free in this coord. system, but follows from imposing length 1 on the signal vector, 
    # consistent with (ct)^2 = x^2 + y^2 + z^2. Without above coordinate rotation, the following nonlinear function is part of the system to be solved:
    
    square = A*A + B*B
    if square < 1:
        C = np.sqrt(1 - square)
    elif (square - 1) < 1e-15: # this does happen because of floating point errors, when time delays 'exactly' match elevation = 0
        C = 0 # so this correction is needed to ensure this function correctly inverts timeDelaysFromDirection(...)
    else:
        return (-1, -1) # calculation fails, arrival times out of bounds!
    
#    print A
#    print B
#    print C
    
    # Now we have to transform this vector back to normal x-y-z coordinates, and then to (az, el) to get our direction.
    # This is where the above xx, yy, zz vectors come in (normal notation: x', y', z')
    
    #T = np.matrix([xx, yy, zz]) # transformation matrix of xx, yy, zz in terms of x, y, z
    #print T
    #print ' '
    #signal_rotated = np.array([A, B, C])
    
#    signal = np.inner(T.I, signal_rotated) # Matrix-vector multiply T * signal_rotated.
    signal = A * xx + B * yy + C * zz
    #signal = - signal
#    print signal
#    print ' '
    # Now get az, el from x, y, z...
    x = signal[0]; y = signal[1]; z = signal[2]
    
    theta = np.arccos(z) # in fact, z/R with R = 1
    phi = np.arctan2(y, x) # gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if phi < 0:
        phi += twopi
#    print x
#    print y
#    print z
        
    return (phi, halfpi - theta)
    

def timeDelaysFromDirection(positions, direction):
    # assuming directions in (az, el), in radians
    n = len(positions) / 3
    phi = direction[0] # warning, 90 degree
    theta = halfpi - direction[1] # theta as in standard spherical coords, while el=90 means zenith...
    
    cartesianDirection = np.array([sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)])
    print cartesianDirection
    timeDelays = np.zeros(n)
    for i in range(n):
        thisPosition = np.array(positions[3*i:3*(i+1)])
        print thisPosition
        timeDelays[i] = - (1/c) * np.dot(cartesianDirection, thisPosition) # note the minus sign! Signal vector points down from the sky.
#        print timeDelays[i]
        
    return timeDelays
    
    
def testDirectionCalculationForThreeAntennas(positions):
    azSteps = 360; elSteps = 90
    
    for i in range(azSteps):
        for j in range(elSteps):
            az = twopi * float(i) / azSteps
            el = halfpi * float(j) / elSteps
            tdelays = timeDelaysFromDirection(positions, [az, el])
            (calc_az, calc_el) = directionFromThreeAntennas(positions, tdelays)
            if abs(calc_az - az) > 1e-12 or abs(calc_el - el) > 1e-7:
                print 'Wrong value for az: ' + str(az*rad2deg) + ' calculated: ' +str(calc_az * rad2deg)
                print 'el: ' + str(el*rad2deg) + ' calculated: ' + str(calc_el * rad2deg)

    