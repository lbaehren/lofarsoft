/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Heino Falcke (H.Facke@astro.ru.nl)                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef HFDATA_H
#define HFDATA_H

//#include <QtCore/QMetaObject>
//#include <QtGui/QLabel>

#define MAGICCODE 260966
#define MAGICCODE_DELETED 180455
#define MAXNETLEVEL 999999
#define SaveCall( REF ) if (!isDataObject( REF )) {ERROR(getName(true) << ": SaveCall - Data Object reference invalid. Network corrupt.");} else 
#define SaveCall2( REF ) if (!isDataObject( REF )) {ERROR("SaveCall - Data Object reference invalid. Network corrupt.");} else 



/* Namespace usage */
using namespace std;

/* Class forward declarations */
class DataFuncDescriptor;
class ObjectFunctionClass;
class DataFuncLibraryClass; 

//========================================================================
//  DataList - A vector of Data object(s) (pointers in c++) 
//========================================================================

typedef vector<Data*> DataList;
//class DataList : vector<Data*> {}

//vector<address> DataListtoIDs(/*const*/ DataList dl);
vector<address> DataListtoIDs(DataList dl);

/*!
  \class Data 
  
  \ingroup CR_GUI
  
  \brief Basic data object for the get/put mechanism
  
  \author Heino Falcke 

  <h3>Synopsis</h3>

  This is the basic data object. It contains pointers to a data vector
  and a function, as well as links to other objects.
  
  The basic method to retrieve data are get and put. Every object is
  dynamically typed, so can change its type during execution. The
  storage type is determined by "put", the output type is determined by
  "get" (i.e. autmatically converted).
  
  Every data is a vector and right now we only allow the follwing types:
  - HPointer,
  - HInteger,
  - HNumber (i.e. floats),
  - HComplex, 
  - HString.
  HComplex data structures (i.e. "structs" or records) have to be built up by a
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
  \code
  data.setFunction(Name, Type, Library)
  \endcode
 (not sure, if type is really needed ...)
  
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

  \verbatim
          Telescope("LOFAR")
           /     \
    Station(1)  Station(2)
          |      |
  Data(1,2,3...) Data(7,6,3,3...)
  \endverbatim

  Here 
  
  - <tt>data.get(string_vector)</tt> would retrieve a vector the name "LOFAR"
  (<tt>data["Telescope"].get</tt> would yield the same).  
  - data["Station:Data"].get(integer_vector) or data["Data"].get(..)  (!)
  would both retrieve (1,2,3...), while 
  - data["Station=2:Data"].get(...) would yield 7,6,3,3
  
  BTW, it is also possible to specify a list of objects. For example 
  
  - data.Find("Station=1,2,4:Data") would yield the objects where 
  the Station number is 1,2, or 4, but not 3
  
  
  Later one could implement more complex logic, eg. "(Station>2&&Station<10):Data" ...

  <h4>Note on Updating and Constructing a Network</h4>

When building a PUSH network it is important to note that a network
is updated proceeding from the top/end backwards to the root. So,
first an update message is send forward propagating through the
network. This is done via an "update worm" which crawls from the root
of a tree to its top, reaching every branch. While doing so it will
remember all objects (leaves) that ened updating. At the end the worm
will trigger the topmost object to update first! This will then call
objects down the tree as parameters. If those have been recognized as
being in need for update, they will do so and in turn call objects
further down the tree. When the update is complete teh worm will
proceed to the next object further down. If that has been updated due
to the previous update action, it will be skipped and the next object
in the worm will be looked at until the entire worm is empty
again. So, if an object down the tree is never called by any object
higher up. It may be updated only AFTER a top-level object has been
executed. It will be updated though, but the effects will only become
visible the next time the top level object is executed!

So, to make sure the data down the network is up-to-date, the objects
down there need to be explicitly called by an object higher up the
chain. If updating does not happen immediately, make sure the object
in question is called, e.g. as a dummy parameter, somewhere in the
network.

Example: the PipelineLauncher object is called by PlotWindow in the CR
network, since otherwise the number of plotpanels is not updated
immediately. Its output is not needed in the PlotWindow function,
however, its effect on the net is.

I am not yet sure, whether that is a bug or a feature. Since the user
really has to ensure self-consistency of the network him/herself when
it comes to updating. The alternative would be to have all objects
call all dependable objects down the net, but that would strike me as
inefficient. So, this way you have a bit more control (and chances for
errors...)


*/


