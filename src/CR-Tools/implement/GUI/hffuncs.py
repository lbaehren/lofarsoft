class NULL_Pointer():
    def null(self):
        return 0

NULL=NULL_Pointer()

verbose = False

class empty():
    def __init__(self): return


class hffunc():
    def hfstartup(self,d):
        if verbose: print "Startup.", self.__doc__
        d.Silent(True)
        self.data=d
        self.pars=[]
        self.obj=empty()
        self.var=empty()
        ret=self.startup(d)
        self.parnames=StringVec()
        self.parnames.append(d.getName())
        if len(self.pars)>1:
            for par in self.pars:
                self.parnames.append(par.objname)
        #Check for paramter object and create it if necessary
            pobj=d["'Parameters="+'"'+self.parnames[0]+'"']
            if type(pobj)!=Data:
                pobj=d.newObject("'Parameters")
                pobj.setNetLevel(100)
                pobj.put_silent(self.parnames)            
        self.getParameters()
        d.Silent(False)
        self.initialize()
        return ret
    def hfprocess(self,d):
        self.getParameters()
        if verbose: print "Process.", self.__doc__
        return self.process(d)
    def hfcleanup(self):
        if verbose: print "Cleanup.", self.__doc__
        return self.cleanup()
    def startup(self,d):
        "Mainly used to define all the parameters and is called once during the creation of the function"
        if verbose: print "startup", self.__doc__
        return 0
    def initialize(self):
        "Initialize is called at the end of the startup procedure when all parameters have been defined. This can be used to assign values to the parameters, e.g. new python objects."
        if verbose: print "initialize", self.__doc__
        return 0
    def process(self,d):
        if verbose: print "process", self.__doc__
        return 0
    def cleanup(self,d):
        if verbose: print "cleanup", self.__doc__
        return 0
    def setParameter(self,parname,defval,prefix="'",objname=""):
        par=funcparameters(parname,defval,prefix,objname)
        self.pars.append(par)
    def getParameters(self):
        if len(self.pars)>0:
        #Check for paramter object which belongs to this object and create it if necessary
            pobj=self.data["'Parameters="+self.parnames[0]]
            if type(pobj)!=Data:
                pobj=self.data.newObject("'Parameters")
                pobj.setNetLevel(100)
                pobj.put_silent(self.parnames)            
        for par in self.pars:
            obj=pobj[par.objname]
        #First search Object linked to Parameter object
            if type(obj)!=Data: 
                #If not present search through the entire net
                obj=self.data[par.objname]
        #Still not found? Then create it attached to the Parameter Object
                if type(obj)!=Data: 
                    obj=pobj.newObject(par.objname)
                    obj.setNetLevel(100)
                    obj.put_silent(par.defval)
        #Finally we are ready to set the local variable.
            if par.type=="Py":
                if obj.getI()==0: 
                    exec("self.obj."+par.parname+"=None")
                    return 0
            exec("self."+par.parname+"=obj.get"+par.type+"()") 
            exec("self.obj."+par.parname+"=obj") 
        return 0

class funcparameters:
    def __init__(self,parname,defval,prefix="'",objname=""):
        self.prefix=prefix
        if objname=="":
            self.objname=prefix+parname
        else:
            self.objname=prefix+objname
        self.parname=parname
        self.defval=defval
        if type(defval)==type(0): self.type="I"
        elif type(defval)==type(0.):  self.type="N"
        elif type(defval)==type(0j):  self.type="C"
        elif defval==None:  self.type="Py"
        elif type(defval)==type("0"):  self.type="S"
        else: 
            print "Error: funcparameters - unknown type", defval
        return

class hfGraphObject(hffunc):
    "hfGraphObject: Creates a mgl graph object and stores it."
    def startup(self,d):
        d.setAutoUpdate(False)
        return 0
    def process(self,d):
        self.gr=mglGraph(mglGraphPS)
        d.noMod()
        d.putPy(self.gr)
        return 0
    def cleanup(self):
        return 0

