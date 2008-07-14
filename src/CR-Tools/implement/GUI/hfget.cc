/*

HFGET Version 0.1 - The basic data get/put mechanism for the Hyper-Flexible Plotting Tool for LOFAR Data

 */


#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/args.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/object_operators.hpp>
#include <boost/thread/thread.hpp>

using namespace std;

#include "mainwindow.h" 
#include "hfdefs.h" 
#include "hfcast.h" 
#include "VectorSelector.h"
#include "hfget.h"
#include "hffuncs.h"  


//------------------------------------------------------------------------
// Globale Definitions
//------------------------------------------------------------------------

Data NullObject("NULL");


//========================================================================
// object logic operators
//========================================================================

/*
  These are function allowing one to compare data objects - eventually that
  should be used to overload operators like +,-,=,<,> etc ...
*/

//Checks if a value equals the first data element of the object
//This is mainly useful for objects containing a single data point (e.g., an parameter)
//The value the object data is  compared with will be converted to the object data type.
// object == value

template <class T>
bool object_logic_cmpr(Data* o_ptr, T val){
  bool result;
  switch (o_ptr->getType()) {
#define SW_TYPE_COMM(X,TYPE) result = ( (mycast<TYPE>(val)) == (o_ptr->getOne<TYPE>()))
#include "switch-type.cc"
    result=false;
  };
  return result;
}

//Checks if the first data element of the object is contained in a set (vector) of values.
//This is mainly useful for objects containing a single data point (e.g., an parameter)
//The value the object data is compared with (set members) will be converted to the object data type.
// object == value1 || object == value2 || object == value3 ....

template <class T>
  bool object_logic_in_set(Data* o_ptr, vector<T> &set){
  bool found=false;
  objectid i,s=set.size();
  DEF_VAL( T );

  switch (o_ptr->getType()) {
#define SW_TYPE_COMM(EXT,TYPE)  \
  val_##EXT = new TYPE; \
    (*val_##EXT)=o_ptr->getOne<TYPE>(); \
    DBG("VALUE=" << (*val_##EXT));\
    for (i=0; i<s && !found; i++) {\
      DBG("set[" <<i<<"]=" << set[i]); \
      found = (mycast<TYPE>(set[i]) == *val_##EXT);	    \
      DBG("FOUND=" << tf_txt(found));\
    };\
    delete val_##EXT
  #include "switch-type.cc"
    found=false;
  };
  DBG("Return FOUND=" << tf_txt(found));				    
  return found;
}

// End object logic operators
//------------------------------------------------------------------------


//========================================================================
//Utilities
//========================================================================

/*
Here we allow all types to be casted into others (even if that measn
to loose information). mycast is the basic function which allows one to do that.

*/


//Convert the various data types into each other:
template<class S> inline String mytostring(S v){std::ostringstream os; os << v; return os.str();}

//Identity
template<class T> inline T mycast(const T v){return v;}

//--Numbers ------------------------------------------------------------

//Convert to arbitrary class T if no specified otherwise
template<class T> inline T mycast(const Pointer v){return mycast<T>(reinterpret_cast<Integer>(v));}
template<class T> inline T mycast(const Integer v){return static_cast<T>(v);}
template<class T> inline T mycast(const Number v){return static_cast<T>(v);}
template<class T> inline T mycast(const Complex v){return static_cast<T>(v);}
template<class T> inline T mycast(const String v){T t; std::istringstream is(v); is >> t; return t;}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline Integer mycast<Integer>(Number v){return static_cast<Integer>(floor(v+0.5));}
template<>  inline Integer mycast<Integer>(Complex v){return static_cast<Integer>(floor(abs(v)+0.5));}

template<>  inline Number mycast<Number>(Complex v){return abs(v);}


//--Strings ------------------------------------------------------------

//Convert Numbers to Strings
template<> inline String mycast<String>(Pointer v){std::ostringstream os; os << v; return os.str();}
template<> inline String mycast<String>(Integer v){std::ostringstream os; os << v; return os.str();}
template<> inline String mycast<String>(Number v){std::ostringstream os; os << v; return os.str();}
template<> inline String mycast<String>(Complex v){std::ostringstream os; os << v; return os.str();}



//--Pointers ------------------------------------------------------------


//Convert Type T to Pointer:
template<> inline Pointer mycast(const Pointer v){return v;}
template<> inline Pointer mycast(const Integer v){return mycast<Pointer>(v);}
template<> inline Pointer mycast(const Number v){return reinterpret_cast<Pointer>(mycast<Integer>(v));}
template<> inline Pointer mycast(const Complex v){return reinterpret_cast<Pointer>(mycast<Integer>(v));}


//The default function pointer to be passed on as reference
//The function will return a vector of type T and take as 
//input a vector of (void) pointers that are assumed to
//point to Data objects

template <class T, class S>
void copycast_vec(void *ptr, vector<S> *sp) {
  address i,s;
  vector<T> *ip =  reinterpret_cast<vector<T>*>(ptr);
  s=(*ip).size();
  (*sp).clear();if ((*sp).capacity()<s) {(*sp).reserve(s);};
  for (i=0;i<s;i++){(*sp).push_back(mycast<S>((*ip)[i]));};
}

template <class T, class S>
void copycast_vec(void *ptr, vector<S> *sp, Vector_Selector *vs) {
  if (vs==NULL) {copycast_vec<T,S>(ptr,sp); return;};

  address i,s1,s2;
  vector<T> *ip =  reinterpret_cast<vector<T>*>(ptr);
  vector<address> *it = (*vs).getList(*ip);
  if (it==NULL) {copycast_vec<T,S>(ptr,sp); return;};

  s1=(*it).size();  s2=(*ip).size();
  (*sp).clear();if ((*sp).capacity()<s1) {(*sp).reserve(s1);};
  for (i=0;i<s1;i++){
    if ((*it)[i]<s2) {(*sp).push_back(mycast<S>((*ip)[(*it)[i]]));};
  };
}

/* Functions that provide textual output for some of the enum types we use here */

char* direction_txt(DIRECTION dir){
  char* DIRECTION_TXT[DIR_NONE+1]={"DIR_FROM", "DIR_TO", "DIR_BOTH","DIR_NONE"};
  if (dir <= DIR_NONE && dir >=0) {return DIRECTION_TXT[dir];} else {return "UNKNOWN_DIRECTION";};
}

char* dataclass_txt(DATACLASS x){
  char* DATACLASS_TXT[DAT_NONE+1]={ "DAT_CONTAINER", "DAT_PARAMETER", "DAT_FILENAME","DAT_VECTOR", "DAT_ARRAY", "DAT_NONE"};
  if (x <= DAT_NONE && x >=0) {return DATACLASS_TXT[x];}  else {return "UNKNOWN_DATACLASS";};
}

char* reftype_txt(REFTYPE x){
  char* REFTYPE_TXT[REF_NONE+1]={"REF_MEMORY", "REF_FUNCTION", "REF_FILE", "REF_NODE", "REF_WEB","REF_NONE"};  
  if (x <= REF_NONE && x >=0) {return REFTYPE_TXT[x];}  else {return "UNKNOWN_REF";};
}

char* datatype_txt(DATATYPE x){
  char* DATATYPE_TXT[6]={"Pointer","Integer", "Number", "Complex", "String", "UNDEF"};
  DBG("datatype_txt: x=" << x);
  if (x <= UNDEF && x >=0) {return DATATYPE_TXT[x];} else {return "UNKNOWN_TYPE";};
}

char* tf_txt(bool x){
  if (x) {return "TRUE";} else {return "FALSE";};
}

char* pf_txt(bool x){
  if (x) {return "PASS";} else {return "FAIL";};
}

DIRECTION inv_dir(DIRECTION dir) {
  if (dir==DIR_FROM) {return DIR_TO;} 
  else if (dir==DIR_TO) {return DIR_FROM;} 
  else {return dir;};
}

/* 
This function allows one to retreive the value stored in the heap of known "type", using a void pointer "ptr" and cast it to the type T. Conversion is only possible between the basic data types known to mycast.
*/


template <class T>
T cast_ptr_to_value(void * ptr, DATATYPE type){
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) return mycast<T>(*(static_cast<TYPE*>(ptr)))
#include "switch-type.cc"
    return mycast<T>(0);
  };
}

/* 
This function allows one to set the value of a variable stored in the
heap of known "type", using a void pointer "ptr" using an input type
T.  Conversion is only possible between the basic data types known to
mycast.
*/

template <class T>
void set_ptr_to_value(void * ptr, DATATYPE type, T value){
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) *(static_cast<TYPE*>(ptr))=mycast<TYPE>(value);
#include "switch-type.cc"
  };
}

