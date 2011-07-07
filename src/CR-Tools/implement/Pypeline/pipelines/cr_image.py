#! /usr/bin/env python

"""Pipeline to generate images of LOFAR Cosmic Ray events.

By default this script generates a 180 x 180 degree all-sky image.
"""

import sys
import pycrtools as cr
from optparse import OptionParser
from pycrtools.tasks import imager

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--startblock", type="int", default=5998)
parser.add_option("--nblocks", type="int", default=1)
parser.add_option("--ntimesteps", type="int", default=1)
parser.add_option("--blocksize", type="int", default=1024)
parser.add_option("--naxis1", type="int", default=180)
parser.add_option("--naxis2", type="int", default=180)
parser.add_option("--ctype1", type="string", default="ALON-STG")
parser.add_option("--ctype2", type="string", default="ALAT-STG")
parser.add_option("--crval1", type="float", default=0.0)
parser.add_option("--crval2", type="float", default=90.0)
parser.add_option("--angres", type="float", default=1)
parser.add_option("--polarisation", type="string", default="odd")
parser.add_option("--antenna-set", type="string", default="LBA_OUTER")
parser.add_option("-o", "--output", dest="output",
                  help="write output to FILE", metavar="FILE", default="out")

(options, args) = parser.parse_args()

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

# Open datafile object
if len(filenames) > 1:
    print "WARNING: only single file mode is currently supported,"
    print "using first file only."

f=cr.open(filenames[0], blocksize)

# Select polarisation
if options.polarisation is "odd":
    selection=range(1, f["NOF_DIPOLE_DATASETS"], 2)
elif options.polarisation is "even":
    selection=range(0, f["NOF_DIPOLE_DATASETS"], 2)
elif options.polarisation is "both":
    selection=range(f["NOF_DIPOLE_DATASETS"])

f["ANTENNA_SET"]=options.antenna_set

# Set antenna selection
f.setAntennaSelection(selection)

# Set image parameters given by commandline options
NAXIS1 = options.naxis1
NAXIS2 = options.naxis2
CTYPE1 = options.ctype1
CTYPE2 = options.ctype2
CRVAL1 = options.crval1
CRVAL2 = options.crval2

# Set fixed or computed image parameters
CRPIX1 = float(NAXIS1) / 2
CRPIX2 = float(NAXIS2) / 2
CDELT1 = -1.0 * options.angres
CDELT2 = 1.0 * options.angres
CUNIT1 = 'deg'
CUNIT2 = 'deg'

# Initialize imager
im = imager.Imager(data = f, output=options.output, startblock = startblock, nblocks = nblocks, ntimesteps = ntimesteps, inversefft = True, NAXIS1 = NAXIS1, NAXIS2 = NAXIS2, CTYPE1 = CTYPE1, CTYPE2 = CTYPE2, CRVAL1 = CRVAL1, CRVAL2 = CRVAL2, CRPIX1 = CRPIX1, CRPIX2 = CRPIX2, CDELT1 = CDELT1, CDELT2 = CDELT2, CUNIT1 = CUNIT1, CUNIT2 = CUNIT2)

# Run imager
im()