class hfPlotData(hffunc):
    "hfPlotData: Plots a single dataobject into a Graphobject/Panel."
    def startup(self,d):
        if verbose: print "Startup hfPlotData"
        self.setParameter("Color","b")
        self.setParameter("GraphObject",None)
        return 0
    def process(self,d):
        if verbose: print "Process hfPlotData user function"
        self.yvec=FloatVec(); 
        self.xvec=FloatVec()
        self.zvec=FloatVec()
        naxis=0;
        axis=d["'yAxis'Data"]
        if not type(axis)==Data: 
            print "hfPlotData: yAxis object not found."
            return 1
        naxis+=1
        axis.get(self.yvec)
        self.ydat=mglData()
        self.ydat.SetVec(self.yvec)
        axis=d["'xAxis'Data"]
        if type(axis)==Data:
            naxis+=1;
            axis.get(self.xvec)
            self.xdat=mglData()
            self.xdat.SetVec(self.xvec)
            if verbose: print "self.xdat.nx=",self.xdat.nx," max=",self.xdat.Maximal()
            axis=d["'zAxis'Data"]
            if type(axis)==Data:
                naxis+=1;
                axis.get(self.zvec)
                self.zdat=mglData()
                self.zdat.SetVec(self.zvec)
        if verbose: print "naxis=",naxis
        if verbose: print "self.ydat.ny=",self.ydat.ny," max=",self.ydat.Maximal()
        if naxis==1: self.GraphObject.Plot(self.ydat,self.Color)
        if naxis==2: self.GraphObject.Plot(self.xdat,self.ydat,self.Color)
        if naxis==3: self.GraphObject.Plot(self.xdat,self.ydat,self.zdat,self.Color)
        return 0
    def cleanup(self):
        return 0


class hfPlotPanel(hffunc):
    "hfPlotPanel: Plots a panel and plots all linked PlotData Objects into it."
    def startup(self,d):
        if verbose: print "PlotPanel - Startup"
        self.setParameter("GraphObject",None)
        self.setParameter("xmin",0.)
        self.setParameter("xmax",1024.)
        self.setParameter("ymin",-70.)
        self.setParameter("ymax",70.)
        self.setParameter("yAxisLabel","y-Axis")
        self.setParameter("xAxisLabel","x-Axis")
        return 0
    def process(self,d):
        if verbose: print "Processing hfPlotPanel."
        if not type(self.GraphObject)==mglGraph:
            print "Error: PlotPanel - GraphObject not of type mglGraph!"
            return 1
        self.GraphObject.SetFontSize(5.)
        self.GraphObject.SetFontDef("rR:r")
        self.GraphObject.SetRanges(self.xmin,self.xmax,self.ymin,self.ymax)
        self.GraphObject.Axis("xy")
        self.GraphObject.Box("g")
        self.GraphObject.Label("x",self.xAxisLabel,1)
        self.GraphObject.Label("y",self.yAxisLabel,1)
        self.PlotData=d["'PlotData"]
        if type(self.PlotData)==Data: 
            self.PlotData.update()
        elif type(self.PlotData)==DataList: 
            for dd in self.PlotData: 
                if verbose: print "PlotPanel calling ",dd.getName(True) 
                dd.update()
        else:
            print "Error: PlotPanel - PlotData not found."
            return 1
        return 0
    def cleanup(self):
        return 0

