#! /usr/bin/env python
# Script to print summary of miscellaneous info about an MS

import sys
import pydal as dal
from numpy import *

if len(sys.argv) == 2:
    file = sys.argv[1]
    doall = 1      # default is to look at all data columns
elif len(sys.argv) == 3:
    file = sys.argv[1]
    doall = int(sys.argv[2])     # faster operation for doall=0
else:
    print "Usage:"
    print "\tmsinfo.py <file> [doall]"
    print "\t<> required"
    print "\t[] optional.  doall=1 (default, a bit slower) looks at all data columns"
    sys.exit(1)
    
# open file
msds= dal.dalDataset()
if ( True != msds.open(sys.argv[1]) ):
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

if doall:
    # get ints/time
    msds.setFilter( "EXPOSURE, DATA", "ANTENNA1 = 0 AND ANTENNA2 = 0" )
    datatab = msds.openTable('MAIN')
    expcol = datatab.getColumn('EXPOSURE')
    exp = expcol.data()
    num_ints = expcol.shape()[0][0]
    # check data columns
    datacol = datatab.getColumn('DATA')
    data = datacol.data(10)
    try:
        msds.setFilter( "CORRECTED_DATA", "ANTENNA1 = 0 AND ANTENNA2 = 0" )
        datatab = msds.openTable('MAIN')
    except RuntimeError:
        correctedcol = 0
    else:
        correctedcol = datatab.getColumn('CORRECTED_DATA')
        corrected = correctedcol.data(10)
        correctedcol = 1

    try:
        msds.setFilter( "MODEL_DATA", "ANTENNA1 = 0 AND ANTENNA2 = 0" )
        datatab = msds.openTable('MAIN')
    except RuntimeError:
        modelcol = 0
    else:
        modelcol = datatab.getColumn('MODEL_DATA')
        model = modelcol.data(10)
        modelcol = 1
        
# get processing history?

# print
print ''
print '\tSummary of UV data for ' + file
print ''
print 'Phase center (deg): (' + str(phasedir[0]*180/3.1415) + ',' + str(phasedir[1]*180/3.1415) + ')'
print 'Frequency range (MHz): (' + str(min(freq/1e6)) + ', ' + str(max(freq/1e6)) + ')'
print 'Wavelength range (m): (' + str(min(299792458.0/freq)) + ', ' + str(max(299792458.0/freq)) + ')'
print 'Time range (MJD): (' + str(time[0]) + ', ' + str(time[1]) + ') s or ' + str((time[1]-time[0])/3600) + ' hrs'
if doall:  print 'Integrations, Time bin size: ' + str(num_ints) + ', ' + str(expcol.data(1)[0]) + ' s'
print 'Channels, Width: ' + str(numchan) + ', ' + str(chan_width/1e3) + ' kHz'
print 'Polarizations: ' + str(num_pols)
print ''
print 'Number of antennas: ' + str(num_ants)
print name
print
if doall:
    print 'Data columns  (first 10 ints, middle channel, xx poln):'
    print 'DATA              ' + str(data[0:10,numchan/2,0])
    if correctedcol:
        print 'CORRECTED_DATA    ' + str(corrected[0:10,numchan/2,0])
    if modelcol:
        print 'MODEL_DATA        ' + str(model[0:10,numchan/2,0])
