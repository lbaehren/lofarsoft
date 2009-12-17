#pdb.set_trace()
execfile("hfimport.py")
from hfinit import *
from hfpywrappers import *
import hfgui

setdebug=False 

class NULL_Pointer():
    def null(self):
        return 0

NULL=NULL_Pointer()
NONEPointer=pytointptr(None)

verbose = False

def logaxisval(xmin,xmax, maxsep=5):
    xmax=abs(xmax)
    if xmax==0.: xmax=1.0
    xmin=max(xmin,0.)
    if xmin==0.: xmin=xmax/10.**maxsep
    return (log(xmin,10.),log(xmax,10.))

def getaxisdivision(xmin,xmax,ndef=6):
    delta=(xmax-xmin)
    ds=delta/ndef
    dns=[10**round(log10(ds)),10**round(log10(ds*2))/2,10**round(log10(ds*4))/4]
    n=map(lambda(x):abs(delta/x-ndef),dns)
    ds0=dns[n.index(min(n))]
    return (ds0,ceil(xmin/ds0+0.5)*ds0)


class empty():
    def __init__(self): return

class hffunc():
    def hfstartup(self,d):
        if verbose: print "Startup.", self.__doc__
        d.Silent(True)
        self.data=d
        self.pars=[]  #This is the parameter_item struct, which is named parameter_list in c++
        self.res={} #This is the parameter_item struct, which ise named result_list in c++
        self.obj=empty()
        self.var=empty()
        ret=self.startup(d)
        self.parnames=StringVec()
        self.parnames.append(d.getName())
        self.resnames=StringVec()
        self.resnames.append(d.getName())
        if len(self.pars)>1:
            for par in self.pars:
                self.parnames.append(par.objname)
        if len(self.res)>1:
            for res in self.res:
                self.resnames.append(self.res[res].objname)
        #Check for paramters object and create it if necessary
        #self.getParametersObject()
        self.getParameters() #create missing parameters objects if not yet present deeper down the net.
        self.setResults() ## make sure result Objects exist.
        d.Silent(False)
        self.initialize(d)
        return ret
    def hfprocess(self,d):
        self.getParameters()
        if verbose: 
            print "Process.", self.__doc__
            self.data.printStatus(True)
        return self.process(d)
    def hfcleanup(self,d):
        if verbose: print "Cleanup.", self.__doc__
        return self.cleanup(d)
    def startup(self,d):
        "Mainly used to define all the parameters and is called once during the creation of the function"
        if verbose: print "startup", self.__doc__
        return 0
    def initialize(self,d):
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
    def setResult(self,parname,defval,prefix="",objname=""): 
        par=funcparameters(parname,defval,prefix,objname)
        self.res[parname]=par
    def putParameter(self,parname,val,prefix="'",objname=""):
        exec("self."+parname+"=val") 
        if (objname==""): objname=parname
        obj=self.getParameterObject(prefix+objname,val)
        obj.put_silent(val)
    def putResult(self,parname,val):
        exec("self."+parname+"=val") 
        res=self.res[parname]
        obj=self.getResultObject(res.objname,val)
        obj.put_silent(val)
    def getParameterObject(self,name,defval): 
        obj=self.data[self.getParametersObjectName()+name]
        #First search Object linked to Parameter object
        if obj.notFound():
        #If not present search through the entire net
            obj=self.data[name]
        #Still not found? Then create it attached to the Parameter Object
            if obj.notFound(): 
                pobj=self.getParametersObject() # getParameter object (and create if necessary)
                obj=pobj.newObject(name)
                obj.setNetLevel(100)
                obj.put_silent(defval)
        return obj
    def getResultObject(self,name,defval): 
#        obj=self.data[self.getResultsObjectName()+"'"+name] - doesn't work well, if for example a parameter object lower in the net has the same name ...
        #First search Object linked to Results object
