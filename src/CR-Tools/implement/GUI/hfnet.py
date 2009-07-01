#pdb.set_trace()

import pdb
vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
vi=IntVec()

d=Data("ROOT")
unitchooser=UnitChooser(d) 
datatypechooser=LOPESDatatypeChooser(d)
CRFile(d) >> ("PipelineLauncher",_f(CRPipelineLauncher))


#To use the designer hfQtPlotWidget needs to be explicitly created here
#Probably we can also do this as a function ...
hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"])
hfm=hfMainWindow(hfQtPlotWidget)
hfm.raise_() # put it on top of all other windows
hfm.show()  # and make it visible
gui=hfm.ui # define an object which provides easy access to the GUI objects

("GUI",gui) >> d["QtPanel"] # and store it, so that the objects have access to the GUI. 

d["QtPanel"]  >> ("QtNetview",_f(hfQtNetview),_l(100)) #Here this is used by the Network Display object

d >> d["QtNetview"] # to not loose it, when the above connection is cut

d.All().clearModification() # to avoid re-initializing the DataReaderObject

##Now setting up some connections to the GUI ... will be done in the code, since we need to do the object selection.
d["PlotWindow'npanels"].connect(gui.npanels)
d["PlotWindow'npanelsx"].connect(gui.npanelsx)
d["PlotWindow'npanelsy"].connect(gui.npanelsy)

#print d["QtNetview'maxNetLevel"]

d["QtNetview'maxNetLevel"].connect(gui.netlevel,"setEditText")

d["PlotPanel:xmin"].connect(gui.xmin)  
d["PlotPanel:ymin"].connect(gui.ymin)  
d["PlotPanel:xmax"].connect(gui.xmax)  
d["PlotPanel:ymax"].connect(gui.ymax)  

d["*LastAntenna"].connect(gui.lastantenna)  #The * in front makes a global search for this name
d["*FirstAntenna"].connect(gui.firstantenna)  
d["*AntennaSelection"].connect(gui.antennaselection,"setText","QString")  
print "Antenna selection connected"

d["PlotPanel'YAuto"].connect(gui.yauto,"setChecked","bool")
d["PlotPanel'XAuto"].connect(gui.xauto,"setChecked","bool")
d["PlotPanel'logX"].connect(gui.logx,"setChecked","bool")
d["PlotPanel'logY"].connect(gui.logy,"setChecked","bool")

d["DataPipeline'Block"].connect(gui.blocknumber,"setValue","int")
#d["GlobalParameters'Blocksize"].connect(gui.blocksize,"setValue","int")
d["Data:maxBlock"].FirstObject().connect(gui.blocknumber,"setMaximum","int",isSlot=False)
d["Data:maxBlock"].FirstObject().connect(gui.blocknumberslider,"setMaximum","int",isSlot=False)
d["Data'maxBlocksize"].connect(gui.blocksize,"setMaximum","int",isSlot=False)
d["Data'maxBlocksize"].connect(gui.blocksizeslider,"setMaximum","int",isSlot=False)

d["PlotPanel'xscale"].connect(gui.xzoomdial,"setValue","int")
d["PlotPanel'yscale"].connect(gui.yzoomdial,"setValue","int")
d["PlotPanel'xshift"].connect(gui.xshiftslider,"setValue","int")
d["PlotPanel'yshift"].connect(gui.yshiftslider,"setValue","int")

#QtCore.QObject.connect(gui.loadfile,QtCore.SIGNAL("triggered()"),gui.HMainPlotter.hfLoad)
connect_action("loadfile","hfLoad")



initializePyQtObjects(d)#This will send a PyQt signal from all objects to the GUI, so that the parameters are set in the GUI

#This sets some aliases for frequently used GUI functions
hfqtplot=d["QtPanel'PlotWidget"].getPy()
guifuncs=d["QtPanel'PlotWidget"].getPy()
gr=d["PlotWindow'GraphObject"].getPy()
qsvg=d["QtNetview'SVGWidget"].getPy()

d.globals["hfqtplot"]=d["QtPanel'PlotWidget"].getPy()
d.globals["guifuncs"]=d["QtPanel'PlotWidget"].getPy()
d.globals["gr"]=d["PlotWindow'GraphObject"].getPy()
d.globals["qsvg"]=d["QtNetview'SVGWidget"].getPy()

hfqtplot.initializechooser()

