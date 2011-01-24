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
from pycrtools import IO
from optparse import OptionParser

from pycrtools.tasks import imager

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--startblock", type="int", default=0)
parser.add_option("--nblocks", type="int", default=16)
parser.add_option("--ntimesteps", type="int", default=(12487 / 16 - 1) / 2)
parser.add_option("--blocksize", type="int", default=16*1024)
parser.add_option("--nfmin", type="int", default=5750)
parser.add_option("--nfmax", type="int", default=6050)
parser.add_option("--antennaset", metavar="NAME", default="HBA")
parser.add_option("--naxis1", type="int", default=41)
parser.add_option("--naxis2", type="int", default=41)
parser.add_option("--angres", type="float", default=0.25)
parser.add_option("-o", "--output", dest="storename",
                  help="write output to FILE", metavar="FILE", default="out")

(options, args) = parser.parse_args()

# Filenames
if not args:
    print "Error: No HDF5 files specified for input"
    sys.exit(1)

filenames = args

print options.storename
storename = options.storename.rstrip(".fits")

print "Using files:"
for n in filenames:
    print n

# Parameters
startblock=options.startblock
nblocks=options.nblocks
ntimesteps=options.ntimesteps
blocksize=options.blocksize
antennaset=options.antennaset
nfmin=options.nfmin
nfmax=options.nfmax
nfreq=nfmax-nfmin

# Select even antennae
selection=range(0,len(filenames)*48,2)

# Open datafile object
crfile=IO.open(filenames, blocksize, selection)

# Set antennaset
crfile.setAntennaset(antennaset)

# Get calibration data and settings from file
nyquistZone=crfile["nyquistZone"]

# Set frequency range
crfile.setFrequencyRangeByIndex(nfmin, nfmax)

frequencies = crfile.getFrequencies()

# Enable weight and gain calibration
crfile.enableCalibration()

# Get observation time (as UNIX timestamp)
obstime = crfile["TIME"][0]

# Set image parameters
# Around the Crab (J2000)
wcs = {
    'NAXIS' : 2,
    'NAXIS1' : options.naxis1,
    'NAXIS2' : options.naxis2,
    'CTYPE1' : 'RA---SIN',
    'CTYPE2' : 'DEC--SIN',
    'CTYPE3' : 'FREQ',
    'CTYPE4' : 'TIME',
    'LONPOLE' : 0.,
    'LATPOLE' : 90.,
    'CRVAL1' : pytmf.hms2deg(5, 34, 31.97),
    'CRVAL2' : pytmf.dms2deg(22, 0, 52.0),
    'CRVAL3' : frequencies[0],
    'CRVAL4' : 0.,
    'CRPIX1' : float(options.naxis1) / 2,
    'CRPIX2' : float(options.naxis2) / 2,
    'CRPIX3' : 0.0,
    'CRPIX4' : 0.0,
    'CDELT1' : -1.0 * options.angres,
    'CDELT2' : 1.0 * options.angres,
    'CDELT3' : frequencies[1]-frequencies[0],
    'CDELT4' : nblocks*blocksize*(1./crfile["sampleFrequency"]),
    'CUNIT1' : 'deg',
    'CUNIT2' : 'deg',
    'CUNIT3' : 'Hz',
    'CUNIT4' : 's',
}

print wcs

# Create empty image
image=cr.hArray(float, dimensions=(ntimesteps, wcs['NAXIS1'], wcs['NAXIS2'], nfreq), fill=0.)

# Initialize imager
im = imager.Imager(image, crfile, startblock, nblocks, ntimesteps, obstime, wcs=wcs)

# Run imager to create image and write it into the provided array
im.run()

print "Image complete, storing to disk."

# Get image as numpy.ndarray
npimage = image.toNumpy()

np.save(storename+".npy", npimage)

npimage = np.rollaxis(npimage, 0, 4).transpose()

# Save image as FITS
hdu = pyfits.PrimaryHDU(npimage)

# Write WCS parameters to header
hdr = hdu.header

keys = wcs.keys()
keys.sort()
for key in keys:
    hdr.update(key, wcs[key])

# Check if file exists and overwrite if so
if os.path.isfile(storename+'.fits'):
    os.remove(storename+'.fits')
hdu.writeto(storename+'.fits')

