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
#import numpy
import sys
from math import *
import os

numberOfRCUsperStation = 96;
ADCRange = 2048
mglGraphPS = 1
timeWindowForCoincidence = 200 # window for coincident pulse in samples
latestTimeInFile = -1 # needed for determining bin-sizes in non-24-hour plots
TwentyfourHourPlot = False
#totalNumberOfTriggers = 0

# define global processed-data variables
# Have to be global as we process incrementally, per block of triggers
# and Python does not have static vars...
#blockSizeForTriggerList = 100000
rcuCount = [0] * numberOfRCUsperStation; # creates a list of 96 zeros
nBins = 240 # bins for time-series
maxThreshold = 100.0
thresholdStep = 1/3.0
nBinsThresholdPlot = 1 + int(maxThreshold / thresholdStep)

firstTime = 0
lastTime = 0
timeSeriesHistogram = [0] * nBins 
timeSeriesProfileCount = [[0 for i in range(nBins)] for j in range(numberOfRCUsperStation)]
timeSeriesProfileValue = [[0 for i in range(nBins)] for j in range(numberOfRCUsperStation)]

thresholdHistogram = [0] * nBinsThresholdPlot
absPeakValueHistogram = [0] * ADCRange

# define RCU groups for noise profile plot.
#group1 = range(16) + range(48,64)
#group2 = range(16,32) + range(64,80)
#group3 = range(32,48) + range(80,96)
#RCUgroups = [group1, group2, group3]

group1 = range(numberOfRCUsperStation) # one single group
RCUgroups = [group1]

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

def processCSVFile(filename): # directly make histogram, not making triggerList
    myKeys = [RCUnrKey, seqnrKey, timeKey, sampleKey, sumKey, nrSamplesKey, peakKey, powerBeforeKey, powerAfterKey]
    lastTimeReader = csv.DictReader(os.popen("tail -1 " + fileName), myKeys, delimiter=' ')
    global latestTimeInFile
    for record in lastTimeReader:
        latestTimeInFile = int(record[timeKey])
#        print latestTimeInFile
    if latestTimeInFile > 2.2e9:
        print "Error reading latest timestamp in the file!"
       
    triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
    firstRecord = triggerReader.next()
   
    global firstTime
    global lastTime # needed even if they ARE defined globally
    firstTime = int(firstRecord[timeKey])
    lastTime = latestTimeInFile
    
    if (TwentyfourHourPlot):
        firstTime = 0
        lastTime = 86400
    
    for record in triggerReader:
        if (type(record[RCUnrKey]) != type('1')) | (type(record[timeKey]) != type('1')) | (type(record[powerBeforeKey]) != type('1')):
        # all values in the record are of type 'string' by default; if error decoding line, it will return 'NoneType', which gives an exception on doing anything with it...
            print 'Error parsing this line! Nr. ' + str(triggerReader.line_num)
        else:
            thisTime = int(record[timeKey])
            if thisTime < 2.2e9: # Unix timestamps are signed ints, and they don't go that far...
    #            triggerList.append(record)
                updateRCUHistogram(record)
                updateThresholdHistogram(record)
                updateAbsPeakValueHistogram(record)
                updateTimeSeriesAndNoiseProfile(record) 
         #   else:
                #print 'Invalid timestamp! ' + str(thisTime)
                
    global totalNumberOfTriggers
    totalNumberOfTriggers = triggerReader.line_num
     
    print str(totalNumberOfTriggers) + ' triggers processed.'

def updateRCUHistogram(trigger):
    thisRCU = int(trigger[RCUnrKey])
    rcuCount[thisRCU] += 1
    
def updateThresholdHistogram(trigger):
    thisPeak = float(trigger[peakKey])
    thisPowerBefore = float(trigger[powerBeforeKey])
    
    binIndex = (thisPeak / thisPowerBefore) / thresholdStep
    if binIndex >= nBinsThresholdPlot:
        binIndex = nBinsThresholdPlot
    
    binIndex = int(binIndex)
#    print 'This peak = ' + str(thisPeak) + ', this power before = ' + str(thisPowerBefore) + ', this bin index = ' + str(binIndex)
    # if cumulative plot, i.e. 'all triggers _exceeding_ this threshold', then increment everything below this index
    for i in range(binIndex):
        thresholdHistogram[i] += 1
        
