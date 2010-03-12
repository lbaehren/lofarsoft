#========================================================================================
from hfinit import *
from hfpywrappers import *
import hffuncs

#start designer 
#pyuic4 hfgui.ui | head --lines=-1 > hfgui_ui.py
#pyuic4 hfgui2.ui > hfgui2_ui.py
#pyuic4 hfgui2.ui | awk -f hfuiprep.awk  > hfgui2_ui.py
#delete last line: from hfQtPlot import hfQtPlot

#pyuic4 hfplotterparameters.ui | head -n -1 > hfplotterparameters_ui.py

#execfile("hfimport.py")

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
CURSORMODE_NONE=0
CURSORMODE_VALUE=1
CURSORMODE_ZOOM=2

qpointf=QtCore.QPointF()

def tex2txt(s):
    "Takes out tex control characters to make a tex string look nicer when printed on the screen as ASCII text."
    s=s.replace("\\","")
    s=s.replace("{","")
    s=s.replace("}","")
    return s

def qtpowerzoom(self,scalez):
    scalez=-scalez
    s=self.parent().parent().size()
    self.zoomscalevalue=scalez
    if hasattr(self,"aspectscalevalue"): scalew=self.aspectscalevalue
    else: scalew=1.0
    self.resize(int(s.width()*1.05**scalez*1.05**scalew),int(s.height()*1.05**scalez))
    self.parent().resize(int(s.width()*1.05**scalez*1.05**scalew),int(s.height()*1.05**scalez))

def qtpoweraspect(self,scalew):
    s=self.parent().parent().size()
    self.aspectscalevalue=scalew
    if hasattr(self,"zoomscalevalue"): scalez=self.zoomscalevalue
    else: scalez=1.0
    self.resize(int(s.width()*1.05**scalez*1.05**scalew),int(s.height()*1.05**scalez))
    self.parent().resize(int(s.width()*1.05**scalez*1.05**scalew),int(s.height()*1.05**scalez))
#    self.resize(int(s.width()*1.05**scale),int(s.width()))
#    self.parent().resize(int(s.width()*1.05**scale),int(s.width()))

def mousePressEvent(self,event):
    print "Mouse pos=",event.pos(),QtCore.QPoint(event.pos());

QtSvg.QSvgWidget.zoom=qtpowerzoom
QtSvg.QSvgWidget.aspect=qtpoweraspect
QtSvg.QSvgWidget.mousePressEvent=mousePressEvent

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
        self.rubberBand=QtGui.QRubberBand(QtGui.QRubberBand.Rectangle, self);
        self.rubberBandH=QtGui.QRubberBand(QtGui.QRubberBand.Line, self);
        self.rubberBandV=QtGui.QRubberBand(QtGui.QRubberBand.Line, self);
        self.rubberOrigin=QtCore.QPoint()
        self.popup=QtGui.QLabel(self)
        self.popup.hide()
        self.popup.setFont(QtGui.QFont("Helvetica", 10, QtGui.QFont.Bold))
        self.popup.setFrameStyle(QtGui.QFrame.StyledPanel)
        self.popup.setBackgroundRole(QtGui.QPalette.ToolTipBase)
        self.popup.setAutoFillBackground(True) 
        self.popup.setFrameShadow(QtGui.QFrame.Raised)
        self.consolelinecount=0
        self.DBGContinueButtonPressed=True
        self.mousemode="z"
        self.mouse_selectmode="p"
        self.presseventpos=()
    def resizeEvent(self,event):
        if hasattr(self,"gr"):
            width=event.size().width(); height=event.size().height()
            self.gr.SetSize(width,height)
            self.setGeometry(0,0,width,height)
            self.hfReplotNetwork()
        self.update()
    def quit(self):
        print "Quitting GUI and Python ...."
        exit()
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
    def hfmouse_zoom(self): 
        self.mousemode="z"
        self.setMouseTracking(False)
        self.setCursor(QtCore.Qt.ArrowCursor)
    def hfmouse_value(self): 
        print "Value Display Mode is diasabled due to recent incompatabilities between the latest SWIG and Boost.Pythong versions"
        return
        print "Attention: Cursor values are only displayed correctly, if all subplots have the same plot ranges... (deselect Auto for Y)"
        self.mousemode="v"
        self.setMouseTracking(True)
        self.setCursor(QtCore.Qt.CrossCursor)
    def hfmouse_select(self): 
        self.mousemode="s"
        self.setCursor(QtCore.Qt.PointingHandCursor)
    def hfmouse_selectwhat(self,str): 
        self.rubberBandH.hide()
        self.rubberBandV.hide()
        self.popup.hide()
        if str=="Values": self.mouse_selectmode="v"
        elif str=="Data Sets": self.mouse_selectmode="d"
        elif str=="Plot Panels": self.mouse_selectmode="p"
        else: print "Programming Error: Mouse select mode unknown ..."
    def mousePressEvent(self,event):
        if event.button()==QtCore.Qt.LeftButton: self.mousePressLeftEvent(event)        
    def mousePressLeftEvent(self,event):
        self.presseventpos=self.screenP2coord(event.pos())
        if self.mousemode=="z":
            self.rubberOrigin = QtCore.QPoint(event.pos());
