
#
#  triggerstats.py
#  
#
#  Created by Arthur Corstanje on 9/1/09.
#  Copyright (c) 2009 __MyCompanyName__. All rights reserved.
#

import csv
from mathgl import *
import numpy

#def nofTriggersFilteredByThreshold(listOfTriggers, threshold)

def triggersFilteredByThreshold(listOfTriggers, threshold):
    outList = []
    for record in listOfTriggers:
        if float(record['peak']) / float(record['powerBefore']) >= threshold:
	    outList.append(record)
    return outList

# keys for our trigger dictionary
RCUnr = 'RCUnr'
seqnr = 'seqnr'
time = 'time'
sample = 'sample'
sum = 'sum'
nrSamples = 'nrSamples'
peak = 'peak'
powerBefore = 'powerBefore'
powerAfter = 'powerAfter'

myKeys = [RCUnr, seqnr, time, sample, sum, nrSamples, peak, powerBefore, powerAfter]
triggerReader = csv.DictReader(open('/Users/acorstanje/triggering/2009-09-03_TRIGGER5.dat'), myKeys, delimiter=' ')
numTriggers = triggerReader.line_num # trigger records don't span multiple lines

triggerList = []
for record in triggerReader:
    triggerList.append(record)

RCUnr = [] # these redefinitions will be gone soon...
seqnr = []
time = []
sample = []
sum = []
nrSamples = []
peak = []
powerBefore = []
powerAfter = []

for record in triggerList:
    RCUnr.append(int(record['RCUnr']))
    seqnr.append(int(record['seqnr']))
    time.append(int(record['time']))
    sample.append(int(record['sample']))
    sum.append(int(record['sum']))
    nrSamples.append(int(record['nrSamples']))
    peak.append(int(record['peak']))
    powerBefore.append(int(record['powerBefore']))
    powerAfter.append(int(record['powerAfter']))

(y, x) = RCUhisto = numpy.histogram(RCUnr, 96, (0, 96)) # counts, bins

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
firstTime = triggerList[0]['time']
lastTime = triggerList[totalTriggers - 1]['time']
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
graph.SetRanges(0.0, gX.Max('x')[0], 0.01, gY.Max('x')[0]) # SetRanges goes before Axis to include ranges in axes
 # min, max is complicated in mglData... the 'x' means first dimension not graph-x !
graph.SetOrigin(0.0, 0.01)

graph.SetFunc("","lg(y)","") 
graph.SetTicks('y',0)		 
graph.SetTicks('x',5.0, 5)
#graph.SetRanges(0.0, gX.Max('x')[0], gY.Min('x')[0], gY.Max('x')[0])


graph.Axis() # comes after all definitions as before
graph.Grid()
graph.Label("x","threshold factor",1)
graph.Label("y","Counts per minute",1)
graph.Plot(gX,gY, 'b2');

graph.WriteEPS("testSTATS_cpm.eps","Counts per minute")

# now continue with separation into 'pulses' (multiple RCUs triggering within a short timespan xxx)

pulseIndices = []
# we have to guess the sampling rate! Well, let's look at the max value for 'sample'...
samplingRate = int(160e6)
if (max(sample) > 160e6):
    samplingRate = int(200e6)

lastTime = 0; 
timeWindow = 100000 # window for pulse in samples
i = 0
for record in triggerList:
    i+=1 # keep track of the index
    thisSecond = int(record['time'])
    thisSample = int(record['sample'])
    thisTime = samplingRate * thisSecond + thisSample
    
    if (abs(thisTime - lastTime) > timeWindow):
        lastTime = thisTime # start new pulse
	pulseIndices.append(i) # at this index a new pulse begins

def pulse(pulseIndices, triggerList, pulseno): # get list of triggers corresponding to 0-based pulseno.
    return triggerList[pulseIndices[pulseno]:pulseIndices[pulseno+1]]


