#        pdb.set_trace()

def UnitChooser(self):
    return MakeChooser(self,"Unit",("UnitPrefix","UnitScaleFactor"),(("",1.0),("f",10.**-15),("p",10.**-12),("n",10.**-9),("{\\mu}",10.**-6),("m",10.**-3),("c",10.**-2),("d",10.**-1),("h",10.**2),("k",10.**3),("M",10.**6),("G",10.**9),("T",10.**12),("P",10.**15),("E",10.**18),("Z",10.**21)))

def LOPESDatatypeChooser(self):
    return MakeChooser(self,"Datatype",("Datatype","UnitName","Axis"),(("Time","s","x"),("Frequency","Hz","x"),("Fx","Counts","y"),("Voltage","V","y"),("CalFFT","","y"),("invFFT","","y")))

def CRFile(self,filename=""):
    if filename=="": 
        lofarMainDir = os.environ.get('LOFARSOFT')
#        filename = lofarMainDir + '/data/lopes/2007.01.31.23:59:33.960.event'
        filename = lofarMainDir + '/data/lopes/example.event'
    fobj=(self >> _d("Filename",filename,_l(90)) >> _d("Parameters","File",_l(999)) >> _d("File",_f("dataReaderObject","CR"))).update()
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
    
settrace=False

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
    dp=self.find_or_make("DataPipeline","",_l(90))
    reference=dp.find_or_make("'Reference",_l(9))
    dp.find_or_make("'Parameters=Data",_l(999)).find_or_make("'Blocksize",defaultblocksize,_l(90))
    aobjects=(dp.find_or_make(AntennaObjectNames,_l(9)).find_or_make("Antenna")).setList(AntennaSelection)
    xaxis=aobjects.find_or_make("x").find_or_make("Data",_f("dataRead")).find_or_make("UnitData").find_or_make("xAxis")
    yaxis=aobjects.find_or_make("y").find_or_make("Data",_f("dataRead")).find_or_make("UnitData").find_or_make("yAxis")
    reference.resetLink(aobjects.FirstObject()["y:Data"])
    xaxis=self["Antenna"]["xAxis"]
    yaxis=self["Antenna"]["yAxis"]
    datpars=self["Antenna"]["'Parameters=Data"]
    datpars //self["DataPipeline"] # separate for a moment, so that the new ParameterObjects can be created ....
    datatypechooser.resetLink(DataUnion(xaxis["'Data"]).find_or_make("'Parameters=Data",_l(999)),DIR.NONE,DIR.TO)
    datatypechooser.resetLink(DataUnion(yaxis["'Data"]).find_or_make("'Parameters=Data",_l(999)),DIR.NONE,DIR.TO)
    datpars >> self["DataPipeline"] # reconnect the more global ParameterObject
    unitchooser.resetLink(DataUnion(xaxis["'UnitData"]).find_or_make("'Parameters=UnitData",_l(999)),DIR.NONE,DIR.TO)
    unitchooser.resetLink(DataUnion(yaxis["'UnitData"]).find_or_make("'Parameters=UnitData",_l(999)),DIR.NONE,DIR.TO)
    xaxis["'UnitData"].setFunc_f_silent(_f("Unit")) 
    yaxis["'UnitData"].setFunc_f_silent(_f("Unit"))
    return [AntennaObjectNames,unwantedAntennaNames,newAntennaNames]

def PlotDataPipeline(self):
    if settrace: pdb.set_trace()
    #The following is a go example on intricacies of network
    #programming. It is important to use double subscripts in the
    #following, since the data pipelines before yAxis could have
    #different lengths for different antennas. If that is so only one
    #object (the closest) will be returned. By first searching for
    #"Antenna" we start already with a DataList and then for each
    #antenna pipeline yAxis is found independently
    gdb=self["Antenna"]["yAxis"].find_or_make("GraphDataBuffer",_f(hfGraphDataBuffer))
    self["Antenna"]["xAxis"].resetLink(gdb,DIR.NONE,DIR.TO)
    x=DataUnion(gdb["Results=GraphDataBuffer"])
    x=x.find_or_make("'Replot",DIR.NONE,DIR.TO)
    x=x.find_or_make("'GraphObject",_f(hfGraphObject),_l(1000))
    DataUnion(gdb["Results=GraphDataBuffer"]).find_or_make("'Parameters=PlotPanel",_l(999))
    gdb["Results=GraphDataBuffer"].find_or_make("PlotData",_f(hfPlotData)).find_or_make("PlotPanel",_f(hfPlotPanel))
    (DataUnion(gdb["Results=PlotPanel"]).find_or_make(":PlotWindow",_f(hfPlotWindow))).find_or_make("QtPanel",_f(hfQtPanel)) 

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
        return 0
    def process(self,d):
        AntennaObjectNames,unwantedAntennaNames,newAntennaNames=CRDataPipeline(d)
#        if settrace: pdb.set_trace()
        PlotDataPipeline(d)
        if len(unwantedAntennaNames)>0: map(lambda x:CRDelAntennaPipeline(d,x),unwantedAntennaNames)
        d.set_silent(AntennaObjectNames)
        d.All().clearModification()
        if len(newAntennaNames)!=0: map(lambda x:x.touch(True),d[newAntennaNames])
        if (len(unwantedAntennaNames)!=0): 
            dgb=d["GraphDataBuffer"]
            dgb.Silent(True)
            dgb.touch().Silent(False)
        d["QtPanel"].update()
        return 0

