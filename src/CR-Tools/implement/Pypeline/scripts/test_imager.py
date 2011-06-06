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
from optparse import OptionParser

from pycrtools.tasks import imager

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--startblock", type="int", default=0)
parser.add_option("--nblocks", type="int", default=16)
parser.add_option("--ntimesteps", type="int", default=None) #(12487 / 16 - 1) / 2)
parser.add_option("--blocksize", type="int", default=16*1024)
parser.add_option("--nfmin", type="int", default=5750)
parser.add_option("--nfmax", type="int", default=6050)
parser.add_option("--naxis1", type="int", default=41)
parser.add_option("--naxis2", type="int", default=41)
parser.add_option("--angres", type="float", default=0.25)
parser.add_option("--dispersion-measure", type="float", default=None)
parser.add_option("-o", "--output", dest="storename",
                  help="write output to FILE", metavar="FILE", default="out")

(options, args) = parser.parse_args()

# Filenames
if not args:
    print "Error: No HDF5 files specified for input"
    sys.exit(1)

filenames = args

storename = options.storename

print "Using files:"
for n in filenames:
    print n

# Parameters
startblock=options.startblock
nblocks=options.nblocks
blocksize=options.blocksize
nfmin=options.nfmin
nfmax=options.nfmax
nfreq = nfmax-nfmin

# Select even antennae
selection=range(0, 48, 2)

# Open datafile object
f=cr.open(filenames[0], blocksize) # Only single file mode supported

# Set antenna selection
f.setAntennaSelection(selection)

if not options.ntimesteps:
    ntimesteps = f["MAXIMUM_READ_LENGTH"] / (nblocks * blocksize)
else:
    ntimesteps = options.ntimesteps
print "Number of time steps:", ntimesteps

# Get frequencies
frequencies = f.getFrequencies()

# Create frequency mask
mask = cr.hArray(int, nfreq, fill=0)

# Get observation time (as UNIX timestamp)
obstime = f["TIME"][0]

# Set image parameters
# Around the Crab (J2000)
NAXIS = 2
NAXIS1 = options.naxis1
NAXIS2 = options.naxis2
CTYPE1 = 'RA---SIN'
CTYPE2 = 'DEC--SIN'
CTYPE3 = 'FREQ'
CTYPE4 = 'TIME'
LONPOLE = 0.
LATPOLE = 90.
CRVAL1 = pytmf.hms2deg(5, 34, 31.97)
CRVAL2 = pytmf.dms2deg(22, 0, 52.0)
CRVAL3 = frequencies[nfmin]
CRVAL4 = 0.
CRPIX1 = float(options.naxis1) / 2
CRPIX2 = float(options.naxis2) / 2
CRPIX3 = 0.0
CRPIX4 = 0.0
CDELT1 = -1.0 * options.angres
CDELT2 = 1.0 * options.angres
CDELT3 = frequencies[nfmin+1]-frequencies[nfmin]

if f["SAMPLE_FREQUENCY_UNIT"][0] == "MHz":
    CDELT4 = nblocks*blocksize*(1./(f["SAMPLE_FREQUENCY_VALUE"][0]*1e6))
elif f["SAMPLE_FREQUENCY_UNIT"][0] == "Hz":
    CDELT4 = nblocks*blocksize*(1./(f["SAMPLE_FREQUENCY_VALUE"][0]))
else:
    raise ValueError("Unknown frequency value")

CUNIT1 = 'deg'
CUNIT2 = 'deg'
CUNIT3 = 'Hz'
CUNIT4 = 's'

# Create empty image
if options.dispersion_measure:
    image = cr.hArray(np.zeros(shape=(ntimesteps, NAXIS1, NAXIS2), dtype='float'))
else:
    image = np.zeros(shape=(ntimesteps, NAXIS1, NAXIS2, nfreq), dtype='float')

# Initialize imager
im = imager.Imager(image = image, data = f, mask = mask, startblock = startblock, nblocks = nblocks, ntimesteps = ntimesteps, obstime = obstime, NAXIS = NAXIS,NAXIS1 = NAXIS1, NAXIS2 = NAXIS2, CTYPE1 = CTYPE1, CTYPE2 = CTYPE2, LONPOLE = LONPOLE,LATPOLE = LATPOLE,CRVAL1 = CRVAL1, CRVAL2 = CRVAL2, CRPIX1 = CRPIX1, CRPIX2 = CRPIX2, CDELT1 = CDELT1, CDELT2 = CDELT2, CUNIT1 = CUNIT1, CUNIT2 = CUNIT2, nfmin = nfmin, nfmax = nfmax, DM = options.dispersion_measure, dt = CDELT4)

#np.save("/Users/pschella/Desktop/frequencies_new.npy", frequencies.toNumpy())

# Run imager to create image and write it into the provided array
im()

print "Image complete, storing to disk."

# Save image as FITS
if options.dispersion_measure:
    image = image.toNumpy().transpose()

#image = np.rollaxis(image, 0, 4).transpose()
hdu = pyfits.PrimaryHDU(image)

# Write WCS parameters to header
hdr = hdu.header

hdr.update('CTYPE1' ,CTYPE1 )
hdr.update('CTYPE2' ,CTYPE2 )
hdr.update('CTYPE3' ,CTYPE3 )
hdr.update('CTYPE4' ,CTYPE4 )
hdr.update('LONPOLE',LONPOLE)
hdr.update('LATPOLE',LATPOLE)
hdr.update('CRVAL1' ,CRVAL1 )
hdr.update('CRVAL2' ,CRVAL2 )
hdr.update('CRVAL3' ,CRVAL3 )
hdr.update('CRVAL4' ,CRVAL4 )
hdr.update('CRPIX1' ,CRPIX1 )
hdr.update('CRPIX2' ,CRPIX2 )
hdr.update('CRPIX3' ,CRPIX3 )
hdr.update('CRPIX4' ,CRPIX4 )
hdr.update('CDELT1' ,CDELT1 )
hdr.update('CDELT2' ,CDELT2 )
hdr.update('CDELT3' ,CDELT3 )
hdr.update('CDELT4' ,CDELT4 )
hdr.update('CUNIT1' ,CUNIT1 )
hdr.update('CUNIT2' ,CUNIT2 )
hdr.update('CUNIT3' ,CUNIT3 )
hdr.update('CUNIT4' ,CUNIT4 )

# Check if file exists and overwrite if so
if os.path.isfile(storename+'.fits'):
    os.remove(storename+'.fits')
hdu.writeto(storename+'.fits')