#            if not self.rubberBand: self.rubberBand = QtGui.QRubberBand(QtGui.QRubberBand.Rectangle, self);
            self.rubberBand.setGeometry(QtCore.QRect(self.rubberOrigin, QtCore.QSize()));
            self.rubberBand.show();
        if self.mousemode=="s":
            hfglobal.app.setOverrideCursor(QtCore.Qt.OpenHandCursor)
    def mouseMoveEvent(self,event):
        if self.mousemode=="z":
            self.rubberBand.setGeometry(QtCore.QRect(self.rubberOrigin, event.pos()).normalized())
        if self.mousemode=="v":
            (xc,yc,nx,ny,npan)=self.screenP2coord(event.pos())
            gdbo=self.d["'PlotPanel:PanelPosition="+str(npan)+"'xAxis:GraphDataBuffer"].FirstObject()
            if gdbo.notFound(): return
            gl=gdbo.getPyList()
            ymin=gdbo["ymin"].val()
            xmin=gdbo["xmin"].val()
            ymax=gdbo["ymax"].val()
            xmax=gdbo["xmax"].val()
            n=mglDataGetVecPos(gl[0],xc);
            x=mglDataGetVecElement(gl[0],n);
            y=mglDataGetVecElement(gl[1],n);
            (xsmin,ysmin)=self.coord2screenP(xmin,ymin,nx,ny,npan)
            (xsmax,ysmax)=self.coord2screenP(xmax,ymax,nx,ny,npan)
            (xs,ys)=self.coord2screenP(x,y,nx,ny,npan)
#             print "xc,yc,npan=",xc,yc,npan
#             print "x,y,n=",x,y,n
#             print "xs,ys=",xs,ys
#             print "x/ymin/max=",xmin,ymin,xmax,ymax
#             print "x/ysmin/max=",xsmin,ysmin,xsmax,ysmax
            if ((x>xmin) & (x<xmax) & (y>ymin) & (y<ymax)):
#                print "Rubberband on!"
                self.rubberBandV.setGeometry(QtCore.QRect(xs,ysmax,2,ysmin-ysmax).normalized())
                self.rubberBandH.setGeometry(QtCore.QRect(xsmin,ys,xsmax-xsmin,2).normalized())
                self.rubberBandH.show()
                self.rubberBandV.show()
                self.popup.setGeometry(xs+10,ys-35,80,25)
                txt="x="+str(x)+"\n"+"y="+str(y)
                self.popup.setText(QtCore.QString(txt))
                self.popup.show()
            else:
                self.rubberBandH.hide()
                self.rubberBandV.hide()
                self.popup.hide()