#        if obj.notFound():
        #If not present search through the entire net
        obj=self.data[name]
        #Still not found? Then create it attached to the Results collector Object
        if obj.notFound(): 
            robj=self.getResultsObject() # get Results collector object (and create if necessary)
            obj=self.data.new(name)
            obj ^ (self.data,robj) # insert result object between this and the result collector object
            obj.setNetLevel(100)
            obj.put_silent(defval)
        return obj
    def getParametersObjectName(self): 
        return "'Parameters="+self.parnames[0]
    def getResultsObjectName(self): 
        return ":Results="+self.resnames[0]
    def getParametersObject(self): #and create it if necessary
        #Check for paramter object which belongs to this object and create it if necessary
        pobj=self.data[self.getParametersObjectName()]
        if pobj.notFound():
            pobj=self.data.newObject("'Parameters")
            pobj.setNetLevel(999)
            pobj.put_silent(self.parnames)
        return pobj
    def getResultsObject(self): #and create it if necessary
        #Check for results identifier object which belongs to this object and create it if necessary
        obj=self.data[self.getResultsObjectName()]
        if obj.notFound():
            obj=self.data.newObject("Results")
            obj.setNetLevel(999)
            obj.put_silent(self.resnames)
        return obj
    def getParameters(self):
        if len(self.pars)>0:
        #Check for paramter object which belongs to this object and create it if necessary
            for par in self.pars:
                if verbose:
                    print "Setting parameter",par.parname
                obj=self.getParameterObject(par.objname,par.defval)
                exec("self.obj."+par.parname+"=obj") 
        #Finally we are ready to set the local variable.
                if par.type=="Py":
                    i=obj.getI()
                    if (i==0) | (i==NONEPointer):
                        if verbose: 
                            exec("print 'getParameters("+self.data.getName(True)+"): self.'+par.parname+'=None'")
                        exec("self."+par.parname+"=0")
                        exec("self."+par.parname+"=None")
                    else: 
                        exec("self."+par.parname+"=obj.get"+par.type+"()") 
                        if verbose: 
                            exec("print 'getParameters("+self.data.getName(True)+"): self.'+par.parname+'=None'")
                elif par.type=="PyList":
                    if verbose: 
                        print "PyList:",par.parname
                    i=obj.getI()
                    if i==0 | i==NONEPointer:
                        exec("self."+par.parname+"=[None]")
                        if verbose: 
                            exec("print 'getParameters("+self.data.getName(True)+"): self.'+par.parname+'=[None]'")
                    else: 
                        exec("self."+par.parname+"=obj.get"+par.type+"()") 
                        if verbose: 
                            exec("print 'getParameters("+self.data.getName(True)+"): self.'+par.parname+'=obj.get'+par.type+'(): =',"+"self."+par.parname+", 'obj=',obj")
                else: 
                    exec("self."+par.parname+"=obj.get"+par.type+"()") 
                    if verbose: 
                        exec("print 'getParameters("+self.data.getName(True)+"): self.'+par.parname+'=obj.get'+par.type+'(): =',"+"self."+par.parname+", 'obj=',obj")
        return 0
    def setResults(self):
        if len(self.res)>0:
        #Check for paramter object which belongs to this object and create it if necessary
            for name in self.res:
                res=self.res[name]
                exec("self."+res.parname+"=res.defval")
                obj=self.getResultObject(res.objname,res.defval)
                exec("self.obj."+res.parname+"=obj") 
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
        elif defval==[None]:  self.type="PyList"
        elif type(defval)==type("0"):  self.type="S"
        elif type(defval)==list:
            if defval==[]: self.type="IL"
            elif type(defval[0])==type(0): self.type="IL"
            elif type(defval[0])==type(0.): self.type="NL"
            elif type(defval[0])==type(0j): self.type="CL"
            elif type(defval[0])==type("0"): self.type="SL"
            else: print "Error: funcparameters - unknown list type", defval
        elif type(defval)==IntVec: self.type="IL"
        elif type(defval)==FloatVec: self.type="NL"
        elif type(defval)==ComplexVec: self.type="CL"
        elif type(defval)==StringVec: self.type="SL"
        else: 
            print "Error: funcparameters - unknown type", defval
        return

class hfGraphObject(hffunc):
    "hfGraphObject: Creates a mgl graph object and stores it."
    def startup(self,d):
        self.setParameter("FontName","adventor")
