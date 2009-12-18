import shutil, os, subprocess
import sys, random
from PyQt4 import QtGui,QtCore,QtSvg
#import hfgui
#import hffuncs
#import hfcrfuncs

app = QtGui.QApplication(sys.argv)
hfQSvgWidget=QtSvg.QSvgWidget
hfQtPlotWidget=0
hfm=0
gui=0
qtgui=0
ncount=0
mglGraphZB=0
mglGraphPS=1
mglGraphGL=2
mglGraphIDTF=3
