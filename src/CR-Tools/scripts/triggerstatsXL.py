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
from math import *

numberOfRCUsperStation = 96;
mglGraphPS = 1
timeWindowForCoincidence = 10000 # window for coincident pulse in samples

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

if len(sys.argv) > 1:
    fileName = sys.argv[1] 
else:
    fileName = '2009-09-03_TRIGGER5.dat'   #  that's just handy for rapid testing...
    print 'No filename given; usage: python triggerstatsXL.py <filename>. Using file ' + fileName + ' by default.'


def nofTriggersFilteredByThreshold(listOfTriggers, threshold):
    n = 0
    for record in listOfTriggers:
        thisPeak = float(record['peak'])
        thisPowerBefore = float(record['powerBefore'])
        if thisPowerBefore == 0.0: 
            n += 1
        elif thisPeak / thisPowerBefore >= threshold:
            n += 1
    return n

def triggersFilteredByThreshold(listOfTriggers, threshold):
    outList = []
    for record in listOfTriggers:
        thisPeak = float(record['peak'])
        thisPowerBefore = float(record['powerBefore'])
        if thisPowerBefore == 0.0: 
	    outList.append(record)
        elif thisPeak / thisPowerBefore >= threshold:
            outList.append(record)
    return outList

def triggersFilteredByRCUNumber(listOfTriggers, rcu):
    outList = []
    for record in listOfTriggers:
        if int(record[RCUnrKey]) == rcu:
            outList.append(record)
    return outList

def listOfTimeDifferences(listOfTriggers, rcu):
    firstTime = int(listOfTriggers[0][timeKey])
    print firstTime
    lastTime = 0
    i=0
    for record in listOfTriggers:
        if int(record[RCUnrKey]) == rcu:
            i += 1
            thisTime = int(record[timeKey])
            thisSample = int(record[sampleKey])
            diff = thisTime + float(thisSample) / 200.0e6 - lastTime
            print str(i) + ': ' + str(diff)
            lastTime = thisTime + float(thisSample / 200.0e6)

def readTriggerListFromCSVFile(filename):
    myKeys = [RCUnrKey, seqnrKey, timeKey, sampleKey, sumKey, nrSamplesKey, peakKey, powerBeforeKey, powerAfterKey]
    triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
    numTriggers = triggerReader.line_num # trigger records don't span multiple lines

    triggerList = []
    for record in triggerReader:
        thisTime = int(record[timeKey])
        if thisTime < 2.2e9: # Unix timestamps are signed ints, and they don't go that far...
            triggerList.append(record)
        else:
            print 'Invalid timestamp! ' + str(thisTime)
    return triggerList        
    
def makeListsPerKey(triggerList): # makes globals, which is not good
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


def sortedTriggerListByTimeAndSample(inputList): # sort triggers on 'time', then on 'sample'

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

    return sorted(inputList, compare_by(timeKey, sampleKey) )
    
    
    
def coincidentPulseIndicesInTriggerList(sortedTriggerList, timeWindowInSamples): # input list is assumed to be time-sorted!
    pulseIndices = []
    # we have to guess the sampling rate! Well, let's look at the max value for 'sample'...
    samplingRate = int(160e6)
    if (max(sample) > 160e6):
        samplingRate = int(200e6)

    # Now search for 'pulses' = coincident triggers within a timeWindow
    # make a list of indices in triggerList; every index in pulseIndices is the start of a new 'pulse'. 
    # Pulse information can then be extracted just from the triggerList, as we know which ones belong together.

    lastTimeInSamples = 0; 
    i = 0
    pulseIndices.append(0) # first pulse starts at 0
    for record in sortedTriggerList: # pulse search
        i+=1 # keep track of the index
        thisSecond = int(record[timeKey])
        thisSample = int(record[sampleKey])
        thisTime = samplingRate * thisSecond + thisSample
        
        if (abs(thisTime - lastTimeInSamples) > timeWindowInSamples):
            lastTimeInSamples = thisTime # start new pulse
            pulseIndices.append(i) # at this index a new pulse begins
    return pulseIndices

def pulse(pulseIndices, triggerList, pulseno): # get list of triggers corresponding to pulse number 'pulseno' (0-based)
    return triggerList[pulseIndices[pulseno]:pulseIndices[pulseno+1]]
    
def timeSpanInMinutes(triggerList): # also fractional minutes
    totalTriggers = len(triggerList)
    firstTime = triggerList[0][timeKey]
    lastTime = triggerList[totalTriggers - 1][timeKey]
    firstTime = float(firstTime)
    lastTime = float(lastTime)
    minutes = (lastTime - firstTime) / 60.0 # time in minutes
    if minutes == 0.0:
        minutes = 1.0 / 60 # avoid div by zero
    return minutes
          