#        self.setParameter("FontPath",os.environ.get('LOFARSOFT')+"/src/CR-Tools/implement/GUI/Fonts")
        self.setParameter("FontPath",os.environ.get('LOFARSOFT')+"/src/CR-Tools/implement/GUI/Fonts")
        d.setAutoUpdate(False)
        return 0
    def process(self,d):
        self.gr=mglGraph(mglGraphZB)
#        print" Load Font disabled in hffuncs.py - self.gr.LoadFont(self.FontName,self.FontPath)"
#        self.gr.LoadFont(self.FontName,self.FontPath)
        d.noMod()
        d.putPy(self.gr)
        return 0
    def cleanup(self,d):
        return 0

class hfGraphDataBuffer(hffunc):
    "hfGraphDataBuffer: Stores the data that is to be plotted in an mglData Object."
    def startup(self,d):
        if verbose: print "Startup hfGraphDataBuffer"
        self.setResult("xminval",-1)
        self.setResult("xmaxval",1)
        self.setResult("yminval",-1)
        self.setResult("ymaxval",1)
        self.setResult("zminval",-1)
        self.setResult("zmaxval",1)
        return 0
    def initialize(self,d):
        if verbose: print "Initialize hfGraphDataBuffer"
        self.yvec=FloatVec() #create local scratch vectors
        self.xvec=FloatVec()
        self.zvec=FloatVec()
        self.xdat=mglData() # create local mglData vectors - better create method which does that directly (getmglData)
        self.ydat=mglData()
        self.zdat=mglData()
    def process(self,d):
        if verbose: print "Process hfGraphDataBuffer user function"
        naxis=0;
        axis=self.data["'yAxis"]
        if not type(axis)==Data: 
            print "hfGraphDataBuffer: yAxis object not found."
            return 1
        naxis+=1
        axis.get(self.yvec)
        axis=self.data["'xAxis"]
        if type(axis)==Data:
            naxis+=1;
            axis.get(self.xvec) #Use Vector selector in the future ...!
            if len(self.xvec)>len(self.yvec): self.xvec=self.xvec[0:len(self.yvec)]
            if len(self.xvec)<len(self.yvec): self.yvec=self.yvec[0:len(self.xvec)]
            if verbose: print "self.xdat.nx=",self.xdat.nx," max=",self.xdat.Maximal()
            axis=self.data["'zAxis"]
            if type(axis)==Data:
                naxis+=1;
                axis.get(self.zvec)
                self.zdat.SetVec(self.zvec)
        if naxis==1: self.xvec.extend(range(len(self.yvec)))
        self.xdat.SetVec(self.xvec)
        self.ydat.SetVec(self.yvec)
        if verbose: print "naxis=",naxis
        if verbose: print "self.ydat.ny=",self.ydat.ny," max=",self.ydat.Maximal()
        self.putResult("xminval",min(self.xvec))
        self.putResult("xmaxval",max(self.xvec))
        self.putResult("yminval",min(self.yvec))
        self.putResult("ymaxval",max(self.yvec))
        self.xvec=self.xvec[0:0]
        self.yvec=self.yvec[0:0]
        self.zvec=self.zvec[0:0]
        if verbose: print "GraphDataBuffer: naxis=",naxis
        if naxis<=2: 
            self.data.putPyList_silent([self.xdat,self.ydat])
        if naxis==3: 
            self.data.putPyList_silent([self.xdat,self.ydat,self.zdat])
            self.putResult("zminval",self.zdat.Minimal())
            self.putResult("zmaxval",self.zdat.Maximal())
        return 0
    def cleanup(self,d):
        return 0

#self=d["Antenna=0:GraphDataBuffer"].PyFunc() 

