#!/usr/bin/python

import sys, ephem
from math import degrees,radians
ra = sys.argv[1]
dec= sys.argv[2]

radec=ephem.Equatorial(radians(float(ra)),radians(float(dec)))
g = ephem.Galactic(radec)

gl = g.long
gb = g.lat

print degrees(gl),degrees(gb)
