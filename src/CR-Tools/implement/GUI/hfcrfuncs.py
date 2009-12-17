#        pdb.set_trace()
#This File is so incredibly ugly .... (hf)
execfile("hfimport.py")
import hfglobal
from hffuncs import *
from hfinit import *
from hfpywrappers import *
from hffuncs import *

defaultblocksize=2**10

def UnitChooser(self):
    return MakeChooser(self,"Unit",("UnitPrefix","UnitScaleFactor"),(("",1.0),("f",10.**-15),("p",10.**-12),("n",10.**-9),("{\\mu}",10.**-6),("m",10.**-3),("c",10.**-2),("d",10.**-1),("h",10.**2),("k",10.**3),("M",10.**6),("G",10.**9),("T",10.**12),("P",10.**15),("E",10.**18),("Z",10.**21)))

def LOPESDatatypeChooser(self):
    return MakeChooser(self,"Datatype",("Datatype","UnitName","Axis"),(("Time","s","x"),("Frequency","Hz","x"),("Fx","Counts","y"),("Voltage","V","y"),("FFT","","y"),("invFFT","","y")))

def CRFile(self,filename=""):
    if filename=="": 
        lofarMainDir = os.environ.get('LOFARSOFT')
        filename = lofarMainDir + '/data/lopes/example.event'
    fobj=(self >> x_d("Filename",filename,x_l(90)) >> x_d("Parameters","File",x_l(999)) >> x_d("File",x_f("dataReaderObject","CR"))).update()
    return fobj["Results"]

def CRAntennaPipelineChain(self,antennaID):
    "Returns the data and plotting pipeline associated with the specified antenna ID."
    if settrace: pdb.set_trace()
    ao=self[antennaID]
    if ao.Found():
        objs=ao.FirstObject().Chain(DIR.TO,["PlotWindow","Chooser","Replot","Datatype","UnitName","UnitPrefix","DataPipeline","Parameters=Data","Parameters=UnitData","Parameters=PlotPanel","Reference","SelectBoard"],False)
        return objs
    else: return DataList()

def CRDelAntennaPipeline(self,antennaID):
    "Deletes the data and plotting pipeline associated with the specified antenna ID."
    objs=CRAntennaPipelineChain(self,antennaID)
    if objs.Found(): ~objs
    
def CRDataPipeline(self):
    nofantennas=self["'nofAntennas"].val() # The total number of Antennas in the File
    AllAntennaIDs=self["'AntennaIDs"].getSL() # All antenna IDs in the file
    AntennaNumbers=range(len(AllAntennaIDs)) # The index of the antenna in the file ranging from 0 to nofAntennas-1
    AllAntennaObjectNames=map(lambda x:"AntennaID="+str(x),AllAntennaIDs) # The object names of the antenna chains to be searched for or created
    FirstAntenna=max(self["'FirstAntenna"].getI(),0) # The smallest antenna number to start with
    LastAntenna=min(self["'LastAntenna"].getI(),nofantennas-1) # The largest antenna number to end with
    if LastAntenna<0: LastAntenna=nofantennas-1 # -1 will default to all antennas
    AntennaSelection=make_unique(self["'AntennaSelection"].getIL()) # The Antenna selection array.
    if len(AntennaSelection)==0: AntennaSelection=range(FirstAntenna, LastAntenna+1) # If no selection list is given, then take all antennas up to thex maximum number requested
    else: AntennaSelection=filter(lambda n:(n<=LastAntenna)&(n>=FirstAntenna),AntennaSelection)
    if len(AntennaSelection)==0: 
        hfERR("No Antenna selected. Check First and LastAntenna.")
        AntennaSelection=[0]
    AntennaIDs=select(AllAntennaIDs,AntennaSelection)
    AntennaObjectNames=select(AllAntennaObjectNames,AntennaSelection)
    existingAntennaNames=set(self["AntennaID"].asList().getSearchName())
    desiredAntennaNames=set(AntennaObjectNames)
    unwantedAntennaNames=list(existingAntennaNames - desiredAntennaNames)
    newAntennaNames=list(desiredAntennaNames-existingAntennaNames)
    newAntennaNames.sort()
    existingAntennaNames=list(existingAntennaNames)
    if settrace: pdb.set_trace()
    dp=self.find_or_make("DataPipeline","",x_l(90))
    reference=dp.find_or_make("'Reference",x_l(9))
    dp.find_or_make("'Parameters=Data",x_l(999)).find_or_make("'Blocksize",defaultblocksize,x_l(90))
    aobjects=(dp.find_or_make(AntennaObjectNames,x_l(9)).find_or_make("Antenna")).setList(AntennaSelection)
    xaxis=aobjects.find_or_make("x").find_or_make("Data",x_f("dataRead")).find_or_make("UnitData").find_or_make("xAxis")
    yaxis=aobjects.find_or_make("y").find_or_make("Data",x_f("dataRead")).find_or_make("UnitData").find_or_make("yAxis")
    reference.resetLink(aobjects.FirstObject()["y:Data"])
    xaxis=self["Antenna"]["xAxis"]
    yaxis=self["Antenna"]["yAxis"]
    datpars=self["Antenna"]["'Parameters=Data"]
    datpars //self["DataPipeline"] # separate for a moment, so that the new ParameterObjects can be created ....
    datatypechooser.resetLink(DataUnion(xaxis["'Data"]).find_or_make("'Parameters=Data",x_l(999)),DIR.NONE,DIR.TO)
    datatypechooser.resetLink(DataUnion(yaxis["'Data"]).find_or_make("'Parameters=Data",x_l(999)),DIR.NONE,DIR.TO)
    datpars >> self["DataPipeline"] # reconnect the more global ParameterObject
    unitchooser.resetLink(DataUnion(xaxis["'UnitData"]).find_or_make("'Parameters=UnitData",x_l(999)),DIR.NONE,DIR.TO)
    unitchooser.resetLink(DataUnion(yaxis["'UnitData"]).find_or_make("'Parameters=UnitData",x_l(999)),DIR.NONE,DIR.TO)
    xaxis["'UnitData"].setFunc_f_silent(x_f("Unit")) 
    yaxis["'UnitData"].setFunc_f_silent(x_f("Unit"))
    return [AntennaObjectNames,unwantedAntennaNames,newAntennaNames]