typedef vector<Data*> dataworm;

bool isDataObject(/*const*/ Data* obj, /*const*/ bool notquiet=true);

template <class T, class S>
void copycast_vec(void *ptr, vector<S> *op);

template <class T, class S>
void copycast_vec(void *ptr, vector<S> *op, Vector_Selector *vs);


enum MOD_ACTION {MOD_CLEARED, MOD_VISITED, MOD_UPDATED,  MOD_LINKCHANGED, MOD_DELETED, MOD_UNDEF};

struct modification_record {
    MOD_ACTION action;
    Data* ref;
    longint version;
  };
  
typedef modification_record visit_marker;

bool ModRecisValid(/*const*/ modification_record mod);

class Data {
  
 public:
  
  int magiccode;

  struct superior_container {
    /*! HPointer to the root Data object in a chain of object */
    Data * root;
    /*! The number of Data objects attached */
    objectid no_of_data_objects;
    /*! Vector of pointers to the attached Data objects */
    DataList data_objects;
    DataFuncLibraryClass *library_ptr;
  };
  
  /*!
    \brief Description of the reference 
  */
  struct reference_descr {
    //! Name of the object linked to
    HString name;
    //! ID of the object
    objectid oid;
    //! HPointer the object links to
    Data* ref;
    //! Stores the input/channel/port the link is established to
    objectid port;
    DIRECTION direction;
    //! Keep track of whether an object was modified - 0 if not modified
    modification_record mod;
    //! Stores the type of object the reference is pointing to
    DATATYPE type;
  };
  
  //Defines freely definably  properties that can be assoicated with the object
  //e.g. whether this is plotable data, or in Frequency or Time Domain, 
  //or whether that is by default an X, or Y-axis value, you name it ...
  
  /*  struct properties_descr {
      HString name;
      HString value;
      };*/
  
  /*Struct containing Information necessary to describe a local parameter, 
    that is connected to another object*/
  
  /*!
    Defines a unit that goes with the data and also a prefix "milli, mikro,
    nano, etc." and scalefactor
  */
  struct unit_descr {
    /*! The unit of the quantity */
    HString unit;
    /*! The scale factor */
    HNumber scalefactor;
    /*! Prefix of the unit */
    HString prefix;
  };
  
  struct data_field {
    
    /*! Name identifying the data field */
    HString name;
    
    /*! ID of the object */
    objectid oid;
    
    REFTYPE origin;
    /*! Specifies the maximum memory size to be resereved for the data */
    address buflen;
    //!specifies the actual length of the data, not really used at the moment (see below)
    address len;
    /*! Not yet implemented, to be used for arrays */
    vector<address> dimensions;
    /*! This object links TO that one or derives FROM a link */
    vector<reference_descr*> to;
    /*! This object links TO that one or derives FROM a link */
    vector<reference_descr*> from;
    /*! If true then modifications are not passed on in a DIR_TO connection */

    longint version; 
    longint visitmarkers; 
    visit_marker received_visit_marker;
    longint netlevel; 
    bool beingmodified;
    bool modified;

    bool silent;
    bool tmpsilent;
    bool linkpathchanged;
    bool noMod;
    bool noSignal;
    bool autoupdate;
    bool updateable;
    longint verbose;
    bool debug;
    bool debuggui;

/*! Flag which states whether the object is modified or not*/
    DIRECTION defdir;

    map<HString,int> mapto,mapfrom;
    map<HString,int>::iterator it; //nonsense, but pure convenience in programming ....
    
    /*! This pointer points to a class describing the function used to operate on
      the data. */
    ObjectFunctionClass* of_ptr;
    
    Vector_Selector* s_ptr; //This pointer points to a selector field
    
    map<HString,HString> *prop_ptr;  //freely definable properties of data set --- may be superfluous 
    
    DATATYPE type; //Gives the data type in vector: HInteger, (real)HNumber, HComplex, or HString ...
    DATACLASS dataclass; //Specifies data class (actual vector data, or just paramters, or an container ...)
    unit_descr *unit_ptr;
    void *d_ptr; //Finally, this pointer points to the data 
    
