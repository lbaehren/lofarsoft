#! /usr/bin/env python

"""Script for testing hEquatorial2Horizontal.
"""

from pytmf import *
import pycrtools as cr

alpha = hms2rad(23, 9, 16.641); # Right assention
delta = dms2rad(-6, 43, 11.61); # Declination
L = dms2rad(77, 3, 56); # Longitude of telescope
phi = dms2rad(38, 55, 17); # Latitude of telescope
utc = gregoriandate2jd(1987, 4, 10. + ((19. + 21. / 60.) / 24.)) # Obstime 

print "alpha", rad2deg(alpha), "delta", rad2deg(delta)
print "L", rad2deg(L), "phi", rad2deg(phi)
print "utc", utc

# Make input and output arrays for conversion
equatorial = cr.hArray([alpha, delta])
horizontal = equatorial.new()

# Convert all coordinates in the input array
# assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)

# Print the result
print "A", rad2deg(horizontal[0]), "h", rad2deg(horizontal[1])
print "Expected result: A 68.034 h 15.125"

