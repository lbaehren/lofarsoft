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
timeKey = 'time';           #time = []
phiKey = 'phi';             #phi = []
thetaKey = 'theta';         #theta = []
varianceKey = 'variance';   #variance = []
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


def processCSVFile(filename): # directly make histogram, not making triggerList
    myKeys = [timeKey, thetaKey, phiKey, varianceKey]
    triggerReader = csv.DictReader(open(fileName), myKeys, delimiter=' ')
    nofPoints = 0
    numberDiscarded = 0
    for record in triggerReader:
        if float(record[varianceKey]) < maximumVarianceAllowed: 
           # phi.append(float(record[phiKey]))
           # theta.append(float(record[thetaKey]))
           # variance.append(float(record[varianceKey]))
            pointList.append(record) # assuming no big files >> 100 MB
            nofPoints += 1
        else:
            numberDiscarded += 1
    print 'Number of points: ' + str(nofPoints)
    print 'Amount of discarded points due to high variance: ' + str(numberDiscarded)
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
qw = hfQtPlot()
qw.show()
qw.setgraph(gr)
qw.raise_()

time.sleep(5)

gr.SetFunc("x*cos(y * 2*3.1415926536 / 360)","x*sin(y *(2*3.1415926536) / 360)","");
# this sets it to polar plot (see MGL doc website), but with angles in degrees

#mglPolar = 1
#gr.SetCoor(mglPolar)
gr.SetRanges(0.0, 90, 0.0, 360)
gr.SetTicks('x', 15.0, 0)
gr.SetFontSize(2.0)
gr.SetMarkSize(0.005) # makes the points smaller

import random
i=0
for record in pointList:
    gX[i] = 90 - float(record[thetaKey]) + random.gauss(0.0, randomizationInDegrees) # set the horizon to 90 degrees
    gY[i] = float(record[phiKey]) + random.gauss(0.0, randomizationInDegrees)
    i += 1
#    gr.Plot(gX,gY, 'r o#'); # 'r' means red; ' ' means no line; 'o' means o symbols; '#' means solid symbols.
#    qw.update()

#gX[1]=30; gY[1]=85 
#gX[2] = 80; gY[2] = 1 ;
#gX[0] = 45; gY[0] = 45 ;
gr.Plot(gX,gY, 'r o#'); # 'r' means red; ' ' means no line; 'o' means o symbols; '#' means solid symbols.

gr.Axis();          
gr.Grid();

qw = hfQtPlot()
qw.show()
qw.setgraph(gr)
qw.raise_()
gr.WriteEPS("VHECRPolarPlotLive.eps","test")

dontquit = input('boe')    