    PyObject * pydbg; //Contains a pointer to a Py object which is called during debugging steps.
    PyObject * pyqt; //Contains a pointer to a PyQT object which is used to send and received Qt SIGNALS
    PyObject * py_func; //Contains a pointer to a Py object which contains a user-defined Python function
    PyObject * py_self; //Contains a pointer to the Python object of the data object itself (set by storePyFunc ....)
    
    //Finally, Finally, the pointer to the superior containing
    //information about the whole network 
    
    superior_container * superior;
  };
  
  HString reference_descr_str(reference_descr *rd);
  void printAllStatus(bool short_output=true,longint maxlevel=MAXNETLEVEL, bool printdeletedobjects=false);
  
  void printDecendants (HString rootname="");
  
  void printStatus(bool short_output=true);
  HString Status(bool short_output=true);
  
  void addReferenceDescriptor(reference_descr rd, map<objectid,reference_descr> &l);
  void join_map_ReferenceDescriptors(map<objectid,reference_descr> &l1, map<objectid,reference_descr> l2);
  map<objectid,reference_descr> listNeighbours(DIRECTION dir,longint maxlevel);
  void getNeighboursList(DIRECTION dir,longint maxlevel,  vector<objectid> &oids, vector<HString> &names, vector<DIRECTION> &dirs,  vector<HString> & flags);
  DataList FindChain(/*const*/ DIRECTION dir,/*const*/ vector<HString> names, /*const*/ bool include_endpoints=true, /*const*/ bool monotonic=false);
  vector<address> FindChainID(/*const*/ DIRECTION dir,/*const*/ vector<HString> names, /*const*/ bool include_endpoints=true, /*const*/ bool monotonic=false);

  vector<HString> listNeighbourNames(DIRECTION dir);
  vector<objectid> listNeighbourIDs(DIRECTION dir,longint maxlevel=MAXNETLEVEL);
  vector<DIRECTION> listNeighbourDirs(DIRECTION dir);
  vector<HString> listNeighbourModFlags(DIRECTION dir);

  DIRECTION getLinkDirection(/*const*/ Data & neighbour);
  DIRECTION getLinkDirectionType(/*const*/ Data & neighbour);

  vector<objectid> getAllIDs();

  Vector_Selector *sel();
  void new_sel();
  void del_sel();
  
  void setPort(objectid port, objectid refport, DIRECTION dir);

  vector<HString> getNeighbourNames(DIRECTION dir);
  DataList getNeighbours(/*const*/ DIRECTION dir);
  int getPort(Data & d, DIRECTION dir);
  reference_descr getLink(objectid port, DIRECTION dir);

  Data* from(HString name);
  Data* to(HString name);
  
  Data* from(objectid port);
  Data* to(objectid port);
  
  Data& create_Ref(HString name);
  Data* create(HString name);
  Data& insert_Ref(Data & d, Data & neighbour);
  Data* insert(Data * d, Data * neighbour);
  Data& insertNew_Ref(HString newname, Data & neighbour);
  Data* insertNew(HString newname, Data * neighbour);

  Data& erase(Data &d,DIRECTION dir=DIR_TO);
  Data& erase_1(Data &d);
  
  void setProperty(HString name,HString value);
  HString getProperty(HString name);
  
  
  DataList find_immediate_relatives(/*const*/ HString name, /*const*/ DIRECTION dir);

  template <class T>
    DataList find_relatives(/*const*/ HString name,  /*const*/ vector<T> &elems, /*const*/ DIRECTION dir);

  void dropVisitmarker(visit_marker vm);
  visit_marker getNewVisitmarker(longint v=-1);
  bool checkVisited(visit_marker vm);

  
  DataList Search(/*const*/ HString s);
  DataList Find(/*const*/ HString s, /*const*/ int rpos=0);
  
  vector<objectid> IDs(HString s);
  objectid ID(HString s);
  
  Data* Ptr(HString name);
  Data* Ptr(objectid oid);
  //This one is used for python
  
  Data& Object_Name(HString name);
  Data& Object_ID(objectid oid);
  Data& Object_Ref(Data& d);
  Data& Object(HString name);
  Data& Object(objectid oid);
  Data& Object(Data& d);
  
  // Data& operator[] (HString name);
  Data& operator[] (objectid oid);
  Data& operator[] (HString name);
  