##At the end clear all modification flags that might have been left hanging during the build-up phase 
d.All().clearModification()

print "Attention: For perfomance reasons the Network Display is toggled off!"
#print 'Use: d["QtPanel"] // d["QtNetview"] or toggle field in the GUI (bottom left)'
d["QtPanel"] // d["QtNetview"]


print '-------------------------------------'
print 'A few tricks to help debugging:'
print 'The GUI itself is accessed through the object "gui" (use dir(gui)) to see all the elements of the gui)'
print 'To get access to the slots/functions in hfplot the GUI is connected to type  dir(guifuncs)'
print "To access the mglGraph object use 'gr'."
print "To debug while a network operation is performed an trace it through the net type:"
print "x=d.All().storePyDBG(dbg).setVerbose(9999).setDebug(True) or debugon(d)"
print "or if you want to use the GUI while debugging:"
print "x=d.All().storePyDBG(dbg).setVerbose(9999).setDebugGUI(True) or debugguion(d)"
print "you can use maxnetlevel=NNN to set the level of detail during the debug phase."  

##########End ....
#
#d.new("Offset",_f("Offset")) ^ (d["x:Data"],d["x:UnitData"])

#max_number_of_antennas=2
#("MaxNumberOfAntennas",max_number_of_antennas) >> d["PipelineLauncher"]

#>> ("DataPipeline","",_l(90))

#CRDataPipeline(datapipeline)

#Choose one initial Unit
#(d["xAxis'Parameters=UnitData'UnitPrefix=m"] >> d["xAxis'Parameters=UnitData"])
#(d["yAxis'Parameters=UnitData'UnitPrefix="] >> d["yAxis'Parameters=UnitData"])
#(d["Data'Parameters=Data'Datatype=Fx"] >> d["Data:yAxis'Parameters=Data"])
#(d["Data'Parameters=Data'Datatype=Time"] >> d["Data:xAxis'Parameters=Data"])

#plotwindow=PlotDataPipeline(d)

#d["PlotWindow"]  >> ("QtPanel",_f(hfQtPanel)) 

#number_of_antennas=d["Results=File'nofAntennas"].val()
#print "Number of antennas in file =",number_of_antennas," antennas!"
#number_of_antennas=min(number_of_antennas,max_number_of_antennas)
#print "Using only",number_of_antennas,"Antennas!"
#print "If you want to see more antennas, you need to change the variable max_number_of_antennas"
#print "in the file hfnet.py for now!"

#d["PlotPanel'Parameters=PlotPanel"].move(d["PlotPanel'GraphObject"],d["PlotPanel"])

#The following can be used to create your own x-axis
#d["Antenna"] >> ("Data",_f("Range"))>> ("UnitData",_f("Unit")) >> "xAxis" >> d["PlotData"]
#d["xAxis'end"]=512


#DataUnion(d["PlotPanel"]) >>  ("PlotWindow",_f(hfPlotWindow))
#pdb.set_trace()

"""





d["Antenna=1:PlotPanel"] // d["Antenna=1:PlotPanel'Parameters"] >> d["File"]
~d["Antenna:PlotPanel'Parameters"]

d["QtNetview'maxNetLevel"]=999

d["Data'Parameters'Datatype"]="CalFFT"
d["Data'Parameters'Datatype"]="Fx"
d["File'Parameters'end"]
d["File'Parameters=Data'end"]=1023
d["xAxis"] // d["PlotData"]


############
d["QtPanel"].update()
d["QtNetview"].update()


d["Antenna=1:PlotData"] >> d["Antenna=0:PlotPanel"]

################

#For Playing with the mathGL object manually use
g=d["PlotWindow"].getPy() ## Needs to be done only once
g.Clf()  ## example modification
d["QtPanel"].update() # update screen

#To reset just use
d["PlotWindow'GraphObject"].touch()


###############
d["Antenna=0,1,3"] # returns three objects belonging to antenna 0,1, and 3.
newobj=d.create("TEST")  # creates a new object
~newobj #deletes it right away
newobj.initializeObject("NAME",val,_f(FUNC),_l(netlevel)) # initializes it
newobj ^ (d,d[1])  # inserts a new object between two other objects
d.new("TEST",1) ^ (d[0],d[1])   #inserts a new object with initialization paramters 
d // d["TEST"]  ## separates a link between two objects

x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel
x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel

d["Antenna=0:PlotWindow"].update()
d["Antenna=0:QtPanel"].update()


"""
