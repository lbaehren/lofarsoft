#! /usr/bin/env python

import sys
import time

if len(sys.argv) > 1:
    filename = sys.argv[1] 
else:
#    filename = '2009-09-03_TRIGGER5.dat'   #  that's just handy for rapid testing...
    print 'No filename given; usage: ./feedfile.py <filename>.'
    exit(1)

skipLines = 10
startTime = time.time() # LOFAR uses UTC timestamps and time.time() gives also UTC

with open(filename,'r') as f:
    for i in range(skipLines):
        line = f.readline()
    
    vars = line.split()
    triggerStartTime = int(vars[2])
    currentTime = 0
    for line in f:
        thisTriggerTime = int(line.split()[2])
        if thisTriggerTime < 2.1e9:
            thisTriggerTime -= triggerStartTime
            while currentTime < thisTriggerTime:
                time.sleep(0.1)
                currentTime = time.time() - startTime

            print line,
     
