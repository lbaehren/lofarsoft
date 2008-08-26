/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
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

#include <QtCore/QMetaObject>
#include <QtGui/QLabel>

#define MAGICCODE 260966

/* Namespace usage */
using namespace std;

/* Class forward declarations */
class DataFuncDescriptor;
class ObjectFunctionClass;
class DataFuncLibraryClass; 

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
  - Pointer,
  - Integer,
  - Number (i.e. floats),
  - Complex, 
  - String.
  Complex data structures (i.e. "structs" or records) have to be built up by a
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
*/
class Data {
  
 public:
  
  int magiccode;
  
  struct superior_container {
    /*! Pointer to the root Data object in a chain of object */
    Data * root;
    /*! The number of Data objects attached */
    objectid no_of_data_objects;
    /*! Vector of pointers to the attached Data objects */
    vector<Data*> data_objects;
    DataFuncLibraryClass *library_ptr;
  };
  
  /*!
    \brief Description of the reference 
  */
  struct reference_descr {
    //! Name of the object linked to
    String name;
    //! ID of the object
    objectid oid;
    //! Pointer the object links to
    Data* ref;
    //! Stores the input/channel/port the link is established to
    objectid port;
    DIRECTION direction;
    //! Keep track of whether an object was modified - 0 if not modified
    modval mod;
    //! Stores the type of object the reference is pointing to
    DATATYPE type;
  };
  
  //Defines freely definably  properties that can be assoicated with the object
  //e.g. whether this is plotable data, or in Frequency or Time Domain, 
  //or whether that is by default an X, or Y-axis value, you name it ...
  
  /*  struct properties_descr {
      String name;
      String value;
      };*/
  
  /*Struct containing Information necessary to describe a local parameter, 
    that is connected to another object*/
  
  /*!
    Defines a unit that goes with the data and also a prefix "milli, mikro,
    nano, etc." adn scalefactor
  */
  struct unit_descr {
    /*! The unit of the quantity */
    String unit;
    /*! The scale factor */
    Number scalefactor;
    /*! Prefix of the unit */
    String prefix;
  };
  
  struct data_field {
    
    /*! Name identifying the data field */
    String name;
    
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
    vector<reference_descr> to;
    /*! This object links TO that one or derives FROM a link */
    vector<reference_descr> from;
    /*! If true then modifications are not passed on in a DIR_TO connection */
    bool silent;
    map<String,int> mapto,mapfrom;
    map<String,int>::iterator it; //nonsense, but pure convenience in programming ....
    
    /*! This pointer points to a class describing the function used to operate on
      the data. */
    ObjectFunctionClass* of_ptr;
    
    Vector_Selector* s_ptr; //This pointer points to a selector field
    
    map<String,String> *prop_ptr;  //freely definabel properties of data set
    
    DATATYPE type; //Gives the data type in vector: Integer, (real)Number, Complex, or String ...
    DATACLASS dataclass; //Specifies data class (actual vector data, or just paramters, or an container ...)
    unit_descr *unit_ptr;
    void *d_ptr; //Finally, this pointer points to the data 
    
    //Finally, Finally, the pointer to the superior containing
    //information about the whole network 
    
    superior_container * superior;
  };
  
  void print_reference_descr(reference_descr rd);
  void printAllStatus(bool short_output=true);
  
  void printDecendants (String rootname="");
  
  void printStatus(bool short_output=true);
  
  Vector_Selector *sel();
  void new_sel();
  void del_sel();
  
  void setPort(objectid port, objectid refport, DIRECTION dir);
  
  Data* from(String name);
  Data* to(String name);
  
  Data* from(objectid port);
  Data* to(objectid port);
  
  void setProperty(String name,String value);
  String getProperty(String name);
  
  
  vector<Data*> find_immediate_relatives(String name, DIRECTION dir);
  vector<Data*> find_relatives(String name, DIRECTION dir);
  
  template <class T>
    vector<Data*> select_relatives(String name, vector<T> &elems, DIRECTION dir);
  