  ObjectFunctionClass* getFunction();
  
  /*!
    Creates an empty vector and calls a get on the object, throwing the vector
    away.
  */

  template <class T>
    void get(vector<T> &v, Vector_Selector *vs=NULL) /*const*/; 
  
  template <class T>
    void get_1_(vector<T> &v) /*const*/;
  
  template <class T>
    void getFirstFromVector(vector<T> &v, Vector_Selector* vs) /*const*/;
  
  template <class T>
    T getOne(/*const*/ address i=0) /*const*/; 
  template <class T>
    T getOne_0_() /*const*/; 
  
  Data* find_name(/*const*/ HString name="", /*const*/ DIRECTION dir=DIR_FROM);
  Data* find_names(/*const*/ vector<HString> names, /*const*/ vector<DIRECTION> dir);
  
  DATATYPE getType() /*const*/;
  void setType(/*const*/ DATATYPE typ);
  address len();
  longint incVersion();
  longint getVersion();
  void setVersion(longint ver);
  longint getNetLevel();
  Data& setNetLevel(longint lev);
  HString getName (/*const*/ bool longname=false) /*const*/;
  HString getSearchName (/*const*/ bool longname=false);
  objectid getOid () /*const*/;
  HString getFuncName();
  address getMod();
  longint getNumberOfLinks(DIRECTION dir=DIR_BOTH);
  vector<modification_record> getModFlags(DIRECTION dir);
  vector<HString> getModFlagsStr();
  HString strModFlag(modification_record mod);
  bool ModFlagSet(modification_record mod);
/*! only returns the local mod values - not a full check for modification. Used for inspection mainly */
  bool isModified();

  dataworm*  Worm; /* A worm for the update check, containing pointers to all the objects to be updated */

  void setDefaultDirection(DIRECTION dir);
  DIRECTION getDefaultDirection();

  REFTYPE getOrigin();

  template <class T>
    void put_silent(vector<T> &v);
  template <class T>
    void putOne_silent(T v);
  template <class T>
    void put(vector<T> &v);
  template <class T>
    void put(string, vector<T> &vin);
  template <class T>
    void put(objectid, vector<T> &vin);
  template <class T>
      void inspect(vector<T> &vec);

  Data * noMod();
  Data & noMod_Ref();
  Data & noSignal();
  bool needsUpdate(); //obsolete?
  bool doesAutoUpdate();
  void setAutoUpdate(bool up=true);
  bool Updateable();
  void setUpdateable(bool up=true);
  bool Silent(bool silent);
  Data* doSilent();
  Data& doSilent_Ref();

  //  template <class T>
  //bool Data::doVerbose(/*const*/ vector<T> &v,/*const*/ bool checkmod);	
  bool doVerbose();
    
  longint setVerbose(longint verbose);
  bool setDebugGUI(bool tf);
  bool getDebugGUI();
  bool setDebug(bool tf);
  bool getDebug();
  bool Verbose();
  longint AllVerbose(longint verbose);
  bool Empty();
  bool hasFunc() /*const*/;
  bool hasData() /*const*/;
  Data& touch_0();
  Data& touch(/*const*/ bool silent=false);
  Data& wakeUp();
  bool checkModification();
  bool checkModification(objectid port, modification_record mod);
  void setModification(/*const*/ MOD_ACTION action=MOD_UPDATED);
  void setModification(modification_record newmod);
  void setModification(objectid port, modification_record mod);
  void clearModification();
  void clearModificationTO(objectid port);
  HString strWorm();
  void printWorm(bool ignoreempty=true);
  Data* setLinkpathChanged(bool change);
  bool getLinkpathChanged();
  void executeUpdateWorm();
  void doAutoUpdate();
  Data& update();
  void updateAll();
  
  template <class T>
    void putOne(T one);
  template <class T>
    void putOne(HString, T one);
  template <class T>
    void putOne(objectid, T one);

  boost::python::handle<> getPyList();
  Data& putPyList(boost::python::object& obj);
  Data& putPy(PyObject* pyobj);
  Data& putPy_silent(PyObject* pyobj);
  boost::python::handle<> getPy();

  Data& storePyQt(PyObject* pyobj);
  Data& deletePyQt(PyObject* pyobj);
  PyObject* retrievePyQt();
  boost::python::handle<> retrievePyQtObject();
  void signalPyQt(HString signal);

