offsets=IntVec()
datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecond.h5", offsets)

offsets=IntVec()
datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/example.event", offsets)


fdata=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,datareader_ptr,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)

d["*AntennaSelection"]=0
d["Data'Block"]=30
d["Data'Blocksize"]=1024*32
d["Filename"]="/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecond.h5"


d["Data'Block"]=30
d["Filename"]="/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecond.h5"
d["Data'Blocksize"]=63488
d["Data'Block"]=598
qtgui.hfadd_fy("abs")
qtgui.hfadd_fy("RunningAverage")
qtgui.hfsetYAuto(False)

offsets=IntVec()
datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecond.h5", offsets)

fdata=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,datareader_ptr,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)

vf=list2vec((1.0,2,3,2.,1.,3.,4.,2.,1.))
v=FloatVec()
w=hWeights(3,hWEIGHTS.LINEAR)
hRunningAverage(vf,v,3,hWEIGHTS.GAUSSIAN)

vi1=list2vec((1,2,3,2,1,3,4,2,1))
vi2=IntVec()
hRunningAverage(vi1,vi2,3,hWEIGHTS.GAUSSIAN)


d=Data("ROOT")
x=d 
for i in range(5): x=x.newObject("TEST"+str(i+1))
~d[4]
~d[5]
d.newObject("NEW")
for i in range(150,201): ~d[i]

x=d; 
for i in range(100,201,2): x=x.newObject("NEW"+str(i+1))

x=d["AntennaID=10101"].Chain(DIR.TO,["PlotWindow","Chooser","GraphObject","Datatype","UnitName","UnitPrefix","GraphDataBuffer","DataPipeline"],False)

d.exportDOT("FILE",9)
#dot -Tsvg ROOT.dot > ROOT.svg

svg=d["QtNetview'SVGWidget"].getPy()
r=svg.renderer()
svg.load("ROOT.svg")

r.matrixForElement("node1").mapRect(r.boundsOnElement("node50")) # with respect to default size
r.defaultSize()


need to manually compare relative position of QRectF wrt to actual screen position.

svg.mousePressEvent=mousePressEvent

def mousePressEvent(self,event):
    print "Mouse pressed"
    print "PosType=",type(event.pos())
    print "Pos=",event.pos()


    #Now set the link to the  unitchooser objects for the newly created antennas using the first existing object as an example
    if (len(existingAntennaNames)>0) & (len(newAntennaNames)>0):
        pdb.set_trace()
        upfx=d[existingAntennaNames[0]+":xAxis'Parameters=UnitData'UnitPrefix"].FirstObject()
        upfy=d[existingAntennaNames[0]+":yAxis'Parameters=UnitData'UnitPrefix"].FirstObject()
#        dtx=d[existingAntennaNames[0]+":xAxis'Parameters=Data'Datatype"].FirstObject()
#        dty=d[existingAntennaNames[0]+":yAxis'Parameters=Data'Datatype"].FirstObject()
        for n in newAntennaNames:
            upfx >> d[n+":xAxis'Parameters=UnitData"]
            upfy >> d[n+":yAxis'Parameters=UnitData"]
#            dtx >> d[n+":xAxis'Parameters=Data"]
#            dty >> d[n+":yAxis'Parameters=Data"]



d["PlotPanel'Parameters=PlotPanel"].Silent(True)
~d["PlotPanel'Parameters=PlotPanel"]
("Parameters","PlotPanel",_l(999)) >> DataUnion(d["PlotPanel"])
d["PlotPanel'Parameters=PlotPanel"].Silent(False)
d["PlotPanel'Parameters=PlotPanel"].touch()


vf=FloatVec()
lofarMainDir = os.environ.get('LOFARSOFT')
hardcodedFileName = lofarMainDir + '/data/lopes/2007.01.31.23:59:33.960.event'
d=Data("ROOT")
d >> _d("Filename",hardcodedFileName,_l(90)) >> _d("File",_f("dataReaderObject","CR")) >> _d("Antenna",0) >>  ("Data",_f("dataRead","CR",TYPE.COMPLEX))