#    print thresholdHistogram[10]
#   otherwise just use:     thresholdHistogram[binIndex] += 1

def updateAbsPeakValueHistogram(trigger):
    thisPeak = int(trigger[peakKey])
    
    absPeakValueHistogram[thisPeak] += 1 # counting every single peak value in 1..2047 as we a priori don't know the maximum i.e. our plot range     

def updateTimeSeriesAndNoiseProfile(trigger):
    thisRCU = int(trigger[RCUnrKey])
    thisTime = int(trigger[timeKey]) 
    if (TwentyfourHourPlot):
        thisTime %= 86400
        
    thisPowerBefore = int(trigger[powerBeforeKey])
    # calculate index in histogram:
    timeSeriesIndex = int(nBins * float(thisTime - firstTime) / float(lastTime - firstTime + 0.000001)) # epsilon to remove boundary effect when thisTime = lastTime           
    timeSeriesHistogram[timeSeriesIndex] += 1  
    timeSeriesProfileCount[thisRCU][timeSeriesIndex] += 1
    timeSeriesProfileValue[thisRCU][timeSeriesIndex] += thisPowerBefore     


def sortedTriggerListByTimeAndSample(inputList): # sort triggers on 'time', then on 'sample'

    def compare_by (fieldname1, fieldname2): # nested compare function for sorting a list of dicts.
        # sorts first on fieldname 1, then on fieldname 2 (here: first on 'time', then on 'samplenr')
        def compare_two_dicts (a, b):
            if cmp(a[fieldname1], b[fieldname1]) == 0:
                return cmp(a[fieldname2], b[fieldname2])
            else:
                return cmp(a[fieldname1], b[fieldname1])
        return compare_two_dicts

    return sorted(inputList, compare_by(timeKey, sampleKey) )
    
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
          
def makeTriggersVersusRCUHistogram(rcuCount): # also uses RCUnr global input
#    (y, x) = RCUhisto = numpy.histogram(RCUnr, 96, (0, 96)) # (counts, bins) comes out
    print totalNumberOfTriggers
    x = range(numberOfRCUsperStation)
    y = rcuCount
    maxY = max(y)
    maxX = max(x)+1
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

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total trigger count = " + str(totalNumberOfTriggers) )
    graph.Label("x","RCU number",1)
    graph.Label("y","Counts",1)
    graph.Bars(gY);
    graph.WriteEPS("triggersVersusRCUNumber-"+fileName+".eps","Histogram of triggers vs RCU number")


def makeTriggersVersusThresholdPlot():
    # no. triggers filtered by threshold factor
    # in counts per minute - extract first and last time

    # float roundoff problems ?!
#    minutes = timeSpanInMinutes(triggerList)
    minutes = (lastTime - firstTime) / 60.0 # note that it's 86400 always when doing 24-hour plots... change? Y-axis is a bit arbitrary anyway??
    nofPoints=nBinsThresholdPlot
    
    #stepsize = 1/3.0
    startThreshold = 0.0
    gX = mglData(nofPoints)
    gY = mglData(nofPoints)

    for i in range(nofPoints):
#        print str(i) + "th point for threshold plot"
        thisThreshold = startThreshold + float(i) * thresholdStep
        #filteredByThreshold = nofTriggersFilteredByThreshold(triggerList, thisThreshold)
        thisThresholdCount = thresholdHistogram[i]
        gX[i] = thisThreshold
        gY[i] = thisThresholdCount / minutes
#        print str(i) + ': ' + str(gY[i])

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

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total triggers per minute = " + format(totalNumberOfTriggers / minutes, "8.1f"))

    graph.SetFunc("","lg(y)","") 
    graph.SetTicks('y',0)		 
    graph.SetTicks('x',20.0)
    #graph.SetRanges(0.0, gX.Max('x')[0], gY.Min('x')[0], gY.Max('x')[0])


    graph.Axis() # comes after all definitions as before
    graph.Grid()
    graph.Label("x","threshold factor",1)
    graph.Label("y","Counts per minute",1)
    graph.Plot(gX,gY, 'b o#'); # ' ' means no line; 'o' means o symbols; '#' means solid symbols.

    graph.WriteEPS("triggersVersusThreshold-"+fileName+".eps","Counts per minute")

