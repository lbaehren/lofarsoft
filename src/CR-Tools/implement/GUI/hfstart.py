import shutil, os, subprocess
import sys, random
from math import *
from PyQt4 import QtGui,QtCore,QtSvg
from mathgl import *
from libhfget import *
app = QtGui.QApplication(sys.argv)

execfile("hfinit.py")
execfile("hfpywrappers.py")
execfile("hffuncs.py")
execfile("hfgui.py")
execfile("hfcrfuncs.py")
execfile("hfnet.py")

