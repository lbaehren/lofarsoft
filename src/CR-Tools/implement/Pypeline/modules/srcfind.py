#
#  srcfind.py - Find a source on the sky based on arrival times and/or phase delays in multiple antennas
#  
#
#  Created by Arthur Corstanje on Aug. 2, 2010.
#  Copyright (c) 2010, Arthur Corstanje. All rights reserved.

"""
Calculate arrival direction from time delays in 3 points in space'
Usage: give positions in a list or array of 9 numbers x,y,z,x,y,z,x,y,z.'
A direction in [az, el] (radians) can be given, then time delays follow from:'
    timeDelaysFromDirection(positions, direction)'
Given a set of time delays, the direction can be calculated using: '
    directionFromThreeAntennas(positions, times) '
A full-sky test can be run using: '
    testDirectionCalculationForThreeAntennas(positions)'
which does these two steps for many (grid)points on the sky, and checks if the results match.'
Some errors may come up in this, because of floating point roundoff and mod-2pi issues. '
"""

#import pycrtools
import numpy as np
from numpy import sin, cos, pi 

c = 299792458.0 # speed of ligth in m/s
twopi = 2 * pi
halfpi = 0.5 * pi
rad2deg = 360.0 / twopi

def directionFromThreeAntennas(positions, times):
    """
    Given three antenna positions, and (pulse) arrival times for each antenna, 
    get a direction of arrival (az, el) assuming a source at infinity (plane wave).
    From three antennas we get in general two solutions for the direction, 
    unless the arrival times are out of bounds, i.e. larger than the light-time between two antennas.
    In: positions array (x1, y1, z1, x2, y2, z2, x3, y3, z3); 
        times array (t1, t2, t3).
    Input is assumed to be Numpy arrays.
    """
    
    p1 = np.array(positions[0:3]) 
    p2 = np.array(positions[3:6]) 
    p3 = np.array(positions[6:9]) # yes, for 10 antennas we'd write it differently...
    t1 = times[0]; t2 = times[1]; t3 = times[2]

    # First take by definition p1 = 0, t1 = 0, use translation invariance of the result
    t2 -= t1; t3 -= t1
    p2 -= p1; p3 -= p1
    t1 = 0; p1 = np.array(np.zeros(3))
    t2 *= c; t3 *= c; # t -> ct
    
    p2 = p2.astype(float)
    p3 = p3.astype(float) # !!! There's nothing that stops you from throwing ints at it, which destroys e.g. the inner product ratio below!
       
    # Now do a coordinate rotation such that z-coordinates z2, z3 are 0, and p2 is on the x-axis (i.e. y2 = 0).
    xx = p2 / np.linalg.norm(p2) # make x-axis by normalizing p2
    yy = p3 - p2 * (np.dot(p2, p3) / np.dot(p2, p2)) # make y-axis by taking the part of p3 that is perpendicular to p2 (cf. Gram-Schmidt)
    yy = yy / np.linalg.norm(yy)
    zz = np.cross(xx, yy); zz = zz / np.linalg.norm(zz)   # and make z-axis by the vector perpendicular to both p2 and p3.
    # We use the fact that we preserve lengths and angles in the new coordinate system.
        
    # We now have to solve for A and B in:
    # t2 = A x2
    # t3 = A x3 + B y3
    # from the general t_i = A x_i + B y_i + C z_i and using our rotated coordinates.
    
    a = np.dot(p3, xx)
#    b = np.sqrt(np.dot(p3, p3) - a*a)      # a and b are x3 and y3 resp. in rotated coordinates. 
#    NB ! This square root is wrong as it should be +/- sqrt(...) and you don't know the sign. Rewrite and make it easier:
    b = np.dot(p3, yy)
       
    A = - t2 / np.linalg.norm(p2)
    B = (1/b) * (-t3 - A * a)
    
    # Which gives the incoming-signal vector in the rotated coord. frame as (A, B, C). 
    # C is free in this coord. system, but follows from imposing length 1 on the signal vector, 
    # consistent with (ct)^2 = x^2 + y^2 + z^2. Without above coordinate rotation, the following nonlinear function is part of the system to be solved:
    
    square = A*A + B*B
    if square < 1:
        C = np.sqrt(1 - square) # Note! TWO solutions, +/- sqrt(...). No constraints apart from elevation > 0 in the end result - otherwise both can apply.
    elif (square - 1) < 1e-15: # this does happen because of floating point errors, when time delays 'exactly' match elevation = 0
        C = 0 # so this correction is needed to ensure this function correctly inverts timeDelaysFromDirection(...)
    else:
        return (-1, -1) # calculation fails, arrival times out of bounds!
       
    # Now we have to transform this vector back to normal x-y-z coordinates, and then to (az, el) to get our direction.
    # This is where the above xx, yy, zz vectors come in (normal notation: x', y', z')
    
    signal  =  A * xx + B * yy + C * zz 
    signal2 =  A * xx + B * yy - C * zz 

    # Now get az, el from x, y, z...
    x = signal[0]; y = signal[1]; z = signal[2]
    
    theta = np.arccos(z) # in fact, z/R with R = 1
    phi = np.arctan2(y, x) # gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if phi < 0:
        phi += twopi
    
    az1 = phi
    el1 = halfpi - theta

    x =  signal2[0]; y =  signal2[1]; z =  signal2[2]
    
    theta = np.arccos(z) # in fact, z/R with R = 1
    phi = np.arctan2(y, x) # gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if phi < 0:
        phi += twopi
    
    az2 = phi
    el2 = halfpi - theta

    return (az1, el1, az2, el2) 

