#! /usr/bin/env python

import numpy as np
from pycr import coordinates

# Set parameters
n_az = 10
n_el = 10
ref_long = 1.0
ref_lat = 1.0
inc_long = 1.0
inc_lat = 1.0
ref_x = 1.0
ref_y = 1.0

# Construct pixel array
pixel = []
for i in range(n_az):
    for j in range(n_el):
        pixel.extend([i,j])
pixel = np.asarray(pixel, dtype='float64')

# Convert pixel to AZEL world coordinates
world = np.zeros_like(pixel)
coordinates.toWorld(world, pixel, 'AZEL', 'STG', ref_long, ref_lat, inc_long, inc_lat, ref_x, ref_y)

# Test world to pixel conversion
pixel_back = np.zeros_like(world)
coordinates.toPixel(pixel_back, world, 'AZEL', 'STG', ref_long, ref_lat, inc_long, inc_lat, ref_x, ref_y)

for i in range(len(pixel)):
    print pixel[i], world[i], pixel_back[i]

