#! /usr/bin/env python
#
# Download (rsync) all trigger logfiles from a list of LOFAR stations
#  Created by Arthur Corstanje on Oct 24, 2011.
#  Copyright (c) 2011. All rights reserved.

import subprocess
import os

stationList = ['CS001C', 'CS002C', 'CS003C', 'CS004C', 'CS005C', 'CS006C', 'CS007C', 'CS011C', 'CS017C', 'CS021C']
topdir = '/Users/acorstanje/triggering/triggers'

for station in stationList:
    thisDir = os.path.join(topdir, station)  
    if not os.path.exists(thisDir):
        os.mkdir(thisDir)
#    triggerFilename = date + '_TRIGGER-'+station+'.dat'
    print 'Downloading station %s' % station
    proc = subprocess.Popen(['rsync', '-avv', station+':/localhome/data/2*_TRIGGER.dat', thisDir])
    proc.wait()
