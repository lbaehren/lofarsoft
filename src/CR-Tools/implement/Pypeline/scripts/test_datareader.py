#! /usr/bin/env python

import sys
import pycrtools as cr

if len(sys.argv)!=2:
    print "USAGE: test_datareader FILENAME"
    sys.exit()

filename = sys.argv[1]
blocksize = 1024
selection = range(0,10,2)
data = cr.hArray(float, dimensions=(len(selection), blocksize))
fftdata = cr.hArray(complex, dimensions=(len(selection), blocksize / 2 + 1))

f = cr.open(filename, blocksize)

print "Reading metadata:"
for key in f.keys():
    print key, f[key]

print "Applying antenna selection:"
f.setAntennaSelection(selection)

print "Reading metadata for selected antennas:"
for key in f.keys():
    print key, f[key]

print "Reading one block of timeseries data:"
f.getTimeseriesData(data, 0)

print data

print "Reading one block of timeseries data in alternative way:"
f.read("TIMESERIES_DATA", data, 0)

print data

print "Reading one block of FFT data:"
f.getFFTData(fftdata, 0)

print data

print "Reading one block of FFT data in alternative way:"
f.read("FFT_DATA", fftdata, 0)

print data

print "Corresponding frequencies"
print f.getFrequencies()

