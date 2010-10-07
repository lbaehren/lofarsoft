#! /usr/bin/env python

import os
import numpy as np
import pycr as cr

LOFARSOFT=os.environ["LOFARSOFT"]
filename=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5"

# Open file guessing filetype from extension
dr = cr.open(filename)

# Or open file with explicit filetype
#dr = cr.open(filename, 'LOFAR_TBB')

# Print list of allowed parameters
print dr.keys()

# Get the sample offsets if possible
if 'sample_offset' in dr:
    print dr['sample_offset']

# Set parameters of the DataReader
dr.blocksize = 512
dr.block = 129

# Get parameters from the DataReader
ants = dr['nofantennas']
blocksize = dr['blocksize']
fftlength = dr['fftlength']

# Read some data
time = np.empty(blocksize)
dr.read("time", time)

print 'Time:'
print time

freqs = np.empty(fftlength)
dr.read("frequency", freqs)

print 'Frequency:'
print freqs

efield = np.empty((ants, blocksize))
dr.read("fx", efield)

print 'Fx:'
print efield

# Get some metadata from an external source
dr.station = 'CS302'
dr.antennaset = 'HBA'

print 'Antenna positions:'
antpos = dr['antpos']
print antpos

