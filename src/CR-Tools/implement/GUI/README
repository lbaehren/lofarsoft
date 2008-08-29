------------------------------------------------------------------------
                 README File for hfplot/hfget 
         - the (eventually) hyper-flexible plotting package -
------------------------------------------------------------------------

This are some scratch notes on my installation procedures for "hfplot"
- mainly for getting boost-python installed. At the end also a
description of the get mechanism.

------------------------------------------------------------------------

Installation of Qt and python was straight forward.


After compilation of the hfplot package with make (Makefile is now
created by qmake, which means that hfget.pro needs to be adapted to
suit the local system) one siimpy runs python with

python -i hfget.py

or

python
>>> execfile("hfget.py")



------------------------------------------------------------------------
Install boost python:
------------------------------------------------------------------------

install "python-dev" with package manager (goes into /usr/include/python2.4)

download bjam

make "bjam" from source not rpm or package manager (was a big problem otherwise): 

/usr/local/boost_1_35_0/tools/jam/src$ ./build.sh

downloaded boost 1.35

unpacked into directory/usr/local/boost_1_35_0

cd /usr/local/boost_1_35_0
./configure --prefix=/home/falcke/boost_1_35_0
make install

This installed the library into my home directory for linking.

Testing the installation with:
/usr/local/boost_1_35_0/libs/python/example/quickstart$ bjam --build-dir=/home/falcke/boost2 toolset=gcc --verbose-test test

One can actually then build boost python without bjam. I found out
which where the correct options and link paths to be set by using:

bjam -n --build-dir=/home/falcke/LOFAR/hfplot toolset=gcc 

This then showed something like the following, which I then edited
further by hand.

"g++"  -ftemplate-depth-128 -O0 -fno-inline -Wall -g -fPIC    -I"/usr/include/python2.4" -I"/usr/local/boost_1_35_0" -c -o "/home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug/hfget.o" "hfget.cc"

gcc.link.dll /home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug/hfget.so

    "g++"    -o "/home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug/hfget.so" -Wl,-h -Wl,hfget.so -shared -Wl,--start-group "/home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug/hfget.o" "/home/falcke/LOFAR/hfplot/boost/bin.v2/libs/python/build/gcc-4.1.2/debug/libboost_python-gcc41-d-1_35.so.1.35.0"  -Wl,-Bstatic  -Wl,-Bdynamic -lutil -lpthread -ldl -Wl,--end-group -g 

To actually link it with Qt, I used "qmake" as described in the Qt
book and later edited my own hfget.pro file (so, no need to run qmake
-project again now ...) using the paths that bjam showed me.


/usr/share/qt4/bin/qmake -project
/usr/share/qt4/bin/qmake hfget.pro 

and then edit hfget.pro 

------------------------------------------------------------------------
Note:
To compile the hfget routines with bjam only, I jused the following.

bjam --build-dir=/home/falcke/LOFAR/hfplot toolset=gcc

This will create a shared object library in a local subdirectory
(./boost/bin.v2/libs/python/build/gcc-4.1.2/debug), which contains all 
the functions python can call.

(but again, using bjam is no longer necessary if one as the the right qmake file, 
which generates a Makefile in turn, so that one can just use "make")
------------------------------------------------------------------------
Here is an example output for a compile run (not necessarily complete):

cd /home/falcke/LOFAR/hfplot/
make -k 
g++ -c -pipe -g -D_REENTRANT -w -fPIC  -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. -o VectorSelector.o VectorSelector.cc
g++ -c -pipe -g -D_REENTRANT -w -fPIC  -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. -o hffuncs.o hffuncs.cc
g++ -c -pipe -g -D_REENTRANT -w -fPIC  -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. -o hfget.o hfget.cc
g++ -c -pipe -g -D_REENTRANT -w -fPIC  -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. -o hfqt.o hfqt.cc
/usr/bin/moc-qt4 -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. hfqt.h -o moc_hfqt.cpp
g++ -c -pipe -g -D_REENTRANT -w -fPIC  -DQT_GUI_LIB -DQT_CORE_LIB -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/python2.4 -I/usr/local/boost_1_35_0 -I. -I. -o moc_hfqt.o moc_hfqt.cpp
rm -f libhfget.so.1.0.0 libhfget.so libhfget.so.1 libhfget.so.1.0
g++ -shared -Wl,-soname,libhfget.so.1 -o libhfget.so.1.0.0 cell.o finddialog.o gotocelldialog.o mainwindow.o sortdialog.o spreadsheet.o plotter.o VectorSelector.o hffuncs.o hfget.o hfqt.o moc_finddialog.o moc_gotocelldialog.o moc_mainwindow.o moc_sortdialog.o moc_spreadsheet.o moc_plotter.o moc_hfqt.o qrc_spreadsheet.o qrc_plotter.o   -L/usr/lib /home/falcke/LOFAR/hfplot/boost/bin.v2/libs/python/build/gcc-4.1.2/debug/libboost_python-gcc41-d-1_35.so.1.35.0 /usr/local/boost_1_35_0/bin.v2/libs/thread/build/gcc-4.1.2/release/threading-multi/libboost_thread-gcc41-mt-1_35.so.1.35.0 -lpthread -lQtGui -lQtCore 
ln -s libhfget.so.1.0.0 libhfget.so
ln -s libhfget.so.1.0.0 libhfget.so.1
ln -s libhfget.so.1.0.0 libhfget.so.1.0

Compilation finished at Thu Jul 10 17:17:10

