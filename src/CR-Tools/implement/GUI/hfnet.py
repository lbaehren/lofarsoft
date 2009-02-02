
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
[_d("Block",1,_l(100)),_d("Blocksize",1024,_l(100))] >> (("Parameters","Data",_l(100)) >> d["File"])

#Create a global Parameter object for the PlotPanel objects
(("Parameters","PlotPanel",_l(100)) >> d["File"])

#Create several Antennas on the same level and add a Data object as placeholder
d["File"] >> NewObjectRange("Antenna",2) >> ("Data",_f("dataRead","CR",TYPE.COMPLEX))

("Datatype","CalFFT",_l(100)) >> (("Parameters","Data",_l(100)) >> DataUnion(d["Data"]))

("Color","b",_l(100)) >>(("Parameters","PlotData",_l(100)) >> d["File"])

("GraphObject",_f(hfGraphObject),_l(1000)) >>  DataUnion(d["Antenna:Data"])

d["Data"]>> ("UnitData",_f("Unit")) >> "yAxis"  >> ("PlotData",_f(hfPlotData)) >>  ("PlotPanel",_f(hfPlotPanel))


d["Antenna"] >> ("Data",_f("Range"))>> ("UnitData",_f("Unit")) >> "xAxis" >> d["PlotData"]

d["xAxis'end"]=512

DataUnion(d["PlotPanel"]) >>  ("PlotWindow",_f(hfPlotWindow))

#verbose=True

d["PlotWindow"].update()
#again a manual update to bring the network into a clean state.
d["PlotWindow"]  >> ("QtPanel",_f(hfQtPanel)) >> ("QtNetview",_f(hfQtNetview),_l(999))


unitchooser=UnitChooser(d)

#To use the designer hfQtPlotWidget needs to be explicitly created here
hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"])
hfm=hfMainWindow(hfQtPlotWidget)
hfm.raise_()
hfm.show()
gui=hfm.ui

verbose=False
d.touch()

##Now setting up some connections to the GUI ... will be done in the code, since we need to do the object selection.
d["PlotWindow'npanels"].connect(gui.npanels)
d["PlotWindow'npanelsx"].connect(gui.npanelsx)
d["PlotWindow'npanelsy"].connect(gui.npanelsy)

d["PlotPanel'xmin"][0].connect(gui.xmin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'ymin"][0].connect(gui.ymin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'xmax"][0].connect(gui.xmax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel'ymax"][0].connect(gui.ymax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!

initializePyQtObjects(d)




"""
d["Antenna=1:PlotPanel"] // d["Antenna=1:PlotPanel'Parameters"] >> d["File"]
~d["Antenna:PlotPanel'Parameters"]

d["QtNetview'maxNetLevel"]=999


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


d["Antenna=0:PlotWindow"].update()
d["Antenna=0:QtPanel"].update()


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

"""