  vector<Data*> Find(const String s, const int rpos=0);
  
  vector<objectid> IDs(String s);
  objectid ID(String s);
  
  Data* Ptr(String name);
  Data* Ptr(objectid oid);
  //This one is used for python
  
  Data& Object_Name(String name);
  Data& Object_ID(objectid oid);
  Data& Object_Ref(Data& d);
  Data& Object(String name);
  Data& Object(objectid oid);
  Data& Object(Data& d);
  
  // Data& operator[] (String name);
  Data& operator[] (objectid oid);
  Data& operator[] (String name);
  
  ObjectFunctionClass* getFunction();
  
  /*!
    Creates an empty vector and calls a get on the object, throwing the vector
    away.
  */
  void redo();
  
  template <class T>
    void get(vector<T> &v, Vector_Selector *vs=NULL, objectid port=-1); 
  
  template <class T>
    void get_1_(vector<T> &v);
  
  template <class T>
    T getParameter(String name, T defval);
  
  template <class T>
    void getFirstFromVector(vector<T> &v, Vector_Selector* vs);
  
  template <class T>
    T getOne(address i=0,objectid port=-1); 
  template <class T>
    T getOne_0_(); 
  template <class T>
    T getOne_1_(address i); 
  
  Data* find_name(String name="", DIRECTION dir=DIR_FROM);
  Data* find_names(vector<String> names, vector<DIRECTION> dir);
  
  DATATYPE getType();
  String getName ();
  objectid getOid ();
  
  REFTYPE getOrigin();
  
  template <class T>
    void put(vector<T> &v);
  template <class T>
    void put(string, vector<T> &vin);
  template <class T>
    void put(objectid, vector<T> &vin);
  
  objectid checkModification(objectid port=-1);
  
  bool Silent(bool silent);
  bool Empty();
  void touch();
  void setModification(objectid port=-1);
  void clearModification(objectid port=-1);
  void recalc(bool force=false);
  
  template <class T>
    void putOne(T one);
  template <class T>
    void putOne(String, T one);
  template <class T>
    void putOne(objectid, T one);
  
  void delFunction();
  void setFunction (String name,
		    DATATYPE type,
		    String library="Sys");
  
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
		bool del_other=true);
  
  void delLink (objectid oid,
		objectid port,
		DIRECTION dir,
		bool del_other=true);
  
  void delLink_ID (objectid oid,
		   objectid port,
		   DIRECTION dir);
  
  DEF_DATA_FUNC_DEF_OF_DPTR_3PARS (objectid,
				   setLink,
				   DIRECTION dir_type=DIR_FROM,
				   DIRECTION dir=DIR_FROM,
				   objectid port=-1)
    
    objectid setLink(Data *ptr, DIRECTION dir_type=DIR_FROM, DIRECTION dir=DIR_FROM , objectid port=-1);
  
  
  void delObject_ID(objectid oid);
  void delObject_Name(String name);
  void delObject(objectid oid);
  void delObject(String name);
  
  vector<objectid> newObjects_ID(String name, DIRECTION dir_type=DIR_FROM);
  vector<Data*> newObjects(String name, DIRECTION dir_type=DIR_FROM);
  
  template <class T>
    vector<Data*> newObjects(String name, T val, DIRECTION dir_type=DIR_FROM);
  template <class T>
    vector<Data*> newObjects(String name, vector<T> vec, DIRECTION dir_type=DIR_FROM);
  
  void delData();
  
  Data(String name="ROOT",superior_container * superior=NULL);
  ~Data(); 
  
 private:
  void newVector(DATATYPE type);
  
  template <class T>
    void get_mem(vector<T> &v,  Vector_Selector *vs); 
  
  template <class T>
    void get_func(vector<T> &v,  Vector_Selector *vs); 
  
  data_field data;
};


//----Some Utilities

vector<objectid> DPtrToOid(vector<Data*> vec); 
bool test_data_object_ptr(Data * dp);


//Global object that represents a Null object returned if no result is found.
extern Data NullObject;

//End Data class
//========================================================================

