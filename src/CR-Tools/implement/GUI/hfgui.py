
#start designer 
#pyuic4 hfgui.ui | head --lines=-1 > hfgui_ui.py
#pyuic4 hfgui2.ui > hfgui2_ui.py
#pyuic4 hfgui2.ui | awk -f hfuiprep.awk  > hfgui2_ui.py
#delete last line: from hfQtPlot import hfQtPlot

#pyuic4 hfplotterparameters.ui | head -n -1 > hfplotterparameters_ui.py

'''

Logic:
hfm=hfMainWindow is a QMainWindow. 

hfm.ui holds a QtObject which was put together by the designer (see
hfgui2_ui.py). It contains all the frames and buttons of the GUI.

Calling hfm.ui.setupUI will create all the  frames and buttons.

The buttons and inputs can be access through, e.g. hfm.ui.color or
hfm.ui.pushButton_2 - there is a flat hierarchy here.

hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"]) defines a simpe class
which will store as hfQtPlotWidget.hfqtplot the Python object stored in
data["'PlotWidget"], which is a hfQtPlot widget.

** When the GUI is launched it will call hfQtPlotWidget(QFrame). The
function will then simply assign the QFrame as a parent to the
PlotWidget. This way we move the PlotWidget that was created in the
network under the GUI. Also, it returns an object of class hfQtPlot
which is the main class we use to interface with the graphics window
(mathGL window).

HMainPlotter will then contain all the QtWidget methods of the
plotting plotting window used for mathGL, but also other methods or
variables like

HMainPLotter.d  is the QtPanel data object we use as the starting
point in out network (it is actually the end, being at the other side
of the root object)

**
HMainPlotter.gr which is the QImage that is displayed (bitmap of graphics)
HMainPlotter.buffer which is the data buffer used to communciated between mathGL and Qt


** gr=d["Antenna=0:PlotPanel'GraphObject"].getPy()
or 
gr= hfm.ui.HMainPlotter.gr

Will get you the mathGL graphics Object that is bein used.

pw=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel
pw.PlotWidget # provides access to the plot widget that holds the MathgGL image
  

************
Still to be worked on:
class hfParameterDispatcher(QtCore.QObject):
    def setparameter(self):
        
           def setpanels():
        txt=yyy.panelselection.currentText()
n
    def PlotDataColorSet():
        self.d["PlotData'Color"].Q=yyy.color.text()

'''


qpointf=QtCore.QPointF()

class hfQtPlotConstructor(QtGui.QWidget):
    def __call__(self,parent):
        self.hfqtplot.setParent(parent)
        return self.hfqtplot
    def __init__(self,data):
        self.hfqtplot=data["'PlotWidget"].getPy()
        self.hfqtplot.setData(data)
        self.data=data
        self.hfqtpanel=data.retrievePyFuncObject()
#        self.hfqtpanel=data["'PythonObject"].getPy()
        self.ui=None


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
    def hfsetNPanels(self):
        print "NPanels"
        print "Sender:",self.sender()
        print " txt=",self.sender().text()
        nt=self.sender().text().toInt()
        if nt[1]: self.d["'npanels"].noSignal().set(nt[0])
    def hfsetNPanelsx(self):
        nt=self.sender().text().toInt()
        if nt[1]: self.d["'npanelsx"].noSignal().set(nt[0])
    def hfsetNPanelsy(self):
        nt=self.sender().text().toInt()
        if nt[1]: self.d["'npanelsy"].noSignal().set(nt[0])
    def hfsetXMin(self):
        nt=self.sender().text().toDouble()
        if nt[1]: self.d["'xmin"].noSignal().set(nt[0])
        print "Set Xmin",nt[0]
    def hfsetXMax(self):
        nt=self.sender().text().toDouble()
        if nt[1]: self.d["'xmax"].noSignal().set(nt[0])
    def hfsetYMin(self):
        nt=self.sender().text().toDouble()
        if nt[1]: self.d["'ymin"].noSignal().set(nt[0])
        print "Set Ymin",nt[0]
    def hfsetYMax(self):
        nt=self.sender().text().toDouble()
        if nt[1]: self.d["'ymax"].noSignal().set(nt[0])
    def netlevel(self,s):
        nt=s.toDouble()
        if nt[1]: self.d["QtNetview'maxNetLevel"].noSignal().set(nt[0])
    def currentplotdataobject(self):
        return self.currentplotpanelobject()["'PlotData"]
    def currentplotpanelobject(self):
        return self.currentplotwindowobject()["'PlotPanel"]
    def currentplotwindowobject(self):
        return self.d["'PlotWindow"]
    def datatypechooser(self):
        what="Datatype"
        obj=self.currentplotpanelobject()
        par=obj["'Data'Parameters=Data"] # find the parameter object to which the chooser is connectedxo
        l=QtCore.QStringList(par["'Chooser'"+what].val()) #gives a list of all possible choices
        l.prepend("<None>")
        self.gui.ydatatype.clear()
        self.gui.ydatatype.addItems(l) 
        self.gui.xdatatype.clear()
        self.gui.xdatatype.addItems(l) 
        self.gui.xdatatype.setCurrentIndex(0)
        self.gui.ydatatype.setCurrentIndex(0)
        sely=obj["'yAxis'"+what].val() #Attention: this might return a list or a single item (assume single item)
        selx=obj["'xAxis'"+what].val()
        if (len(selx)>0):
            if selx in l: self.gui.xdatatype.setCurrentIndex(l.indexOf(selx)) 
        if (len(sely)>0):
            if sely in l: self.gui.ydatatype.setCurrentIndex(l.indexOf(sely)) 
    def ydatatype(self,s): ##hf ...
        "Slot used by the GUI to change the yAxis Datatype"
        if s=="<None>": return
        s=str(s) #Otherwise all string become QString objects ....
        what="Datatype"
        obj=self.currentplotpanelobject()
        par=obj["'yAxis'Data'Parameters=Data"]
        oldobj=par["'"+what] # find the parameter object to which the chooser is connected
        newobj=par["'Chooser'"+what+"="+s]
        if (type(newobj)==Data) &(type(oldobj)==Data) & (not newobj==oldobj):
            oldobj // par
            newobj >> par
        else: print what+"="+s,"Object not found (y)." 
    def xdatatype(self,s):
        "Slot used by the GUI to change the xAxis Datatype"
        if s=="<None>": return
        s=str(s) #Otherwise all string become QString objects ....
        what="Datatype"
        obj=self.currentplotpanelobject()
        par=obj["'xAxis'Data'Parameters=Data"]
        oldobj=par["'Datatype"] # find the parameter object to which the chooser is connected
        newobj=par["'Chooser'"+what+"="+s]
        if (type(newobj)==Data) & (not newobj==oldobj):
            oldobj // par
            newobj >> par
        else: print what+"="+s,"Object not found (x)." 


#Use 
#self=d["QtPanel'PlotWidget"].getPy()
#to get access to the above functions
#e.g. self.xdatatype


execfile("hfgui2_ui.py")

class hfMainWindow(QtGui.QMainWindow):
    def __init__(self, hfqtplotwidget,parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        hfqtplotwidget.ui=self.ui
        hfqtplotwidget.hfqtpanel.gui=self.ui
        hfqtplotwidget.hfqtplot.gui=self.ui
        self.ui.setupUi(self)

