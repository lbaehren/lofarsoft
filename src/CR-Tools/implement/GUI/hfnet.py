#pdb.set_trace()
execfile("hfimport.py")

hfQtPlotWidget=0

from hfinit import *
from hfpywrappers import *
from hffuncs import *
from hfgui import *
#from hfcrfuncs import *


defaultblocksize=2**10
print "--------------------------------------------"
print "              CR-TOOLS hGUI"
print "--------------------------------------------"

import pdb
vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
vi=IntVec()

d=Data("ROOT")
unitchooser=UnitChooser(d) 
datatypechooser=LOPESDatatypeChooser(d)
CRFile(d) >> ("PipelineLauncher",x_f(CRPipelineLauncher))


#To use the designer hfQtPlotWidget needs to be explicitly created here
#Probably we can also do this as a function ...


selectboard=d["QtPanel'SelectBoard"]
selectboard.setFunc("Neighbours","Sys",TYPE.STRING)

d >> d["QtNetview"] # to not loose it, when the above connection is cut

d.All().clearModification() # to avoid re-initializing the DataReaderObject

##Now setting up some connections to the GUI ... will be done in the code, since we need to do the object selection.
ConnectGUIButtons(d)
d["QtNetview'maxNetLevel"].connect(hfglobal.gui.netlevel,"setEditText")


#QtCore.QObject.connect(gui.loadfile,QtCore.SIGNAL("triggered()"),gui.HMainPlotter.hfLoad)
connect_action("loadfile","hfLoad")
connect_action("actionDelete","hfDelete")
connect_action("actionSplit_Panels","hfSplitPanels")
connect_action("actionSelect_All","hfSelectAll")
connect_action("actionDeselect_All","hfDeselectAll")


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

#Attention: For perfomance reasons the Network Display is toggled off!"
d["QtPanel"] // d["QtNetview"]


#d["Antenna=0:y:Data"] >> "Reference" >> d["DataPipeline"]

##########End ....
#
#d["*AntennaSelection"]=[0]; d.new("Offset",x_f("Offset")) ^ (d["x:UnitData"],d["x:xAxis"]); ~(d["PlotPanel'OffsetValue"]); d["Data'Block"]=1

#x_d("abs",x_f("abs")) ^ (d["y:UnitData"],d["yAxis"])

#pdb.set_trace()

"""

A few tricks to help debugging:
-------------------------------

The GUI itself is accessed through the object "gui" (use dir(gui)) to see all the elements of the gui)
To get access to the slots/functions in hfplot the GUI is connected to type  dir(guifuncs)
To access the mglGraph object use 'gr'.
To debug while a network operation is performed an trace it through the net type:
x=d.All().storePyDBG(dbg).setVerbose(9999).setDebug(True) or debugon(d)
or if you want to use the GUI while debugging:
x=d.All().storePyDBG(dbg).setVerbose(9999).setDebugGUI(True) or debugguion(d)
you can use maxnetlevel=NNN to set the level of detail during the debug phase.  



Language Examples:
------------------

d["Antenna=1:PlotPanel"] // d["Antenna=1:PlotPanel'Parameters"] >> d["File"]
~d["Antenna:PlotPanel'Parameters"]

d["QtNetview'maxNetLevel"]=999

d["Data'Parameters'Datatype"]="CalFFT"
d["Data'Parameters'Datatype"]="Fx"
d["File'Parameters'end"]
d["File'Parameters=Data'end"]=1023
d["xAxis"] // d["PlotData"]


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
newobj.initializeObject("NAME",val,x_f(FUNC),x_l(netlevel)) # initializes it
newobj ^ (d,d[1])  # inserts a new object between two other objects
d.new("TEST",1) ^ (d[0],d[1])   #inserts a new object with initialization paramters 
d // d["TEST"]  ## separates a link between two objects

x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel
x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel

d["Antenna=0:PlotWindow"].update()
d["Antenna=0:QtPanel"].update()


"""
