
#
#  triggerstats.py
#  

#
#  Created by Arthur Corstanje on 9/1/09.
#  Copyright (c) 2009 __MyCompanyName__. All rights reserved.
#

# list of free parameters (to be removed from this code...):
# - input filename [ DONE ]
# - (output directory)
# - graph width/height
# - number of points in the counts vs thresholds graph
# - time window for coincidence check

import csv
from mathgl import *
import numpy
import sys

if len(sys.argv) > 1:
    fileName = sys.argv[1] 
else:
    fileName = '2009-08-04_TRIGGER.dat' #  that's just handy for rapid testing...

#def nofTriggersFilteredByThreshold(listOfTriggers, threshold)

numberOfRCUsperStation = 96;

def triggersFilteredByThreshold(listOfTriggers, threshold):
    outList = []
    for record in listOfTriggers:
        if float(record['peak']) / float(record['powerBefore']) >= threshold:
	    outList.append(record)
    return outList

# keys for our trigger dictionary; separate lists for some of the statistics
RCUnrKey = 'RCUnr';             RCUnr = [] 
seqnrKey = 'seqnr';             seqnr = []
timeKey = 'time';               time = []
sampleKey = 'sample';           sample = []
sumKey = 'sum';                 sum = []
nrSamplesKey = 'nrSamples';     nrSamples = []
peakKey = 'peak';               peak = []
powerBeforeKey = 'powerBefore'; powerBefore = []
powerAfterKey = 'powerAfter';   powerAfter = [] 

myKeys = [RCUnrKey, seqnrKey, timeKey, sampleKey, sumKey, nrSamplesKey, peakKey, powerBeforeKey, powerAfterKey]
triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
numTriggers = triggerReader.line_num # trigger records don't span multiple lines

triggerList = []
for record in triggerReader:
    triggerList.append(record)

for record in triggerList:
    RCUnr.append(int(record[RCUnrKey]))
    seqnr.append(int(record[seqnrKey]))
    time.append(int(record[timeKey]))
    sample.append(int(record[sampleKey]))
    sum.append(int(record[sumKey]))
    nrSamples.append(int(record[nrSamplesKey]))
    peak.append(int(record[peakKey]))
    powerBefore.append(int(record[powerBeforeKey]))
    powerAfter.append(int(record[powerAfterKey]))

(y, x) = RCUhisto = numpy.histogram(RCUnr, 96, (0, 96)) # (counts, bins) comes out

maxY = max(y)
y = 1000.0 * y / max(y)

width=1200  
height=600
mglGraphPS = 1
graph = mglGraph(mglGraphPS, width, height)

gY = mglData(len(y))
gX = mglData(len(x))

for i in range(len(y)):
    gY[i] = float(y[i])

for i in range(len(x)):
    gX[i] = float(x[i])
    
graph.Clf()
graph.SetFontSize(3.0)
graph.SetRanges(0.0, float(max(x)), 0.0, float(max(y)))
graph.SetTicks('x', 16.0, 4)

graph.Axis("xy")
graph.Grid()

graph.Title("Number of triggers versus RCU number")
graph.Label("x","RCU number",1)
graph.Label("y","Counts",1)
graph.Bars(gY);
graph.WriteEPS("testSTATS.eps","Test Plot")

# no. triggers filtered by threshold factor
# in counts per minute - extract first and last time

totalTriggers = len(triggerList)
firstTime = triggerList[0][timeKey]
lastTime = triggerList[totalTriggers - 1][timeKey]
firstTime = float(firstTime)
lastTime = float(lastTime)
minutes = (lastTime - firstTime) / 60.0 # time in minutes - time limiting should go here!
# float roundoff problems ?!

nofPoints=100
stepsize = 1/3.0
startThreshold = 3.0
gX = mglData(nofPoints)
gY = mglData(nofPoints)

for i in range(nofPoints):
    thisThreshold = startThreshold + float(i) * stepsize
    filteredByThreshold = triggersFilteredByThreshold(triggerList, thisThreshold)
    gX[i] = thisThreshold
    gY[i] = float(len(filteredByThreshold)) / minutes

width = 800
height = 600
graph = mglGraph(mglGraphPS, width, height)

graph.Clf()
graph.SetFontSize(2.0)

graph.Title("# single triggers per minute, vs threshold")

# set y-min range so you can just see 1 count in the entire interval.
yRangeMin = 0.95 / minutes
graph.SetRanges(0.0, gX.Max('x')[0], yRangeMin, gY.Max('x')[0]) # SetRanges goes before Axis to include ranges in axes
 # min, max is complicated in mglData... the 'x' means first dimension not graph-x !
graph.SetOrigin(0.0, yRangeMin)

graph.SetFunc("","lg(y)","") 
graph.SetTicks('y',0)		 
graph.SetTicks('x',5.0, 5)
#graph.SetRanges(0.0, gX.Max('x')[0], gY.Min('x')[0], gY.Max('x')[0])


graph.Axis() # comes after all definitions as before
graph.Grid()
graph.Label("x","threshold factor",1)
graph.Label("y","Counts per minute",1)
graph.Plot(gX,gY, ' o#'); # ' ' means no line; 'o' means o symbols; '#' means solid symbols.

graph.WriteEPS("testSTATS_cpm.eps","Counts per minute")

# now continue with separation into 'pulses' (multiple RCUs triggering within a short timespan xxx)