//End Casting Operations
//------------------------------------------------------------------------


//Casting Utilities
//------------------------------------------------------------------------
 
/* join two vectors: (1,2,3) + (4,5,6) = (1,2,3,4,5,6) */
template <class T>
vector<T> vec_combine(vector<T> v1,vector<T> v2){
  vector<T> v;
  v.reserve(v1.size() + v2.size());
  v.insert(v.end(), v1.begin(), v1.end());
  v.insert(v.end(), v2.begin(), v2.end());
  return v;
}

/* append one vector to the end of the other */
template <class T>
void vec_append(vector<T> &v1,const vector<T> &v2){
  v1.reserve(v1.size() + v2.size());
  v1.insert(v1.end(), v2.begin(), v2.end());
  return;
}

template <class T>
String vectostring(vector<T> v,address maxlen=8){
  int i;
  int s;
  String out="";
  s=v.size(); 
  if (s==0) {return out;};
  if (s<=maxlen+1) {
    for (i=0;i<s-1;i++) {out += mycast<String>(v[i])+", ";};
    out += mycast<String>(v[i]);
  } else {
    for (i=0;i<maxlen/2;i++) {out += mycast<String>(v[i])+", ";};
    out += "...";
    for (i=s-maxlen/2;i<s;i++) {out += ", " + mycast<String>(v[i]);};
  };
  return out;
}

template <class T>
void printvec_noendl(vector<T> v,address maxlen=8){
  int i;
  int s;
  s=v.size(); 
  if (s==0) {return;};
  if (s<=maxlen+1) {
    for (i=0;i<s-1;i++) {cout << v[i] << ", ";};
    cout << v[i];
  } else {
    for (i=0;i<maxlen/2;i++) {cout << v[i] << ", ";};
    cout << "...";
    for (i=s-maxlen/2;i<s;i++) {cout << ", " << v[i];};
  };
}

template <class T>
void printvec(vector<T> v,address maxlen=8){
  printvec_noendl(v,maxlen);
  cout << endl;
}


//!!Check - this doesn't work properly for direction_txt
template <class T, class S>
void printvec_txt(vector<T> v, char* (*f)(S)){
  int i;
  int s;
  s=v.size();
  if (s==0) {cout <<endl; return;};
  for (i=0;i<s-1;i++) {cout << *f(v[i]) << ", ";};
  cout << f(v[i]) << endl;
  return;
}

template <class T>
DATATYPE WhichType() {
  DATATYPE t;
       if (typeid(T)==typeid(Pointer)) t = POINTER;
  else if (typeid(T)==typeid(Integer)) t = INTEGER;
  else if (typeid(T)==typeid(Number )) t = NUMBER;
  else if (typeid(T)==typeid(Complex)) t = COMPLEX;
  else if (typeid(T)==typeid(String )) t = STRING;
  else t = UNDEF;
  return t;
}


//End Casting Functions
//------------------------------------------------------------------------

void * new_vector(DATATYPE type, address len){
  void*ptr; 
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) ptr = new vector<TYPE>(len)
#include "switch-type.cc"
    return NULL;
  }
  return ptr;
}

bool del_vector(void *ptr, DATATYPE type) {
  DEF_D_PTR( Integer );
  d_ptr_v=ptr;
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) \
    DBG("Delete " << #TYPE << " ptr=" << d_ptr_v << endl);\
    delete d_ptr_##EXT;\
    DBG("Deleted: " << #TYPE << " ptr=" << d_ptr_v << ", type=" << type << endl)
#include "switch-type.cc"
    ERROR("del_vector: While attemptting to delete a vector.");
  }
}