def makeTriggersVersusAbsPeakValuePlot():
    # no. triggers versus absolute peak value
    # in counts per minute - extract first and last time

#    minutes = timeSpanInMinutes(triggerList)
    minutes = (lastTime - firstTime) / 60.0 # note that it's 86400 always when doing 24-hour plots... change? Y-axis is a bit arbitrary anyway??
    nofPoints=100 # -> needs re-binning!
    # Get maximum peak value that actually occurs
    maxPeakValue = 0
    for i in range(ADCRange): # can be done more efficiently, but don't care...
        if absPeakValueHistogram[i] > 0:
            maxPeakValue = i
    # Now re-bin for 100 points between 1 and maxPeakValue
    
    stepsize = int(round(float(maxPeakValue) / float(nofPoints))) # all bins should be equally wide!
    nofPoints = 1 + maxPeakValue / stepsize
    gX = mglData(nofPoints)
    gY = mglData(nofPoints)

    for i in range(1, nofPoints):
#        print str(i) + "th point for threshold plot"
#        thisThreshold = startThreshold + float(i) * stepsize
        #thisMaxPeak = int(round(float(i) * stepsize))
        #thisMinPeak = int(round(float(i-1) * stepsize)) # hmm, bins won't be all equally wide
#        print str(thisMinPeak) + ' ' + str(thisMaxPeak)
 
        thisMinPeak = (i-1) * stepsize
        thisMaxPeak = i * stepsize
        
        thisBinValue = 0
        for j in range(thisMinPeak, thisMaxPeak): # should avoid double-counting at boundaries - check!
            thisBinValue += absPeakValueHistogram[j]
            
        #filteredByThreshold = nofTriggersFilteredByThreshold(triggerList, thisThreshold)
#        thisThresholdCount = thresholdHistogram[i]
        gX[i] = thisMinPeak + 0.5 * (thisMaxPeak - thisMinPeak)
        gY[i] = thisBinValue / minutes

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
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Single triggers per minute, vs peak value")
    graph.SetFontSize(3.0)

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total triggers per minute = " + format(totalNumberOfTriggers / minutes, "8.1f"))

    graph.SetFunc("","lg(y)","") 
    graph.SetTicks('y',0)		 
    graph.SetTicks('x', maxX / 8.0)
    #graph.SetRanges(0.0, gX.Max('x')[0], gY.Min('x')[0], gY.Max('x')[0])


    graph.Axis() # comes after all definitions as before
    graph.Grid()
    graph.Label("x","ADC peak value",1)
    graph.Label("y","Counts per minute",1)
    graph.Plot(gX,gY, 'b o#'); # ' ' means no line; 'o' means o symbols; '#' means solid symbols.

    graph.WriteEPS("triggersVersusPeakValue-"+fileName+".eps","Counts per minute")


def plotBinnedTimeSeriesOfTriggers(binnedData, firstTime, lastTime): 
    # make binned timeseries of trigger counts to see variations over time
    
    nBins = len(binnedData)
    
    timeBinSize = float(lastTime - firstTime) / nBins # seconds
#    print nBins
#    print firstTime
#    print lastTime
#    print timeBinSize

    x = [timeBinSize * float(k) for k in range(nBins)] # look, I can write compact Python statements! ;)
    y = binnedData
    width=1200  
    height=600
    mglGraphPS = 1
    graph = mglGraph(mglGraphPS, width, height)

    gY = mglData(len(y))
    gX = mglData(len(x))

    for i in range(len(y)):
        gY[i] = float(y[i])

    for i in range(len(x)):
        gX[i] = float(x[i]) / 3600.0

    maxX = float(lastTime - firstTime) / 3600.0
    maxY = float(max(y))
        
    graph.Clf()
    graph.SetFontSize(3.0)
    graph.SetRanges(0.0, maxX, 0.0, maxY)
    
    if (TwentyfourHourPlot):
      graph.SetTicks('x', 1.0, 0)

    graph.Axis("xy")
    graph.Grid()

    #graph.Title("Time series of # triggers (binned)")
    graph.SetFontSize(6.0)
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Time series of triggers (binned)")
    graph.SetFontSize(3.0)

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.05,0,"Total trigger count = " + str(totalNumberOfTriggers) + "; bin width = " + format(timeBinSize, "4.1f") + " s")

    graph.Label("x","Time (hr)",1)
    graph.Label("y","Counts",1)
    graph.Bars(gY)

    graph.WriteEPS("timeSeriesOfTriggers-"+fileName+".eps","Binned timeseries of triggers")