------------------------------------------------------------------------
However, before running python properly a few paths have to be set so that python 
finds the shared object libraries from which it imports the new hfplot package. The easiest 
is actually to copy the .so file into your local directory. Otherwise I did

export LD_LIBRARY_PATH=/usr/bin:/usr/lib:/usr/lib32:/usr/lib64:/home/falcke/boost_1_35_0/lib:/home/falcke/LOFAR/hfplot/boost/bin.v2/libs/python/build/gcc-4.1.2/debug:/home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug
export PYTHONPATH=/usr/lib/python2.4:/home/falcke/LOFAR/hfplot/boost/bin.v2/libs/python/build/gcc-4.1.2/debug:/home/falcke/LOFAR/hfplot/boost-hfplot/gcc-4.1.2/debug


(try out what is really necessary here)

Then run 

python 

and then some random input in python:

from libhfget import *

d=Data("LOFAR")
d.printStatus(1)
v=d.newObjects("Header")
d2=d.ObjectID(v[0])
v=d.newObjects("Header:X")
len(v)
d.printDecendants("")
DIR.TO+1

v2=IntVec()
v[:]=[0,1,2,3,4,5,6,7]
v[3:4]=[8,9]

------------------------------------------------------------------------

Copy of a short description for the c++ Data object class (for how to
use python look at hfget.py):

//========================================================================
//class Data 
//========================================================================
/*

This is the basic data object. It contains pointers to a data vector
and a function, as well as links to other objects.

The basic method to retrieve data are get and put. Every object is
dynamically typed, so can change its type during execution. The
storage type is determined by "put", the output type is determined by
"get" (i.e. autmatically converted).

Every data is a vector and right now we only allow the follwing types:
Pointer, Integer, Number (i.e. floats), Complex, and String. Complex
data structures (i.e. "structs" or records) have to be built up by a
network of object! Single parameters are stored as vectors of length
one. 

In principle n-dimensional data cubes should also be supported by
specifiying dimension information - still the data would be stored as
an STL vector. However, high-dimensionality is not yet implemented.

It is crucial for the logic to work that every data in the entire
system is stored in the objects!


Objects could alse be assigned a function instead or in addition to
the data vector. This function is executed whenever the data in the
object is retrieved by get (and if a data vector is present, the
function is executed only if the data actually needs to be
recalculated).

This is done using 

data.setFunction(Name, Type, Library). (not sure, if type is really needed ...)

So, data["Data:Squared"].setFunction("Square","Sys") will assign a
squaring function to the object "Squared", which will then give the
square of the elements in "Data".

New objects are created and linked by newObjects...

Deletion and changing of links is not yet fully supported. 

A new link is established with "setLink".

The links of an object in the network can have two directions: To and
From (an object). Also, each To and From can have either a To or From
direction for signalling that a change of data has occurred! (this can
be confusing, but allows one to have "push" and "pull" dynamic actions
in the net.

Every object can be used as a starting object to search for other
objects in the network, relative to the current object.

e.g. data["root:name"] searches for the next object in the "from"
direction with name "name" and retrieves that data object. (This does
not necessarily mean that name is directly attached to "root")

Hence, data["root:name"].get(vec) actually retrieves not the data
object data but the one with name "name" that is attached to "root"
being attached to data. Most of the objects don't have a c++ variable
name (like data) - typically only the first one. The others have just
their links stored in the network (and in a master list, called the
"superior")

data["root:name'directory"] actually steps first forward to root, then
to name, and then BACKWARD to "directory". So, name actually is
derived from "directory" in this example.

It is possible that objects can have the same name. If they are on
different levels, then one will simply shadow the other, unless
explicitly specified. So,

data["root:name:name"] is possible, but data["name"] would only access
the first object.

If objects on the same level have the same name, then either all
objects are returned using the method data.Find("Station") - which
returns a list of pointers. Or just the first one, when using
data["Station"]. However, in this case it is also possible to retrieve
a specific object based on its contents by using the "name=value"
syntax.

Imagine you have a structure like this, where the name of the object
is shown and in brackets its contents.


          Telescope("LOFAR")
           /     \
    Station(1)  Station(2)
          |      |
  Data(1,2,3...) Data(7,6,3,3...)


Here 

- data.get(string_vector) would retrieve a vector the name "LOFAR"
(data["Telescope"].get would yield the same).

- data["Station:Data"].get(integer_vector) or data["Data"].get(..)  (!)
would both retrieve (1,2,3...), while 

- data["Station=2:Data"].get(...) would yield 7,6,3,3

BTW, it is also possible to specify a list of objects. For example 

- data.Find("Station=1,2,4:Data") would yield the objects where 
the Station number is 1,2, or 4, but not 3


Later one could implement more complex logic, eg. "(Station>2&&Station<10):Data" ...


Things still to be done (very incomplete ....):

- add unit support - this could, however, be done by special objects 
attached to each real data vector containing the unit name (e.g. "m"), 
scale letter (e.g. "k" for km) and a scale factor (here 1000). In fact per
unit there needs to be only on single unit object that all data objects point 
to if needed.

- do more operator overloading, such that data["Station=1:Data"]=vector, 
replaces data["Station=1:Data"].put(vector).

- use operators like -> or so for setting Links (also in Python).

- There is also a class Vector_Selector which is intended to allow you retrieving
sub-parts of the data vector (e.g. only every 2nd value, or just values 
above a certain threshold ...). That needs a bit more work still ...

*/