def PlotDataPipeline(self):
    if settrace: pdb.set_trace()
    #The following is a good example on intricacies of network
    #programming. It is important to use double subscripts in the
    #following, since the data pipelines before yAxis could have
    #different lengths for different antennas. If that is so only one
    #object (the closest) will be returned. By first searching for
    #"Antenna" we start already with a DataList and then for each
    #antenna pipeline yAxis is found independently
    gdb=self["Antenna"]["yAxis"].find_or_make("GraphDataBuffer",x_f(hfGraphDataBuffer))
    self["Antenna"]["xAxis"].resetLink(gdb,DIR.NONE,DIR.TO)
    x=DataUnion(gdb["Results=GraphDataBuffer"])
    x=x.find_or_make("'Replot",DIR.NONE,DIR.TO)
    x=x.find_or_make("'GraphObject",x_f(hfGraphObject),x_l(1000))
    DataUnion(gdb["Results=GraphDataBuffer"]).find_or_make("'Parameters=PlotPanel",x_l(999))
    gdb["Results=GraphDataBuffer"].find_or_make("PlotData",x_f(hfPlotData)).find_or_make("PlotPanel",x_f(hfPlotPanel))
    (DataUnion(gdb["Results=PlotPanel"]).find_or_make(":PlotWindow",x_f(hfPlotWindow))).find_or_make("QtPanel",x_f(hfQtPanel)) 
#    ("Parameters=PlotPanel",x_l(999)) >> DataUnion(gdb["Results=GraphDataBuffer"])
#    gdb["Results=GraphDataBuffer"] >> x_q("PlotData",x_f(hfPlotData)) >> x_q("PlotPanel",x_f(hfPlotPanel))
#    DataUnion(gdb["Results=PlotPanel"]) >> x_q(":PlotWindow",x_f(hfPlotWindow)) >> x_q("QtPanel",x_f(hfQtPanel)) 
    self.All().clearModification()

def ConnectGUIButtons(d):
    d["PlotWindow'npanels"].connect(hfglobal.gui.npanels)
    d["PlotWindow'npanelsx"].connect(hfglobal.gui.npanelsx)
    d["PlotWindow'npanelsy"].connect(hfglobal.gui.npanelsy)
#
    d["PlotPanel:xmin"].connect(hfglobal.gui.xmin)  
    d["PlotPanel:ymin"].connect(hfglobal.gui.ymin)  
    d["PlotPanel:xmax"].connect(hfglobal.gui.xmax)  
    d["PlotPanel:ymax"].connect(hfglobal.gui.ymax)  
#
    d["*LastAntenna"].connect(hfglobal.gui.lastantenna)  #The * in front makes a global search for this name
    d["*FirstAntenna"].connect(hfglobal.gui.firstantenna)  
    d["*AntennaSelection"].connect(hfglobal.gui.antennaselection,"setText","QString")  
