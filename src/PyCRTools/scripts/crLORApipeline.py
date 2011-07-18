#!/usr/bin/env python
#
#  crLORApipeline.py
#  CRPipeline
#
#  Created by Arthur Corstanje on 6/14/11.
#  Copyright (c) 2011 __MyCompanyName__. All rights reserved.

from pycrtools import *
import pycrtools as cr 
from pycrtools import pulsefit as pf
from pycrtools import beamformer as bf
import numpy as np

dateKey = 0
hourKey = 1
timestampKey = 2
nsecKey = 3
thetaKey = 4
phiKey = 5

def readLORAevents(infile):
    eventsFile = open(infile, mode='r')
    results = []
    for line in eventsFile:
        thisEvent = line.split()
        if thisEvent[0][0] == '#':
            continue
        thisTimestamp = int(thisEvent[timestampKey])
        #thisTimestamp = int(time.mktime(time.strptime(thisDateString, '%Y-%m-%d %H:%M:%S'))) 
        #                    - time.timezone + 3600 * time.localtime().tm_isdst  
                            # Account for time zone and daylight saving time!
        thisNanosec = int(float(thisEvent[nsecKey])) # int('3.0') fails!
        thisTheta = float(thisEvent[thetaKey])
        thisPhi = float(thisEvent[phiKey])
        
        results.append([thisTimestamp, thisNanosec, thisTheta, thisPhi])
    
    return results

def matchLOFARfile(crfile, LORAevents):
# return the LORA event corresponding to this LOFAR file. Also return LOFAR file's timestamps
    result = dict(success=False)
    timestamp = crfile["TIME"][0]
    samplenumber = crfile["SAMPLE_NUMBER"][0]
    datalength = crfile["DATA_LENGTH"][0]
    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    filename = crfile["FILENAME"]
    if nofAntennas < 80: # should be cmd-line parameter!
        result.update(reason = format('Number of antennas too small: %d' % 80))
        return result
            
    nsec = samplenumber * 5
    #print ' %d' % nsec
    endnsec = nsec + datalength * 5
    endtime = timestamp
    if endnsec > 1e9:
        endtime = timestamp + endnsec % 1e9
        endnsec %= 1e9

    LOFARstarttime = float(timestamp) + float(nsec) / 1e9
    LOFARendtime = float(endtime) + float(endnsec) / 1e9
    # now match to given LORA events
    for LORAevent in LORAevents:
        #print '%d --- %d' % (LORAevent[0], thisTimestamp)
        LORAtimestamp = LORAevent[0]
        LORAnsec = LORAevent[1]
        LORAddate = float(LORAtimestamp) + float(LORAnsec) / 1e9
        if abs(LORAddate - LOFARstarttime) < 1.3: # actually, LOFAR ddate must be earlier! But: check for errors.
            tdiff = LORAddate - LOFARstarttime
#                tdiffsec = float(tdiff) / 1e9
            tdiffms = float(tdiff) * 1e3
            
            print 'Match found! %s, (%d, %d) === (%d, %d), difference is: %3.6f ms' % (filename, timestamp, nsec, LORAtimestamp, LORAnsec, tdiffms)
            if (LOFARstarttime > LORAddate):
                print '--- ERROR: start of LOFAR data too late!'
                result.update(reason = 'start of LOFAR data too late!')
                return result
            if (LOFARendtime < LORAddate):
                print '--- ERROR: end of LOFAR data too early!'       
                result.update(reason = 'end of LOFAR data too early!')
                return result
            
            LOFARtimes = [timestamp, nsec, endtime, endnsec, LOFARstarttime, LOFARendtime]
            result.update(success = True, LOFARtimes = LOFARtimes, LORAevent = LORAevent)
            return result
    # if we get here, no match was found...
    result.update(reason = 'No match found')
    return result

