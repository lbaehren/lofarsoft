#!/usr/bin/env python
#
#  crosschecktimes.py
#  CRPipeline
#
#  Created by Arthur Corstanje on 4/5/11.
#  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
#
# TO DO: 
# - save LOFAR timestamps to file
# - read in LOFAR timestamps from file saved earlier
# - put cross-check results in list, with time diffs and file name
# - post-process that list... best match, etc.

import subprocess
import sys
import os
import time
import pickle
from datetime import datetime

dateKey = 0
hourKey = 1
nsecKey = 2
thetaKey = 3
phiKey = 4

def readLORAevents(infile):
    eventsFile = open(infile, mode='r')
    results = []
    for line in eventsFile:
        thisEvent = line.split()
        if thisEvent[0][0] == '#':
            continue
        thisDate = int(thisEvent[dateKey])
        thisTime = int(thisEvent[hourKey])
        day = thisDate % 100
        thisDate = (thisDate - day) / 100
        month = thisDate % 100
        thisDate = (thisDate - month) / 100
        year = thisDate
        
        second = thisTime % 100
        thisTime = (thisTime - second) / 100
        minute = thisTime % 100
        thisTime = (thisTime - minute) / 100
        hour = thisTime
        
        d = datetime(year, month, day, hour, minute, second)
        
        thisDateString = '%d-%d-%d %d:%d:%d' % (year, month, day, hour, minute, second)
        print 'Date string = %s' % thisDateString
        
        thisTimestamp = int(time.mktime(d.timetuple()) - time.timezone)
        
        #thisTimestamp = int(time.mktime(time.strptime(thisDateString, '%Y-%m-%d %H:%M:%S'))) 
        #                    - time.timezone + 3600 * time.localtime().tm_isdst  
                            # Account for time zone and daylight saving time!
        print thisTimestamp
        thisNanosec = int(float(thisEvent[nsecKey])) # int('3.0') fails!
        thisTheta = float(thisEvent[thetaKey])
        thisPhi = float(thisEvent[phiKey])
        
        results.append([thisTimestamp, thisNanosec, thisTheta, thisPhi])
    
    return results

def readLOFARevents(fileList):
    results = []
    for file in fileList:
        print 'Processing file: %s:       ' % file,
        p1 = subprocess.Popen(['h5ls -rv ' + file + ' | grep -A2 TIME | grep -m1 Data:'], shell=True, stdout=subprocess.PIPE)
        output = p1.communicate()[0]
        if len(output) < 5:
            print 'error reading file: %s' % file
            continue
        timestamp = int(output.split()[1])
        print timestamp,

        p1 = subprocess.Popen(['h5ls -rv ' + file + ' | grep -A2 SAMPLE_NUMBER | grep -m1 Data:'], shell=True, stdout=subprocess.PIPE)
        output = p1.communicate()[0]
        samplenumber = int(output.split()[1])
        
        nsec = samplenumber * 5
        print ' %d' % nsec
        
        results.append([timestamp, nsec, file])
    return results

def crossCheckTimes(LORAevents, LOFARevents):
    # LORA events have [timestamp, nsec, theta, phi]
    # LOFAR events have [timestamp, nsec] for the time being
    
    for event in LOFARevents:
        # match for seconds only
        thisTimestamp = event[0]
        match = []
        for LORAevent in LORAevents:
            #print '%d --- %d' % (LORAevent[0], thisTimestamp)
            if LORAevent[0] - 15 == thisTimestamp:
                tdiff = event[1] - LORAevent[1]
#                tdiffsec = float(tdiff) / 1e9
                tdiffms = float(tdiff) / 1e6
                
                print 'Match found! %s, (%d, %d) === (%d, %d), difference is: %3.6f ms' % (event[2], event[0], event[1], LORAevent[0], LORAevent[1], tdiffms)
                match.append([LORAevent, event])
        
#        if len(match) > 0:
#            print 'Match found! %s, (%d, %d) === (%d, %d)' % (event[2], event[0], event[1], LORAevent[0], LORAevent[1])
    
    
    
if len(sys.argv) > 2:
    datafiles = sys.argv[1]
    crossCheckFilename = sys.argv[2]
else:
    print 'No files given on command line, using a default set instead.'
#    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/automatic_obs_test-15febOvernight--147-10*.h5'
#    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/automatic_obs_test-15febOvernight--147-441.h5'
#    datafiles = '/Users/acorstanje/triggering/MACdatarun_2feb2011/automatic_obs_test-2feb-2-26.h5'
    datafiles = '/Users/acorstanje/triggering/fullLOFAR-12hr/superterp/fullLOFAR-*.h5'
    crossCheckFilename = '/Users/acorstanje/triggering/fullLOFAR-12hr/LORAevents.dat'
       
#eventList = readLORAevents(crossCheckFilename)
#pickle.dump(eventList, 'loraevents.pic')

sortstring = 'sort -n --field-separator="-" --key=18'
outfile = 'crPipelineResults.txt'
outfileAscii = 'crossCheckResults.txt'
antennaset="LBA_OUTER"

#fd = os.popen('ls '+ datafiles+' | ' + sortstring)
#p1 = subprocess.Popen(['ls '+ datafiles + ' | ' + sortstring], shell=True, stdout=subprocess.PIPE)
(directory, files) = os.path.split(datafiles)
p1 = subprocess.Popen(['find ' + directory + ' -type f -name "'+ files + '" | ' + sortstring], shell=True, stdout=subprocess.PIPE)

output = p1.communicate()[0]
files = output.splitlines()
nofiles = len(files)
print "Number of files to process:", nofiles

LOFARlist = readLOFARevents(files)
#pickle.dump(LOFARlist, 'lofarevents.pic')
print LOFARlist
#crossCheckTimes(eventList, LOFARlist)