bool del_value(void *ptr, DATATYPE type) {
  switch (type) {
#define SW_TYPE_COMM(EXT,TYPE) \
    DBG("Delete value " << #TYPE << " ptr=" << ptr << endl);\
    delete static_cast<TYPE*>(ptr);\
    DBG("Deleted: " << #TYPE << " ptr=" << ptr << ", type=" << type << endl)
#include "switch-type.cc"
    ERROR("del_value: Undefined type encountered while attemptting to delete a value.");
  }
}

//splits a string of type name1:name2:name3 into a vector of strings
vector<String> split_str_into_vector(String str, char c)
{
  vector<String> out;
  int pos,oldpos=0;
  for (pos=str.find(c);pos>-1;pos=str.find(c,pos+1)) {
    out.push_back(str.substr(oldpos,pos-oldpos));
    oldpos=pos+1;
  };
  out.push_back(str.substr(oldpos));
  return out;
}

//finds the first location of certain characters, n contains the position
int string_find_chars (const String s, const String c, int &n, const int npos)
{
  int i, slen=s.size();
  int nchrs=c.size();
  bool found=false;
  for (i=npos; i<slen && !found; i++) {
    for (n=0;n<nchrs && !found;n++) {found = c[n]==s[i];};
  };
  n=n-1;
  if (i>=slen) {i=0;};
  return i-1;
}

/*
finds the last location of certain characters (c) in string s, n contains the character number that was found, 
npos gives a starting position relative to the end
*/

int string_rfind_chars (const String s, const String c, int &n, const int npos)
{
  int i, slen=s.size();
  int nchrs=c.size();
  bool found=false;
  for (i=slen-1-npos; i>=0 && !found; i--) {
    for (n=0;n<nchrs && !found;n++) {found = c[n]==s[i];};
  };
  n=n-1;
  if (found) {return i+2;} else {return i+1;};
}

/*
finds the last location of certain characters (c) in string s, npos gives a starting position,
and the returns the string from the end up to (and excluding) the (punctuation) character 
*/

String string_rfind_chars_substr (const String s, const String c, const int npos)
{
  int n,i=string_rfind_chars (s, c, n, npos);
  return s.substr(i,s.size()-i);
}


/*
Takes a string and splits it into a vector of strings (names) at the punctuation characters
contained in string c =":'". If a ":" is encountered the corresponding element in the 
direction vector dirs is set to DIR_TO otherwise it is set to DIR_FROM.
 */
void parse_record_name_to_vector(String name, vector<String> &names, vector<DIRECTION> &dirs, String c)
{
  DIRECTION dir = DIR_TO;
  int nc,pos,oldpos=0;
  names.clear(); dirs.clear();
  for (pos=string_find_chars(name,c,nc);pos>-1;pos=string_find_chars(name,c,nc,pos+1)) {
    if (pos>oldpos) {
      names.push_back(name.substr(oldpos,pos-oldpos));
      dirs.push_back(dir);
    };
    if (nc==0) {dir=DIR_TO;} else {dir=DIR_FROM;};
    oldpos=pos+1;
  };
  names.push_back(name.substr(oldpos));
  dirs.push_back(dir);
}

bool parse_record_selection_to_vector(String str, String &name, vector<String> &elems)
{
  int pos;
  pos=str.find('=');
  if (pos==-1) {name=str; elems.clear(); return false;};
  name=str.substr(0,pos);
  DBG(endl << "Input=" << str);
  DBG("name=" << name);
  elems=split_str_into_vector(str.substr(pos+1),',');
  DBG2("elems="); DBG3(printvec(elems));
  return true;
}

vector<objectid> DPtrToOid(vector<Data*> objects) 
{
  vector<objectid> object_oids;
  vector<Data*>::iterator it;
  for (it=objects.begin(); it<objects.end(); it++) {
    object_oids.push_back((*it)->getOid());
  };
  return object_oids;
}

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


Vector_Selector * Data::sel(){return data.s_ptr;}
void Data::new_sel(){if (data.s_ptr==NULL) {data.s_ptr=new Vector_Selector;};}
void Data::del_sel(){delete data.s_ptr;data.s_ptr=NULL;}


void Data::print_reference_descr(reference_descr rd){
  cout << "name=" << rd.name;
  cout << ", ref=" << rd.ref;
  cout << ", port=" << rd.port; 
  cout << ", direction=" << direction_txt(rd.direction);
  cout << ", mod=" << rd.mod;
  cout << ", type=" << datatype_txt(rd.type) << endl;
}

void Data::printAllStatus(bool short_output){
  Integer i;
  if (data.superior!=NULL) {
    for (i=0; i<data.superior->data_objects.size(); i++) {
      data.superior->data_objects[i]->printStatus(short_output);
    };
  } else {
    ERROR("printAllStatus: No superior created yet, can't find any objects!");
  }
}

void Data::printStatus(bool short_output){
  objectid i;
  DEF_D_PTR(Integer); 
  if (short_output) {
    cout << "#" << data.name << ": ";
    if (data.of_ptr != NULL) {cout << data.of_ptr->getName() <<"[";};
    cout << datatype_txt(data.type);
    if (data.of_ptr != NULL) {cout << "]";};
    if (data.len>0) {cout << "(" << data.len << ")";};
    if (data.d_ptr!=NULL) {
      cout << "=";
      d_ptr_v=data.d_ptr;
      CALL_FUNC_D_PTR (printvec_noendl);
    };
    if (data.to.size()>0) {
      cout << " ->";
      for (i=0; i<data.to.size(); i++) {
	cout << data.to[i].ref->getName();
	if (i<data.to.size()-1) {cout << ",";};
      };
    };
    if (data.from.size()>0) {
      cout << " <-";
      for (i=0; i<data.from.size(); i++) {
	cout << data.from[i].ref->getName();
	if (i<data.from.size()-1) {cout << ",";};
      };
    };
    cout << endl;
  } else {
  cout << endl;
  cout << "------------------------------------------------------------------------"<<endl;
  cout << "Name: " << data.name <<endl;
  cout << "oid: " << data.oid <<endl;
  cout << "this:" << this << endl;
  cout << "------------------------------------------------------------------------"<<endl;
  cout << "origin: " << reftype_txt(data.origin) <<endl;
  cout << "buflen: " << data.buflen <<endl;
  cout << "len: " << data.len <<endl;
  cout << "To  : "<<endl; 
  for (  i=0; i<data.to.size(); i++) {cout <<"["<<i<<"] "; this->print_reference_descr(data.to[i]);};
  cout << "From: "<<endl; 
  for (  i=0; i<data.from.size(); i++) {cout <<"["<<i<<"] "; this->print_reference_descr(data.from[i]);};
  cout << "datatype=" << datatype_txt(data.type) << endl;
  cout << "of_ptr=" << data.of_ptr << endl;
  cout << "d_ptr=" << data.d_ptr << endl;
  cout << "s_ptr=" << data.s_ptr << endl;

  if (data.d_ptr!=NULL) {
    cout << "========================================================================"<<endl;
    cout << "data = ";
    d_ptr_v=data.d_ptr;
    CALL_FUNC_D_PTR (printvec);
  };
  cout << "========================================================================"<<endl;
  };
}

void Data::printDecendants(String rootname){
 objectid i;
  String name;
  if (rootname=="") {name=data.name;} else {name = rootname + ":" + data.name;};
  cout <<  name << "[" << data.len << "]";
  if (data.d_ptr != NULL || data.of_ptr != NULL) {
    cout << " = ";
    DEF_D_PTR(Number); 
    d_ptr_v=new_vector(data.type,data.len);
    CALL_FUNC_D_PTR(get);
    CALL_FUNC_D_PTR(printvec);
    del_vector(d_ptr_v,data.type);
  } else {cout << endl;};
  for (i=0; i<data.to.size(); i++) {if (data.to[i].ref!=NULL) {data.to[i].ref->printDecendants(name);};};
}

 DATATYPE Data::getType(){return data.type;}
 String Data::getName(){return data.name;}
 objectid Data::getOid(){return data.oid;}
 ObjectFunctionClass* Data::getFunction(){return data.of_ptr;};

//  sendMessage(MSG_MODIFIED,DIR_TO);


//modified without a parameter is local, i.e. the current object has been modified and all modified flags are incremented
//With a port number, the modification came from one port and the from fag is set and a recalc is initiated.

objectid Data::checkModification(objectid port){
  objectid size,i,mods;
  size=data.from.size();
  mods=0;
  for (i=0; i<size /*&& mods==0*/; i++) {//check out all "from"-ports and query predecessor objects if requested
    if (data.from[i].direction == DIR_FROM || data.from[i].direction == DIR_BOTH) {
      data.from[i].mod=data.from[i].mod+data.from[i].ref->checkModification(data.from[i].port);
    };//if it is TO direction for communication, then only check local mod value 
      //otherwise go through tree and set "from" modvalues based on objects lower down.
    mods=mods+data.from[i].mod;
  };
  if (port>=0 && port < data.to.size()) {  //if a special port is queried (from higher up) then, also check .to mod value (which is the actual query)
    mods=mods+data.to[port].mod++;
  };
  return mods;
}


/* Make an object silent. That allows you to change the value of an object, withou triggering an instant recalc of the net.*/

bool Data::Silent(bool silent){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return false;};
  bool old;
  old=data.silent;
  data.silent=silent;
  return old;
}

void Data::touch(){setModification(-1);}

/*Set all moodification flags and propagate the message through the net, if necessary*/

void Data::setModification(objectid port){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  objectid size,i,count;
  size=data.to.size();
  count=0;
  DBG("SETMOD name=" << getName());
  for (i=0; i<size; i++) {
    DBG("MOD TO port=" << i);
    data.to[i].mod++;   //if an object is modified all data.to flags will be set to modified
    if ((data.to[i].direction == DIR_TO || data.to[i].direction == DIR_BOTH) && !data.silent) {
      DBG("CALL MOD for port=" << i << " and ptr=" <<  data.to[i].ref << " to port=" << data.to[i].port);
      count++;
      data.to[i].ref->setModification(data.to[i].port);   //If requested, notify any objects higher up in the food chain about the modification
    };
  };
  if (port>=0 && port < data.from.size()) {
    DBG("SET FROM MODIFICATION port=" << port);
    data.from[port].mod++;
    if (count==0) {recalc();};  //If the message is not passed any higher, start a recalc trickeling down the net.
  };
  DBG("SET MODIFICATION:");
}

void Data::clearModification(objectid port){
  objectid size,i;
  if (port>=0 && port < data.to.size()) {
    data.to[port].mod=0;
  };
  size=data.from.size();
  for (i=0; i<size; i++) {data.from[i].mod=0;};   
}

/*Not yet really used, but probably useful at some point */
void Data::sendMessage(MSG_CODE msg, DIRECTION dir, objectid count, void* ptr) {
  objectid i,s;
  if (dir == DIR_FROM || dir == DIR_BOTH) {
    s=data.from.size();
    for (i=0; i<s-1; i++) {
      if (data.from[i].direction == DIR_FROM || data.from[i].direction == DIR_BOTH) {
	data.from[i].ref->getMessage(msg,dir,data.from[i].port,count,ptr);
      };
    };
  };
  
  if (dir == DIR_TO || dir == DIR_BOTH) {
    s=data.to.size();
    for (i=0; i<s-1; i++) {
      if (data.to[i].direction == DIR_TO || data.to[i].direction == DIR_BOTH) {
	data.to[i].ref->getMessage(msg,dir,data.to[i].port,count,ptr);
      };
    };
  };
}

//if count is positive it indicates the number of levels a message is propagated
//with count=0 it only reaches the immediate neighbours.

void Data::getMessage(MSG_CODE msg, DIRECTION dir, objectid port, objectid count, void* ptr) {
  objectid i,s;
  switch (msg) {
  case MSG_MODIFIED:
    data.from[port].mod++;

    if (dir==DIR_TO) {};
    if (dir==DIR_FROM) {data.to[port].mod++;};
    return;
    break;
  default:
    ERROR("Message " << msg << "is unknown.");
    break;
  };
  if (count > 0) {sendMessage(msg, dir, count-1, ptr);};
}

/*
This function assigns the corresponding port number of a referencing
object, i.e. port "port" in the current object will point to port
report in the related object. dir specifies whether that is a "to" or a
"from" reference.
*/
void Data::setPort(objectid port, objectid refport, DIRECTION dir){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  vector<reference_descr> * tf;
  if (dir == DIR_FROM ) {tf=&data.from;} 
  else if (dir == DIR_TO) {tf=&data.to;} 
  else {
    ERROR("setPort: Improper direction specified- neiter to nor from.");
    return;
  };
  if ((*tf).size()>port) {(*tf)[port].port=refport;}
  else {ERROR("setPort: Changing non-existing port.");};
}

/*
This function deletes a link to another object, specified by the port number
*/

void Data::delLink(objectid oid, objectid port, DIRECTION dir, bool del_other) {
  Ptr(oid)->delLink(port,dir,del_other);
}
void Data::delLink_ID(objectid oid, objectid port, DIRECTION dir) {
  Ptr(oid)->delLink(port,dir);
}

void Data::delLink(objectid port, DIRECTION dir, bool del_other) {
  objectid corresponding_port;
  vector<reference_descr> * tf;
  if (dir == DIR_FROM ) {tf=&data.from;} 
  else if (dir == DIR_TO) {tf=&data.to;} 
  else {
    ERROR("delLink: Improper direction specified: neither to nor from.");
    return;
  };
  DBG("delLink: link=" << this << " port=" << port << " to/from.size()=" << (*tf).size());
  DBG3(this->printStatus(false));
  if (port < (*tf).size()) {
    //If this was the first call to the function delete the corresponding entrance in the
    //other object
    if (del_other) {
      (*tf)[port].ref->delLink((*tf)[port].port,inv_dir(dir),false);
    };
    //If this was the last reference in the vector, just delete it
    //otherwise copy the last object to the current reference port and then delete the last
    if (port<(*tf).size()-1) {
      (*tf)[port] = (*tf).back(); //(*tf)[(*tf).size()-1];
      if ((*tf)[port].ref!=NULL) {
	(*tf)[port].ref->setPort((*tf)[port].port,port,inv_dir(dir));
      } else {ERROR("DelLink: Deleting a reference to a non-existing object");};
    };
    (*tf).pop_back();
  } else {
    ERROR("delLink: Attempting to delete a non-existing Link. Port number too high.");
  };
}

//The next was used for python binding - still necessary?
DEF_DATA_FUNC_OF_DPTR_3PARS(objectid,setLink, DIRECTION, dir_type, DIRECTION, dir, objectid, port)


/* Set a link to another object*/

objectid Data::setLink(Data *d, DIRECTION dir_type, DIRECTION dir, objectid port) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return -1;};
  objectid prt;
  Data* p = this;
  reference_descr rd;
  
  if (d == this) {
    ERROR("setLink: Object can't point to itself.");
    return -1;
  }
  if (d == &NullObject) {
    ERROR("setLink: Attempting to link Object to NullObject. It is likely that a wrong object name was specified.");
    return -1;
  }
  prt=port;
  if (dir == DIR_FROM || dir == DIR_BOTH) {
    if (prt<0) {prt=(d->setLink(this, dir_type, DIR_TO,data.from.size()));};
    rd.direction=dir_type; rd.ref=d; rd.port=prt; rd.mod=0; rd.type=d->getType();
    rd.name=d->getName(); rd.oid=d->getOid(); 
    data.mapfrom[rd.name]=data.from.size();
    data.from.push_back(rd);
    return data.from.size()-1;
  } else if (dir == DIR_TO || dir == DIR_BOTH) {
    if (prt<0) {prt=(d->setLink(this, dir_type,DIR_FROM,data.to.size()));};
    rd.direction=dir_type; rd.ref=d; rd.port=prt; rd.mod=0; rd.type=d->getType();
    rd.name=d->getName(); rd.oid=d->getOid(); 
    data.mapto[rd.name]=data.to.size();
    data.to.push_back(rd);
    return data.to.size()-1;
  };
};