def incoherentadd(crf, length = -1, blockno = -1, doPlot = True): # actually coherent addition assuming zenith
    if length == -1:
        len = crf["DATA_LENGTH"][0]
    else:
        len = length
    print crf["DATA_LENGTH"]
    nofAntennas = crf["NOF_DIPOLE_DATASETS"]
    crf["SELECTED_DIPOLES"] = [crf["DIPOLE_NAMES"][0]]
    if length == -1:
        crf["BLOCKSIZE"] = len
    else:
        crf["BLOCKSIZE"] = length
    if blockno == -1:
        crf["BLOCK"] = 0
    else:
        crf["BLOCK"] = blockno
    result = crf["EMPTY_TIMESERIES_DATA"]
    for i in range(1, nofAntennas, 2): # nofAntennas
        print 'Doing antenna %d' % i
        crf["SELECTED_DIPOLES"] = [crf["DIPOLE_NAMES"][i]]
        a = crf["TIMESERIES_DATA"]
        result.add(a)
    #crf["SELECTED_DIPOLES"] = crf["DIPOLE_NAMES"] # put selection back to all
    if doPlot:
        toplot = result.toNumpy()
        print toplot[0].size
#        import pdb; pdb.set_trace()
#        print len(result)
        x = np.arange(toplot[0].size, dtype=float)
        x *= 5.0e-6 # ms
        timeoffset = crf["SAMPLE_NUMBER"][0] * 5.0e-6 + blockno * len * 5.0e-6
        x += timeoffset
        print x[1]
        print x.size
        print toplot.size
        plt.plot(x, toplot[0])
        plt.ylabel('Voltage (ADC units)')
        plt.xlabel('Time (ms)')
#        result.plot()
        raw_input("Plotted incoherent addition of all antennas.")
    return result
    
filename = '/Users/acorstanje/triggering/LORAweekdump/LORAweekdump--4-3.h5'
LORAeventsfile = '/Users/acorstanje/triggering/LORAweekdump/LORAevents.dat'
#filename = '/data/acorstanje/LORA/LORAdump--5-123.h5'
LORAevents = readLORAevents(LORAeventsfile)

crfile = cr.open(filename)
nofAntennas = crfile["NOF_DIPOLE_DATASETS"]

result = matchLOFARfile(crfile, LORAevents)
print result
raw_input('Result of matching...')
if not result["success"]:
    exit()
# now cut out a region around the supposed CR pulse and plot.
blocksize = 65536 # big block, then cut out small block
smallblocksize = 2048
crfile["BLOCKSIZE"] = blocksize

LORAevent = result["LORAevent"]
LOFARtimes = result["LOFARtimes"]
pulsetimestamp = LORAevent[0]
pulsensec = LORAevent[1] # change! No magic numbers

LOFARtimestamp = LOFARtimes[0]
LOFARnsec = LOFARtimes[1]

pulseSamplenumber = (pulsetimestamp - LOFARtimestamp) * 200e6 + (pulsensec - LOFARnsec) / 5
if pulseSamplenumber < 0:
    # 'should' not happen with the tests above...
    print 'Samplenumber < 0!!!!'
    exit()
pulseBlock = int(pulseSamplenumber / blocksize)

cr_bigblock = crfile["EMPTY_TIMESERIES_DATA"]
crfile.getTimeseriesData(cr_bigblock, pulseBlock)

pulseMidpoint = pulseSamplenumber % blocksize
cr_efield = cr.hArray(copy=cr_bigblock, dimensions = [nofAntennas, smallblocksize])
start = int(pulseMidpoint - smallblocksize/2)
stop = int(pulseMidpoint + smallblocksize/2)
if start < 0 or stop > blocksize:
    print 'Start / stop position outside block!' # fix.....
    exit()

cr_efield[...].copy(cr_bigblock[..., start:stop])
toplot = cr_efield.toNumpy()
x = np.arange(toplot[0].size, dtype=float)
x *= 5.0e-6 # ms
timeoffset = LOFARnsec * 1e-6 + pulseBlock * blocksize * 5.0e-6 + start * 5.0e-6
x += timeoffset
print x[1]
print x.size
print toplot.size
plt.plot(x, toplot.T)
plt.ylabel('Voltage (ADC units)')
plt.xlabel('Time (ms)')

raw_input('Plotted...')


