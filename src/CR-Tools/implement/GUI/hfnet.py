
vf=FloatVec()
d=Data("LOPES")
#Setting the Filename

d >> _d("Filetype","LOPESEvent",_l(2)) \
  >> _d("Filename","/Users/falcke/LOFAR/usg/data/lopes/2007.01.31.23:59:33.960.event",_l(2)) \
  >> _d("FileObject",_f("dataReaderObject","CR")) \
  >> "File"
#Provide general input parameters for the data file, which are supposed to affect all antennas
[_d("Block",1,_l(100)),_d("Blocksize",1024,_l(100))] >> d["File"]

#Create several Antennas on the same level and add a Data object as placeholder
d["File"] >> [("Antenna",0),("Antenna",1)] >> ("Data",_f("dataRead","CR",TYPE.NUMBER))
("Datatype","Fx",_l(100)) >> DataUnion(d["Data"])

("Color","b") >> d["File"]

("GraphObject",_f(hfGraphObject),_l(1000)) >>  DataUnion(d["Antenna:Data"])
d["Data"] >> "yAxis" >> ("PlotData",_f(hfPlotData)) >>  ("PlotPanel",_f(hfPlotPanel))


d["Antenna"] >> ("Data",_f("Range")) >> "xAxis" >> d["PlotData"]

d["xAxis'end"]=1023

DataUnion(d["PlotPanel"]) >>  ("PlotWindow",_f(hfPlotWindow))

#verbose=True

d["PlotWindow"].update()
#again a manual update to bring the network into a clean state.
d["PlotWindow"]  >> ("QtPanel",_f(hfQtPanel)) >> ("QtNetview",_f(hfQtNetview),_l(999))



hfQtPlotWidget=hfQtPlotConstructor(d["QtPanel"])
mf=MyForm()
mf.raise_()
mf.show()

verbose=False
d.touch()

"""

############
d["QtPanel"].update()
d["QtNetview"].update()

d["QtNetview'maxNetLevel"]=999

d["Antenna=1:PlotData"] >> d["Antenna=0:PlotPanel"]

################

#For Playing with the mathGL object manually use
g=d["PlotWindow"].getPy() ## Needs to be done only once
g.Clf()  ## example modification
d["QtPanel"].update() # update screen

#To reset just use
d["PlotWindow'GraphObject"].touch()


###############
d.create("TEST")  # creates a new object
d.initializeObject("NAME",val,_f(FUNC),_l(netlevel)) # initializes it
d[89] ^ (d,d[1])  # inserts a new object between two other objects
_n("TEST3",1) ^ (d[0],d[1])   #inserts a new object with initialization paramters given in _n
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