Data* Data::from(objectid port) {
  if (port >=0 && port < data.from.size()) {return data.from[port].ref;}
  else {ERROR("data.from(): unknown reference port=" << port); return NULL;};
}

Data* Data::to(objectid port) {
  if (port >=0 && port < data.to.size()) {return data.to[port].ref;}
  else {ERROR("data.to(): unknown reference port=" << port); return NULL;};
}

Data* Data::from(String name) {

  data.it=data.mapfrom.find(name);
  if (data.it != data.mapfrom.end()){return data.from[data.it->second].ref;}
  else {ERROR("data.from(): unknown reference \"" << name << "\""); return NULL;};
}

Data* Data::to(String name) {
  data.it=data.mapto.find(name);
  if (data.it != data.mapto.end()){return data.to[data.it->second].ref;}
  else {ERROR("data.to(): unknown reference \"" << name << "\""); return NULL;};
};


//Currently the property list is not really used - delete it?
void Data::setProperty(String name,String value){
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  map<String,String>::iterator it;
  if (data.prop_ptr==NULL) {data.prop_ptr=new map<String,String>;}; //create if not yet present
  (*data.prop_ptr)[name,value];
  return;
};

String Data::getProperty(String name){
  if (data.prop_ptr==NULL) {return "";};
  map<String,String>::iterator it;
  it=data.prop_ptr->find(name);
  if (it != data.prop_ptr->end()){return it->second;} else {return "";};
};


