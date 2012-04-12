#! /usr/bin/env python

"""Script for testing hEquatorial2Horizontal.
"""

from pytmf import *
import pycrtools as cr
import math

deg=math.pi/180.
pi2=math.pi/2.

old_test=0

if old_test:
    alpha = hms2rad(23, 9, 16.641); # Right assention
    delta = dms2rad(-6, 43, 11.61); # Declination
    L = dms2rad(77, 3, 56); # Longitude of telescope
    phi = dms2rad(38, 55, 17); # Latitude of telescope
    
    utc = gregorian2jd(1987, 4, 10. + ((19. + 21. / 60.) / 24.)) # Obstime 

else: # By EE
    alpha = hms2rad(11, 30, 02.74); # Right assention  (Mars)
    delta = dms2rad(07, 42, 48.6); # Declination    
#    alpha = hms2rad(03,32,59.37 ); # Right assention  (Pulsar)
#    delta = dms2rad(54,34,44.9); # Declination    

    phi = deg2rad(52.915122495)  #(LOFAR Superterp)
    L = deg2rad(6.869837540)
    
    utc= gregorian2jd(2012,02,14.00)

print "alpha", rad2deg(alpha), "delta", rad2deg(delta)
print "L", rad2deg(L), "phi", rad2deg(phi)
print "utc", utc

# Make input and output arrays for conversion
equatorial = cr.hArray([alpha, delta])
horizontal = equatorial.new()

# Convert all coordinates in the input array
# assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)

horizontal2 = radec2azel(alpha,delta,utc,0.,L,phi)
horizontal3 = equatorial2horizontal(last(utc,utc+delta_tt_utc(utc),L)-alpha,delta,phi)   #  note the name discrepansies of the functions' inputs/results!!!  ie. radec2azel = hEquatorial2Horizontal != equatorial2horizontal)

#if horizontal[0] < 0:  horizontal[0] = 90*deg-horizontal[0] #2.*math.pi
if horizontal[0] < 0:  horizontal[0] += 2.*math.pi


# Print the result
print '-------------------------'
print 'horizontal='
print "A", rad2deg(horizontal[0]), "h", rad2deg(horizontal[1])
print 'horizontal2='
print "A", rad2deg(horizontal2[0]), "h", rad2deg(horizontal2[1])
print 'horizontal3='
print "A", rad2deg(horizontal3[0]), "h", rad2deg(horizontal3[1])
if old_test:
    print "Expected result: A 68.034 h 15.125"
else:
    print "-------- \n Expected result for Mars at 2012:02:14.00 UT, from LOFAR-Superterp:\n A 149.9026 h 41.2025 \n --------" # From HORIZONS (NASA/JPL webtool for ephemerides.), as well as Stellarium (V.0.10.6)

  