class hfPlotData(hffunc):
    "hfPlotData: Plots a single data set in the plotpanel."
    def startup(self,d):
        if verbose: print "Startup hfPlotData"
        self.setParameter("Antenna",0)
        self.setParameter("AntennaID","")
        self.setParameter("Color","")
        self.setParameter("GraphObject",None)
        self.setParameter("GraphDataBuffer",[None])
        return 0
    def process(self,d):
        if verbose: print "Process hfPlotData user function"
        naxis=len(self.GraphDataBuffer);
        if verbose: print "self.GraphDataBuffer=",self.GraphDataBuffer
        if (self.GraphObject==None) | (self.GraphDataBuffer==[None]): return
        self.GraphObject.SetBaseLineWidth(1); 
        if self.Color=="":
            self.GraphObject.AddLegend(str(self.Antenna)+": "+str(self.AntennaID),self.GraphObject.currentcolor)
            if naxis==1: self.GraphObject.Plot(self.GraphDataBuffer[0],self.GraphObject.currentcolor) ## shouldn't happen
            if naxis==2: self.GraphObject.Plot(self.GraphDataBuffer[0],self.GraphDataBuffer[1],self.GraphObject.currentcolor)
            elif naxis==3: self.GraphObject.Plot(self.GraphDataBuffer[0],self.GraphDataBuffer[1],self.GraphDataBuffer[2],self.GraphObject.currentcolor)
        else:
            self.GraphObject.AddLegend(str(self.Antenna)+": "+str(self.AntennaID),self.Color)
            if naxis==1: self.GraphObject.Plot(self.GraphDataBuffer[0],self.Color) ## shouldn't happen
            if naxis==2: self.GraphObject.Plot(self.GraphDataBuffer[0],self.GraphDataBuffer[1],self.Color)
            elif naxis==3: self.GraphObject.Plot(self.GraphDataBuffer[0],self.GraphDataBuffer[1],self.GraphDataBuffer[2],self.Color)
        self.GraphObject.SetBaseLineWidth(1); 
        return 0
    def cleanup(self,d):
        return 0

#For debugging: type 
#self=d["Antenna=0:PlotData"].PyFunc() 
#to retrieve that function

class hfPlotPanel(hffunc):
    "hfPlotPanel: Plots a panel and plots all linked PlotData Objects into it."
    def startup(self,d):
        if verbose: print "PlotPanel - Startup"
        self.setParameter("GraphDataBuffer",None)
        self.setParameter("GraphObject",None)
        self.setParameter("xmin",0.)
        self.setParameter("xmax",1024.)
        self.setParameter("ymin",-70.)
        self.setParameter("ymax",70.)
        self.setParameter("yscale",0)
        self.setParameter("yscalestep",0.05)
        self.setParameter("xscale",0)
        self.setParameter("xscalestep",0.05)
        self.setParameter("xshift",0)
        self.setParameter("yshift",0)
        self.setParameter("OffsetValue",0.0)
        self.setParameter("XAuto",int(True))
        self.setParameter("YAuto",int(True))
        self.setParameter("logX",int(False))
        self.setParameter("logY",int(False))
        self.setParameter("yAxisLabel","")
        self.setParameter("xAxisLabel","")
        self.setParameter("xAxisUnit","",objname="xAxis'UnitName")
        self.setParameter("yAxisUnit","",objname="yAxis'UnitName")
        self.setParameter("xAxisUnitPrefix","",objname="xAxis'UnitPrefix")
        self.setParameter("yAxisUnitPrefix","",objname="yAxis'UnitPrefix")
        self.setParameter("xAxisDatatype","",objname="xAxis'Datatype")
        self.setParameter("yAxisDatatype","",objname="yAxis'Datatype")
        self.setParameter("ColorPalette","brcmyhlnqeupg")
        self.setResult("xmin",0.)
        self.setResult("xmax",1024.)
        self.setResult("ymin",-70.)
        self.setResult("ymax",70.)
        self.setResult("PanelPosition",0)
        if settrace: pdb.set_trace()
        return 0
    def process(self,d):
        if verbose: print "Processing hfPlotPanel."
        if not type(self.GraphObject)==mglGraph:
            print "Error: PlotPanel - GraphObject not of type mglGraph!"
            return 1
        gdbo=d["'Results=GraphDataBuffer"]
        xminval=min(toList(gdbo["'xminval"].val()))
        xmaxval=max(toList(gdbo["'xmaxval"].val()))
        yminval=min(toList(gdbo["'yminval"].val()))
        ymaxval=max(toList(gdbo["'ymaxval"].val()))
        self.selected=self.data.isNeighbour("SelectBoard")
        self.GraphObject.SetFontDef("rR:r")
        self.GraphObject.SetFontSize(9.)
        self.GraphObject.SetCut(False);
        self.GraphObject.SetBaseLineWidth(1); 
        self.GraphObject.SetTickLen(0.1,2);
        if self.XAuto:
            xmin=xminval
            xmax=xmaxval
            if verbose: print "PlotPanel: XAuto=True - xmin=",xmin,"xmax=",xmax
        else: 
            xmin=self.xmin
            xmax=self.xmax
        if self.logX: xmin,xmax=logaxisval(xmin,xmax)
        if (not ((self.xscale==0) & (self.xshift==0))):
            delta=(xmax-xmin)
            x0=xmin+delta/2+(xmaxval-xminval)/2.*(self.xshift/100.)
            delta=delta*(1.0+self.xscalestep)**-self.xscale
            xmin=x0-delta/2
            xmax=x0+delta/2
        if self.YAuto:
            ymin=yminval
            ymax=ymaxval
            if self.logY: ymin,ymax=logaxisval(ymin,ymax)
            if verbose: print "PlotPanel: YAuto=True - ymin=",ymin,"ymax=",ymax
            extra=(ymax-ymin)*0.1/2
        else: 
            ymin=self.ymin
            ymax=self.ymax
            if self.logY: ymin,ymax=logaxisval(ymin,ymax)
            extra=0
        if (not (self.yscale==0 & self.yshift==0)):
            delta=(ymax-ymin)
            y0=ymin+delta/2+(ymaxval-yminval)/2.*(self.yshift/100.)
            delta=delta*(1+self.yscalestep)**-self.yscale
            ymin=y0-delta/2
            ymax=y0+delta/2
        ymin=ymin-extra
        ymax=ymax+extra
        xfunc="";yfunc="";
        if self.logX: 
            xfunc="lg(x)"
            self.GraphObject.SetTicks('x',0)
            xmin=10**xmin;xmax=10**xmax;
            self.GraphObject.SetRanges(xmin,xmax,ymin,ymax)
        else: 
            self.GraphObject.SetRanges(xmin,xmax,ymin,ymax)