#            print "----------------------------------------"
            txt="x="+str(xc)+"\n"+"y="+str(yc)+"\n"+"Panel="+str(npan)
            self.gui.CursorValueLabel.setText(QtCore.QString(txt))
    def mouseReleaseEvent(self,event):
        if event.button()==QtCore.Qt.LeftButton: self.mouseReleaseLeftEvent(event)        
    def mouseReleaseLeftEvent(self,event):
        if self.mousemode=="z":
            self.rubberBand.hide();
            xy=self.screenR2coord(self.rubberBand.geometry())
            obj=self.currentplotpanelobject()
            obj["'XAuto"].set_silent(int(False))
            obj["'xmin"].set_silent(xy[0])
            obj["'xmax"].set_silent(xy[2])
            obj["'ymin"].set_silent(xy[1])
            obj["'ymax"].set_silent(xy[3])
            self.hfReplotNetwork()
        elif self.mousemode=="s":
            hfglobal.app.restoreOverrideCursor()
            pos=self.screenP2coord(event.pos())
            npanel=pos[4]
            npanelold=self.presseventpos[4]
            if self.mouse_selectmode=="p":
                panel=self.currentplotwindowobject()["'PlotPanel:PanelPosition="+str(npanel)+"'PlotPanel"]
                if panel.Found():
                    if npanel==npanelold:  ##Then this is just a select or deselect click
                        name=panel.getName(True)+" #"+str(npanel)
                        if panel.isNeighbour("SelectBoard"):
                            panel // self.d["'SelectBoard"]
                        else:
                            panel >> self.d["'SelectBoard"]
                        self.hfReplotNetwork()
                    else:  ##Then this is a move event
                        self.movepanels(self.d["'SelectBoard'PlotPanel"],panel)
    def hfSelectAll(self):
        if self.mouse_selectmode=="p": self.selectallpanels()
    def hfDeselectAll(self):
        if self.mouse_selectmode=="p": self.deselectallpanels()
    def selectallpanels(self):
        for p in self.currentplotwindowobject()["'PlotPanel"].asList():
            if not p.isNeighbour("SelectBoard"): p >> self.d["'SelectBoard"]
        self.hfReplotNetwork()
    def deselectallpanels(self):
        for p in self.currentplotwindowobject()["'PlotPanel"].asList():
            if p.isNeighbour("SelectBoard"): p // self.d["'SelectBoard"]
        self.hfReplotNetwork()
    def movepanels(self,panels,panel):
        "This will move the PlotData objects from the object in 'panels' to the panel object. Hence the data sets displayed previously in those panels will now be displayed in one plot panel."
        if panel.Found():
            for p in panels.asList():
                if not p==panel:
                    for pd in p["'PlotData"].asList(): 
                        pd >> panel # Create the new link
                    l=p.Chain(DIR.TO,["PlotWindow","SelectBoard"],False) # Find the result objects
                    ~l # Delete the old plotpanelobject plus results
            self.hfReplotNetwork()
    def hfSplitPanels(self):
        panels=self.d["'SelectBoard'PlotPanel"].asList()
        for p in panels: self.splitpanel(p)
    def splitpanel(self,panel):
        "This will move the PlotData objects from the object in 'panel' to individual plot panels. Hence the data sets displayed previously in panel will now be displayed separately."
        if panel.Found():
            for pd in panel["'PlotData"].asList()[1:]: 
                plotpanels=pd["PlotPanel"].asList() # find all plotpanel neighbours
                if len(plotpanels)==1:
                    x = pd >> ("PlotPanel",x_f(hffuncs.hfPlotPanel)) # Create a new PlotPanel Object if none exists
                    x["Results"] >> pd["PlotWindow"] 
                pd // panel # OK, now delete the link to the old plotpanel
            self.hfReplotNetwork()
    def whichpanel(self,xs,ys):
        "Returns the number of rows and columns of panels displayed and returns an index to the panel which contains screen position xs,xy. Returns a tupel of the form (nx,ny,n) and calls SubPlot with this input."
        n=self.d["'PlotWindow:npanelsplotted"]
        if n>1:
            nx=self.d["'PlotWindow:npanelsplottedx"].val()
            ny=self.d["'PlotWindow:npanelsplottedy"].val()
            width=self.gr.GetWidth()
            height=self.gr.GetHeight()
            npx=int(ceil(1.0*xs/width*nx))
            npy=int(ceil(1.0*ys/height*ny))
            n=(npy-1)*nx+(npx-1)
        else: 
            nx=1
            ny=1
            n=1
        self.gr.SubPlot(nx,ny,n)
        return (nx,ny,n)
    def coord2screenP(self,x,y,nx,ny,n):
        panel_width=self.gr.GetWidth()/nx
        panel_height=self.gr.GetWidth()/ny
        row=n/nx;
        col=n-ny*row
        xs1=col*panel_width+1
        ys1=row*panel_height+1
        xs2=xs1+panel_width-3
        ys2=ys1+panel_height-3
        xy1=self.gr.CalcXYZ(xs1,ys1)
        xy2=self.gr.CalcXYZ(xs2,ys2)
        xp=round((x-xy1.x)/(xy2.x-xy1.x)*(panel_width-2)+xs1)
        yp=round((y-xy1.y)/(xy2.y-xy1.y)*(panel_height-2)+ys1)