class hfPlotWindow(hffunc):
    "hfPlotWindow: Plots multiple panels in one Window."
    def startup(self,d):
        self.setParameter("Title","Draft!")
        self.setParameter("npanels",-1)
        self.setParameter("npanelsx",-1)
        self.setParameter("npanelsy",-1)
        self.setParameter("GraphObject",None)
        return 0
    def process(self,d):
        if not type(self.GraphObject)==mglGraph:
            print "Error: PlotWindow - GraphObject not of type mglGraph!"
            return 1
        self.GraphObject.Clf()
        self.GraphObject.Title(self.Title)
        self.PlotPanel=d["'PlotPanel"]
        if type(self.PlotPanel)==Data: 
            self.PlotPanel.update()
        elif type(self.PlotPanel)==DataList:
            npanels=len(self.PlotPanel)
            if self.npanels>=0: npanels=min(self.npanels,npanels)
            if self.npanelsx<0: nx=int(ceil(sqrt(npanels)))
            else: nx=self.npanelsx
            if self.npanelsy<0: ny=int(ceil(1.0*npanels/nx))
            else: ny=self.npanelsy
            n=0
            if verbose: print "nx,ny=",nx,ny," No. of PlotPanel Objects=",len(self.PlotPanel)
            for dd in self.PlotPanel: 
                if n<npanels:
                    self.GraphObject.SubPlot(nx,ny,n)
                    if verbose: print "Processing Plotpanel",dd.getName(True)
                    dd.update()
                n+=1
        else:
            print "Error: PlotWindow - PlotPanel not found."
            return 1
        d.noMod(); d.putPy(self.GraphObject)
        return 0
    def cleanup(self):
        return 0

class hfQtPanel(hffunc):
    "hfQtPanel: Start the GUI Window"
    def startup(self,d):
        if verbose: print "QtPanel startup!"
        self.setParameter("Parent",None)
        self.setParameter("Title","HFLPLOT")
        self.setParameter("Width",765)
        self.setParameter("Height",510)
        self.setParameter("PlotWidget",None)
        self.setParameter("PlotWindow",None)
        self.setParameter("GraphObject",None)
        return 0
    def initialize(self):
        "Called at the end of the startup routine. Will create the PlotWidget and insert it into the PlotWidget Object."
        if self.PlotWidget == None:
            self.PlotWidget=hfQtPlot()
#        d["'PlotWidget"].putPy_silent(self.PlotWidget)
#        d["'PlotWidget"].setNetLevel(999)
            self.obj.PlotWidget.putPy_silent(self.PlotWidget)
            self.obj.PlotWidget.setNetLevel(999)
            self.PlotWidget.setWindowTitle(self.Title)
            self.setSize(self.Width,self.Height)
            self.PlotWidget.setImage(QtGui.QImage())
        return 0
    def setSize(self,width,height):
        self.GraphObject.SetSize(width,height)
        self.PlotWidget.setGeometry(0,0,width,height)
    def process(self,d):
        if verbose: print "QtPanel process"
        self.PlotWidget.setGraph(self.PlotWindow)
        return 0

class hfQtNetview(hffunc):
    "hfQtNetview: Start a QtSVGWidget which shows the current network of objects."
    def startup(self,d):
        if verbose: print "QtNetview startup!"
        self.setParameter("SVGWidget",None)
        self.setParameter("maxNetLevel",99)
        return 0
    def initialize(self):
        "Called at the end of the startup routine. Will create the SVGWidget."
        if self.SVGWidget == None:
            self.SVGWidget=QtSvg.QSvgWidget()
            self.obj.SVGWidget.putPy_silent(self.SVGWidget)
            self.obj.SVGWidget.setNetLevel(999)
        self.SVGWidget.setWindowTitle(d.getName()+" Network Display")
        return 0
    def process(self,d):
        if verbose: print "QtNetview process"
        self.SVGWidget.load(d.netsvg(self.maxNetLevel))
        self.SVGWidget.raise_()
        self.SVGWidget.show()
        return 0

def UnitChooser(self):
    return MakeChooser(self,"Unit",("ScaleFactor","Prefix"),(("",1.0),("f",10**-15),("p",10**-12),("n",10**-9),("mu",10**-6),("m",10**-3),("c",10**-2),("d",10**-1),("h",10**2),("k",10**3),("M",10**6),("G",10**9),("T",10**12),("P",10**15),("E",10**18),("Z",10**21)))

def LOPESDatatypeChooser(self):
    return MakeChooser(self,"LOPESDatatype",("DataType","UnitName","Axis"),(("Time","s","x"),("Frequency","Hz","x"),("Fx","Counts","y"),("Voltage","V","y"),("CalFFT","","y"),("invFFT","","y")))
