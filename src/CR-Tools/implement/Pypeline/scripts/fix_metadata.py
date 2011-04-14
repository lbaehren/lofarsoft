#! /usr/bin/env python

from optparse import OptionParser
from pycrtools import metadata as md
import sys
import h5py

# Parse commandline options
usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = OptionParser(usage=usage)
parser.add_option("--antenna_set", type="string", default="UNDEFINED")
parser.add_option("--nyquist_zone", type="int", default=1)

(options, args) = parser.parse_args()

# Filenames
if not args:
    parser.print_help()
    sys.exit(1)

filenames = args

# Check arguments
icd_antenna_set = ["LBA_INNER", "LBA_OUTER", "LBA_SPARSE_EVEN", "LBA_SPARSE_ODD", "LBA_X", "LBA_Y", "HBA_ZERO", "HBA_ONE", "HBA_DUAL", "HBA_JOINED"]
if not options.antenna_set in icd_antenna_set:
    print "ERROR: Invalid antennaset, can only be one of:"
    print icd_antenna_set
    sys.exit(1)

for name in filenames:
    print "fixing", name
    
    f = h5py.File(name, "a")
    
    for station in f.itervalues():
        # Get station number
        sn = int(station.name.strip("/Station"))

        # Get cable delays for station
        delays = md.getCableDelays(sn, options.antenna_set)

        selection = station.keys()

        # Get antenna positions for all antennas
        antpos = md.get("AbsoluteAntennaPositions", selection, options.antenna_set)
        antpos = antpos.reshape(len(selection),3)

        for i, dataset in enumerate(station.itervalues()):

            # Set dataset level attributes

            dataset.attrs["NYQUIST_ZONE"] = options.nyquist_zone
            dataset.attrs["ANTENNA_POSITION_VALUE"] = antpos[i]
            dataset.attrs["ANTENNA_POSITION_UNIT"] = ["m", "m", "m"]
            dataset.attrs["ANTENNA_POSITION_FRAME"] = "ITRF"
            dataset.attrs["CABLE_DELAY"] = delays[i]
            dataset.attrs["CABLE_DELAY_UNIT"] = "s"

    # Write changes to file and close
    f.flush()
    f.close()

