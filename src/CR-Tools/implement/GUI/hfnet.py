
number_of_antennas=4

print "Using",number_of_antennas," antennas only!"
print " "
print "If you want to see more antennas, you need to change the variable number_of_antennas"
print "in the file hfnet.py for now!"
print " " 

vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
vi=IntVec()
d=Data("ROOT")
#Setting the Filename

datatypechooser=LOPESDatatypeChooser(d)
unitchooser=UnitChooser(d) # A subnet of objects which provides the different options for choosing unit scales from

lofarMainDir = os.environ.get('LOFARSOFT')


#hardcodedFileName = lofarMainDir + '/data/lofar/rw_20080701_162002_0109.h5' 
#d >> _d("Filetype","LOFAR_TBB",_l(2)) \
hardcodedFileName = lofarMainDir + '/data/lopes/2007.01.31.23:59:33.960.event'
d >> _d("Filetype","LOPESEvent",_l(2)) \
  >> _d("Filename",hardcodedFileName,_l(2)) \
  >> _d("FileObject",_f("dataReaderObject","CR")) \
  >> "File"

#Provide general input parameters for the data file, which are supposed to affect all antennas
[_d("Block",0,_l(90)),_d("Blocksize",1024,_l(90)),_d("maxBlocksize",65536,_l(90)),_d("maxBlock",63,_l(90)),_d("Stride",0,_l(90)),_d("Shift",0,_l(90))] >> (("Parameters","Data",_l(90)) >> d["File"])



#Create a global Parameter object for the PlotPanel objects
(("Parameters","PlotPanel",_l(999)) >> d["File"])

#Create several Antennas on the same level and add a Data object as placeholder
d["File"] >> NewObjectRange("Antenna",number_of_antennas) >> ("Data",_f("dataRead","CR",TYPE.COMPLEX)) >>  ("UnitData",_f("Unit")) >> "yAxis"

d["Antenna"] >>  ("Data",_f("dataRead","CR",TYPE.NUMBER)) >>  ("UnitData",_f("Unit")) >> "xAxis"

unitchooser >>  ("Parameters","UnitData",_l(999)) >> d["xAxis'UnitData"]
unitchooser >>  ("Parameters","UnitData",_l(999)) >> d["yAxis'UnitData"]


(d["xAxis'Parameters=UnitData'UnitPrefix=m"] >> d["xAxis'Parameters=UnitData"]) #.touch()
(d["yAxis'Parameters=UnitData'UnitPrefix="] >> d["yAxis'Parameters=UnitData"]) #.touch()

datatypechooser >> ("Parameters","Data",_l(999)) >> d["yAxis'Data"]
datatypechooser >>  ("Parameters","Data",_l(999)) >> d["xAxis'Data"]

(d["Data'Parameters=Data'Datatype=Fx"] >> d["Data:yAxis'Parameters=Data"]).touch()
(d["Data'Parameters=Data'Datatype=Time"] >> d["Data:xAxis'Parameters=Data"]).touch()

("Color","b",_l(90)) >>(("Parameters","PlotData",_l(999)) >> d["File"])


("GraphObject",_f(hfGraphObject),_l(1000)) >>  DataUnion(d["Antenna:Data"])



#d["Data"]>> ("UnitData",_f("Unit")) >> "yAxis" >> ("GraphDataBuffer",_f(hfGraphDataBuffer))  
d["yAxis"] >> ("GraphDataBuffer",_f(hfGraphDataBuffer))  
d["xAxis"] >> d["GraphDataBuffer"]

#The Results object is created during the initialization of
#GraphDataBuffer, this is not immediately obvious and maybe one can
#find a more transparent solution...


d["Results=GraphDataBuffer"] >> ("PlotData",_f(hfPlotData)) >>  ("PlotPanel",_f(hfPlotPanel))

#verbose=True
#d.AllVerbose(True)
d["GraphDataBuffer"].update() # This is necessary and I don't really understand why ....

#The following can be used to create your own x-axis
#d["Antenna"] >> ("Data",_f("Range"))>> ("UnitData",_f("Unit")) >> "xAxis" >> d["PlotData"]
#d["xAxis'end"]=512


DataUnion(d["PlotPanel"]) >>  ("PlotWindow",_f(hfPlotWindow))

#verbose=True
d["PlotWindow"].update()
#verbose=False

#again a manual update to bring the network into a clean state.
d["PlotWindow"]  >> ("QtPanel",_f(hfQtPanel)) 


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

verbose=False
d.touch()

##Now setting up some connections to the GUI ... will be done in the code, since we need to do the object selection.
d["PlotWindow'npanels"].connect(gui.npanels)
d["PlotWindow'npanelsx"].connect(gui.npanelsx)
d["PlotWindow'npanelsy"].connect(gui.npanelsy)

print d["QtNetview'maxNetLevel"]
d["QtNetview'maxNetLevel"].connect(gui.netlevel,"setEditText")

d["PlotPanel:xmin"].connect(gui.xmin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel:ymin"].connect(gui.ymin)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel:xmax"].connect(gui.xmax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!
d["PlotPanel:ymax"].connect(gui.ymax)  ##OK the [0] shouldn't be necessary, but somehow the search does not work!

d["PlotPanel'YAuto"].connect(gui.yauto,"setChecked","bool")
d["PlotPanel'XAuto"].connect(gui.xauto,"setChecked","bool")

d["File'Block"].connect(gui.blocknumber,"setValue","int")
d["File'Blocksize"].connect(gui.blocksize,"setValue","int")
d["Data'maxBlock"].connect(gui.blocknumber,"setMaximum","int",isSlot=False)
d["Data'maxBlock"].connect(gui.blocknumberslider,"setMaximum","int",isSlot=False)
d["Data'maxBlocksize"].connect(gui.blocksize,"setMaximum","int",isSlot=False)
d["Data'maxBlocksize"].connect(gui.blocksizeslider,"setMaximum","int",isSlot=False)

d["PlotPanel'xscale"][0].connect(gui.xzoomdial,"setValue","int")
d["PlotPanel'yscale"][0].connect(gui.yzoomdial,"setValue","int")
d["PlotPanel'xshift"][0].connect(gui.xshiftslider,"setValue","int")
d["PlotPanel'yshift"][0].connect(gui.yshiftslider,"setValue","int")

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

##At the end clear all modification flags that might have been left hanging during teh build-up phase 
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
print "x=d.All().storePyDBG(dbg).setVerbose(9999).setDebug(True) od debugon(d)"
print "or if you want to use the GUI while debugging:"
print "x=d.All().storePyDBG(dbg).setVerbose(9999).setDebugGUI(True) or debugguion(d)"
print "you can use maxnetlevel=NNN to set the level of detail during the debug phase."  

##########End ....

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
