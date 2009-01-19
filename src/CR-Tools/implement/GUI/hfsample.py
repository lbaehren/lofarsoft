


#y.Set(vf)


#python -i hfget.py
#or in python
#execfile("hfget.py")

from libhfget import *
d=Data("LOPES")
d.newObjects("dataReaderObject",DIR.FROM)
d["dataReaderObject"].newObjects("dataRead",DIR.FROM)
d["dataReaderObject"].setFunction("dataReaderObject",TYPE.INTEGER,"CR")
d["dataRead"].newObjects("'AntennaNumber",DIR.FROM)
d["dataRead'AntennaNumber"]=1
d["dataRead"].setFunction("dataRead",TYPE.NUMBER,"CR")
vf=FloatVec()
d["dataRead"].get(vf); vf[100000]


from libhfget import *
d=Data("LOPES")
d.newObjects("dataReaderObject",DIR.FROM)
d["dataReaderObject"].setFunction("dataReaderObject",TYPE.INTEGER,"CR")
d["dataReaderObject"].newObjects("dataRead",DIR.FROM)
d["dataRead"].newObjects("'AntennaNumber",DIR.FROM)
d["dataRead'AntennaNumber"]=1
d["dataRead"].setFunction("dataRead",TYPE.NUMBER,"CR")
vf=FloatVec()
d["dataReaderObject"].getI()
d["dataRead"].get(vf)


#Next we add a print object to X. Again with TO direction
#hence the vector will be automatically printed when 
#the Range parameter objects are changed

d.newObjects("Data:X:Print",DIR.TO)
d["Data:X:Print"].setFunction("Print",TYPE.INTEGER)

#Let's also add a squaring operation
d.newObjects("Data:X:Square",DIR.TO)
d["Data:X:Square"].setFunction("Square",TYPE.INTEGER)


#Now we change the parameter objects of Range and something should
#happen in the output...

d["X'start"].putI(1)


#Here we define some Python objects that emulate c++ STL vectors 
#Rather use standard python lists here eventually?


vi=IntVec()
vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
vi[0:10]=[7,6,5,4,3,2,1,2,3,4,5]


#Next we demonstrate operator overloading with "=" (for put)
d.newObjects("Data:Z",DIR.FROM)
d["Z"]=vi
d["Z"].printStatus();
d["Z"].newObjects("Print",DIR.TO)
d["Data:Z:Print"].setFunction("Print",TYPE.INTEGER)
#This will set the data container of the object to an integer vector


#This doesn't work yet well for single values, (d["Z"]=1 instead of
#d["Z"].putI(1)) since Integer values will unfortunately be converted
#to Number (actually Complex) automatically.  This probably has to do
#with the automatic casting that boost-python.  I suspect that can be
#overwritten, but haven't found yet how.


#d["Z"]="Hallo"
#d["Z"].printStatus();
#should work though.

#Retrieve the data into a vector 
d["Z"].get(vs)

#and converting to a python list, gives some readable output
list(vs)


#Now we print out a quick summary of the network - currently only runs
#in one direction - we don't see start & end
d.printDecendants()

#Ahh, here they are
d["X"].printStatus()

#A more extended summary of the contents of X is here
d["X"].printStatus(0)


#Now let's create an object with a pointer to a mainwindow, which is
#also initialized

d.newObjects("Mainwindow",DIR.FROM)
d["Mainwindow"].newObjects("'Name",DIR.FROM)

#We can give the window a name (not yet implemented..)
d["Mainwindow'Name"]="Mein Fenster"

#Add the mainwindow function to the plotting window, will launch a (dummy) GUI
d["Mainwindow"].setFunction("Mainwindow",TYPE.INTEGER,"Qt")

print "Mainwindow Pointer:"
print d["Mainwindow"].getI()

#Connect a data vector to the Qt window through a qprint object
vi=d.newObjects("X:Square:QPrint",DIR.TO)
d["QPrint"].setLink(d.ID("Mainwindow"),DIR.FROM) 
d["QPrint"].setFunction("QPrint",TYPE.INTEGER,"Qt")
d["QPrint"].printStatus()

#Note that the next command comes too fast for the other thread, so type it manually again ...
#There is not yet a waiting loop implemented, that waits untile the window is launched.
print '------------------------------------------------------------------------'
print "Now type the following command"
print ' '
print 'd["QPrint"].get(vs)'
print ' '
print "The title of the main window should now display the contents of Object X"
print ' '
print "Next type ..." 
print ' '
print 'd["X'+"'"+'inc"].putI(4)' 
print ' '
print "... which will change the increment value of the Range object X."
print 'This will then immediately propagate to "Square" and then into the Mainwindow Title.'
print 'Since the "Print" object is attached to "X" this content will'
print 'be printed on the terminal.'
