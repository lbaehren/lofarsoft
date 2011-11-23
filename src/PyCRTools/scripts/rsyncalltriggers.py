#! /usr/bin/env python
#
# Download (rsync) all trigger logfiles from a list of LOFAR stations
#  Created by Arthur Corstanje on Oct 24, 2011.
#  Copyright (c) 2011. All rights reserved.

import subprocess
import os

stationList = ["CS001","CS002","CS003","CS004","CS005","CS006","CS007","CS011","CS013","CS017","CS021","CS024","CS026","CS028","CS030","CS031","CS101","CS103","CS201","CS301","CS302","CS401","CS501","RS106","RS205","RS208","RS306","RS307","RS406","RS503"]
#topdir = '/Users/acorstanje/triggering/triggers'
topdir = '/Volumes/WDdata/triggers'
for station in stationList:
    thisDir = os.path.join(topdir, station+'C')  
    if not os.path.exists(thisDir):
        os.mkdir(thisDir)
#    triggerFilename = date + '_TRIGGER-'+station+'.dat'
    print 'Downloading station %s' % station
    proc = subprocess.Popen(['rsync', '-avv', '--progress', station+'C' + ':/localhome/data/201[123456789]*_TRIGGER.dat', thisDir])
    proc.wait()