pulseIndices = []
# we have to guess the sampling rate! Well, let's look at the max value for 'sample'...
samplingRate = int(160e6)
if (max(sample) > 160e6):
    samplingRate = int(200e6)

# sort triggers on 'time', then on 'sample'

def compare_by (fieldname1, fieldname2): # nested compare function for sorting a list of dicts.
    # sorts first on fieldname 1, then on fieldname 2 (here: first on 'time', then on 'samplenr')
    def compare_two_dicts (a, b):
        if cmp(a[fieldname1], b[fieldname1]) == 0:
            return cmp(a[fieldname2], b[fieldname2])
        else:
            return cmp(a[fieldname1], b[fieldname1])
    return compare_two_dicts

#triggerList[0][timeKey] = 5
#triggerList[0][sampleKey] = 700
#triggerList[1][timeKey] = 5
#triggerList[1][sampleKey] = 750
#triggerList[2][timeKey] = 10
#triggerList[2][sampleKey] = 700
#triggerList[3][timeKey] = 5
#triggerList[3][sampleKey] = 699
#triggerList[4][timeKey] = 4
#triggerList[4][sampleKey] = 140000

sortedTriggerList = sorted(triggerList, compare_by(timeKey, sampleKey) )

# end sorting. Now search for 'pulses' = coincident triggers within a timeWindow
# make a list of indices in triggerList; every index in pulseIndices is the start of a new 'pulse'. 
# Pulse information can then be extracted just from the triggerList, as we know which ones belong together.

lastTime = 0; 
timeWindow = 100000 # window for pulse in samples
i = 0
pulseIndices.append(0) # first pulse starts at 0
for record in sortedTriggerList: # pulse search
    i+=1 # keep track of the index
    thisSecond = int(record[timeKey])
    thisSample = int(record[sampleKey])
    thisTime = samplingRate * thisSecond + thisSample
    
    if (abs(thisTime - lastTime) > timeWindow):
        lastTime = thisTime # start new pulse
	pulseIndices.append(i) # at this index a new pulse begins

def pulse(pulseIndices, triggerList, pulseno): # get list of triggers corresponding to pulse number 'pulseno' (0-based)
    return triggerList[pulseIndices[pulseno]:pulseIndices[pulseno+1]]
    
# get pulses one by one, make histogram of # triggers per pulse. Scale to 'counts per minute'

gX = mglData(numberOfRCUsperStation)
for i in range(numberOfRCUsperStation):
    gX[i] = i

gY = mglData(numberOfRCUsperStation)

lastIndex = -1;    
for index in pulseIndices: # we can work only on the indices to get # RCUs 
    gY[index - lastIndex] += 1.0 / minutes
    lastIndex = index

width=800  
height=600
mglGraphPS = 1
graph = mglGraph(mglGraphPS, width, height)

graph.Clf()
graph.SetFontSize(2.0)
# set y-min range so you can just see 1 count in the entire interval.
yRangeMin = 0.95 / minutes
graph.Title("Histogram of number of RCUs per pulse")

graph.SetRanges(0.0, 30, yRangeMin, 2 * gY.Max('x')[0]) # needs better x-ranging!
# make logplot
graph.SetOrigin(0.0, yRangeMin) 
graph.SetFunc("","lg(y)","") 
graph.SetTicks('y',0)		 

graph.SetTicks('x', 4.0, 4)

graph.Axis("xy")
graph.Grid()

graph.Label("x","Number of RCUs",1)
graph.Label("y","Counts per minute",1)
graph.Plot(gX,gY, ' o#'); 
graph.WriteEPS("testRCUsperPulse.eps","Test Plot")

# now get a histogram of total-sum per pulse (counts per minute)

# make a list of all total-sums

allTotalSums = []
for i in range(len(pulseIndices) - 1):
    thisPulse = pulse(pulseIndices, triggerList, i)
    thisTotalSum = 0;
    for record in thisPulse:
        thisTotalSum += int(record[sumKey])
    allTotalSums.append(thisTotalSum)

print(len(allTotalSums))
print(len(pulseIndices))
  
(y, x) = numpy.histogram(allTotalSums, 50)

gY = mglData(len(y))
gX = mglData(len(x))

for i in range(len(y)):
    gY[i] = float(y[i]) / minutes # counts per minute

for i in range(len(x)):
    gX[i] = float(x[i])
    
width=1200  
height=600
mglGraphPS = 1
graph = mglGraph(mglGraphPS, width, height)

graph.Clf()
graph.SetFontSize(3.0)
#graph.SetRanges(0.0, float(max(x)), 0.0, float(max(y)) / minutes)
#graph.SetTicks('x', 16.0, 4)
#graph.SetRanges(gX[0], gX.Max('x')[0], yRangeMin, 2 * gY.Max('x')[0]) 
graph.SetRanges(0, gX.Max('x')[0], yRangeMin, 2 * gY.Max('x')[0])
# make logplot
graph.SetOrigin(0, yRangeMin) 
graph.SetFunc("","lg(y)","") 
graph.SetTicks('y',0)		 
#graph.SetTicks('x',0)

graph.Axis("xy")
graph.Grid()

graph.Title("Histogram of total sum per pulse")
graph.Label("x","total sum",1)
graph.Label("y","Counts per minute",1)
graph.Bars(gY);
graph.WriteEPS("testSTATS_totalsum.eps","Test Plot")
