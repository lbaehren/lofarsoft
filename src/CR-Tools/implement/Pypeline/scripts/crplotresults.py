"""
Parse batch results from the CR Analysis Pipeline, filter on quality criteria, plot results

.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

import os
from pycrtools import *
import numpy as np
import matplotlib.pyplot as plt
#import time

# parse file

trigAzKey = 0   # how to improve this?
trigElKey = 1
trigMseKey = 2
oddAzKey = 3
oddElKey = 4
oddRKey = 5
oddHeightKey = 6
evenAzKey = 7
evenElKey = 8
evenRKey = 9
evenHeightKey = 10
avgCountKey = 11
maxCountKey = 12
summedHeightKey = 13
coherencyKey = 14
timeKey = 15
filenameKey = 16

resultsFile = open('asciiPipelineResults_stabilityRun_2995files.txt', mode='r')
results = []
for line in resultsFile:
    theseResults = line.split()
    if theseResults[0][0] not in '1234567890':
        print theseResults
        continue # don't import header
    for i in range(len(theseResults)):
        if theseResults[i][0] in '1234567890': # exclude header and filename from float conversion
            theseResults[i] = float(theseResults[i])
    results.append(theseResults)
resultsFile.close()

# transpose results
results = zip(*results)

# make scatter plot with arrival directions (triggers)
trigAz = np.array(results[trigAzKey])
trigEl = np.array(results[trigElKey])
trigMse = np.array(results[trigMseKey])

avgCount = np.array(results[avgCountKey]) # will filter on this

dataAz = np.array(results[oddAzKey])
dataEl = np.array(results[oddElKey])

#az = []
#for entry in results: # fixme: has to be improved!
#    if type(entry[trigAzKey]) != type('string'):
#        az.append(entry[trigAzKey])
#
#el = np.array()
#for entry in results:
#    if type(entry[trigElKey]) != type('string'):
#        el.append(90.0 - entry[trigElKey])

rlim = (0.0, 90.0) # r-axis of polar plot, here: 90.0 - elevation

#selectionIndices = np.where(trigMse < 30.0)
selectionIndices = np.where(avgCount < 2.0)

trigAz = trigAz[selectionIndices]
trigEl = trigEl[selectionIndices]
goodDataAz = dataAz[selectionIndices]
goodDataEl = dataEl[selectionIndices]

badIndices = np.where(trigMse > 30.0)
falseDataAz = dataAz[badIndices]
falseDataEl = dataEl[badIndices]

thisplot = plt.axes(polar = True)
thisplot.scatter(trigAz * 2*np.pi / 360.0, 90.0 - trigEl, c=[1, 0, 0], lw=0, s=80, label = 'From triggers')
thisplot.set_ylim(rlim)

#plt.figure()
dataplot = plt.axes(polar = True)
#dataplot.scatter(goodDataAz * 2*np.pi / 360.0, 90.0 - goodDataEl, c=[0, 0, 1], lw = 0, s = 80)
#dataplot.scatter(falseDataAz * 2*np.pi / 360.0, 90.0 - falseDataEl, c=[1, 1, 0], lw=0, s=80)

# no selection on trig mse:
dataplot.scatter(goodDataAz * 2*np.pi / 360.0, 90.0 - goodDataEl, c=[0, 0, 1], lw = 0, s = 80, label = 'From data')
dataplot.set_ylim(rlim)
plt.legend(loc = 0)
