

#pyuic4 hfgui.ui | head --lines=-1 > hfgui_ui.py
#delete last line: from hfQtPlot import hfQtPlot

qpointf=QtCore.QPointF()

class hfQtPlotConstructor():
    def __call__(self,parent):
        self.qwidget.setParent(parent)
        return self.qwidget
    def __init__(self,data):
        self.qwidget=data["'PlotWidget"].getPy()
        self.qwidget.setData(data)


class hfQtPlot(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.img=(QtGui.QImage())
    def setGraph(self,gr):
        self.gr=gr;
        self.createImage()
    def createImage(self):
        self.buffer='\t' 
        self.buffer=self.buffer.expandtabs(4*self.gr.GetWidth()*self.gr.GetHeight())
        self.gr.GetBGRN(self.buffer,len(self.buffer))
        self.setImage(QtGui.QImage(self.buffer, self.gr.GetWidth(),self.gr.GetHeight(),QtGui.QImage.Format_ARGB32))
    def setImage(self,img):
        self.img=img
        self.update()
    def setData(self,data):
        self.d=data
    def paintEvent(self, event):
        paint = QtGui.QPainter()
        paint.begin(self)
        paint.drawImage(qpointf,self.img)
        paint.end()

execfile("hfgui_ui.py")

class MyForm(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)


#dt = hfQtPlot()
#dt.show()


#execfile("hfgui.py")
