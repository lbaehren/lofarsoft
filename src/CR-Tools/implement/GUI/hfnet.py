
vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
vi=IntVec()
d=Data("LOPES")
#Setting the Filename

d >> _d("Filetype","LOPESEvent",_l(2)) \
  >> _d("Filename","/Users/falcke/LOFAR/usg/data/lopes/2007.01.31.23:59:33.960.event",_l(2)) \
  >> _d("FileObject",_f("dataReaderObject","CR")) \
  >> "File"

#Provide general input parameters for the data file, which are supposed to affect all antennas
[_d("Block",1,_l(90)),_d("Blocksize",1024,_l(90))] >> (("Parameters","Data",_l(90)) >> d["File"])

#Create a global Parameter object for the PlotPanel objects
(("Parameters","PlotPanel",_l(90)) >> d["File"])

#Create several Antennas on the same level and add a Data object as placeholder
d["File"] >> NewObjectRange("Antenna",2) >> ("Data",_f("dataRead","CR",TYPE.COMPLEX))


datatypechooser=LOPESDatatypeChooser(d)

datatypechooser >> (("Parameters","Data",_l(90)) >> DataUnion(d["Data"]))

d["Data'Parameters=Data'Datatype=Fx"] >> d["Data'Parameters=Data"]

("Color","b",_l(90)) >>(("Parameters","PlotData",_l(90)) >> d["File"])

("GraphObject",_f(hfGraphObject),_l(1000)) >>  DataUnion(d["Antenna:Data"])

d["Data"]>> ("UnitData",_f("Unit")) >> "yAxis"  >> ("PlotData",_f(hfPlotData)) >>  ("PlotPanel",_f(hfPlotPanel))


#The following can be used to create your own x-axis
#d["Antenna"] >> ("Data",_f("Range"))>> ("UnitData",_f("Unit")) >> "xAxis" >> d["PlotData"]
#d["xAxis'end"]=512

DataUnion(d["PlotPanel"]) >>  ("PlotWindow",_f(hfPlotWindow))

#verbose=True

d["PlotWindow"].update()
#again a manual update to bring the network into a clean state.
d["PlotWindow"]  >> ("QtPanel",_f(hfQtPanel)) 


unitchooser=UnitChooser(d) # A subnet of objects which provides the different options for choosing unit scales from

#To use the designer hfQtPlotWidget needs to be explicitly created here
#Probably we can also do this as a function ...
hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"])
hfm=hfMainWindow(hfQtPlotWidget)
hfm.raise_() # put it on top of all other windows
hfm.show()  # and make it visible
gui=hfm.ui # define an object which provides easy access to the GUI objects

("GUI",gui) >> d["QtPanel"] # and store it, so that the objects have access to the GUI. 

d["QtPanel"] >> ("QtNetview",_f(hfQtNetview),_l(999)) #Here this is used by the Network Display object


verbose=False
d.touch()

##Now setting up some connections to the GUI ... will be done in the code, since we need to do the object selection.
d["PlotWindow'npanels"].connect(gui.npanels)
d["PlotWindow'npanelsx"].connect(gui.npanelsx)
d["PlotWindow'npanelsy"].connect(gui.npanelsy)
d["QtNetview'maxNetLevel"].connect(gui.netlevel,QtCore.SLOT("setEditText(QString)"))

d["PlotPanel'xmin"][0].connect(gui.xmin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'ymin"][0].connect(gui.ymin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'xmax"][0].connect(gui.xmax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'ymax"][0].connect(gui.ymax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!

initializePyQtObjects(d)#This will send a PyQt signal from all objects to the GUI, so that the parameters are set in the GUI
hfqtplot=d["QtPanel'PlotWidget"].getPy()
hfqtplot.datatypechooser()



"""
d["Antenna=1:PlotPanel"] // d["Antenna=1:PlotPanel'Parameters"] >> d["File"]
~d["Antenna:PlotPanel'Parameters"]

d["QtNetview'maxNetLevel"]=999

d["Data'Parameters'Datatype"]="CalFFT"
d["Data'Parameters'Datatype"]="Fx"
d["File'Parameters'end"]
d["File'Parameters=Data'end"]=1023
d["xAxis"] // d["PlotData"]

Unit

-> Unit, "Jy","erg/sec/cm^2/Hz","Watt/m^2/Hz"

-> Scale, "n","mu","m","c","","d","h","k","M","G","T"
-> ScaleFactor (1e-9,1e-6,1e-3,1e-2,1,10,100,1000,1e6,1e9,1e12)

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
newobj=d.create("TEST")  # creates a new object
~newobj #deletes it right away
newobj.initializeObject("NAME",val,_f(FUNC),_l(netlevel)) # initializes it
newobj ^ (d,d[1])  # inserts a new object between two other objects
_n("TEST",1) ^ (d[0],d[1])   #inserts a new object with initialization paramters given in _n
d // d["TEST"]  ## separates a link between two objects

x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel
x=d["QtPanel"].retrievePyFuncObject() #Retrieves the function interfacing with the QtPanel

d["Antenna=0:PlotWindow"].update()
d["Antenna=0:QtPanel"].update()


"""
