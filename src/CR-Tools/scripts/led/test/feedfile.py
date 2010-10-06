#! /usr/bin/env python

import sys
import os
import time

if len(sys.argv) > 2:
    filename = sys.argv[1]
    runningTime = int(sys.argv[2]) 
else:
#    filename = '2009-09-03_TRIGGER5.dat'   #  that's just handy for rapid testing...
    print 'No filename given; usage: ./feedfile.py <filename> <runningtime>. Give runningtime = 0 for real-time'
    exit(1)

def nextTrigger(file):
    while True:
        line = file.readline()
        if line == '':
            return ''
            break
        trigger = line.split()
        if int(trigger[2]) > 1e9 and int(trigger[2]) < 2.1e9:
            break
    return line

def latestTriggerTime():
    with os.popen("tail -1 " + filename) as f:
        trigger = f.readline().split()
        if int(trigger[2]) < 1e9 or int(trigger[2]) > 2.1e9:
            print "Error reading latest timestamp in the file!"
            exit(1)
    return int(trigger[2])

startTime = time.time() # LOFAR uses UTC timestamps and time.time() gives also UTC
currentTime = startTime
triggerEndTime = latestTriggerTime()
    
with open(filename,'r') as f:
    line = nextTrigger(f)
    trigger = line.split()
    triggerStartTime = int(trigger[2])
    if runningTime == 0:
        runningTime = triggerEndTime - triggerStartTime
    print runningTime
    print triggerStartTime
    print triggerEndTime
    #time.sleep(10)
    linecount = 0
    while len(trigger) > 0:
        thisTriggerTime = int(trigger[2])       
        while (currentTime - startTime) / runningTime < float((thisTriggerTime - triggerStartTime)) / (triggerEndTime - triggerStartTime):
            time.sleep(0.1)
            currentTime = time.time()

        print line,
        line = nextTrigger(f)
        trigger = line.split()
#        linecount += 1
#        if linecount > 1000:
#            break


