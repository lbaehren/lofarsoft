
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

def qtpowerzoom(self,scale):
    s=self.parent().parent().size()
    self.resize(int(s.width()*1.05**scale),int(s.height()*1.05**scale))
    self.parent().resize(int(s.width()*1.05**scale),int(s.height()*1.05**scale))

def qtpoweraspect(self,scale):
    s=self.parent().parent().size()
    self.resize(int(s.width()*1.05**scale),int(s.width()))
    self.parent().resize(int(s.width()*1.05**scale),int(s.width()))

QtSvg.QSvgWidget.zoom=qtpowerzoom
QtSvg.QSvgWidget.aspect=qtpoweraspect


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
    def hfsetTextT(self,name,type):
        exec("nt=self.sender().text().to"+type+"()")
        if nt[1]: self.d[name].noSignal().set(nt[0])
    def hfsetNPanels(self):
        self.hfsetTextT("'npanels","Int")
    def hfsetNPanelsx(self):
        self.hfsetTextT("'npanelsx","Int")
    def hfsetNPanelsy(self):
        self.hfsetTextT("'npanelsy","Int")
    def hfsetXMin(self):
        self.hfsetTextT("'xmin","Double")
    def hfsetXMax(self):
        self.hfsetTextT("'xmax","Double")
    def hfsetYMin(self):
        self.hfsetTextT("'ymin","Double")
    def hfsetYMax(self):
        self.hfsetTextT("'ymax","Double")
    def hfLoad(self):
        qi=QtCore.QFileInfo(self.currentplotdataobject()["'Filename"].val())
        f=str(QtGui.QFileDialog.getOpenFileName(hfm,"Load Data File",qi.absolutePath(),"*.event"))
        if f=="": return
        qi=QtCore.QFileInfo(f)
        if qi.exists():
            print "New File=",f
            self.currentplotdataobject()["'Filename"].set(f)
        else: hfERR("File "+f+"does not exist.")
    def netlevel(self,s):
        nt=s.toDouble()
        if nt[1]: self.d["QtNetview'maxNetLevel"].noSignal().set(nt[0])
    def hfsetNetworkOn(self,yesno):
        if yesno: self.d >> self.d["'LOPES:QtNetview"]
        else: self.d // self.d["'LOPES:QtNetview"]
    def hfsetXAuto(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'xmin"].put_silent(obj["xmin"].val())
        obj["'xmax"].put_silent(obj["xmax"].val())
        obj["'XAuto"].put_silent(int(yesno))
        obj.touch()
    def hfsetYAuto(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'ymin"].put_silent(obj["ymin"].val())
        obj["'ymax"].put_silent(obj["ymax"].val())
        obj["'YAuto"].put_silent(int(yesno))
        obj.touch()
    def hfsetXShift(self,i):
        self.currentplotpanelobject()["'xshift"].set(i)
    def hfsetYShift(self,i):
        self.currentplotpanelobject()["'yshift"].set(i)
    def hfsetYScale(self,i):
        self.currentplotpanelobject()["'yscale"].set(i)
    def hfsetXScale(self,i):
        self.currentplotpanelobject()["'xscale"].set(i)
    def hfXReset(self):
        self.gui.xzoomdial.setValue(0)
        self.gui.xshiftslider.setValue(0)
        self.gui.xauto.setChecked(True)
    def hfYReset(self):
        self.gui.yzoomdial.setValue(0)
        self.gui.yshiftslider.setValue(0)
        self.gui.yauto.setChecked(True)
    def hfsetBlock(self,i):
        self.currentplotdataobject()["'Block"].set(i)
    def hfsetBlocksize(self,i):
        print "Blocksize should be ",i
#        self.currentplotdataobject()["'Blocksize"].set(i)
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
        if (newobj.NFound()) & (oldobj.NFound()) & (not newobj==oldobj):
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

