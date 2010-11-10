#! /usr/bin/env python

"""Script for testing hEquatorial2Horizontal.
"""

from pytmf import *
import pycrtools as cr

print """Calculating azimuth and elevation of the Crab nebula:
(ra, dec) = (5 34 31.97, +22 00 52.0)
as seen by LOFAR (core station 2)
(lon, lat) = (6.869837540, 52.915122495)
on UTC 2010-11-04T5:34:16
"""

alpha = hms2rad(5, 34, 31.97); # Right assention
delta = dms2rad(22, 0, 52.0); # Declination

L = deg2rad(6.869837540); # Longitude of telescope
phi = deg2rad(52.915122495); # Latitude of telescope

utc = gregoriandate2jd(2010, 11, 4. + ((5. + 34. / 60. + 16. / 3600.) / 24.)) # Obstime 

print "Input:"
print "alpha", rad2deg(alpha), "delta", rad2deg(delta)
print "L", rad2deg(L), "phi", rad2deg(phi)
print "utc", utc
print ""

# Make input and output arrays for conversion
equatorial = cr.hArray([alpha, delta])
horizontal = equatorial.new()

# Convert all coordinates in the input array
# assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)

# Print the result
print "Result:"
print "A", rad2deg(horizontal[0]), "h", rad2deg(horizontal[1])
print "Expected result:"
print "A", -109.18470278, "h", 41.0800365834
print ""

