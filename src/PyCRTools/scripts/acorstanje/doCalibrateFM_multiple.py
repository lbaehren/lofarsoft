#! /usr/bin/env python

# Test script that has to become a 'pipeline' appended to the normal analysis pipeline...
# Run findRFI and CalibrateFM Task on one data (event) file.
# Output text file with:
# filefilter, date, time, timestamp, strongest freq. MHz, az, el (strongest), phase error of direction fit [ns^2], phase RMS of best channel, interStationDelays sorted ascending on station name (to be replaced with dict)

import os
import sys
nogui=True
import pycrtools as cr
import datetime
from pycrtools.tasks import findrfi, calibratefm

#filefilter = '*20120[89]*CS003*' # one station per event, time interval

#goodEventIDs = [80495081, 81147431, 82312457, 86129434, 87892283, 94175691, 94294418, 95021378, 95193025, 95228015]
goodEventIDs = [86129434, 87892283, 92380604, 94175691, 94294418, 95021378, 95193025, 95228015] # moet na 15 sep 2012 zijn (Smilde)
#goodEventIDs = [95228015]
# 15 sep 2012 == eventID 85363200
def getDatafilesForEventID(eventID, stations = None, datadir = '/vol/astro/lofar/vhecr/lora_triggered/data'):
    eventTime = datetime.datetime(2010, 1, 1) + datetime.timedelta(seconds = eventID)
    # make date+time string
    s = eventTime.strftime("%Y%m%dT%H%M%S")
    filefilter = os.path.join(datadir, '*'+s+'*.h5')
    filelist = cr.listFiles(filefilter)

    sortedlist = []
    if stations is not None:
        for station in stations:
            thisStationsFile = [filename for filename in filelist if station in filename]
            if len(thisStationsFile) > 0:
                sortedlist.append(thisStationsFile[0])
    else:
        sortedlist = filelist

    return sortedlist



#if len(sys.argv) > 1:
#    filefilter = sys.argv[1]
#else:
#    print 'Usage: doCalibrateFM.py filefilter'
#    exit()
#filefilter = '/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS00[26]_R000_tbb.h5'

#filelist = cr.listFiles(filefilter)

# Sort filelist on station name. Suppress some outer stations.
superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
#if len(filelist) == 1:
#    print 'There is only 1 file'
#    filelist = filelist[0]
#else:  # sort files on station ID
#    sortedlist = []
#    for station in superterpStations:
#        thisStationsFile = [filename for filename in filelist if station in filename]
#        if len(thisStationsFile) > 0:
#            sortedlist.append(thisStationsFile[0])
#    filelist = sortedlist

#f = cr.open(filelist, blocksize=8000)
cr.tasks.task_write_parfiles = False # No annoying par files

#lines = [88.0, 88.6, 90.8, 91.8, 94.8]
lines = [88.0]
if not os.path.isfile('calibratefm_output.dat'):
    outfile = open('calibratefm_output.dat', 'w')
    header = 'ID filefilter date time timestamp pol freq phaseError STATUS '
    #for name in superterpStations:
    #    header += name + '_az ' + name + '_el '
    for name in superterpStations: # az/el, then interStationDelays
        header += name + ' '
    outfile.write(header + '\n')
else:
    outfile = open('calibratefm_output.dat', 'a')

for eventid in goodEventIDs:
    filelist = getDatafilesForEventID(eventid, stations=superterpStations, datadir='/Users/acorstanje/triggering/CR/goodlist/')
    f = cr.open(filelist, blocksize=8000)
    print 'DOING EVENT: %d'% eventid

    for pol in range(2):
        print 'DOING POL %d' % pol
        selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == pol]
        f["SELECTED_DIPOLES"] = selected_dipoles

        findRFI_output = cr.trerun("FindRFI", "0", f = f, nofblocks = 1000, apply_hanning_window = True)

        n = 0
        interStationDelays = []
        for freq in lines:
            print 'Doing line %d of %d: %2.1f MHz' % (n, len(lines), freq)

            cal_output = cr.trerun("CalibrateFM", str(n), f = f, phase_average = findRFI_output.phase_average, median_phase_spreads = findRFI_output.median_phase_spreads, phase_RMS = findRFI_output.phase_RMS, freq_range = [freq-0.1, freq+0.1], pol = pol, direction_resolution = [1, 10], referenceTransmitterGPS = (6.403565, 52.902671), doplot=True, testplots=True, save_plots=True, plot_prefix=str(eventid), plot_type='pdf' )

            timestamp = f["TIME"][0]
            datestring = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d')
            timestring = datetime.datetime.fromtimestamp(timestamp).strftime('%H:%M:%S')
            outstr = str(eventid) + ' ' + filelist[0] + ' ' + datestring + ' ' + timestring + ' ' + str(timestamp) + ' ' + str(pol) + ' '
            outstr += format('%2.1f' % freq) + ' '
            outstr += format('%2.3f' % cal_output.phaseError) + ' '
            outstr += cal_output.calibrationStatus + ' '

    #        for name in superterpStations:
    #            fit = cal_output.fittedDirections[name] if name in cal_output.fittedDirections else (-1, -1)
    #            outstr += str(fit[0]) + ' ' + str(fit[1]) + ' ' # az, el

            for name in superterpStations:
                thisDelay = cal_output.interStationDelays[name] if name in cal_output.interStationDelays else -1
                outstr += format('%2.2f' % thisDelay) + ' '
    #        interStationDelays.append(rfilines_output.interStationDelays)
            outfile.write(outstr + '\n')
            n += 1

outfile.close()