dbg=hfPyDBGObject()
x=d.All().storePyDBG(dbg).setVerbose(9999).setDebug(True).setVerboseGUI(True);
d["File'Block"]=2

d.AllVerbose(9999)

d.AllVerbose(999)
d["QtNetview'maxNetLevel"].setVerbose(0)
d["QtNetview"].setVerbose(0)
d["File'Block"]=2

def mousePressEvent(self,event):
    print "Mouse pressed"
    print "PosType=",type(event.pos())
    print "Pos=",event.pos()

def mymousePressEvent():
    print "Mouse pressed"

hfqtplot.mousePressEvent=mousePressEvent




xmax=30.
xmin=10.
ndef=7.
delta=(xmax-xmin)
ds=delta/ndef
dns=[10**round(log10(ds)),10**round(log10(ds*2))/2,10**round(log10(ds*4))/4]
n=map(lambda(x):abs(delta/x-ndef),dns)
ds0=dns[n.index(min(n))]
org=ceil(xmin/ds0+0.5)*ds0
return (ds0,org)


ymax=364.5
ymin=362.135

width=800
height=600
size=1024
gr=mglGraph(mglGraphPS,width,height)
y=mglData(size)
y.Modify("cos(2*pi*x*10)+362.135+1")
x=mglData(size)
x.Modify("x*1024");
ymax=y.Maximal()
ymin=y.Minimal()
gr.Clf()
#gr.SetRanges(0,1024,-2,2)
#gr.SetRanges(ymin,ymax,ymin,ymax)
gr.SetTuneTicks(False)
xtick=getaxisdivision(0,1023)
gr.SetTicks("x",xtick[0],5,xtick[1])
ytick=getaxisdivision(ymin,ymax)
gr.SetTicks("y",ytick[0],5,ytick[1])
gr.SetRanges(0,1024,ymin,ymax)
gr.Axis("xy")
gr.Title("Test Plot")
gr.Label("x","x-Axis",1)
gr.Label("y","y-Axis",1)
gr.Plot(y);
gr.WritePNG("test.png","Test Plot")


class hfPlotDataOld(hffunc):
    "hfPlotDataOld: Plots a single dataobject into a Graphobject/Panel."
    def startup(self,d):
        if verbose: print "Startup hfPlotDataOld"
        self.setParameter("Color","b")
        self.setParameter("GraphObject",None)
        return 0
    def initialize(self):
        if verbose: print "Initialize hfPlotDataOld"
        self.yvec=FloatVec() #create local scratch vectors
        self.xvec=FloatVec()
        self.zvec=FloatVec()
        self.xdat=mglData() # create local mglData vectors - better create method which does that directly (getmglData)
        self.ydat=mglData()
        self.zdat=mglData()
    def process(self,d):
        if verbose: print "Process hfPlotDataOld user function"
        self.xvec=self.xvec[0:0]  # delete the local scratch vectors
        self.yvec=self.yvec[0:0]
        self.zvec=self.zvec[0:0]
        naxis=0;
        axis=d["'yAxis'Data"]
        if not type(axis)==Data: 
            print "hfPlotDataOld: yAxis object not found."
            return 1
        naxis+=1
        axis.get(self.yvec)
        axis=d["'xAxis'Data"]
        if type(axis)==Data:
            naxis+=1;
            axis.get(self.xvec) #Use Vector selector in the future ...!
            if len(self.xvec)>len(self.yvec): self.xvec=self.xvec[0:len(self.yvec)]
            if len(self.xvec)<len(self.yvec): self.yvec=self.yvec[0:len(self.xvec)]
            if verbose: print "self.xdat.nx=",self.xdat.nx," max=",self.xdat.Maximal()
            axis=d["'zAxis'Data"]
            if type(axis)==Data:
                naxis+=1;
                axis.get(self.zvec)
                self.zdat.SetVec(self.zvec)
        if naxis==1: self.xvec.extend(range(len(self.yvec)))
        self.xdat.SetVec(self.xvec)
        self.ydat.SetVec(self.yvec)
        if verbose: print "naxis=",naxis
        if verbose: print "self.ydat.ny=",self.ydat.ny," max=",self.ydat.Maximal()
        if naxis<=2: self.GraphObject.Plot(self.xdat,self.ydat,self.Color)
        self.putParameter("xmin",self.xdat.Minimal())
        self.putParameter("xmax",self.xdat.Maximal())
        self.putParameter("ymin",self.ydat.Minimal())
        self.putParameter("ymax",self.ydat.Maximal())
        if naxis==3: 
            self.GraphObject.Plot(self.xdat,self.ydat,self.zdat,self.Color)
            self.putParameter("zmin",self.zdat.Minimal())
            self.putParameter("zmax",self.zdat.Maximal())
        return 0
    def cleanup(self):
        return 0