/*Creates a new, dynamic data vector of a given type*/

void Data::newVector(DATATYPE type){
  if (data.d_ptr!=NULL) {delData();};
  data.type=type; 
  data.d_ptr=new_vector(data.type,data.buflen); 
  DBG("new-vec data.d_ptr=" << data.d_ptr);
}

void Data::delData(){
  del_vector(data.d_ptr,data.type); 
  data.d_ptr=NULL; 
  data.type=UNDEF;
}

template <class T>
void Data::putOne(T one) {
  vector<T> vp(1,one);
  DBG("putOne: Type=" << datatype_txt(WhichType<T>()) << " val=" << one);
  put(vp);
}

template <class T>
void Data::putOne(String name, T one) {
  Ptr(name)->putOne(one); 
}

template <class T>
void Data::putOne(objectid oid, T one) {
  Ptr(oid)->putOne(one); 
}

template <class T>
void Data::put(String name, vector<T> &vin) {
  Ptr(name)->put(vin); 
}

template <class T>
void Data::put(objectid oid, vector<T> &vin) {
  Ptr(oid)->put(vin); 
}

template <class T>
void Data::put(vector<T> &vin) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  DEF_D_PTR(T);
  if (data.d_ptr==NULL) {
    DBG("Put Vector: name=" << data.name);
    data.origin=REF_MEMORY; 
    newVector(WhichType<T>());
    DBG("Put Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
  } else if (data.type!=WhichType<T>()) {
    DBG("Delete Vector: name=" << data.name);
    DBG("Delete< Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
    delData(); 
    newVector(WhichType<T>());
    DBG("Put Vector: name=" << data.name);
    DBG("Put Vector: d_ptr=" << data.d_ptr << " data.type=" << data.type);
  };
  d_ptr_v=data.d_ptr;
  (*d_ptr_T) = vin;
  data.len=vin.size();
  DBG("put: d_ptr=" << data.d_ptr << ", &vin=" << &vin << ", len=" << data.len);
  setModification();  //Note that the data has changed
}

/*
  This method deletes an object function if present 
*/

void Data::delFunction(){
  ObjectFunctionClass * of = getFunction();
  if (of!=NULL) {
    //Calls the function and destruct the local static variables in the function
    delete of;
    data.of_ptr=NULL;
  };
}

/*
This method adds a function to the object that will performed whenever a recalc is requested.
*/

void Data::setFunction(String name, DATATYPE type, String library) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return;};
  if (data.superior!=NULL && data.superior->library_ptr!=NULL) {
    if (data.superior->library_ptr->inLibrary(name,library)) {
      delFunction(); //Delete old function if necessary;
      DataFuncDescriptor fd = *(data.superior->library_ptr->FuncDescriptor(name,library));
      data.of_ptr = fd.newFunction(this);
      if (data.d_ptr==NULL) {data.type=type;};
      data.origin=REF_FUNCTION;
      setModification();
    } else {
      ERROR("setFunction: Function " << name << " in library " << library << " not found.");
      return;
    };
  } else {
    ERROR("setFunction: superior object or Function library non-existent.");
    return;
  };
}


//Needed for Python Bindings
template <class T> T Data::getOne_1_(address i){return getOne<T>(i);}
template <class T> T Data::getOne_0_(){return getOne<T>();}

template <class T>
T Data::getOne(address i,objectid port){
  Vector_Selector *vs;
  vs = new Vector_Selector;
  vs->setOne(i);
  vector<T> v(1);
  get(v,vs,port);
  delete vs;
  return v.at(0);
}

void Data::recalc(bool force){
  DBG("RECALC: name=" << data.name << " Empty=" << tf_txt(Empty())<< " silent=" << tf_txt(data.silent));
  if (!Empty() && (!data.silent || force)) {
    DEF_D_PTR(Number);
    d_ptr_v=new_vector(data.type,data.buflen);
    CALL_FUNC_D_PTR(get);
    del_vector(d_ptr_v, data.type);
  };
}

//Go backward or forward through the net until you find an object of the specified name
//returns NULL if not found and "this" (pointer to current object) for an empty string
Data* Data::find_name(String name, DIRECTION dir) {
  Data *D_ptr=&NullObject; 
  if (name != data.name && name!="") {
    objectid i;
    vector<reference_descr> *p_ref;
    if (dir==DIR_TO) {p_ref = &data.to;} else {p_ref = &data.from;};
    for (i=0;i<(*p_ref).size() && D_ptr==&NullObject;i++){
      D_ptr=(*p_ref)[i].ref->find_name(name,dir);
    };
    return D_ptr;
  } else {
    return this;
  };
}


//Check if any of the immediate relatives in direction dir has a given name and return
//pointers to these objects. Returns multiple pointers in vec if multiple names are present.
//The function returns true if the relative is found, and false if not.

  vector<Data*> Data::find_immediate_relatives(String name, DIRECTION dir) {
  objectid i;
  vector<reference_descr> *p_ref;
  vector<Data*> vec;
  DBG("find_immediate_relatives: name=" << name << " dir=" << direction_txt(dir) << " current object=" << data.name);
  if (dir==DIR_TO) {p_ref = &data.to;} else {p_ref = &data.from;};
  for (i=0;i<(*p_ref).size();i++){
    if ((*p_ref)[i].name==name) {
      vec.push_back((*p_ref)[i].ref);
    };
  };
  DBG2("find_immediate_relatives: found=" << tf_txt(vec.size()>0) << " i=" << i); DBG3(printvec(vec));
  return vec;
}

