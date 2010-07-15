#! /usr/bin/env python

import os
import pycr as cr
import numpy as np

LOFARSOFT=os.environ["LOFARSOFT"]
filename=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5"

dr = cr.open(filename)

dr.antennaset = 'LBA_OUTER'

print dr["antennaids"].shape
print dr["antenna_position"].shape
print dr["cable_delay"].shape
print dr["station_phase_calibration"].shape
print dr["clock_correction"].shape