x=Data("Hi")
t=x.newObject("TST")
o=x.new("INS")
o2=x.new("INS2")
x.insert(o,t)


class tst(list):
    def __getitem__(self,val):
        print type(val),val


QtCore.QObject.connect(gui.loadfile,QtCore.SIGNAL("triggered()"),gui.hfLoad)

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
  

##################

d["Antenna=0:Data"].get(vf)

d["File'Block"]=2

d["Antenna=0:PlotWindow"].update()
d["Antenna=0:QtPanel"].update()

gr=d[13].getPy()
gr.WritePNG('test.png')

pw=d["Antenna=0:QtPanel'PlotWidget"].getPy()

d["Antenna=0:PlotData"] >>  ("PlotPanel",_f(hfPlotPanel))

#-----
d >> ("GraphObject",_f(hfGraphObject)) 







d["File'Block"]=1

d["hfPlotData"])
x=hfGraphObject()

("PythonObject",x) >> d["Antenna=0:hfPlotData"]

d["Antenna=0:hfPlotData"].setFunction("PyFunc",TYPE.INTEGER,"Py")

d["Antenna=0:hfPlotData"].getI()

d["Antenna=0:hfPlotData"].setFunction("hfGraphObject
d["Antenna=0:hfPlotData"].getI()

                 d["Test"] =
d["Antenna=0:hfPlotData'GraphObject"]=_f(hfGraphObject)

"GraphObject" >>  DataUnion(d["hfPlotData"])
d["hfPlotData'GraphObject"]=_f(hfGraphObject)

("GraphObject",_f(hfGraphObject)) >>  d["hfPlotData"]



d["hfPlotData"].newObject("'PythonObject",PUSH).putPy(obj3)


d["yAxis"] >> "hfPlotData" >> "hfPlotPanel" >> "hfPlotGraph"

d["Antenna=0:Data"].setAutoUpdate(False)

d["Antenna=0:Data"].get(vf);vf
d["Antenna=0:Data"].get(vf);vf



d["y-Axis"].setFunction("Copy",TYPE.NUMBER)

obj1=hfPlotGraph()
d["hfPlotGraph"].newObject("'PythonObject",PUSH).putPy(obj1)
d["hfPlotGraph"].setFunction("PyFunc",TYPE.NUMBER,"Py") 

obj2=hfPlotPanel()
d["hfPlotPanel"].newObject("'PythonObject",PUSH).putPy(obj2)
d["hfPlotPanel"].setFunction("PyFunc",TYPE.NUMBER,"Py")

obj3=hfPlotData()
d["hfPlotData"].newObject("'PythonObject",PUSH).putPy(obj3)
d["hfPlotData"].setFunction("PyFunc",TYPE.NUMBER,"Py")

obj4=hfQtPanel()
d["hfPlotGraph"].newObject("hfQtPanel",PUSH).newObject("'PythonObject",PUSH).putPy(obj4)
d["hfQtPanel"].setFunction("PyFunc",TYPE.NUMBER,"Py")

d["hfPlotGraph"].redo()
d["hfQtPanel"].redo()

hfQtPlotWidget=hfQtPlotConstructor(d["hfQtPanel"])


#d["Antenna=0:Data"].get(vf);vf[0]

#d["File'Block"].putI(0)
#d["Antenna=0:Data"].get(vf);vf[0]
#
#for i in ids:
#    d[i].putS("Frequency")

#d["dataRead"].newObjects("plotpanel",PULL)
#d["plotpanel"].newObjects("'xmin",PULL)
#d["plotpanel"]["'xmin"]=-1