#            self.GraphObject.AdjustTicks('x')
        if self.logY: 
            yfunc="lg(y)"
            ymin=10**ymin;ymax=10**ymax;
            self.GraphObject.SetTicks('y',0)
            self.GraphObject.SetRanges(xmin,xmax,ymin,ymax)
        else: 
            self.GraphObject.SetRanges(xmin,xmax,ymin,ymax)
#            self.GraphObject.AdjustTicks("y")
        self.GraphObject.SetFunc(xfunc,yfunc,"");
        self.GraphObject.SetXTT("%g")
        self.GraphObject.SetYTT("%g")
        self.GraphObject.SetTuneTicks(True,-1.2)
        self.GraphObject.AdjustTicks('xy')
        self.GraphObject.Axis("xy")
        if self.selected: self.GraphObject.Box("r")
        else: self.GraphObject.Box("g")
        self.GraphObject.SetBaseLineWidth(1); 
        self.putResult("xmin",xmin)
        self.putResult("xmax",xmax)
        self.putResult("ymin",ymin)
        self.putResult("ymax",ymax)
        if settrace: pdb.set_trace()
        if self.OffsetValue==0: xoffsettxt=""
        elif self.OffsetValue>0: xoffsettxt=" + "+str(self.OffsetValue)
        else: xoffsettxt=" - "+str(abs(self.OffsetValue))
 #ugly fix - to display potential functions (like exp, log, sqrt,...) before the axis labels
        xl=d["'x:UnitData"].FirstObject().Chain(DIR.TO,["xAxis","maxBlock"],False,True).getName()
        yl=d["'y:UnitData"].FirstObject().Chain(DIR.TO,["yAxis","maxBlock"],False,True).getName()
        if type(xl)==str: xfunctxt=""
        else: xl.reverse(); xfunctxt=list2str(xl[:-1])+" "
        if type(yl)==str: yfunctxt=""
        else: yl.reverse(); yfunctxt=list2str(yl[:-1])+" "
        if self.xAxisLabel=="": self.xAxisLabel=xfunctxt+str(self.xAxisDatatype)
        if self.yAxisLabel=="": self.yAxisLabel=yfunctxt+str(self.yAxisDatatype)
        self.xAxisLabel+=xoffsettxt
        if not self.xAxisUnit=="": self.xAxisLabel=self.xAxisLabel+" ["+self.xAxisUnitPrefix+self.xAxisUnit+"]"
        if not self.yAxisUnit=="": self.yAxisLabel=self.yAxisLabel+" ["+self.yAxisUnitPrefix+self.yAxisUnit+"]"
        self.GraphObject.SetFontSize(7.)
        self.GraphObject.Label("x",self.xAxisLabel,1)
        self.GraphObject.Label("y",self.yAxisLabel,1)
        self.GraphObject.SetFontSize(5.)
        self.GraphObject.ClearLegend()
        if verbose: print "PlotPanel: plotted Box in Graphobject, calling PlotData"
        self.PlotData=self.data["'PlotData"]
        self.GraphObject.currentcolor=self.ColorPalette[0]
        if type(self.PlotData)==Data: 
            self.PlotData.update()
        elif type(self.PlotData)==DataList: 
            ci=0 # ColorIndex for Palette
            for dd in self.PlotData: 
                if verbose: print "PlotPanel calling ",dd.getName(True) 
                self.GraphObject.currentcolor=self.ColorPalette[ci]
                dd.update()
                ci=(ci+1) % len(self.ColorPalette)
        else:
            print "Error: PlotPanel - PlotData not found."
            return 1
        npanels=d["npanelsplotted"].val()
        self.GraphObject.Legend(3,"rL",-0.7*npanels**0.5)
        return 0
    def cleanup(self,d):
        return 0
