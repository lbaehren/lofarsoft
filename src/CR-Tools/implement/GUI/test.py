import shutil, os, subprocess
import sys, random
from math import *
from PyQt4 import QtGui,QtCore
from mathgl import *

width=800
height=600
size=1024
gr=mglGraph(mglGraphPS,width,height)
y=mglData(size)
y.Modify("cos(2*pi*x)")
x=mglData(size)
x.Modify("x*1024");
ymax=y.Maximal()
ymin=y.Minimal()
gr.Clf()
gr.SetRanges(0,0.5,ymin,ymax)
gr.Axis("xy")
gr.Title("Test Plot x")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.Plot(y);
gr.WriteEPS("test-y.eps","Test Plot")
gr.Clf()
gr.SetRanges(0,size*2,ymin/2,ymax/2)
gr.Axis("xy")
gr.Title("Test Plot xy")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.Plot(x,y);
gr.WriteEPS("test-xy.eps","Test Plot")



