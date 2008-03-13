#! /usr/bin/env python
# Script to print summary of miscellaneous info about an MS

import sys
import pydal as dal
from numpy import *

if len(sys.argv) == 2:
    file = sys.argv[1]
else:
    print "Usage:"
    print "\tmsinfo.py <file>"
    print "\t<> required"
    sys.exit(1)
    
# open file
msds= dal.dalDataset()
if ( msds.open(sys.argv[1]) ):
        sys.exit(1)

# get frequencies
freqtab = msds.openTable('SPECTRAL_WINDOW')
freqcol = freqtab.getColumn('CHAN_FREQ')
freq = freqcol.data()[0]
freqcol = freqtab.getColumn('NUM_CHAN')
numchan = freqcol.data()[0]
freqcol = freqtab.getColumn('CHAN_WIDTH')
chan_width = freqcol.data()[0][0]

# get pols
poltab = msds.openTable('POLARIZATION')
polcol = poltab.getColumn("NUM_CORR")
num_pols = polcol.data()[0]

# get times
obstab = msds.openTable('OBSERVATION')
timecol = obstab.getColumn('TIME_RANGE')
time = timecol.data()[0]

# get phase dir
fieldtab = msds.openTable('FIELD')
phasedircol = fieldtab.getColumn('PHASE_DIR')
phasedir = phasedircol.data()[0][0]

# get antenna names
anttab = msds.openTable('ANTENNA')
namecol = anttab.getColumn('NAME')
name = namecol.data()
num_ants = len(name)

# get ints/time
msds.setFilter( "EXPOSURE, DATA", "ANTENNA1 = 0 AND ANTENNA2 = 0" )
datatab = msds.openTable('MAIN')
expcol = datatab.getColumn('EXPOSURE')
exp = expcol.data()
num_ints = len(exp)
# get processing history?

# print
print ''
print '\tSummary of UV data for ' + file
print ''
print 'Phase center (deg): (' + str(phasedir[0]*180/3.1415) + ',' + str(phasedir[1]*180/3.1415) + ')'
print 'Frequency range (MHz): (' + str(min(freq/1e6)) + ', ' + str(max(freq/1e6)) + ')'
print 'Wavelength range (m): (' + str(min(299792458.0/freq)) + ', ' + str(max(299792458.0/freq)) + ')'
print 'Time range (MJD): (' + str(time[0]) + ', ' + str(time[1]) + ') s or ' + str((time[1]-time[0])/3600) + ' hrs'
print 'Integrations, Time bin size: ' + str(num_ints) + ', ' + str(exp[0]) + ' s'
print 'Channels, Width: ' + str(numchan) + ', ' + str(chan_width/1e3) + ' kHz'
print 'Polarizations: ' + str(num_pols)
print ''
print 'Number of antennas: ' + str(num_ants)
print name