#For debugging: type 
#self=d["Antenna=0:PlotPanel"].PyFunc() 
#to retrieve that function

class hfPlotWindow(hffunc):
    "hfPlotWindow: Plots multiple panels in one Window."
    def startup(self,d):
        if verbose: print "PlotWindow - Startup"
        self.setParameter("Title","")
        self.setParameter("Filename","")
        self.setParameter("BackgroundColor",[1.0,1.0,1.0])
        self.setParameter("npanels",-1)
        self.setParameter("npanelsx",-1)
        self.setParameter("npanelsy",-1)
        self.setParameter("ZoomFactor",1.45) # Make sure this is float()!
        self.setParameter("WindowShiftX",0.2) # Make sure this is float()!
        self.setParameter("WindowShiftY",-0.2) # Make sure this is float()!
        self.setParameter("PipelineLauncher",[""])
        self.setParameter("GraphObject",None)
        self.setParameter("SortPanelKey","'Antenna")
        self.setResult("npanelsplotted",0)
        self.setResult("npanelsplottedx",0)
        self.setResult("npanelsplottedy",0)
        return 0
    def process(self,d):
#        hfglobal.ncount+=1; print "PLOTWINDOW:", hfglobal.ncount, d.getModFlags()
        if verbose: print "PlotWindow - Process"
        if not type(self.GraphObject)==mglGraph:
            print "Error: PlotWindow - GraphObject not of type mglGraph!"
            return 1
        self.GraphObject.Clf(self.BackgroundColor[0],self.BackgroundColor[1],self.BackgroundColor[2])
        self.GraphObject.SetPlotFactor(self.ZoomFactor)
        if self.Title=="":  self.GraphObject.Title(self.Filename,"iC:k",-0.7)
        else: self.GraphObject.Title(self.Title,"iC:k",-0.8)
        self.PlotPanel=self.data["'PlotPanel"]