#
    d["PlotPanel'YAuto"].connect(hfglobal.gui.yauto,"setChecked","bool")
    d["PlotPanel'XAuto"].connect(hfglobal.gui.xauto,"setChecked","bool")
    d["PlotPanel'logX"].connect(hfglobal.gui.logx,"setChecked","bool")
    d["PlotPanel'logY"].connect(hfglobal.gui.logy,"setChecked","bool")
#
    d["DataPipeline'Block"].connect(hfglobal.gui.blocknumber,"setValue","int")
    d["DataPipeline'Blocksize"].connect(hfglobal.gui.blocksize)
    d["Data:maxBlock"].FirstObject().connect(hfglobal.gui.blocknumber,"setMaximum","int",isSlot=False)
    d["Data:maxBlock"].FirstObject().connect(hfglobal.gui.blocknumberslider,"setMaximum","int",isSlot=False)
#    d["Data'maxBlocksize"].connect(gui.blocksize,"setMaximum","int",isSlot=False)
#    d["Data'maxBlocksize"].connect(gui.blocksizeslider,"setMaximum","int",isSlot=False)
#
    d["PlotPanel'xscale"].connect(hfglobal.gui.xzoomdial,"setValue","int")
    d["PlotPanel'yscale"].connect(hfglobal.gui.yzoomdial,"setValue","int")
    d["PlotPanel'xshift"].connect(hfglobal.gui.xshiftslider,"setValue","int")
    d["PlotPanel'yshift"].connect(hfglobal.gui.yshiftslider,"setValue","int")


class CRPipelineLauncher(hffunc):
    "CRPipelineLauncher: Start or update the CR Pipeline"
    def startup(self,d):
        if verbose: print "CRPipelineLauncher!"
        self.setParameter("FirstAntenna",0)
        self.setParameter("LastAntenna",-1)
        self.setParameter("AntennaSelection",IntVec())
        self.setParameter("File",0)
        return 0
    def initialize(self,d):
        "Called at the end of the startup routine. Will setup th pipeline for the first time"
        result=CRDataPipeline(d)
#Choose one initial Unit
        (d["xAxis'Parameters=UnitData'UnitPrefix=m"] >> d["xAxis'Parameters=UnitData"])
        (d["yAxis'Parameters=UnitData'UnitPrefix="] >> d["yAxis'Parameters=UnitData"])
        (d["Data'Parameters=Data'Datatype=Fx"] >> d["Data:yAxis'Parameters=Data"])
        (d["Data'Parameters=Data'Datatype=Time"] >> d["Data:xAxis'Parameters=Data"])
        PlotDataPipeline(d)
        d.noMod()
        d.set(result[0])
        hfglobal.hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"])
        hfglobal.hfm=hfMainWindow(hfglobal.hfQtPlotWidget)
        hfglobal.hfm.raise_() # put it on top of all other windows
        hfglobal.hfm.show()  # and make it visible
        hfglobal.gui=hfglobal.hfm.ui # define an object which provides easy access to the GUI objects
        hfglobal.qtgui=d["QtPanel'PlotWidget"].getPy() # The object that contains the user GUI functions 
        ("GUI",hfglobal.gui) >> d["QtPanel"] # and store it, so that the objects have access to the GUI. 
        d["QtPanel"]  >> ("QtNetview",x_f(hfQtNetview),x_l(100)) #Here this is used by the Network Display object
        #Creating some Validators for inputfields
        qvblocksize=QtGui.QIntValidator(hfglobal.gui.blocksize)
        qvblocksize.setRange(0,d["Data'maxBlocksize"].getI())
        hfglobal.gui.blocksize.setValidator(qvblocksize)
        return 0
    def process(self,d):
        AntennaObjectNames,unwantedAntennaNames,newAntennaNames=CRDataPipeline(d)
#        if settrace: pdb.set_trace()
        PlotDataPipeline(d)
        if len(unwantedAntennaNames)>0: map(lambda x:CRDelAntennaPipeline(d,x),unwantedAntennaNames)
        d.set_silent(AntennaObjectNames)
        d.All().clearModification()
        map(lambda x:x.touch(True),d[AntennaObjectNames])
        if (len(unwantedAntennaNames)!=0): 
            dgb=d["GraphDataBuffer"]
            dgb.Silent(True)
            dgb.touch().Silent(False)
        d["QtPanel"].update()
        ConnectGUIButtons(d)
        return 0