#         print "----------------------------------------"
#         print "x,y,nx,ny,n=",x,y,nx,ny,n
#         print "row,col=",row,col
#         print "panel_width,panel_height=",panel_width,panel_height
#         print "xs1,ys1,xs2,ys2",xs1,ys1,xs2,ys2
#         print "xy1.x,xy1.y,xy2.x,xy2.y=",xy1.x,xy1.y,xy2.x,xy2.y
#         print "xp,yp=",xp,yp
#         print "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
        return (xp,yp)
    def screenP2coord(self,qp):
        "Given a QPoint this will return the coordinates  in the current units chosed by the axis of the panel closest to the point. It will also return the total rows & columns, and index of the selected panel. Returns a tupel of the form: (x1,y1,nx,ny,n)"
        npan=self.whichpanel(qp.x(),qp.y())
        apply(self.gr.SubPlot,npan)
        xy=self.gr.CalcXYZ(qp.x(),qp.y())
        return (xy.x,xy.y) + npan
    def screenR2coord(self,qr):
        "Given a QRectangle this will return the coordinates of the top left and bottom right corner in the current units chosed by the axis of the panel closest to the bottom left position. It wil also return the total rows & columns, and index of the selected panel. Returns a tupel of the form: (x1,y1,x2,y2,nx,ny,n)"
        npan=self.whichpanel(qr.left(),qr.bottom())
        xy1=self.gr.CalcXYZ(qr.left(),qr.bottom())
        xy2=self.gr.CalcXYZ(qr.right(),qr.top())
        return (xy1.x,xy1.y,xy2.x,xy2.y) + npan
    def paintEvent(self, event):
        paint = QtGui.QPainter()
        paint.begin(self)
        paint.drawImage(qpointf,self.img)
        paint.end()
    def hfDBGContinueButtonPressed(self):
        self.DBGContinueButtonPressed=True
    def hfDBGStartButtonPressed(self):
        self.d.debugguion()
        self.gui.consoleoutput.append(">> debugguion()")
    def hfDBGStopButtonPressed(self):
        self.d.debugoff()
        self.d.debugguioff()
        self.hfDBGContinueButtonPressed()
        self.gui.consoleoutput.append(">> debugguioff()")
    def hfConsoleInput(self):
        self.consolelinecount=self.consolelinecount+1
        txt=str(self.gui.consoleinput.text())
        print "txt[0]=",txt[0]
        self.consolelastinput=txt
        self.gui.consoleinput.clear()
        self.gui.consoleoutput.append("["+str(self.consolelinecount)+"]>> "+txt)
        if txt[0]=='!': exec(str(txt[1:]))
        else: self.gui.consoleoutput.append(str(eval(str(txt))))
    def hfsetTextT(self,name,type):
        if type=="IntList":
            t=str(self.sender().text())
            if (t=="") | (t.isspace()): l=[]
            else: l=text2IntVec(t)
            self.d[name].noSignal().set(l)
        else:
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
    def hflastantenna(self):
        self.hfsetTextT("'LastAntenna","Int")
    def hffirstantenna(self):
        self.hfsetTextT("'FirstAntenna","Int")
    def hfantennaselection(self):
        self.hfsetTextT("'AntennaSelection","IntList")
    def hfDelete(self):
        "This is called from the delete action in the GUI (Edit/Delete or backspace)."
        for a in self.d["'SelectBoard'AntennaID"].asList().getSearchName():
            print "Deleting",a
            CRDelAntennaPipeline(self.d["'ROOT"],a)
        self.hfReplotNetwork()
    def hfLoad(self):
        qi=QtCore.QFileInfo(self.currentplotdataobject()["'Filename"].val())
        f=str(QtGui.QFileDialog.getOpenFileName(hfglobal.hfm,"Load Data File",qi.absolutePath()," TimeSeries (*.event *.h5)"))
        if f=="": return
        qi=QtCore.QFileInfo(f)
        if qi.exists():
            self.currentplotdataobject()["'Filename"].set(f)
        else: hfERR("File "+f+"does not exist.")
    def netlevel(self,s):
        nt=s.toDouble()
        if nt[1]: self.d["QtNetview'maxNetLevel"].noSignal().set(nt[0])
    def hfsetNetworkOn(self,yesno):
        if yesno: (self.d >> self.d["'ROOT:QtNetview"]).touch()
        else: (self.d // self.d["'ROOT:QtNetview"]).touch()
    def hfReplotNetwork(self):
        self.currentplotpanelobject()["'Replot"].touch()
#        self.d["'ROOT:QtNetview"].touch()
    def hflogXaxis(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'logX"].set_silent(int(yesno))
        obj["'Replot"].touch()
    def hflogYaxis(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'logY"].set_silent(int(yesno))
        obj["'Replot"].touch()
    def hfsetXAuto(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'XAuto"].set_silent(yesno)
        obj["'xmin"].set_silent(min(toList(obj["xmin"].val())))
        obj["'xmax"].set_silent(max(toList(obj["xmax"].val())))
        obj["'Replot"].touch()
    def hfsetYAuto(self,yesno):
        obj=self.currentplotpanelobject()
        obj["'YAuto"].set_silent(yesno)
        obj["'ymin"].set_silent(min(toList(obj["ymin"].val())))
        obj["'ymax"].set_silent(max(toList(obj["ymax"].val())))
        obj["'Replot"].touch()
    def hfshift_left(self):
        obj=self.currentplotpanelobject()
        xmin=obj["'xmin"].val()
        xmax=obj["'xmax"].val()
        delta=(xmax-xmin)/2
        obj["'xmin"].set_silent(xmin-delta)
        obj["'xmax"].set_silent(xmax-delta)
        obj["'XAuto"].set_silent(False)
        obj["'xscale"].set_silent(1)
        self.hfReplotNetwork()
    def hfshift_right(self):
        obj=self.currentplotpanelobject()
        xmin=obj["'xmin"].val()
        xmax=obj["'xmax"].val()
        delta=(xmax-xmin)/2
        obj["'xmin"].set_silent(xmin+delta)
        obj["'xmax"].set_silent(xmax+delta)
        obj["'xscale"].set_silent(1)
        obj["'XAuto"].set_silent(False)
        self.hfReplotNetwork()
    def hfzoom_in(self):
        obj=self.currentplotpanelobject()
        xmin=obj["'xmin"].val()
        xmax=obj["'xmax"].val()
        delta=(xmax-xmin)/4
        obj["'xmin"].set_silent(xmin+delta)
        obj["'xmax"].set_silent(xmax-delta)
        obj["'xscale"].set_silent(1)
        obj["'XAuto"].set_silent(False)
        self.hfReplotNetwork()
    def hfzoom_out(self):
        obj=self.currentplotpanelobject()
        xmin=obj["'xmin"].val()
        xmax=obj["'xmax"].val()
        delta=(xmax-xmin)/4
        obj["'xmin"].set_silent(xmin-delta)
        obj["'xmax"].set_silent(xmax+delta)
        obj["'xscale"].set_silent(1)
        obj["'XAuto"].set_silent(False)
        self.hfReplotNetwork()
    def hfsetXShift(self,i):
        self.currentplotpanelobject()["'xshift"].set_silent(i)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfsetYShift(self,i):
        self.currentplotpanelobject()["'yshift"].set_silent(i)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfsetYScale(self,i):
        self.currentplotpanelobject()["'yscale"].set_silent(i)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfsetXScale(self,i):
        self.currentplotpanelobject()["'xscale"].set_silent(i)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfXReset(self):
        self.gui.xzoomdial.setValue(0)
        self.gui.xshiftslider.setValue(0)
        self.gui.xauto.setChecked(True)
    def hfYReset(self):
        self.gui.yzoomdial.setValue(0)
        self.gui.yshiftslider.setValue(0)
        self.gui.yauto.setChecked(True)
    def hfsetBlock(self,i):
        self.currentplotdataobject()["'Block"].set(max(min(min(self.d["'Data:maxBlock"].asList().val()),i),0))
    def hfsetBlockPressed(self):
        self.hfsetBlock(self.sender().text().toInt()[0])
    def hfsetBlocksize(self,txt):
        size=min(txt.toInt()[0],self.currentplotdataobject()["'maxBlocksize"].getI())
        if size*(self.currentplotdataobject()["'Block"].getI()+1)>self.currentplotdataobject()["'Filesize"].getI(): self.currentplotdataobject()["'Block"].set_silent(0)
        self.currentplotdataobject()["'Blocksize"].set(size)
    def hfsetBlocksizePressed(self):
        self.hfsetBlocksize(self.sender().text())
    def currentplotdataobject(self):
        return self.currentplotpanelobject()["'PlotData"]
    def currentplotpanelobject(self):
        return self.currentplotwindowobject()["'PlotPanel"]
    def currentplotwindowobject(self):
        return self.d["'PlotWindow"]
    def fillchooser(self,what,rootobj,parname,slot):
        "Fill the GUI selection (ComboBox) with the selections available in the chooser object."
        par=rootobj[parname]
#        l=QtCore.QStringList(map(tex2txt,par["'Chooser'"+what].val())) #gives a list of all possible choices
        l=QtCore.QStringList(par["'Chooser'"+what].val()) #gives a list of all possible choices
        l.prepend("<None>")
        slot.clear()
        slot.addItems(l) 
        slot.setCurrentIndex(0)
        sel=rootobj["'"+what].val() #Attention: this might return a list or a single item (assume single item)
        if (len(sel)>0):
            if sel in l: slot.setCurrentIndex(l.indexOf(sel)) 
    def initializechooser(self):
        self.fillchooser("Datatype",self.currentplotpanelobject()["'xAxis"],"'Parameters=Data",self.gui.xdatatype)
        self.fillchooser("Datatype",self.currentplotpanelobject()["'yAxis"],"'Parameters=Data",self.gui.ydatatype)
        self.fillchooser("UnitPrefix",self.currentplotpanelobject()["'xAxis"],"'Parameters=UnitData",self.gui.xaxisunit)
        self.fillchooser("UnitPrefix",self.currentplotpanelobject()["'yAxis"],"'Parameters=UnitData",self.gui.yaxisunit)
        l=QtCore.QStringList(self.currentplotpanelobject().FirstObject().listFunctions("Math",False)); l.prepend("")
        self.gui.add_fx.clear();  
        self.gui.add_fx.addItems(l); 
        self.gui.add_fx.setCurrentIndex(0)
        self.gui.add_fy.clear();  self.gui.add_fy.addItems(l); self.gui.add_fy.setCurrentIndex(0)
    def chooser(self,s,what,par): ##hf ...
        "Function used to change the selection made in a data chooser object connected to parameter object 'par'. The new selection is 'what=s'"
        if s=="<None>": return
        s=str(s) #Otherwise all string become QString objects ....
        oldobj=par["'"+what] # find the parameter of which is currently connected to the parameter object
        newobj=par["'Chooser'"+what+"="+s] # Find the chosen parameter behind the Chooser object which will become the new parameter 
        if (newobj.NFound()) & (oldobj.NFound()) & (not newobj==oldobj):
            oldobj // par
            newobj >> par
            newobj.touch() 
            #self.hfReplotNetwork()
#        else: print what+"="+s,"Object not found (",par,")." 
    def ydatatype(self,s): ##hf ...
        "Slot used by the GUI to change the yAxis Datatype"
        self.chooser(s,"Datatype",self.currentplotpanelobject()["'yAxis'Data'Parameters=Data"])
    def xdatatype(self,s): ##hf ...
        "Slot used by the GUI to change the xAxis Datatype"
        obj=self.currentplotpanelobject()
        self.chooser(s,"Datatype",self.currentplotpanelobject()["'xAxis'Data'Parameters=Data"])
    def xaxisunitscale(self,s): ##hf ...
        "Slot used by the GUI to change the xAxis Unit scale"
        self.chooser(s,"UnitPrefix",self.currentplotpanelobject()["'xAxis'Parameters=UnitData"])
    def yaxisunitscale(self,s): ##hf ...
        "Slot used by the GUI to change the xAxis Unit scale"
        self.chooser(s,"UnitPrefix",self.currentplotpanelobject()["'yAxis'Parameters=UnitData"])
    def hfadd_fx(self,s): ##hf ...
        "Slot used by the GUI to add a function object into the x-Axis chain"
        fname=str(s)
        if fname=="": return
        obj2=self.currentplotpanelobject()["'xAxis"]
        obj1=obj2.getNeighbour(DIR.FROM)
        map(lambda d1,d2:x_d(fname,x_f(fname)) ^ (d1,d2),obj1.asDataList(),obj2.asDataList())
        self.currentplotpanelobject()["'Replot"].touch()
    def hfadd_fy(self,s): ##hf ...
        "Slot used by the GUI to add a function object into the y-Axis chain"
        fname=str(s)
        if fname=="": return
        obj2=self.currentplotpanelobject()["'yAxis"]
        obj1=obj2.getNeighbour(DIR.FROM)
        map(lambda d1,d2:x_d(fname,x_f(fname)) ^ (d1,d2),obj1.asDataList(),obj2.asDataList())
        self.currentplotpanelobject()["'Replot"].touch()
    def hfremove_fy(self): 
        base=self.currentplotpanelobject()["'y:UnitData"]
        l=base.Chain(DIR.TO,["yAxis","maxBlock","Reference"],False,True)
        if settrace: pdb.set_trace()
        if base.isDataList():
            for dl in l:
                for obj in dl.asDataList()[1:]: dl[0] //= obj
        else: 
            for obj in l.asDataList()[1:]: l[0] //= obj
        self.gui.add_fy.setCurrentIndex(0)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfremove_fx(self): 
        base=self.currentplotpanelobject()["'x:UnitData"]
        l=base.Chain(DIR.TO,["xAxis","maxBlock","Reference"],False,True)
        if settrace: pdb.set_trace()
        if base.isDataList():
            for dl in l:
                for obj in dl.asDataList()[1:]: dl[0] //= obj
        else: 
            for obj in l.asDataList()[1:]: l[0] //= obj
        self.gui.add_fx.setCurrentIndex(0)
        self.currentplotpanelobject()["'Replot"].touch()
    def hfview_fx(self): 
        self.currentplotpanelobject()["'Antenna"].FirstObject()["x"].Chain(DIR.TO,["x","Antenna","Parameters=Data","Parameters=UnitData","GraphDataBuffer","maxBlock","Reference"],False,True).gv(999)
    def hfview_fy(self): 
        self.currentplotpanelobject()["'Antenna"].FirstObject()["y"].Chain(DIR.BOTH,["y","Antenna","Parameters=Data","Parameters=UnitData","GraphDataBuffer","maxBlock","Reference"],False,True).gv(999)
        

#Use 
#self=d["QtPanel'PlotWidget"].getPy()
#to get access to the above functions
#e.g. self.xdatatype
#in hfnet.py this is already defined as "qtgui"


execfile("hfgui2_ui.py")

class hfMainWindow(QtGui.QMainWindow):
    def __init__(self, hfqtplotwidget,parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        hfqtplotwidget.ui=self.ui
        hfqtplotwidget.hfqtpanel.gui=self.ui
        hfqtplotwidget.hfqtplot.gui=self.ui
        self.ui.setupUi(self)
