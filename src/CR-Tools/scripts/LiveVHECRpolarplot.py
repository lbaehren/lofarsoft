#
#  LiveVHECRpolarplot.py
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
import time

timeKey = 1
thetaKey = 2
phiKey = 3
varianceKey = 4 # indices in list after stripping the 'prefix' (if present)

keepGoing = True

#timeKey = 'time';           #time = []
#phiKey = 'phi';             #phi = []
#thetaKey = 'theta';         #theta = []
#varianceKey = 'variance';   #variance = []
pointList = []

maximumVarianceAllowed = 40.0
randomizationInDegrees = 1.0 # randomize angular positions with sigma = this number

from PyQt4 import QtGui,QtCore
app = QtGui.QApplication(sys.argv)
qpointf=QtCore.QPointF()

class hfQtPlot(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.img=(QtGui.QImage())
    def setgraph(self,gr):
        self.buffer='\t' 
        self.buffer=self.buffer.expandtabs(4*gr.GetWidth()*gr.GetHeight())
        gr.GetBGRN(self.buffer,len(self.buffer))
        self.img=QtGui.QImage(self.buffer, gr.GetWidth(),gr.GetHeight(),QtGui.QImage.Format_ARGB32)
        self.update()
    def paintEvent(self, event):
        paint = QtGui.QPainter()
        paint.begin(self)
        paint.drawImage(qpointf,self.img)
        paint.end()


def processCSVFile(): # Work on global filename to keep file handle around, for growing files...
#    myKeys = [timeKey, thetaKey, phiKey, varianceKey]
#    triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
    
    nofPoints = 0
    numberDiscarded = 0
    eof = False
    pointList = []
    while eof == False:
        where = triggerFile.tell()
        line = triggerFile.readline()

        if not line:
            eof = True
            triggerFile.seek(where)
        else:
            thisPoint = line.split()
            # for new-style logfiles, the 'if', and all keys + 1:
            if thisPoint[0] == 'FitResult:':
                if float(thisPoint[varianceKey]) < maximumVarianceAllowed:
                    thisStrippedPoint = thisPoint[1:]
                    print thisPoint
                    pointList.append(thisPoint)
                    nofPoints += 1
                else:
                    numberDiscarded += 1
#    for record in triggerReader:
#        if float(record[varianceKey]) < maximumVarianceAllowed: 
#           # phi.append(float(record[phiKey]))
#           # theta.append(float(record[thetaKey]))
#           # variance.append(float(record[varianceKey]))
#            pointList.append(record) # assuming no big files >> 100 MB
#            nofPoints += 1
#        else:
#            numberDiscarded += 1
    print 'Number of points: ' + str(nofPoints)
    print 'Amount of discarded points due to high variance: ' + str(numberDiscarded)
    return pointList
    
if len(sys.argv) > 1:
    fileName = sys.argv[1] 
else:
    fileName = 'VHECRTaskLogfile.dat'   #  that's just handy for rapid testing...
    print 'No filename given; usage: python VHECRpolarplot.py <filename>. Using file ' + fileName + ' by default.'

triggerFile = open(fileName)
#processCSVFile()

mglGraphPS = 1
width=800  
height=800 # make it square for circle plot
gr = mglGraph(mglGraphPS, width, height)
#gr.CirclePts = 1000 # don't know how to improve circle drawing... this is obsolete and not working

datalen = len(pointList)
pointsAtATime = 10 # 1 for live stuff!

#gX = mglData(2)
#gY = mglData(2)
# X = theta, Y = phi
# theta = 0 is zenit, have to adjust for coincidence log coordinates.
qw = hfQtPlot()
qw.show()
qw.setgraph(gr)
qw.raise_()

gr.SetFunc("x*cos(y * 2*3.1415926536 / 360)","x*sin(y *(2*3.1415926536) / 360)","");
# this sets it to polar plot (see MGL doc website), but with angles in degrees

#mglPolar = 1
#gr.SetCoor(mglPolar)
gr.SetRanges(0.0, 90, 0.0, 360)
gr.SetTicks('x', 15.0, 0)
gr.SetFontSize(2.0)
gr.SetMarkSize(0.01) # makes the points smaller
gr.Axis();          
gr.Grid();
app.processEvents()

import random
while True: # in C++ this is yucky, but in Python it's good practice according to Google...
    pointList = processCSVFile()
    if len(pointList) == 0:
        app.processEvents()
        time.sleep(1) # so don't hammer to quickly on the file
    else:
        print len(pointList)
        
        i=0
        gX = mglData(len(pointList) + 1)
        gY = mglData(len(pointList) + 1) # may leak memory!
        for record in pointList:

    #        print 'entered loop'
    #        print i%pointsAtATime
            gX[i] = 90 - float(record[thetaKey]) + random.gauss(0.0, randomizationInDegrees) # set the horizon to 90 degrees
            gY[i] = float(record[phiKey]) + random.gauss(0.0, randomizationInDegrees)
            i += 1

    #        if i % pointsAtATime == 0:
        print 'Plot no. ' + str(i/pointsAtATime)
        gr.Plot(gX,gY, 'r o#'); # 'r' means red; ' ' means no line; 'o' means o symbols; '#' means solid symbols.
        gr.Axis();          
        gr.Grid();
        qw.setgraph(gr)
        qw.update()
        app.processEvents() # why is it slowing down even tho only storing/plotting 10 pts at a time?

    if not keepGoing:
        break;

#gX[1]=30; gY[1]=85 
#gX[2] = 80; gY[2] = 1 ;
#gX[0] = 45; gY[0] = 45 ;
#gr.Plot(gX,gY, 'r o#'); # 'r' means red; ' ' means no line; 'o' means o symbols; '#' means solid symbols.

#qw = hfQtPlot()
#qw.show()
#qw.setgraph(gr)
#qw.raise_()
#gr.WriteEPS("VHECRPolarPlotLive.eps","test")

#dontquit = input('boe')    