def timeDelaysFromDirection(positions, direction):
    """
    Get time delays for antennas at given positions (np-array x1, y1, z1, x2, y2, z2, ...)
    and a given direction (az, el) in radians.
    Time delays come out as an np-array.
    
    >>> pos = np.array([0, 0, 0, 23, -21, 54, 11, 21, 33])
    >>> dir = (1.2, 0.7)
    >>> timeDelaysFromDirection(pos, dir)
    array([ -0.00000000e+00,  -8.73671261e-08,  -1.31017019e-07])

    """
    
    n = len(positions) / 3
    phi = direction[0] # warning, 90 degree?
    theta = halfpi - direction[1] # theta as in standard spherical coords, while el=90 means zenith...
    
    cartesianDirection = np.array([sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)])
    timeDelays = np.zeros(n)
    for i in range(n):
        thisPosition = np.array(positions[3*i:3*(i+1)])
        timeDelays[i] = - (1/c) * np.dot(cartesianDirection, thisPosition) # note the minus sign! Signal vector points down from the sky.
        
    return timeDelays
    
    
def testDirectionCalculationForThreeAntennas(positions):
    """
    Given antenna positions, calculate time delays for 'every' signal vector on the sky.
    Then, invert and calculate the signal vector from the given time delays. One out of the 2 possible solutions has to match 
    within some floating point roundoff interval. A two-pi error may occur (and gets written out), but that's no problem.
    
    >>> testDirectionCalculationForThreeAntennas(np.array([0, 0, 0, 23, -21, 54, 11, 21, 33]))
    Wrong value for az: 0.0 calculated: 195.081863616 or: 360.0
    el: 18.0 calculated: 60.2318806131 or: 18.0
    Wrong value for az: 0.0 calculated: 196.898077333 or: 360.0
    el: 24.0 calculated: 66.1764021336 or: 24.0
    Wrong value for az: 0.0 calculated: 285.811432122 or: 360.0
    el: 48.0 calculated: 86.4889718066 or: 48.0
    Wrong value for az: 0.0 calculated: 358.619827389 or: 360.0
    el: 66.0 calculated: 71.2661264901 or: 66.0
    Wrong value for az: 0.0 calculated: 360.0 or: 1.33942285143
    el: 72.0 calculated: 72.0 or: 65.3494016156
    Wrong value for az: 0.0 calculated: 360.0 or: 3.05501838316
    el: 78.0 calculated: 78.0 or: 59.4017926229

    """
    azSteps = 60; elSteps = 15
    tolerance = 1e-7
    s = ''    
    for i in range(azSteps):
        for j in range(elSteps):
            az = twopi * float(i) / azSteps
            el = halfpi * float(j) / elSteps
            tdelays = timeDelaysFromDirection(positions, [az, el])
            result = (calc_az, calc_el, calc_az2, calc_el2) = directionFromThreeAntennas(positions, tdelays)           
            
            if (abs(calc_az - az) > tolerance or abs(calc_el - el) > tolerance) and (abs(calc_az2 - az) > tolerance or abs(calc_el2 - el) > tolerance):
                print 'Wrong value for az: ' + str(az*rad2deg) + ' calculated: ' +str(calc_az * rad2deg) + ' or: ' + str(calc_az2 * rad2deg)
                print 'el: ' + str(el*rad2deg) + ' calculated: ' + str(calc_el * rad2deg) + ' or: ' + str(calc_el2 * rad2deg)
    
    
## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

