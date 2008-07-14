#ifndef HFGET_H
#define HFGET_H

///usr/share/qt4/bin/qmake hfget.pro

#include "VectorSelector.h"
#include "mainwindow.h"
#include "hfget-defs.h"

#include <QtCore/QMetaObject>
#include <QtGui/QLabel>

using namespace std;

class DataFuncDescriptor;
class ObjectFunctionClass;

//#include <gtk/gtk.h>
//#include "thread.h" -> qt

//Define Address Space for data elements (i.e. position in an array)
//Use long int if you want to address long data sets


//Define Default Buffer Size for get/put arrays number 
#define MAX_DATA_BUFFER_SIZE  1024
#define DEF_DATA_BUFFER_SIZE  1024


#define RECORD_SEP ":'"


/*
  Define pointers to the data and function of different types, all using the
  same memory location. This allows calling a void with variables types (e.g.
  using a switch statement)
*/

#define F_VECTOR_SELECTOR_DEFINITION( VS )  Vector_Selector *vs1,*vs2;	\
   if (VS != NULL) { \
    if (VS->SelectorType()<=SEL_LIST) {vs1=VS;  vs2=NULL;} \
    else {vs1 = NULL; vs2=VS;};} else {vs1=NULL; vs2=NULL;}


//Empty forward definitions
class DataFuncLibraryClass; 


//------------------------------------------------------------------------
// Casting Operations
//------------------------------------------------------------------------



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// FUNCTION DEFINITIONS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//========================================================================
//Utilities
//========================================================================

void * new_vector (DATATYPE type, address len);
bool del_vector (void *p, DATATYPE type);
bool del_value (void *p, DATATYPE type);

vector<String> split_str_into_vector (String str,
				      char c=',');

int string_find_chars (const String s,
		       const String c,
		       int &n,
		       const int npos=0);

int string_rfind_chars (const String s,
			const String c,
			int &n,
			const int npos=0);

String string_rfind_chars_substr (const String s,
				  const String c=RECORD_SEP,
				  const int npos=0);

void parse_record_name_to_vector (String name,
				  vector<String> &names,
				  vector<DIRECTION> &dirs,
				  String c=RECORD_SEP);

bool parse_record_selection_to_vector (String str,
				       String &name,
				       vector<String> &elems);

//End Utilities
//========================================================================



//========================================================================
//class Data 
//========================================================================

//--------------------------------------------------------------------------------

#define MAGICCODE 260966

class Data {

 public:
  
  int magiccode;

  struct superior_container {
    Data * root;
    objectid no_of_data_objects;
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
  
  //Defines a unit that goes with the data and also a prefix "milli, mikro, nano, etc." adn scalefactor
  struct unit_descr {
    String unit;
    Number scalefactor;
    String prefix;
  };

struct data_field {

  /*! Name identifying the data field */
  String name;

  /*! ID of the object */
  objectid oid;
  
  REFTYPE origin;
  //! Specifies the maximum memory size to be resereved for the data
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
 
 void redo(); //Creates an empty vector and calls a get on the object, throwing the vector away.
 
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




//------------------------------------------------------------------------
// object logic operators
//------------------------------------------------------------------------

template <class T>
bool object_logic_cmpr(Data* o_ptr, T val);

template <class T>
bool object_logic_in_set(Data* o_ptr, vector<T> &set);

// End object logic operators
//------------------------------------------------------------------------


#endif