#Find out how many PlotPanels are in the network 
        if type(self.PlotPanel)==Data:  #One Object => just one panel
            npanels=1
            nx=1
            ny=1
            self.putResult("npanelsplotted",npanels)
            self.putResult("npanelsplottedx",nx)
            self.putResult("npanelsplottedy",ny)
            self.GraphObject.SubPlot(1,1,0,self.WindowShiftX,self.WindowShiftY)
            self.PlotPanel[":PanelPosition"].set_silent(0)
            self.PlotPanel.update()
        elif type(self.PlotPanel)==DataList: #A Datalist is returned => more than one panel
            PlotPanelList=self.PlotPanel.sort(self.SortPanelKey)
            npanels=len(PlotPanelList)
            if self.npanels>=0: npanels=min(self.npanels,npanels)
            if self.npanelsx<0: nx=int(ceil(sqrt(npanels)))
            else: nx=self.npanelsx
            if self.npanelsy<0: ny=int(ceil(1.0*npanels/nx))
            else: ny=self.npanelsy
            n=0
            self.putResult("npanelsplotted",npanels)
            self.putResult("npanelsplottedx",nx)
            self.putResult("npanelsplottedy",ny)
            if verbose: print "nx,ny=",nx,ny," No. of PlotPanel Objects=",len(PlotPanelList)
            for dd in PlotPanelList: 
                if n<npanels:
                    self.GraphObject.SubPlot(nx,ny,n,self.WindowShiftX,self.WindowShiftY)
#                    self.GraphObject.SetPlotFactor(self.ZoomFactor)
                    if verbose: print "Processing Plotpanel",dd.getName(True)
                    dd.update()
                    dd[":PanelPosition"].set_silent(n)
                n+=1
        else:
            print "Error: PlotWindow - PlotPanel not found."
            return 1
        self.data.noMod(); self.data.putPy(self.GraphObject)
        return 0
    def cleanup(self,d):
        return 0

class hfQtPanel(hffunc):
    "hfQtPanel: Start the GUI Window"
    def startup(self,d):
#        hfglobal.ncount+=1; print "QTPANEL", hfglobal.ncount, d.getModFlags()
        if verbose: print "QtPanel startup!"
        self.setParameter("Parent",None)
        self.setParameter("Title","HFLPLOT")
        self.setParameter("Width",600)
        self.setParameter("Height",400)
        self.setParameter("SelectBoard","")
        self.setParameter("PlotWidget",None)
        self.setParameter("PlotWindow",None)
        self.setParameter("GraphObject",None)
        return 0
    def initialize(self,d):
        if verbose: print "QtPanel - initialize."
        "Called at the end of the startup routine. Will create the PlotWidget and insert it into the PlotWidget Object."
        if self.PlotWidget == None:
            self.PlotWidget=hfgui.hfQtPlot()
#        d["'PlotWidget"].putPy_silent(self.PlotWidget)
#        d["'PlotWidget"].setNetLevel(999)
            self.obj.PlotWidget.putPy_silent(self.PlotWidget)
            self.obj.PlotWidget.setNetLevel(999)
            self.PlotWidget.setWindowTitle(self.Title)
            self.setSize(self.PlotWidget.width(),self.PlotWidget.height()-20)
            self.PlotWidget.setImage(QtGui.QImage())
        return 0
    def setSize(self,width,height):
        self.GraphObject.SetSize(width,height)
        self.PlotWidget.setGeometry(0,0,width,height)
    def process(self,d):
        if verbose: print "QtPanel process"
#        pwin=d["'PlotWindow"].getPy()
        self.PlotWidget.setGraph(self.PlotWindow)
        return 0

#f=d["QtPanel"].PyFunc()
#f.setSize(400,400)

class hfQtNetview(hffunc):
    "hfQtNetview: Start a QtSVGWidget which shows the current network of objects."
    def startup(self,d):
        if verbose: print "QtNetview startup!"
        self.setParameter("GUI",None)
        self.setParameter("SVGWidget",None)
        self.setParameter("maxNetLevel",99)
        return 0
    def initialize(self,d):
        "Called at the end of the startup routine. Will create the SVGWidget."
        if self.SVGWidget == None:
            if not self.GUI==None:
                if "networkdisplay" in dir(hfglobal.gui): 
                    self.SVGWidget = self.GUI.networkdisplay
            if self.SVGWidget == None:
                self.SVGWidget=QtSvg.QSvgWidget()
#            
            self.SVGWidget.setAutoFillBackground(True)
            pal = QtGui.QPalette(self.SVGWidget.palette()) 
            pal.setColor(QtGui.QPalette.Window, QtGui.QColor('white')) 
            self.SVGWidget.setPalette(pal) 
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

 
