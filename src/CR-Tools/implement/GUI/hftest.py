#A simple demonstration network programme
#python -i hftest.py 
#------------------------------------------------------------------------
#Startup commands, which are also found in hfstart.py
import shutil, os, subprocess
import sys, random
from math import *
from PyQt4 import QtGui,QtCore,QtSvg
from mathgl import *
from libhfget import *
app = QtGui.QApplication(sys.argv)

execfile("hfinit.py")
execfile("hffuncs.py")
execfile("hfgui.py")
#End Startup commands
#------------------------------------------------------------------------

#Now define a network
vf=FloatVec()
d=Data("LOPES")
x=d.listFunctions("",True)
x=d.listFunctions("*",True)
x=d.listFunctions("Math",True)

d >>  ("end",10) >> ("Range",_f("range"))   >> ("SquarePUSH",_f("square"),vf) >> ("sin",_f("sin"),vf) >> ("PrintPUSH",_f("print","Sys",TYPE.NUMBER))
#Change a parameter
d["end"]=5

#Add a network branch of PULL type
d["Range"] << ("SquarePULL",_f("square","Math"),vf) << ("Square2PULL",_f("log","Math"),vf) << ("PrintPULL",_f("print","Sys",TYPE.NUMBER))
d["end"]=7

#Show that the PULL network is only executed when demanded
d["PrintPULL"].val()

#And display the network in graphviz (on Mac, otherwise use d.nv())
d.gv()


