#execfile("mgl-pyqt-test.py")

from PyQt4 import QtGui,QtCore
from mathgl import *
import libhfget
import sys
app = QtGui.QApplication(sys.argv)
qpointf=QtCore.QPointF()

mglData.SetVec = mglDataSetVecN

class hfQtPlotter(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.img=(QtGui.QImage())
    def plotGraph(self,gr):
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


        
mglGraphZB=0
mglGraphPS=1
mglGraphGL=2
mglGraphIDTF=3
#mglGraphAB is not available - ZB is default
BackgroundColor=[1.0,1.0,1.0]

def hplot(gr,yvec):
    size=len(yvec)
    y=mglData(size)
    x=mglData(size)
    y.SetVec(yvec)
    x.Modify("x*40+40");
    ymax=y.Maximal()
    ymin=y.Minimal()
    xmax=x.Maximal()
    xmin=x.Minimal()
    height=gr.GetHeight();
    width=gr.GetWidth();
    gr.Clf(BackgroundColor[0],BackgroundColor[1],BackgroundColor[2])
    gr.SetFontDef("rR:r")
    gr.SetFontSize(5.)
    gr.Title("Average Power","iC:k",-1.8)
    gr.SetFontSize(4.)
    gr.SetCut(False);
    gr.SetBaseLineWidth(1); 
    gr.SetTickLen(0.1,2);
    gr.SetRanges(xmin,xmax,ymin,ymax)
    gr.SetTuneTicks(True,-1.2)
    gr.AdjustTicks('xy')
    gr.SetXTT("%g")
    gr.SetYTT("%g")
    gr.Axis("xy")
    gr.Box("g")
    gr.Label("y","Power",1)
    gr.Label("x","Frequency [MHz]",1)
#    gr.SetFontSize(5.)
#    gr.ClearLegend()
#    gr.SetBaseLineWidth(1);     
#    gr.AddLegend("Antenna: "+str(n),"k")
#    gr.Legend(3,"rL",-0.7*(nx*ny)**0.5)
    gr.Plot(x,y)
    qw = hfQtPlotter()
    qw.show()
    qw.plotGraph(gr)
    qw.setGeometry(QtCore.QRect(100, 100, width+100, height+100))
    qw.raise_()
    return qw

#plotpanel(gr,x,y,1,1,0,xmin,xmax,ymin,ymax)
#gr.WritePNG("averagepower.png","Test Plot")

