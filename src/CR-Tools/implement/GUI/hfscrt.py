obj=d["PlotWindow'npanels"]
obj.connect(gui.npanels)
obj.connect(gui.npanels,QtCore.SLOT("setText(QString)"))

d["PlotWindow'npanels"]=8

obj.activatePyQt()
QtCore.QObject.connect(obj.PyQt(),obj.SIGNAL(),gui.npanels,QtCore.SLOT("setText(QString)"))
QtCore.QObject.connect(hfm.ui.dummy,QtCore.SIGNAL("clicked()"),obj.SLOT())

hfo.connect(hfm.ui.dummy,QtCore.SIGNAL("clicked()"),hfo.put)
hfo.connect(hfo,QtCore.SIGNAL("hfupdated(QString)"),hfm.ui.npanels,QtCore.SLOT("setText(QString)"))


class hfobject(QtCore.QObject):
    def put(self):
        print "Clicked:",self.sender().objectName()
        return 0
    def updated(self,object):
        self.hfupdated(object.getS())
        return 0
    def hfupdated(self,value):
        self.emit(QtCore.SIGNAL("hfupdated(QString)"),QtCore.QString(str(value)))
        return 0


hfo=hfobject()
d["PlotWindow'npanels"].storePyQt(hfo)

hfo.connect(hfm.ui.dummy,QtCore.SIGNAL("clicked()"),hfo.put)
hfo.connect(hfo,QtCore.SIGNAL("hfupdated(QString)"),hfm.ui.npanels,QtCore.SLOT("setText(QString)"))

hfo.connect(hfo,QtCore.SIGNAL("hfupdated(QString)"),hfm.ui.npanels.setText)

d["PlotWindow'npanels"]=8

QtCore.QObject.disconnect(hfm.ui.npanels, QtCore.SIGNAL("returnPressed()"), hfm.ui.HMainPlotter.hfsetNPanels)
QtCore.QObject.connect(hfm.ui.npanels, QtCore.SIGNAL("returnPressed()"), hfm.ui.HMainPlotter.hfsetNPanels)




pw=d["QtPanel'PlotWidget"]
g=d["PlotWindow"].getPy()
pw.setGraph(g)

##This is better ...
g=d["PlotWindow"].getPy()
g.Clf()
d["QtPanel"].update()



obj=hffunc()
t=Data("Root")
t.newObjects("PythonObject",DIR.FROM)
t["PythonObject"].putPy(obj)
t["PythonObject"].newObjects("PyFunc",DIR.FROM)
t
t["PythonObject"].printStatus(0)
t["PyFunc"].setFunction("PyFunc",TYPE.INTEGER,"Py")
t["PyFunc"].getI()
t
t["PythonObject"].printStatus(0)



d["Antenna=0:Data"].newObject("hfPlotData",DIR.FROM).newObject("hfPlotPanel",DIR.FROM).newObject("hfPlotWindow",DIR.FROM)
d["hfPlotPanel"].newObject("'GraphObject").pystore(





#y=mglData(50,3)
#y.Modify("cos(2*pi*x)")

size=len(vf)
y=mglData(size)
mglDataSet(y,vf)
ymax=y.Maximal()
ymin=y.Minimal()

width=800
height=600
gr=mglGraph(mglGraphPS,width,height)
gr.Clf(50,50,50)
gr.SetFontSize(3.)
gr.SetFontDef("rR:r")
gr.SetRanges(ymin,ymax,ymin,ymax)
gr.Axis("xy")
#gr.SetRanges(0,size,ymin,ymax)
gr.Title("Test Plot")
gr.Box("g")
gr.Plot(y,y,"b")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.WriteEPS('test.ps')
os.system("kghostview test.ps&")

gr.WritePNG('test.png')
os.system("xview test.png&")

gr.WriteEPS("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/GUI/test.eps","Test Plot")

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
#gr.SetRanges(0,1024,-2,2)
#gr.SetRanges(ymin,ymax,ymin,ymax)
gr.SetRanges(0,1024,ymin,ymax)
gr.Axis("xy")
gr.Title("Test Plot")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.Plot(y);
gr.WriteEPS("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/GUI/test.eps","Test Plot")


#rgb=gr.GetRGB()
#img=QtGui.QImage(rgb, width,height, QtGui.QImage.Format_RGB32)

d["hfQtPanel"].redo()
qw=d["hfQtPanel'PlotWidget"].getPy()
l=qw.img.scanLine(200)
qw.img.save("qtest.jpg")
qw.img.save(QtCore.QString("qtest.jpg"))


from PyQt4 import QtGui,QtCore
from mathgl import *
gr = mglGraph();
gr.Box();
buffer='\t'; buffer=buffer.expandtabs(4*gr.GetWidth()*gr.GetHeight());
gr.GetBGRN(buffer,len(buffer))
qimg=QtGui.QImage(buffer, gr.GetWidth(),gr.GetHeight(),QtGui.QImage.Format_ARGB32)
#Note: if you delete the variable buffer, the qimage will be destroyed since
#it will still be using the buffer. In particular you can't use buffer as a
#local variable in a function or method and just return the qimage.

#To save the image to file use
qimg.save("qtest.png")

#To view the file launch an image viewer program from your operating system
import shutil, os
os.system("xview qtest.png")


#Finally if you want to creat a QWidget with the image you need to do the additional steps.

import sys
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

qw = hfQtPlot()
qw.show()
qw.setgraph(gr)







#os.system("xview qtest.jpg&")


#Example:
#adding new methods in pyhton - this will have effect on all Data objects
#from libhfget import *
#def Data_objstore(self,object):
#    self.object=object
#    return
#
#Data.objstore = Data_objstore
  