//Check if any of the immediate relatives in direction dir has a given name and return
//pointers to these object. Returns multiple pointers in vec if multiple names are present.
//If not, seek the next generation of relatives. Returns only the relatives of the first object
//to have relatives of the name searched for.
//The function returns true if the relatives are found, and false if not.

vector<Data*> Data::find_relatives(String name, DIRECTION dir) {
  objectid i;
  vector<Data*> vec;
  vector<reference_descr> *p_ref;
  vec=find_immediate_relatives(name,dir);
  if (vec.size()==0) {
    //search the next generation, if name is not found in the immediate vicinity
    if (dir==DIR_TO) {p_ref = &data.to;} else {p_ref = &data.from;};
    for (i=0;i<p_ref->size();i++){
      vec_append(vec,(*p_ref)[i].ref->find_relatives(name,dir));
    };
  };
  return vec;
}


//find all relatives of name=name which have a vlaue in vector elems
template <class T>
vector<Data*> Data::select_relatives(String name, vector<T> &elems, DIRECTION dir) {
  vector<Data*> vec1,vec2;
  objectid i,s;
  bool found;
  vec1=find_relatives(name,dir);
  DBG2("find_relatives: name=" << name << " dir=" << direction_txt(dir) << " vec1="); DBG3(printvec(vec1));
  s=vec1.size();
  for (i=0;i<s;i++){
    DBG2("in_set vec1[" << i << "] in ["); DBG3(printvec_noendl(elems)); DBG3(cout << "]" <<endl);
    found=object_logic_in_set(vec1[i],elems);
    DBG("found=" << tf_txt(found));
    if (found) {DBG2("Found! "); 
      vec2.push_back(vec1[i]); 
      DBG("i=" << i << " vec2.size()=" << vec2.size()); 
      DBG3(vec2[vec2.size()-1]->printStatus());};
  };
  return vec2;
}

//find objects of the given name (includes : and ' and selection specifications)
//if rpos>0 then only take rpos objects from the right side
//if rpos<0 then drops the last -npos objects from the right side
//rpos is defaulted to 0

vector<objectid> Data::IDs(String s) {return DPtrToOid(Find(s));}
objectid Data::ID(String s) {return DPtrToOid(Find(s)).front();}

/*
Returns a reference  to the object with the given object ID or Name
 */

Data& Data::operator[] (String name) {return Object_Name(name);}
Data& Data::operator[] (objectid oid) {return Object_ID(oid);}

Data& Data::Object(String name) {return Object_Name(name);}
Data& Data::Object(objectid oid) {return Object_ID(oid);}
Data& Data::Object(Data &d) {return d;}


Data& Data::Object_Name(String name) {
  Data* ptr=Ptr(name);
  if (ptr==&NullObject || ptr==NULL) {ERROR("Object_Name: Object of name=" << name << " not found.");};
  return *ptr;
}

Data& Data::Object_ID(objectid oid) {
  Data* ptr=Ptr(oid);
  if (ptr==&NullObject || ptr==NULL) {ERROR("Object_ID: Object of ID=" << oid << " not found.");};
  return *ptr;
}

Data& Data::Object_Ref(Data &d) {
  return d;
}


/*
Returns a pointer to the object with the given Object ID
 */
Data* Data::Ptr(objectid oid) {
  if (data.superior!=NULL) {
    if (oid<data.superior->data_objects.size()){
      return (data.superior->data_objects[oid]);
    } else {ERROR("Ptr: OID too large.");}}
  else {ERROR("Ptr: No superior exists ...!? (internal error).");};
  return &NullObject;
}

/*
Returns a pointer to the first object with the given name
 */
Data * Data::Ptr(String name) {
  vector<Data*> vec = Find(name);
  if (vec.size()==0) {
    return &NullObject;
  } else {return vec.front();};
}

vector<Data*> Data::Find(String s, const int rpos) {
  vector<Data*> vec_in,vec_out;
  bool found=true;
  bool selection;
  String name;
  vector<String> names,elems;
  vector<DIRECTION> dirs;
  parse_record_name_to_vector(s, names, dirs);
  DBG("Find: names=" << s); 
  DBG2("names="); DBG3(printvec(names));
  DBG2("Directions="); DBG3(printvec_txt(dirs,&direction_txt)); 
  objectid i,beg=0,end=names.size()-1;
  objectid j,n;
  vec_in.push_back(this); n=1;
  if (rpos>0) {beg=max(end-rpos+1,0);};
  if (rpos<0) {end=end+rpos;};
  DBG("beg=" << beg << " end=" << end);
  for (i=beg;i<=end && n>0;i++) {
    selection=parse_record_selection_to_vector(names[i],name,elems);
    DBG2("parse_record_selection_to_vector: names[" << i << "]=" << names[i] << " selection=" << tf_txt(selection) << " name=" << name << " elems="); 
    DBG3(printvec(elems));
    vec_out.clear(); 
    for (j=0;j<n;j++) {
      DBG("pre_append : " << "j=" <<j << " n=" << n << " vec_out.size()=" << vec_out.size());
      if (selection) {
	vec_append(vec_out,vec_in[j]->select_relatives(name,elems,dirs[i]));
      } else {
	vec_append(vec_out,vec_in[j]->find_relatives(name,dirs[i]));
      };
      DBG("post_append: " << "j=" <<j << " n=" << n << " vec_out.size()=" << vec_out.size());
    };
    vec_in=vec_out; n=vec_in.size();
    DBG("n=" << n << "vec_in.size()=" << vec_in.size());
    DBG2("vec_in="); DBG3(printvec(vec_in));
  };
  return vec_in;
}

//Sequentially go through the list  of names and find the corresponding name in the net, one after the other.
// This corresponds to finding fields in records, such as: name1.name2.name3
//returns the NullObject if not found and "this" if the vector is of length 0.


Data* Data::find_names(vector<String> names, vector<DIRECTION> dir) {
  if (this==&NullObject) {ERROR("Operation on NullObject not allowed."); return &NullObject;};
  Data *D_ptr=this; 
  objectid i;
  for (i=0;i<names.size() && D_ptr!=&NullObject;i++){
    D_ptr=D_ptr->find_name(names[i],dir[i]);
  };
  return D_ptr;
}

//Check if an object is empty, i.e. has neither function nor data vector ...


bool Data::Empty(){
  return (data.of_ptr==NULL && data.d_ptr==NULL);
}

/*
  Returns a parameter of an object. This means that for a relative of the current object is searched that has the name  "name". If found the value of that object is returned, otherwise a default value.

*/
template <class T>
T Data::getParameter(String name, T defval){
  Data * dp;
  dp = Ptr(name); 
  DBG("getParameter: name=" << name << " ptr=" << dp << " NullObject=" << &NullObject << " Empty=" << tf_txt(dp->Empty()));
  if (dp==&NullObject || dp->Empty()) {
    DBG("getParameter: defval=" << defval);
    return defval;
  } else {
    DBG("getParameter: val=" << dp->getOne<T>());
    return dp->getOne<T>();
  };
}