  Data& AllStorePyDBG(PyObject* pyobj);
  Data& storePyDBG(PyObject* pyobj);
  Data& deletePyDBG(PyObject* pyobj);
  PyObject* retrievePyDBG();
  boost::python::handle<> retrievePyDBGObject();
  void signalPyDBG(HString signal,HString input);

  Data& storePyFunc(PyObject* pyobj, PyObject* self);
  Data& deletePyFunc(PyObject* pyobj);
  PyObject* retrievePyFunc();
  boost::python::handle<> retrievePyFuncObject();

  bool callSimplePyObjectMethod(PyObject* pyobj, HString method);

  void delFunction();
  Data& setFunction (/*const*/ HString name,
		    /*const*/ HString library="",
		    /*const*/ DATATYPE typ=UNDEF);
  
  vector<HString> listFunctions (HString lib="", bool doprint=true);

  void sendMessage (MSG_CODE msg,
		    DIRECTION dir,
		    objectid count=0,
		    void* ptr=NULL);
  
  void getMessage (MSG_CODE msg,
		   DIRECTION dir,
		   objectid port,
		   objectid count=0,
		   void* ptr=NULL);
  
  void delLink (objectid port,
		DIRECTION dir,
		bool del_other=true,
		bool delete_empty=false);

  void delLink (Data & d);
  
  void delLink (objectid oid,
		objectid port,
		DIRECTION dir,
		bool del_other=true);
  
  void delLink_ID (objectid oid,
		   objectid port,
		   DIRECTION dir);
  

  Data * resetLink(Data *d, /*const*/ DIRECTION dir_type=DIR_NONE, /*const*/ DIRECTION dir=DIR_FROM);
  Data& resetLink_Ref_3(Data &d, /*const*/ DIRECTION dir_type=DIR_NONE, /*const*/ DIRECTION dir=DIR_FROM);
  Data& resetLink_Ref_2(Data &d, /*const*/ DIRECTION dir_type=DIR_NONE);
  Data& resetLink_Ref_1(Data &d);


  objectid setLink(Data* d, /*const*/ DIRECTION dir_type=DIR_NONE, /*const*/ DIRECTION dir=DIR_FROM , /*const*/ objectid otherport=-1, /*const*/ objectid thisport=-1);
  Data& setLink_Ref_3(Data &d, /*const*/ DIRECTION dir_type=DIR_NONE, /*const*/ DIRECTION dir=DIR_FROM);
  Data& setLink_Ref_2(Data &d, /*const*/ DIRECTION dir_type=DIR_NONE);
  Data& setLink_Ref_1(Data &d);
  
  
  void delObject_ID(objectid oid=-1);
  void delObject_Name(HString name);
  void delObject();
  void delOtherObject(Data *ptr);

  vector<objectid> newObjects_ID(HString name, DIRECTION dir_type=DIR_NONE);

  Data& newObject_Ref(HString name, DIRECTION dir_type=DIR_NONE);
  Data& newObject_Ref_1(HString name);

  Data* newObject(HString name, DIRECTION dir_type=DIR_NONE);

  vector<Data*> newObjects(HString name, DIRECTION dir_type=DIR_NONE, objectid maxparents=-1);
  
  template <class T>
    vector<Data*> newObjects(HString name, T val, DIRECTION dir_type=DIR_NONE, objectid maxparents=-1);
  template <class T>
    vector<Data*> newObjects(HString name, vector<T> vec, DIRECTION dir_type=DIR_NONE, objectid maxparents=-1);
  
  void delData();

  void newVector(DATATYPE type);
  
  Data(HString name="ROOT",superior_container * superior=NULL);
  ~Data(); 
  
 private:
  
  template <class T>
    void get_mem(vector<T> &v,  Vector_Selector *vs); 
  
  template <class T>
    void get_func(vector<T> &v,  Vector_Selector *vs); 
  
  data_field data;
};


//----Some Utilities

HInteger getPointerFromPythonObject(PyObject* pyobj);
vector<objectid> DPtrToOid(vector<Data*> vec); 

//Global object that represents a Null object returned if no result is found.
extern Data NullObject;

//End Data class
//========================================================================

#endif