def plotProfileTimeSeries(timeSeriesProfileCount, timeSeriesProfileValue, firstTime, lastTime, RCUgroups): 
    # make profile histogram timeseries of noise level ('power before') to see variations over time
    
    nBins = len(timeSeriesProfileCount[0])
    nGroups = len(RCUgroups)
    
    # average RCUS in one group
    groupedCount = [[0 for i in range(nBins)] for j in range(nGroups)]
    groupedValue = [[0 for i in range(nBins)] for j in range(nGroups)]
#   DO NOT use: groupedCount = [[0]*nBins] * nGroups

    groupIndex = -1
    global groups
    groups = RCUgroups
#    print RCUgroups
#    print nGroups
#    global tpcount
#    tpcount = timeSeriesProfileCount
    for group in RCUgroups:
        groupIndex += 1
        for RCU in group:
            for i in range(nBins):
                groupedCount[groupIndex][i] += timeSeriesProfileCount[RCU][i]
                groupedValue[groupIndex][i] += timeSeriesProfileValue[RCU][i]
  
   # print groupedCount[0]
    #print groupedCount[1]
    timeBinSize = float(lastTime - firstTime) / nBins # seconds

    x = [timeBinSize * float(k) for k in range(nBins)] # look, I can write compact Python statements! ;)
  
    width=1200  
    height=1200
    mglGraphPS = 1
    graph = mglGraph(mglGraphPS, width, height)

    gY = mglData(nBins, nGroups)
    gX = mglData(nBins, nGroups)
    maxY = 0
    for n in range(nGroups):
        for i in range(nBins):
            if groupedCount[n][i] != 0:
                thisValue = float(groupedValue[n][i]) / float(groupedCount[n][i])
                if thisValue > maxY:
                    maxY = thisValue
                gY.Put(thisValue, i, n)
            else:
                gY.Put(float('nan'), i, n)
            gX.Put(float(x[i]) / 3600.0, i, n)  
 
                   # if timeSeriesProfileCount[RCU][i] != 0:
          #      gY.Put(5.0 * RCU + float(timeSeriesProfileValue[RCU][i]) / float(timeSeriesProfileCount[RCU][i]), i, RCU)
          # else:
          #      gY.Put(NaN, i, RCU)
          # gX.Put(float(x[i]) / 60.0, i, RCU)
            
    maxX = float(lastTime - firstTime) / 3600.0
    print 'max'
    print maxX
    print maxY    
    graph.Clf()
    graph.SetFontSize(2.0)
    graph.SetRanges(0.0, maxX, 0.0, maxY * 1.05)
    if (TwentyfourHourPlot):
        graph.SetTicks('x', 1.0, 0) 

    graph.Axis("xy")
    graph.Grid()

    #graph.Title("Time series of # triggers (binned)")
    graph.SetFontSize(4.0)
    graph.Puts(float(maxX) * 0.5, float(maxY) * 1.15, 0, "Profile histogram of noise level")
    graph.SetFontSize(2.0)

    graph.Puts(float(maxX) * 0.5,float(maxY)*1.08,0,"Total trigger count = " + str(totalNumberOfTriggers) + "; bin width = " + format(timeBinSize, "4.1f") + " s")
    graph.SetFontSize(3.0)
    graph.Label("x","Time (hr)",1)
    graph.Label("y","Noise level",1)
    graph.Plot(gX, gY)

    graph.WriteEPS("noiseProfileHistogram"+fileName+".eps","Profile histogram of noise level")


def runFullAnalysis():
    print "Reading trigger list..."
    processCSVFile(fileName)
    
    print 'Creating plots...'
    makeTriggersVersusRCUHistogram(rcuCount)
    plotBinnedTimeSeriesOfTriggers(timeSeriesHistogram, firstTime, lastTime)
   
    plotProfileTimeSeries(timeSeriesProfileCount, timeSeriesProfileValue, firstTime, lastTime, RCUgroups)
    makeTriggersVersusThresholdPlot()
    makeTriggersVersusAbsPeakValuePlot()
    print 'Done!'

# main
runFullAnalysis()