/*
This looks for the first object that is available on port 0 and fill
the data vector. This is mainly used by DataFunc functions to get the 
vector to work on.
*/

template <class T>
void Data::getFirstFromVector(vector<T> &v, Vector_Selector* vs){
  if (data.from.size()>0) {
    if (data.from[0].ref!=NULL) {
      data.from[0].ref->get(v,vs,0);
    } else {
      ERROR("getFirstFromVector: Pointer to first related object = NULL, object=" << getName());
    };
  } else {
    ERROR("getFirstFromVector: No related object. Object=" << getName());
  };
}


//Thin python wrapper for get
template <class T> 
void Data::get_1_(vector<T> &v){get(v);};


//Creates an empty vector of proper type and calls a get on the object, throwing the vector away afterwards.
//This is a bit ugly?
void Data::redo(){
  DEF_D_PTR( Integer );
  switch (getType()) {
#define SW_TYPE_COMM(EXT,TYPE) \
  d_ptr_##EXT=new vector<TYPE>;\
  get(*d_ptr_##EXT);            \
  delete d_ptr_##EXT
#include "switch-type.cc"
  };
}

//The vector Selector will overwrite the default Selector of the data set

//The basic get function described above extensively
template <class T> 
void Data::get(vector<T> &v, Vector_Selector *vs, objectid port) {
  objectid checkmod;
  DATATYPE type=WhichType<T>();
  DEF_D_PTR(T);
  ObjectFunctionClass* f_ptr=getFunction();
  d_ptr_v = data.d_ptr;

  Vector_Selector *s_ptr;
  if (vs!=NULL) {s_ptr=vs;} else {s_ptr=data.s_ptr;};
  F_VECTOR_SELECTOR_DEFINITION( s_ptr );

  DBG("get: name=" << data.name << ", port=" << port << ", vs1=" << vs1 << ", vs2=" << vs2);

  if (f_ptr==NULL) {   //No function but data with different type
    if (d_ptr_v==NULL) {MESSAGE("No Data in Data Object!"); return;};
    switch (data.type) {  //here we just have a data vector present, so just copy the data buffer to the vector
#define SW_TYPE_COMM(EXT,TYPE) \
       copycast_vec<TYPE,T>(d_ptr_v, &v, vs1)
#include "switch-type.cc"
      ERROR("get: Undefined Datatype encountered while retrieving vector.");
      return;
    };
  } else { //f_ptr != NULL - that means we need to execute a function
      DBG("get: Function " << f_ptr->getName() << " f_ptr=" << f_ptr);
      if (d_ptr_v==NULL) {  //here we have just a function, so execute
			    //it, with output going to the vector to
			    //be returned
	switch (type){
#define SW_TYPE_COMM(EXT,TYPE) \
	  DBG("get: Calling .process of function " << f_ptr->getName() << "<" << #TYPE << "> started. ");\
	  f_ptr->process_##EXT(static_cast<vector<TYPE>*>(static_cast<void*>(&v)),this, vs1);  //Note: only one of these is actually ever called
#include "switch-type.cc"
	  ERROR("Error (get): Unknown type encountered while processing an object function.");
	};  //end switch types
	data.len=v.size();
      } else {  //function and vector (buffer) are present, so execute
		//function if network in the from-direction has been
		//modified, with output to internal data vector and
		//copy that to the output vector
	checkmod=checkModification();
	DBG("GET: name=" << data.name << " checkmod=" << checkmod);
	if (checkmod>0) {
	  switch (type){
#define SW_TYPE_COMM(EXT,TYPE) \
            f_ptr->process_##EXT(d_ptr_##EXT,this, vs1);\
            copycast_vec<TYPE,T>(d_ptr_v, &v)
#include "switch-type.cc"
	    ERROR("Error (get): unknown type.");
	  };
	};
	data.len=v.size();
      }; //end else d_ptr!=0 && f_ptr!=0
  }; // end else f_ptr!=0
  
  if (vs2!=NULL) {v=(*vs2).get(v);}; //Select on values if necessary ....
  clearModification(port);
}

Data::Data(String name,superior_container * superior){

  magiccode = MAGICCODE; //allows one to verify the data object as such

  DBG("Data: name=" << name << " this=" << this);
  data.name=name;
  data.origin=REF_NONE;
  data.buflen=DEF_DATA_BUFFER_SIZE;
  data.len=0;
  data.dimensions.push_back(0);
  data.dataclass=DAT_CONTAINER;
  data.type=UNDEF;
  data.unit_ptr=NULL;
  data.prop_ptr=NULL;
  data.s_ptr=NULL;
  data.of_ptr=NULL;
  data.d_ptr=NULL;
  //There is no superior, so create one
  if (superior==NULL){
    DBG("Data: Create new superior");
    data.superior = new superior_container;
    data.oid=0; 
    data.superior->root=this;
    data.superior->no_of_data_objects=1;
    data.superior->data_objects.push_back(this);
    DBG("Data: Create new Data Func Library Class");
    data.superior->library_ptr=new DataFuncLibraryClass;
    //Initialize the data function libraries
    DBG("Data: Publish library functions Sys");
    DataFunc_Sys_Library_publish(data.superior->library_ptr); 
    DBG("Data: Publish library functions Qt");
    DataFunc_Qt_Library_publish(data.superior->library_ptr); 
  } else {
    data.superior=superior;
    //The superior vector is densely filled, so expand it
    if (data.superior->no_of_data_objects>=data.superior->data_objects.size()) {
      if (data.superior->no_of_data_objects > data.superior->data_objects.size()) {
	ERROR("Data Constructor: no_of_data_objects larger than vector size. Internal error.");
      }
      data.superior->data_objects.push_back(this);
      data.superior->no_of_data_objects=data.superior->data_objects.size();
      data.oid=data.superior->no_of_data_objects-1;
    } else {
    //The superior vector has gaps, so find them
      data.oid=0;
      while (data.oid<data.superior->data_objects.size()-1 && data.superior->data_objects[data.oid]!=NULL ) {
	data.oid++;
      };
      if (data.oid>=data.superior->data_objects.size()-1 || data.superior->data_objects[data.oid]!=NULL) {
	ERROR("Data Constructor: Found no free OID for new object. no_of_data_objects and data_objects are inconsistent. Internal error.");
      } else {
	data.superior->no_of_data_objects++;
	data.superior->data_objects[data.oid]=this;
	DBG("Data Constructor: new object id=" << data.oid);
      };
    };
  };
}

Data::~Data() {
  objectid i,size;
  DBG("Data Destructor: Deleting name=" << getName() << " Oid=" << getOid());
  DBG3(printAllStatus());
  if (data.superior!=NULL) {
    if (getOid()==0) {
      DBG("Destructor: Deleting root object and all related objects");
      for (i=data.superior->data_objects.size()-1;i>0;i--) { //Delete all other objects in superior
	if (test_data_object_ptr(data.superior->data_objects[i])) {
	  data.superior->data_objects[i]->~Data();
	};
      };
    };
    data.superior->data_objects[data.oid]=&NullObject;
    data.superior->no_of_data_objects--;
    if (data.oid==data.superior->data_objects.size()-1) {
      data.superior->data_objects.pop_back();
    };
    //delete the superior if this was the last object in the network
    if (data.superior->no_of_data_objects==0) {
      DBG("Data Destructor: Delete superior");
      delete data.superior->library_ptr;
      data.superior->library_ptr=NULL;
      delete data.superior;
      data.superior=NULL;
    };
  };
  delFunction(); //Deletes the function pointer and local static variables, calls the destructor
  if (data.s_ptr!=NULL) {DBG("Delete Selector " << sel()); del_sel();};
  if (data.d_ptr!=NULL) {DBG("Delete Object: name=" << data.name << ", ptr=" << data.d_ptr); delData();};
  if (data.unit_ptr!=NULL) {delete data.unit_ptr;};
  if (data.prop_ptr!=NULL) {delete data.prop_ptr;};
  size=data.to.size()-1; for (i=size;i>=0;i--) {delLink(i,DIR_TO);};
  size=data.from.size()-1; for (i=size;i>=0;i--) {delLink(i,DIR_FROM);};
}

//Create a new object with name. The name may contain record specification (:'=) to select the parents of the 
//new object otherwise it is the current object. Automatically adds a link to the parent


void Data::delObject_ID(objectid oid)
{
  Ptr(oid)->~Data();
}

void Data::delObject_Name(String name)
{
  Ptr(name)->~Data();
}

void Data::delObject(String name)
{
  Ptr(name)->~Data();
}

void Data::delObject(objectid oid)
{
  Ptr(oid)->~Data();
}


vector<objectid> Data::newObjects_ID(String name,DIRECTION dir_type) 
{
  vector<Data*> objects = newObjects(name,dir_type);
  return DPtrToOid(objects);
}

vector<Data*> Data::newObjects(String name, DIRECTION dir_type) 
{
  vector<Data*> parents;
  vector<Data*> children;
  DIRECTION dir;
  int n1,n2=0;
  //Determine the direction of the link. If name is preceded by ' then the new object links to the old one
  //and is not derived from it.
  n1=string_rfind_chars(name,RECORD_SEP,n2);
  if ((n1>0) && (n2==1)) {dir=DIR_TO;} else {dir=DIR_FROM;};
  DBG("newObjects: determine direction n1 = " << n1 << " n2=" << n2 << " dir=" << direction_txt(dir));
  String newname=string_rfind_chars_substr(name);
  objectid i;
  if (name!=newname) {parents=Find(name,-1);} else {parents.push_back(this);};
  if (parents.size()==0) {
    ERROR("new(\"" << name << "\"): Parent not found!");
  } else {
    for (i=0; i<parents.size(); i++) {
      if (parents[i]!=&NullObject) {
	DBG("Creating new object: " << newname << " i_parent=" << i);
	children.push_back(new Data(newname,data.superior));
	DBG("No of objects=" << data.superior->no_of_data_objects);
	children.back()->setLink(parents[i],dir_type,dir,-1);
      }
      else {ERROR("new(\"" << name << "\"[" << i <<"]): Parent pointer = &NullObject - that should not happend!");};
    };
  };
  return children;
}

template <class T>
vector<Data*> Data::newObjects(String name, T val, DIRECTION dir_type) {
  vector<Data*> dobs;
  objectid i;
  dobs=newObjects(name,dir_type);
  for (i=0;i<dobs.size();i++) {dobs[i]->putOne(val);};
  return dobs;
}

template <class T>
vector<Data*> Data::newObjects(String name, vector<T> vec, DIRECTION dir_type) {
  vector<Data*> dobs;
  objectid i;
  dobs=newObjects(name,dir_type);
  for (i=0;i<dobs.size();i++) {dobs[i]->put(vec);};
  return dobs;
}


//------------------------------------------------------------------------
//End Data class
//------------------------------------------------------------------------




/* ------------------------------------------------------------------------
Test Routines
------------------------------------------------------------------------ 
Currently no longer used - tests are done from Python now.

*/

bool test_result_verbose = true;
map<String,bool> test_results;

template <class T>
bool test_result(String routine, vector<T> v1, vector<T> v2){
  bool result=(v1==v2);
  test_results[routine]=result;
  if (test_result_verbose) {
    cout << "Test Result for " << routine <<endl;
    cout << "Expected: "; printvec(v1);
    cout << "Result  : "; printvec(v2);
  };
  cout << "TEST " << routine << " " << pf_txt(result) << endl;
  return result;
}

template <class T>
bool test_result(String routine, T v1, T v2){
  bool result=(v1==v2);
  test_results[routine]=result;
  if (test_result_verbose) {
    cout << "Test Result for " << routine <<endl;
    cout << "Expected: " << v1 << endl;
    cout << "Result  : " << v2 << endl;
  };
  cout << "TEST " << routine << ": " << pf_txt(result) << endl;
  return result;
}

int test_parse() {
  vector<DIRECTION> v;
  vector<String> s;
  String name;
  name="LOFAR:Station:Ant'inc'dec:list"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name="LOFAR"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name=":list"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);
  name="'inc'"; parse_record_name_to_vector(name,s,v); cout <<name<<endl; printvec(s); printvec_txt(v,&direction_txt);

  name="LOFAR:Station:Ant";
  MSG("string_rfind_chars_substr:");
  MSG("name=" << name << " result=" << string_rfind_chars_substr(name));

  MSG(" parse_record_selection_to_vector");
  parse_record_selection_to_vector("Field=1", name, s); cout <<"Field=1: " <<name<<": "; printvec(s);
  parse_record_selection_to_vector("Field=a,b,c", name, s); cout <<"Field=a,b,c: " <<name<<": "; printvec(s);

}

String mytest(){cout << "Hello HF World" << endl; return "Hi!";}
vector<int> mytest2(){
  vector<int> v; 
  for (int i=0;i<10; i++) {v.push_back(i);};
  cout << "mytest2" << endl; 
  return v;}
vector<String> mytest3(){
  vector<String> v; 
  for (int i=0;i<10; i++) {v.push_back("Hi");};
  cout << "mytest3" << endl; 
  return v;
}



//Just a trick to make sure all instances are created, it is
// never intended to actually call this function ...
void instantiate_hfget(DATATYPE type){

  Data d;
  Vector_Selector vs;

  DEF_D_PTR(Integer);
  DEF_VAL(Integer);

  switch (type){

#define SW_TYPE_COMM(EXT,TYPE) \
vectostring(*d_ptr_##EXT); \
d.getParameter("NIX",*val_##EXT);\
d.getFirstFromVector(*d_ptr_##EXT, &vs);\
d.putOne(*val_##EXT);\
d.put(*d_ptr_##EXT);\
WhichType<TYPE>();\
printvec(*d_ptr_##EXT,8);\
printvec_noendl(*d_ptr_##EXT,8);\
set_ptr_to_value(NULL, INTEGER, *val_##EXT);\
set_ptr_to_value<TYPE>(NULL, INTEGER,*val_##EXT);\
cast_ptr_to_value<TYPE>(NULL, INTEGER)
#include "switch-type.cc"
  }
}


#include "hfget.hpp"
 