def makeTriggersVersusRCUHistogram(triggerList): # also uses RCUnr global input
    (y, x) = RCUhisto = numpy.histogram(RCUnr, 96, (0, 96)) # (counts, bins) comes out

    maxY = max(y)
    maxX = max(x)
    #y = 1000.0 * y / max(y)

    width=1200  
    height=600
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

    #graph.Title("Number of triggers versus RCU number")
    graph.SetFontSize(6.0)
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Number of triggers versus RCU number")
    graph.SetFontSize(3.0) # Work-around for MathGL 1.9 bug in mglGraph.Title()

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total trigger count = " + str(len(triggerList)))
    graph.Label("x","RCU number",1)
    graph.Label("y","Counts",1)
    graph.Bars(gY);
    graph.WriteEPS("triggersVersusRCUNumber.eps","Histogram of triggers vs RCU number")


def makeTriggersVersusThresholdPlot(triggerList):
    # no. triggers filtered by threshold factor
    # in counts per minute - extract first and last time

    # float roundoff problems ?!
    minutes = timeSpanInMinutes(triggerList)
    nofPoints=100
    stepsize = 1/3.0
    startThreshold = 3.0
    gX = mglData(nofPoints)
    gY = mglData(nofPoints)

    for i in range(nofPoints):
        print str(i) + "th point for threshold plot"
        thisThreshold = startThreshold + float(i) * stepsize
        filteredByThreshold = nofTriggersFilteredByThreshold(triggerList, thisThreshold)
        gX[i] = thisThreshold
        gY[i] = filteredByThreshold / minutes

    width = 800
    height = 600
    graph = mglGraph(mglGraphPS, width, height)

    maxX = gX.Max('x')[0]
    maxY = gY.Max('x')[0] # min, max is complicated in mglData... the 'x' means first dimension not graph-x !

    graph.Clf()
    graph.SetFontSize(3.0)

    # set y-min range so you can just see 1 count in the entire interval.
    yRangeMin = 0.95 / minutes
    graph.SetRanges(0.0, maxX, yRangeMin, maxY) # SetRanges goes before Axis to include ranges in axes
    graph.SetOrigin(0.0, yRangeMin)

    graph.SetFontSize(5.0)
    #graph.Title("Single triggers per minute, vs threshold")
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Single triggers per minute, vs threshold")
    graph.SetFontSize(3.0)

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total triggers per minute = " + format(len(triggerList) / minutes, "8.1f"))

    graph.SetFunc("","lg(y)","") 
    graph.SetTicks('y',0)		 
    graph.SetTicks('x',5.0, 5)
    #graph.SetRanges(0.0, gX.Max('x')[0], gY.Min('x')[0], gY.Max('x')[0])


    graph.Axis() # comes after all definitions as before
    graph.Grid()
    graph.Label("x","threshold factor",1)
    graph.Label("y","Counts per minute",1)
    graph.Plot(gX,gY, 'b o#'); # ' ' means no line; 'o' means o symbols; '#' means solid symbols.

    graph.WriteEPS("triggersVersusThreshold.eps","Counts per minute")


# now continue with separation into 'pulses' (multiple RCUs triggering within a short timespan xxx)

    
# get pulses one by one, make histogram of # triggers per pulse. Scale to 'counts per minute'

def makeNumberOfRCUsPerPulsePlot(sortedTriggerList, pulseIndices): 
    gX = mglData(numberOfRCUsperStation)
    for i in range(numberOfRCUsperStation):
        gX[i] = i

    gY = mglData(numberOfRCUsperStation)
    
    minutes = timeSpanInMinutes(sortedTriggerList)
    
    lastIndex = -1;    
    for index in pulseIndices: # we can work only on the indices to get # RCUs 
        gY[index - lastIndex] += 1.0 / minutes
        lastIndex = index

    width=800  
    height=600
    mglGraphPS = 1
    graph = mglGraph(mglGraphPS, width, height)

    graph.Clf()
    graph.SetFontSize(2.5)
    # set y-min range so you can just see 1 count in the entire interval.
    yRangeMin = 0.95 / minutes
    graph.Title("Histogram of number of RCUs per pulse")

    graph.SetRanges(0.0, 30, yRangeMin, 2 * gY.Max('x')[0]) # needs better x-ranging!

    graph.Puts(float(30) * 0.5, 2 * gY.Max('x')[0]*1.05, 0, "Coincidence window size = " + format(timeWindowForCoincidence, "6.0f"))

    # make logplot
    graph.SetOrigin(0.0, yRangeMin) 
    graph.SetFunc("","lg(y)","") 
    graph.SetTicks('y',0)		 

    graph.SetTicks('x', 4.0, 4)

    graph.Axis("xy")
    graph.Grid()

    graph.Label("x","Number of RCUs",1)
    graph.Label("y","Counts per minute",1)
    graph.Plot(gX,gY, 'b o#'); 
    graph.WriteEPS("numberOfRCUsPerPulse.eps","Histogram of number of RCUs per coincident pulse")

