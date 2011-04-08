#!/usr/bin/env python
#
#  downloadtriggerlogs.py
#  CRPipeline
#
#  Created by Arthur Corstanje on 4/8/11.
#  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
#

from datetime import datetime, timedelta
import time
import subprocess
import sys
import os

def readStationListAndDates(obsID):
    obsIDfile = open(obsID, mode='r')
    for line in obsIDfile:
        if 'stationList' in line:
            stationList = line.split('=')[1].strip('[]\n').split(',')        
        elif 'Observation.startTime' in line:
            startTime = line.split('=')[1].strip('\n').split(' ')[0]
            startDate = datetime.strptime(startTime, '%Y-%b-%d')
#            startDateString = str(startDate.year) + '-' + str(startDate.month) + '-' + str(startDate.day)
        elif 'Observation.stopTime' in line:
            stopTime = line.split('=')[1].strip('\n').split(' ')[0]
            stopDate = datetime.strptime(stopTime, '%Y-%b-%d')
    # now also make all intermediate date strings
    nofDays = (stopDate - startDate).days
    inc = timedelta(1) # 1 day
    thisDate = startDate
    dateStrings = []
    for i in range(nofDays+1):
        thisString = thisDate.strftime('%Y-%m-%d')
        dateStrings.append(thisString)
        thisDate = thisDate + inc
        
    return (stationList, dateStrings)
    


if len(sys.argv) > 1:
    obsID = sys.argv[1] # The file name of the observation
    print 'Taking observation ID file: %s' % obsID
else:
    print 'Usage: downloadtriggerlogs Observation<xxxx>'
    print 'i.e. pass the Observation ID file as parameter.'
    exit()

(stationList, dateStrings) = readStationListAndDates(obsID)
print 'Station list for this observation: '
print stationList
print 'Date strings: '
print dateStrings

for station in stationList:
    for date in dateStrings:
        print 'Downloading station %s, file for date %s' % (station, date)        
        proc = subprocess.Popen(['scp','-C', station+'C:/localhome/data/'+date+'_TRIGGER.dat', '.'])
        proc.wait()

