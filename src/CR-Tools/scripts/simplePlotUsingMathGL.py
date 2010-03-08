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

from mathgl import *
#import numpy
from math import *

mglGraphPS = 1
width = 1200
height = 800
nofPoints = 1024

graph = mglGraph(mglGraphPS, width, height)

gX = mglData(nofPoints)
gY = mglData(nofPoints)

for k in range(nofPoints):
    gY[k] = exp(-k / 700.0) * sin(2*pi*k / 40.0) # fill array with some data
    gX[k] = k

maxY = gY.Max('x')[0] # min, max is complicated in mglData... the 'x' means first dimension not graph-x !
maxX = gX.Max('x')[0]

graph.SetRanges(0.0, maxX, -1.0, 1.0)
graph.SetFontSize(3.0)
graph.Title("Simple plot using MathGL")
graph.Axis()
graph.Grid()
graph.Label("x","label for x-axis",1)
graph.Label("y","label for y-axis",1)
graph.Plot(gX, gY)

graph.WriteEPS("simplePlotUsingMathGL.eps","test plot")
print 'Done writing simple plot!'