# now get a histogram of total-sum per pulse (counts per minute)

# all 'sum' values of triggers inside one coincidence pulse get summed (total sum)
def makeHistogramOfTotalSumInPulses(sortedTriggerList, pulseIndices): 
    allTotalSums = []
    for i in range(len(pulseIndices) - 1):
        thisPulse = pulse(pulseIndices, sortedTriggerList, i)
        thisTotalSum = 0;
        for record in thisPulse:
            thisTotalSum += int(record[sumKey])
        allTotalSums.append(thisTotalSum)

    print(len(allTotalSums))
    print(len(pulseIndices))
      
    (y, x) = numpy.histogram(allTotalSums, 50)

    gY = mglData(len(y))
    gX = mglData(len(x))

    minutes = timeSpanInMinutes(sortedTriggerList)
    yRangeMin = 0.95 / minutes

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
    graph.WriteEPS("totalSumPerPulse.eps","Histogram of total sum per pulse")


def makeBinnedTimeSeriesOfTriggers(triggerList): # works on global 'time' list
    # make binned timeseries of trigger counts to see variations over time

    nBins = min(200, len(triggerList) / 10)
    totalTriggers = len(triggerList)
    firstTime = int(triggerList[0][timeKey])
    lastTime = int(triggerList[totalTriggers - 1][timeKey])

    timeBinSize = float(lastTime - firstTime) / nBins # seconds
    print nBins
    print firstTime
    print lastTime
    print timeBinSize
    #nBins = (lastTime - firstTime) / timeBinSize
    # use again numpy's histogram function
    (y, x) = numpy.histogram(time, int(nBins))

    width=1200  
    height=600
    mglGraphPS = 1
    graph = mglGraph(mglGraphPS, width, height)

    gY = mglData(len(y))
    gX = mglData(len(x))

    for i in range(len(y)):
        gY[i] = float(y[i])

    for i in range(len(x)):
        gX[i] = (float(x[i]) - firstTime) / 60.0

    maxX = gX.Max('x')[0]
    maxY = float(max(y))
        
    graph.Clf()
    graph.SetFontSize(3.0)
    graph.SetRanges(0.0, maxX, 0.0, maxY)
    #graph.SetTicks('x', 16.0, 4)

    graph.Axis("xy")
    graph.Grid()

    #graph.Title("Time series of # triggers (binned)")
    graph.SetFontSize(6.0)
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Time series of triggers (binned)")
    graph.SetFontSize(3.0)

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total trigger count = " + str(len(triggerList)) + "; bin width = " + format(timeBinSize, "4.1f") + " s")

    graph.Label("x","Time (min)",1)
    graph.Label("y","Counts",1)
    graph.Bars(gY)

    #ggY = mglData(len(y),4)
    #for i in range(len(y)):
    #    ggY.Put(float(y[i]), i, 0)
    #    ggY.Put(float(y[i]) * 0.7*0.7, i, 1)
    #    ggY.Put(float(y[i]) * 0.7*0.7*0.7, i, 2)
    #    ggY.Put(float(y[i]) * 0.7*0.7*0.7*0.7, i, 3)

    #ggY=mglData(len(y))
    #for i in range(len(y)):
    #    ggY[i] = y[i] * 0.5
    #    
    #graph.Bars(ggY)

    graph.WriteEPS("timeSeriesOfTriggers.eps","Binned timeseries of triggers")

def runFullAnalysis():
    print "Reading trigger list..."
    triggerList = readTriggerListFromCSVFile(fileName)
    print "Making lists per key..."
    makeListsPerKey(triggerList)
    print "Making triggers versus RCU histogram..."
    makeTriggersVersusRCUHistogram(triggerList)
    print "Sorting trigger list..."
    sortedTriggerList = sortedTriggerListByTimeAndSample(triggerList)
    print "Making coincident pulse list..."
    pulseIndices = coincidentPulseIndicesInTriggerList(sortedTriggerList, timeWindowForCoincidence)
    print "Making numer of RCUs per pulse plot..."
    makeNumberOfRCUsPerPulsePlot(sortedTriggerList, pulseIndices)
    
    #makeHistogramOfTotalSumInPulses(sortedTriggerList, pulseIndices)
    print "Making binned time-series of triggers..."
    makeBinnedTimeSeriesOfTriggers(triggerList)
    print "Making triggers versus threshold plot..."
    makeTriggersVersusThresholdPlot(triggerList)

# main
runFullAnalysis()
