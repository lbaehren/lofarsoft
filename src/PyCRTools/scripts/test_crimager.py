#! /usr/bin/env python

"""Test script for CRImager task.
"""

import pycrtools as cr
from pycrtools import xmldict
from pycrtools.tasks import crimager

results = xmldict.load("VHECR_LORA-20110612T231913.199Z/pol0/002/results.xml")

antpos = cr.hArray(results["antenna_positions_array_XYZ_m"], dimensions=(47, 3))
data = cr.hArrayRead("VHECR_LORA-20110612T231913.199Z/pol0/002/VHECR_LORA-20110612T231913.199Z-002-pol0-calibrated-timeseries-cut.pcr")

blocksize = data.shape()[1]

# Get frequencies
frequencies=cr.hArray(float, blocksize / 2 + 1)
cr.hFFTFrequencies(frequencies, 200.e6, 1)

# Initialize imager
cr.trun("CRImager", data = data, blocksize = blocksize, antpos = antpos, frequencies = frequencies, output="crimage.fits")

