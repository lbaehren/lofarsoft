import shutil, os, subprocess
import sys, random
from math import *
from PyQt4 import QtGui,QtCore,QtSvg
from libhfget import *
from mathgl import *
app = QtGui.QApplication(sys.argv)

execfile("hfinit.py")
execfile("hffuncs.py")
execfile("hfgui.py")
execfile("hfnet.py")

