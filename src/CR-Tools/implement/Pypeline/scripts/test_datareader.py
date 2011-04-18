#! /usr/bin/env python

import sys
import pycrtools as cr
from optparse import OptionParser

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--blocksize", type="int", default=1024)

(options, args) = parser.parse_args()

# Filenames
if not args:
    parser.print_help()
    sys.exit(1)

for filename in args:

    blocksize = options.blocksize
    selection = range(0,10,2)
    data = cr.hArray(float, dimensions=(len(selection), blocksize))
    fftdata = cr.hArray(complex, dimensions=(len(selection), blocksize / 2 + 1))
    
    f = cr.open(filename, blocksize)
    
    print "Reading metadata:"
    for key in sorted(f.keys()):
        print key, f[key]
        print ""
    
    print "Applying antenna selection:"
    #f.setAntennaSelection(selection)
    f["SELECTED_DIPOLES"] = selection
    
    print "Reading metadata for selected antennas:"
    for key in sorted(f.keys()):
        print key, f[key]
        print ""
    
    print "Reading one block of timeseries data:"
    f.getTimeseriesData(data, 0)
    
    print data
    
    print "Reading one block of timeseries data in alternative way:"
    f.read("TIMESERIES_DATA", data, 0)
    
    print data
    
    print "Reading one block of FFT data:"
    f.getFFTData(fftdata, 0)
    
    print fftdata
    
    print "Reading one block of FFT data in alternative way:"
    f.read("FFT_DATA", fftdata, 0)
    
    print fftdata
    
    print "Corresponding frequencies"
    print f.getFrequencies()
    
