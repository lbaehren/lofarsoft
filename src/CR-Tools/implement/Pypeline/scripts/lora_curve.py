#! /usr/bin/env python

"""Example script showing how to use the imager.

This script is used to make basic images of the Crab pulsar.

.. note:: Please do not add too much functionality here since this would defeat the instructive purpose of this script.
"""

import os
import os.path
import sys
import numpy as np
import pyfits
import pycrtools as cr
import pytmf
import matplotlib.pyplot as plt
from optparse import OptionParser

from pycrtools.tasks import imager
from pycrtools import srcfind

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--startblock", type="int", default=5998)#93)#5888)
parser.add_option("--nblocks", type="int", default=1)
parser.add_option("--ntimesteps", type="int", default=1)
parser.add_option("--blocksize", type="int", default=1024)#65536)
parser.add_option("--naxis1", type="int", default=90)
parser.add_option("--naxis2", type="int", default=90)
parser.add_option("--angres", type="float", default=2)
parser.add_option("-o", "--output", dest="output",
                  help="write output to FILE", metavar="FILE", default="out")

(options, args) = parser.parse_args()

log = open(options.output+"-"+"log.txt", 'w')

# Filenames
if not args:
    print "Error: No HDF5 files specified for input"
    sys.exit(1)

filenames = args

output = options.output

print "Using files:"
for n in filenames:
    print n

# Parameters
startblock=options.startblock
nblocks=options.nblocks
blocksize=options.blocksize
ntimesteps=options.ntimesteps

# Select even antennae
selection=range(0, 92, 2)#["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]

# Open datafile object
f=cr.open(filenames[0], blocksize) # Only single file mode supported
f["ANTENNA_SET"]='LBA_OUTER'

# Set antenna selection
f.setAntennaSelection(selection)

data=cr.hArray(float, dimensions=(f["NOF_SELECTED_DATASETS"], f["BLOCKSIZE"]))

pos=f["ANTENNA_POSITIONS"].toNumpy()

f.getTimeseriesData(data, options.startblock)
    
data2 = data.toNumpy()

plt.clf()
col=[]
for j in range(f["NOF_SELECTED_DATASETS"]):
    col.append(np.argmax(data2[j]))
    print j, np.argmax(data2[j]), pos[j]
#    plt.plot(np.square(data2[j]))
    
#plt.savefig("CR.png")

#plt.figure()
plt.scatter(pos[:,0], pos[:,1], c=col)
for i in range(f["NOF_SELECTED_DATASETS"]):
    plt.annotate(str(i), (pos[i,0],pos[i,1]))
plt.colorbar()
plt.figure()

# Run with pulsecal
pulse=cr.trun("LocatePulseTrain", data, nsigma=7, maxgap=3)

if pulse.npeaks <= 0:
    raise ValueError("No pulse found!")

# Normalize the data which was cut around the main pulse
pulse.timeseries_data_cut[...]-=pulse.timeseries_data_cut[...].mean()
pulse.timeseries_data_cut[...]/=pulse.timeseries_data_cut[...].stddev(0)

# Cross correlate all pulses with each other 
crosscorr=cr.trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=5)

# And determine the relative offsets between them
mx=cr.trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=10**-9,peak_width=6,splineorder=2)

print "Time lag [ns]: ", mx.lags 
print " "

plt.figure()
plt.scatter(pos[:,0], pos[:,1], c=cr.hArray(mx.lags).toNumpy())
for i in range(f["NOF_SELECTED_DATASETS"]):
    plt.annotate(str(i), (pos[i,0],pos[i,1]))
plt.colorbar()

# Find direction
result_brute = srcfind.findDirectionBruteAzElStandard(pos, cr.hArray(mx.lags).toNumpy().ravel())
result_fmin = srcfind.findDirectionFminAzElStandard(pos, cr.hArray(mx.lags).toNumpy().ravel())
print "Brute search", [pytmf.rad2deg(item) for item in result_brute]
print "Fmin search", [pytmf.rad2deg(item) for item in result_fmin]

plt.show()

