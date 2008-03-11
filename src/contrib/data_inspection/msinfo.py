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

# get ints/time
# what table?

# print
print ''
print '\tSummary of UV data for ' + file
print ''
print 'Phase center: ' + str(phasedir)
print 'Frequency range (MHz): (' + str(min(freq/1e6)) + ', ' + str(max(freq/1e6)) + ')'
print 'Wavelength range (m): (' + str(min(299792458.0/freq)) + ', ' + str(max(299792458.0/freq)) + ')'
print 'Time range (MJs): (' + str(time[0]) + ', ' + str(time[1]) + ') or ' + str((time[1]-time[0])/3600) + ' hrs'
#print 'Integrations, Size: ' + str(num_ints) + ', ' + str(time_bin) + ' s'
print 'Channels, Width: ' + str(numchan) + ', ' + str(chan_width/1e3) + ' kHz'
print 'Polarizations: ' + str(num_pols)
