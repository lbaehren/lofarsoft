#
#  VHECRpolarplot.py
#  
#
#  Created by Arthur Corstanje on 1/20/10.
#  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
#

import csv
from mathgl import *
import numpy
import sys
from math import *
    
timeKey = 'time';           #time = []
phiKey = 'phi';             #phi = []
thetaKey = 'theta';         #theta = []
varianceKey = 'variance';   #variance = []
pointList = []

maximumVarianceAllowed = 40.0
randomizationInDegrees = 1.0 # randomize angular positions with sigma = this number

def processCSVFile(filename): # directly make histogram, not making triggerList
    myKeys = [thetaKey, phiKey, varianceKey]
    triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
    for record in triggerReader:
        if float(record[varianceKey]) < maximumVarianceAllowed: 
           # phi.append(float(record[phiKey]))
           # theta.append(float(record[thetaKey]))
           # variance.append(float(record[varianceKey]))
            pointList.append(record) # assuming no big files >> 100 MB
    
if len(sys.argv) > 1:
    fileName = sys.argv[1] 
else:
    fileName = 'VHECRTaskLogfile.dat'   #  that's just handy for rapid testing...
    print 'No filename given; usage: python VHECRpolarplot.py <filename>. Using file ' + fileName + ' by default.'

processCSVFile(fileName)

mglGraphPS = 1
width=800  
height=800 # make it square for circle plot
gr = mglGraph(mglGraphPS, width, height)
gr.CirclePts = 1000 # don't know how to improve circle drawing... this is obsolete and not working

datalen = len(pointList)

gX = mglData(datalen)
gY = mglData(datalen)
# X = theta, Y = phi
# theta = 0 is zenit, have to adjust for coincidence log coordinates.
import random
i=0
for record in pointList:
    gX[i] = 90 - float(record[thetaKey]) + random.gauss(0.0, randomizationInDegrees) # set the horizon to 90 degrees
    gY[i] = float(record[phiKey]) + random.gauss(0.0, randomizationInDegrees)
    i += 1

#gX[1]=30; gY[1]=85 
#gX[2] = 80; gY[2] = 1 ;
#gX[0] = 45; gY[0] = 45 ;

gr.SetFunc("x*cos(y * 2*3.1415926536 / 360)","x*sin(y *(2*3.1415926536) / 360)","");
# this sets it to polar plot (see MGL doc website), but with angles in degrees

#mglPolar = 1
#gr.SetCoor(mglPolar)
gr.SetRanges(0.0, 90, 0.0, 360)
gr.SetTicks('x', 15.0, 0)
gr.SetFontSize(2.0)
gr.SetMarkSize(0.005) # makes the points smaller
gr.Plot(gX,gY, 'r o#'); # 'r' means red; ' ' means no line; 'o' means o symbols; '#' means solid symbols.

gr.Axis();          
gr.Grid();

gr.WriteEPS("VHECRPolarPlot.eps","test")

