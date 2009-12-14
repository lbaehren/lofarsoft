//#define DBG_MODE 0
//#define DBG_MODE 1

// CASA/casacore header files

//using casa::Bool;
//using casa::Double;
//using casa::Matrix;
//using casa::String;
//using casa::Vector;
//using casa::Array;

using namespace std;
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

#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>


//#include <QtGui/QApplication>
//#include <QtGui/QtGui>

//#include <mgl/mgl_qt.h>

//#include <GUI/mainwindow.h>
#include <GUI/hfdefs.h>
#include <GUI/hfcast.h> 
#include <GUI/hfget.h>
#include <GUI/hffuncs.awk.h>
#include <GUI/hfanalysis.h>  
#include <crtools.h>

#include "IO/LopesEventIn.h"
#include "IO/LOFAR_TBB.h"
#include "dal/TBB_Timeseries.h"

/*!
\brief Get the extension of the filename
 */
HString file_get_extension(HString filename){
  HInteger size=filename.size();
  HInteger pos=filename.rfind('.',size);
  pos++;
  if (pos>=size) {return "";}
  else {return filename.substr(pos,size-pos);};
}
/*!
\brief Determine the filetype based on the extension of the filename
 */

HString determine_filetype(HString filename){
  HString ext=file_get_extension(filename);
  HString typ="";
  if (ext=="event") typ="LOPESEvent";
  else if (ext=="h5") typ="LOFAR_TBB";
  return typ;
}


/*========================================================================
  class ObjectFunctionClass
  ========================================================================

  Generic class that object functions are based on

*/

//virtual functions//

/* 
The process_X methods will be defined in the object functions to call
a single templated (or not if typing is not relevant) method process
that defines the function to be performed.
*/
void ObjectFunctionClass::process_P(F_PARAMETERS_T(HPointer)) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type HPointer not defined");};
void ObjectFunctionClass::process_I(F_PARAMETERS_T(HInteger)) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type HInteger not defined");};
void ObjectFunctionClass::process_N(F_PARAMETERS_T(HNumber) ) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type HNumber not defined");};
void ObjectFunctionClass::process_C(F_PARAMETERS_T(HComplex)) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type HComplex not defined");};
void ObjectFunctionClass::process_S(F_PARAMETERS_T(HString) ) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type HString not defined");};

//These methods can be set by the programmer to initialize parametes
//at the beginning or free allocated memory at destruction time
void ObjectFunctionClass::setParameters(){};
void ObjectFunctionClass::startup(){};
void ObjectFunctionClass::cleanup(){};
void ObjectFunctionClass::process_end(){
  DBG("setLinkpathChanged(false): " << data_pointer->getName(true) << ", funcname=" << getName(true));	      
  data_pointer->setLinkpathChanged(false);	      
};


//This is a dummy function we need in order to fool the compiler
//so that templated methods of all typese are created ...
//This calls all templated methods in the class
//I currently don't know of a smater way doing this, after all the actual typing
//is only done at run time and by using void pointers, there is no other way
//the compiler knows which methods to create.
template <class T>
void ObjectFunctionClass::instantiate_one(){
  vector<T> vec; T val; HString s = "";
  setParameter(s,val);
  getParameter(s,vec);
  putParameter(s,vec);
  getParameterDefault<T>(s);
  putResult("",val);
}; 
//This ensures all templates for all known types are created
//The user could change this and only create selected ones.
void ObjectFunctionClass::instantiate(){
  DATATYPE* type;
  switch (*type) {
#define SW_TYPE_COMM(EXT,TYPE) instantiate_one<TYPE>();
#include "switch-type.cc"
  };
}

/*!  Function to set names and default values of parameters used in the
function. It is also possible to set a default value in case that object is
not found.  The external names are names of other objects in the network that
contain the parameter values. If not given the object name is the parameter
name plus a prefix (either ' or ":").

I have to use explicit overloading here
*/

void ObjectFunctionClass::setParameter(HString internal_name, HPointer default_value, HString prefix, HString external_name){setParameterT(internal_name, default_value, prefix, external_name);};
void ObjectFunctionClass::setParameter(HString internal_name, HInteger default_value, HString prefix, HString external_name){setParameterT(internal_name, default_value, prefix, external_name);};
void ObjectFunctionClass::setParameter(HString internal_name, HNumber default_value, HString prefix, HString external_name){setParameterT(internal_name, default_value, prefix, external_name);};
void ObjectFunctionClass::setParameter(HString internal_name, HComplex default_value, HString prefix, HString external_name){setParameterT(internal_name, default_value, prefix, external_name);};
void ObjectFunctionClass::setParameter(HString internal_name, HString default_value, HString prefix, HString external_name){setParameterT(internal_name, default_value, prefix, external_name);};


template <class T>  
void ObjectFunctionClass::setParameterT(HString internal_name, T default_value, HString prefix, HString external_name){
  DATATYPE type=WhichType<T>();
  HString ext_name;
  if (external_name=="") {ext_name=internal_name;} else {ext_name=external_name;};
  ext_name=prefix+ext_name;
  pit=parameter_list.find(internal_name);
  parameter_item p_i;
  DBG("ObjectFunctionClass::setParameter: internal_name=" << internal_name 
      << ", ext_name=" << ext_name << ", default_value=" << default_value 
      << " Type=" << datatype_txt(type));
  if (pit != parameter_list.end()) {  //Name already exists
    if ((pit->second).type==type) {  // and type is the same
      *(static_cast<T*>(p_i.ptr))=default_value; //just assign a new value
      return;
    }
    else { //need to delete old value first to make room for a new one of different type
      del_value(p_i.ptr,type);
    };
  };

  p_i.name=ext_name;
  p_i.type=type;
  p_i.ptr=new T;
  set_ptr_to_value(p_i.ptr,p_i.type,default_value);
  parameter_list[internal_name]=p_i;
}

/*!
\brief Retrieves all parameters from the network and create them, if they do not exist. 

In this form it is only useful at the time of initialization. Since
the value is not stored locally yet.

 */
void ObjectFunctionClass::getParameters(){
  pit=parameter_list.begin();
  SaveCall(data_pointer){
    while (pit!=parameter_list.end()){
      switch (pit->second.type) {
#define SW_TYPE_COMM(EXT,TYPE)						\
	getParameter(pit->second.name,*(static_cast<TYPE*>(pit->second.ptr)));
#include "switch-type.cc"
	ERROR("getParameters: Undefined Datatype encountered while retrieving vector." << " Object name=" << data_pointer->getName(true));
      };
      pit++;
    };
  };
}

HString ObjectFunctionClass::getParametersObjectName(){ 
  return "'Parameters="+data_pointer->getName();
}

HString ObjectFunctionClass::getResultsObjectName(){ 
  return ":Results="+data_pointer->getName();
}


/*! 
\brief Find the object named "Parameters", which is linked to all
objects containing parameters of the current (function) object. Create
it, if not found.
 */
Data* ObjectFunctionClass::getParametersObject(){
  DataList pobjs=data_pointer->Find(getParametersObjectName());
  if (pobjs.size()==0) {
    Data* pobj;
    pobj=data_pointer->newObject("'Parameters",data_pointer->getDefaultDirection());
    pobj->setNetLevel(999);
    vector<HString> parlist=getParameterList(data_pointer->getName());
    pobj->put_silent(parlist);
    return pobj;
  } else {
    return pobjs[0];
  };
}

/*!  Find the object named "Result", which is linked to all
objects containing results from the current (function) object. Create
it, if not found.
 */
Data* ObjectFunctionClass::getResultsObject(){
  DataList objs=data_pointer->Find(getResultsObjectName());
  if (objs.size()==0) {
    Data* obj;
    obj=data_pointer->newObject("Results");
    obj->setNetLevel(999);
    obj->putOne_silent(data_pointer->getName());
    return obj;
  } else {
    return objs[0];
  };
}

/*!
\brief Find (and create if necessary) an object containing the
parameter "name" for an operation.

 First search for an object connected to a Parameters (note Plural)
object, i.e. an object of name "Parameter" which by default is linked to
all Parameter (note Singuar) objects.

To improve efficiency, the pointer to the data objects are
stored. Hence the second time the function is called one need not
search for the object again. This happens only, when the network path
was changed (link changed or object deleted/added).

THIS ONLY WOKRS FOR PUSH CONNECTIONS - A VERSION FOR PULL CONNECTIONS
IS NOT YET IMPLEMENTED.
*/

Data* ObjectFunctionClass::getParameterObject(HString name){
  if (data_pointer->getLinkpathChanged()) {
    DBG("getParameter(" << data_pointer->getName(true) <<"): Searching object " << name);
    Data* pobj=getParametersObject(); //will be  created if it does not exist
    DataList objs=pobj->Find("'"+name); // First search Object linked to Parameter object
    if (objs.size()==0) {
      objs=data_pointer->Find("'"+name); // Search entire net
      if (objs.size()==0) { // Still not found? Then create it.
	objs.push_back(pobj->newObject(name,data_pointer->getDefaultDirection()));
	objs[0]->setNetLevel(100);
      };
    };
    setParameterObjectPointer(name,objs[0]);
    return objs[0];
  } else {
    Data * obj=getParameterObjectPointer(name);
    if (!isDataObject(obj)){
      ERROR("getParameter(" << data_pointer->getName(true) <<"): Pointer to parameter " << name << " was not properly cached. System error.");
      data_pointer->setLinkpathChanged(true);
      obj=getParameterObject(name);
    };
    return obj;
  }
} 

/*!

\brief Retrieves the pointer to a parameter object (used for temporary
cashing of the object pointer) - assumes that the network structure
has not changed, since the last setParametersObjectPointer operation.

 */


Data* ObjectFunctionClass::getParameterObjectPointer(HString name){
  ppit=parameter_pointer.find(name);
  if (ppit==parameter_pointer.end()) return &NullObject;
  else return ppit->second;
}

/*!

\brief Stores the pointer to a parameter object for temporary cashing
until the network has changed again

 */
void ObjectFunctionClass::setParameterObjectPointer(HString name, Data* ptr){ parameter_pointer[name]=ptr;}

/*!
\brief Retrieve the corresponding parameter object (with name "name") and return its value. If object does not exist, create it and return and assign the default value
*/

template <class T>
T ObjectFunctionClass::getParameter(const HString name, const T defval){
  Data * obj=getParameterObject(name);
  if (obj->Empty()) {
    obj->noMod()->putOne(defval);
    return defval;
  } else {
    return obj->getOne<T>();
  };
}

/*!
\brief Retrieve the corresponding parameter object (with name "name") and return its value in the vector (vec). If object does not exist, create it and return and assign the default values which are assumed to be in vec. 
*/

template <class T>
void ObjectFunctionClass::getParameter(const HString name, vector<T> &vec){
  Data * obj=getParameterObject(name);
  if (obj->Empty()) {
    obj->noMod()->put(vec);
  } else {
    obj->get(vec);
  };
}

/*!
This will look for a parameter object of name "name" and then store in it (using "put") the value val.

A parameter object is in principle a normal object, but there is a
particular order for searching for it (e.g., looking for one that is
connected to an object called "Parameter" that belongs to the current
object).
 */
template <class T>
T ObjectFunctionClass::putParameter(const HString name, const T val){
  Data * obj=getParameterObject(name);
  obj->noMod()->putOne(val);
}

/*!
This will look for a parameter object of name "name" and then store in it (using "put") the vector vec.

A parameter object is in principle a normal object, but there is a
particular order for searching for it (e.g., looking for one that is
connected to an object called "Parameter" that belongs to the current
object).
 */
template <class T>
T ObjectFunctionClass::putParameter(const HString name, vector<T>& vec){
  Data * obj=getParameterObject(name);
  obj->noMod()->put(vec);
}


/*!
\brief Find (and create if necessary) an object containing the result of an operation.
 */
Data* ObjectFunctionClass::getResultObject(HString name){
  Data* robj=getResultsObject(); //will be  created if it does not exist
  DataList objs=robj->Find("'"+name); // First search Object linked to Results object
  if (objs.size()==0) {
    Data* obj=data_pointer->insertNew(name,robj);
    obj->setNetLevel(100);
    return obj;}
  else {
    return objs[0];
  };
}

/*!

\brief Stores the result of an operation in a result object, which will be created if necessary.

This method will find create a result object (if necessary) and attach
to it an object containing output of the operation from the currently
active data object. Will be called at the end of a Data object
function call.

 */
template <class T>  
Data* ObjectFunctionClass::putResult(HString name,vector<T> vec){
  Data* robj=getResultObject(name);
  robj->put_silent(vec);
  return data_pointer;
}

template <class T>  
Data* ObjectFunctionClass::putResult(HString name,T val){
  Data* robj=getResultObject(name);
  robj->putOne_silent(val);
  return data_pointer;
}



/*!
Returns the external name (i.e. the name of the object to be accessed) of an internal parameter
*/
HString ObjectFunctionClass::getParameterName(HString internal_name) {
  map<HString,parameter_item>::iterator it;
  DBG("ObjectFunctionClass::getParameterName: internal_name=" << internal_name);
  it=parameter_list.find(internal_name);

  if (it != parameter_list.end()) {
    return (it->second).name;
  } else {
    ERROR("ObjectFunctionClass::getParameterName: Name " << internal_name << " not found. Error in programming network function.");
    return "";
  };
}

/*!
Returns a string vector containing all defined parameters (i.e., return the external names). The string parameter first_element can be inserted as the first element.
*/

vector<HString> ObjectFunctionClass::getParameterList(HString first_element) {
  DBG("getParameterList: start.");
  map<HString,parameter_item>::iterator it;
  it=parameter_list.begin();
  vector<HString> vec;
  if (first_element!="") vec.push_back(first_element);
  while (it != parameter_list.end()) {
    vec.push_back((it->second).name);
    ++it;
  };
  return vec;
}


/*!
Returns the default value of an internal parameter, typically used  if the external name (object) does not exist. Can also be used as a static variable of an object.
*/
template <class T>
T ObjectFunctionClass::getParameterDefault(HString internal_name) {
  map<HString,parameter_item>::iterator it;
  it=parameter_list.find(internal_name);
  if (it != parameter_list.end()) {
    DBG("getParameterDefault: Type=" << datatype_txt(WhichType<T>()));
    DBG("getParameterDefault: type=" << datatype_txt((it->second).type));
    DBG("getParameterDefault: ptr=" << (it->second).ptr);
    DBG("getParameterDefault: cast<Int>=" << cast_ptr_to_value<HInteger>((it->second).ptr,(it->second).type));
    //.ptr contains a pointer to a location storing the default value.
    //cast_ptr_to_value retrieves that value under the assumption it is of type .type
    T ret=cast_ptr_to_value<T>((it->second).ptr,(it->second).type);
    DBG("ret=" << ret);
    return ret;
  } else {
    ERROR("ObjectFunctionClass::getParameterDefault: Name " << internal_name << " not found. Error in programming of a network function.");
    return mycast<T>(0);
  };
}

ObjectFunctionClass::ObjectFunctionClass(Data * dp){
  DBG("ObjectFunctionClass: Constructor");
  data_pointer=dp;
};

ObjectFunctionClass::~ObjectFunctionClass(){
  map<HString,parameter_item>::iterator it; //Now destroy all the parameter default variables, created with setParameter
  it = parameter_list.begin();
  while (it !=parameter_list.end()){
    DBG("~ObjectFunctionClass(): delete parameter " << (it->second).name);
    del_value ((it->second).ptr,(it->second).type);
    ++it;
  };
};


/*========================================================================
  class DataFuncDescriptor
  ========================================================================

This class stores informations about the functions that are being used
in an object. The Desriptor is then stored in the library (with empty
pointer) and in each object, but with a pointer to the actual instance
of the class.

*/

DataFuncDescriptor::DataFuncDescriptor(){setType(UNDEF);}

DataFuncDescriptor::~DataFuncDescriptor(){}

//Save the basic information about the function class (name, library it belongs to, and a short description)
void DataFuncDescriptor::setInfo(HString name, HString library, HString shortdocstring, HString docstring){
  DBG("DataFuncDescriptor.set: name=" << name << " library=" << library);
  fd.name=name;
  fd.library=library;
  fd.docstring=docstring;
  fd.shortdocstring=shortdocstring;
}

/*
Used to describe the default behaviour during creation of function. Is a vector buffer present? And what is its type
*/
void DataFuncDescriptor::setType(DATATYPE typ){fd.deftype=typ;}
bool DataFuncDescriptor::getBuffered(){return fd.buffered;}
void DataFuncDescriptor::setBuffered(bool buf){fd.buffered=buf;}

/*!
Creates a new instance of the class (on the heap) and returns a
  function descriptor, which also contains the pointer to the new
  instance. This instance will be called by "get" later on.
*/

ObjectFunctionClass* DataFuncDescriptor::newFunction(Data *dp, DATATYPE typ){
  DataFuncDescriptor fdnew;
  DATATYPE newtype=typ;
  
  //Call the Constructor of that function - defined with
  //DATAFUNC_CONSTRUCTOR (this will actually create the method and
  //hence call its constructor.
  fdnew=(*getConstructor())(dp);
  fdnew.getFunction()->setInfo(fdnew.getName(false),fdnew.getLibrary(),
			       fdnew.getDocstring(true),fdnew.getDocstring(false));
  fdnew.getFunction()->setConstructor(fdnew.getConstructor());
  fdnew.getFunction()->setFunction(fdnew.getFunction());
  //Create a new buffer if that is the default behaviour
  if (newtype==UNDEF) {newtype=fdnew.getType();};
  if (fdnew.getBuffered() && !(dp->hasData())) {
    dp->newVector(newtype);
  } else {
    dp->setType(newtype);
  };
  return fdnew.getFunction();
}
    
//returns the function to call
void DataFuncDescriptor::setFunction(ObjectFunctionClass* f_ptr){fd.f_ptr=f_ptr;}
ObjectFunctionClass* DataFuncDescriptor::getFunction(){return fd.f_ptr;}

//This is a pointer to an ordinary function, that puts the new class on the heap (constructs it)
//and returns a pointer to the instance of the class (which is retrieved by a call to f_ptr)
void DataFuncDescriptor::setConstructor(ConstructorFunctionPointer ptr){fd.constructor=ptr;}
ConstructorFunctionPointer DataFuncDescriptor::getConstructor(){return fd.constructor;}

//Return the library name the function belongs to
HString DataFuncDescriptor::getLibrary(){return fd.library;};

//Return the default type of the function 
DATATYPE DataFuncDescriptor::getType(){return fd.deftype;};

//Return the name of the function being described 
//(either with (fullname=false) or without the library name attached)
HString DataFuncDescriptor::getName(bool fullname){
  HString s;
  if (fullname){
    s= fd.library + ":" + fd.name;
  } else {
    s= fd.name;
  };
  DBG("DataFuncDescriptor.getName = " << s);
  return s;
}

//Retrieve the long or short description of the function
HString DataFuncDescriptor::getDocstring(bool shortdoc){
  if (shortdoc) {return fd.shortdocstring;}
  else {return fd.docstring;};
}


/*========================================================================
  class DataFuncLibraryClass 
  ========================================================================*/
/*
This class stores Libraries of pointers to function classes that an object can use.
*/


DataFuncLibraryClass::DataFuncLibraryClass(){
  DBG("FuncLibrary: Constructor");
  DBG("FuncLibrary: size=" << func_library.size());
}

DataFuncLibraryClass::~DataFuncLibraryClass(){}

/*! 
  \brief Adds a function constructor to the library, so that it can be called later on

Here provide the library with a pointer to the function constructing the object. 
The constructor function will be called with construct=false, which means that
only the Function Descriptor is returned, but no actual instance is created. 

When a function is later assigned to an object, then the call to the
constructor fucntion stored in the libray with construct=true is made, 
an instance is created on the heap, and the Function Descriptor will contain 
an actual pointer to the class.
*/
void DataFuncLibraryClass::addFunc(ConstructorFunctionPointer c_ptr){
  DBG ("FuncLibrary: add");
  DataFuncDescriptor fd=(*c_ptr)(false);
  HString fname=fd.getName(true);
  HString name=fd.getName(false);
  HString lib=fd.getLibrary();
  addFuncnameToLibrary(name,lib);
  func_library[fname]=fd;
}

/*!  \brief Adds the name of the function to a vector (list) of
function names of the respective library. If the library does not yet
exist, create it.

 */

void DataFuncLibraryClass::addFuncnameToLibrary(HString name,HString lib){
  map<HString,vector<HString> >::iterator fit=func_library_members.find(lib);

  if (fit==func_library_members.end()) {
    vector<HString> newlib; //If library does not exist create a new library entry
    func_library_members[lib]=newlib;
    fit=func_library_members.find(lib);
  };

  if (!in_vector(name,fit->second)) {
    (fit->second).push_back(name);
  };
}

/*! 
\brief Check whether a particular function name is in a particular library
 */
bool DataFuncLibraryClass::isFuncnameInLibrary(HString name, HString lib){
  map<HString,vector<HString> >::iterator fit=func_library_members.find(lib);
  if (fit==func_library_members.end()) return false;
  return in_vector(name,fit->second);
}


/*! 

\brief Searches all libraries and returns the name of the first one
that contains a function of name "name". Returns "" if no match is
found.

 */

HString DataFuncLibraryClass::findLibrary(HString name, bool fullname){
  map<HString,vector<HString> >::iterator fit=func_library_members.begin();
  bool found=false;
  HString lib;
  while (fit!=func_library_members.end() && !found) {
    lib=fit->first;
    if (isFuncnameInLibrary(name,lib)) found=true;
    fit++;
  };
  if (found) {
    if (fullname) return lib + ":" + name;
    else return lib;
  } else return "";
}

/*!
Retrieves the pointer to the function class
*/
ObjectFunctionClass* DataFuncLibraryClass::f_ptr(HString name, HString library){
  HString lib = library;
  if (lib=="") lib=findLibrary(name);
  it=func_library.find(lib+":"+name);
  if (it !=func_library.end()) {
    return (it->second).getFunction();
  } else {
    return NULL;
  };
}

/*!
\brief Retrieves the struct describing the function class 
*/

DataFuncDescriptor* DataFuncLibraryClass::FuncDescriptor(HString name, HString library){
  HString lib = library;
  if (lib=="") lib=findLibrary(name);
  it=func_library.find(lib+":"+name);
  if (it !=func_library.end()) {
    return &(it->second);
  } else {
    return NULL;
  };
}

/*!
\brief Checks whether a function is in the library or not
*/
bool DataFuncLibraryClass::inLibrary(HString name, HString library){
  HString lib = library;
  if (lib=="") lib=findLibrary(name);
  it=func_library.find(lib+":"+name);
  return (it !=func_library.end());
}

/*!
\brief Prints a directory of all functions in all libraries
*/
vector<HString> DataFuncLibraryClass::listFunctions(bool doprint){
  vector<HString> out;
  HString s;
  for (it=func_library.begin();it!=func_library.end();++it) {
    s=(it->second).getName(true) + " - " + (it->second).getDocstring(); 
    out.push_back(s);
    if (doprint) cout << s <<endl;
  };
  return out;
}

/*!
\brief Returs a vector of all available library names. Also prints it if doprint=true.
*/
vector<HString> DataFuncLibraryClass::getLibraries(bool doprint){
  vector<HString> out;
  map<HString,vector<HString> >::iterator fit=func_library_members.begin();
  while (fit!=func_library_members.end()) {
    out.push_back(fit->first);
    fit++;
  };
  if (doprint) printvec(out,out.size());
  return out;
}

/*!
\brief Returs a vector of all available library names. Also prints it if doprint=true.
*/
vector<HString> DataFuncLibraryClass::getFuncnames(HString library, bool doprint){
  map<HString,vector<HString> >::iterator fit=func_library_members.find(library);
  if (fit!=func_library_members.end()) {
    if (doprint) printvec(fit->second,(fit->second).size());
    return fit->second;
  } else {
    vector<HString> out; 
    return out;
  };
}

//End DataFuncLibraryClass 
//........................................................................


/*
========================================================================
  Math functions
========================================================================

These are needed to deal with mathematical operations on strings (and
vectors).

They will be called by the process method. Maybe it would be possible
to generate the associated .process method automatically by a nawk
preprocessor step.


*/


#ifndef HF_MATH_FUNC
#undef HF_MATH_FUNC
#endif
#ifndef HF_MATH_FUNC2
#undef HF_MATH_FUNC2
#endif

#define HF_MATH_FUNC( FUNC ) \
template<>        inline HString FUNC<HString>(const HString v){return mycast<HString>(FUNC(mycast<HComplex>(v)));};\
template<>        inline HPointer FUNC<HPointer>(const HPointer v){return v;};\
template<class T> inline T FUNC(const T v)


#define HF_MATH_FUNC2( FUNC )						\
  inline HString FUNC(const HString v1, const HString v2){return mycast<HString>(FUNC(mycast<HComplex>(v1),mycast<HComplex>(v2)));}; \
  template<class S>        inline HString FUNC(const HString v1, const S v2){return mycast<HString>(FUNC(mycast<S>(v1),v2));}; \
  template<class S>        inline HString FUNC(const S v1, const HString v2){return mycast<HString>(FUNC(v1,mycast<S>(v2)));}; \
  inline HPointer FUNC(const HPointer v1, const HPointer v2){return mycast<HPointer>(FUNC(mycast<HInteger>(v1),mycast<HInteger>(v2)));}; \
  template<class S>        inline HPointer FUNC(const HPointer v1, const S v2){return mycast<HPointer>(FUNC(mycast<S>(v1),v2));}; \
  template<class S>        inline HPointer FUNC(const S v1, const HPointer v2){return mycast<HPointer>(FUNC(v1,mycast<S>(v2)));}; \
  template<class T, class S> inline T FUNC(const T v1,const S v2)	

//---square
//template<class T> inline T hf_square(const T v)

//$ITERATE MFUNC abs,acos,asin,atan,ceil,cos,cosh,exp,floor,log,log10,sin,sinh,sqrt,tan,tanh
HF_MATH_FUNC(hf_$MFUNC){return $MFUNC(v);};
//$ENDITERATE

HF_MATH_FUNC(hf_square){return v*v;}; 
HF_MATH_FUNC(hf_negative){return -v;}; 
HF_MATH_FUNC(hf_ssqrt){if (v<0) return -sqrt(-v); else return sqrt(v);}; 

HF_MATH_FUNC2(hf_sub){return v1-v2;};
HF_MATH_FUNC2(hf_mul){return v1*v2;}; 
HF_MATH_FUNC2(hf_add){return v1+v2;}; 
HF_MATH_FUNC2(hf_div){return v1/v2;}; 
HF_MATH_FUNC2(hf_pow){return pow(v1,v2);}; 

template<class T> inline T hf_phase(const T v){return mycast<T>(0.0);};
template<> inline HComplex hf_phase<HComplex>(const HComplex v){return arg(v);}
template<class T> inline T phase(const T v) {return arg(v);}

//--End Math functions-----------------------------------------------------


/*========================================================================
  Define DataFunc Object Library "Sys"
  ========================================================================

This contains a library of some basic functions that can be assigned
to objects for exectuion in the network.

There are a few special macros used here:

DEFINE_PROCESS_CALLS: this defines the virtual methods process_X,
which will call the non-virtual put templated method process<T>. Use
F_PARAMETERS to define the function variable list.

DEFINE_PROCESS_CALLS_NUMONLY: only defines the calls for numeric
datatypes, allowing one to do an explicit specialization for Strings
and Pointers (See Sys_Range for an example).

DEFINE_PROCESS_CALLS_IGNORE_DATATYPE: this defines the virtual methods
process_X, which will call the non-virtual non-templated method
process. Use F_PARAMETERS-NOVEC to define the function variable
list. Use this, if the function does not perform any operation on the
data vector in the object.

DATAFUNC_CONSTRUCTOR(Name,Library,Doc): is a macro that needs to come
AFTER every new class definiton to generate a constructor function
that generates a description of the function for the library AND
constructs an instance of the class when it is assigned to an object.

In addition the class needs to be "added" to the library in the
"DataFunc_XXX_Library_publish" function. Ideally the macro would do
this automatically, but it doesn't yet ...

Also, the "DataFunc_XXX_Library_publish" functions needs to be called
by the constructor of the data function - also not yet an ideal
situation. I'd prefer a dynamic solution.

A somewhat smarter preprocesser (like an (n)awk-script) could probably
do that easily.

AND IN FACT THAT IS WHAT FOLLOWS NOW ....
*/

//The folllowing are definitions that will be picked up by an
//awkscript and applied to the subsequent functions.

//------------------------------------------------------------------------------
//$DEFINE PREPROCESSOR
/*------------------------------------------------------------------------------
$INDEX: Function Lib Name
$BEGIN: Function class DataFunc_$Lib_$Name : public ObjectFunctionClass {	\
public:\
DEFINE_PROCESS_CALLS\
 DataFunc_$Lib_$Name (Data* dp) : ObjectFunctionClass(dp){	\
   dp->setUpdateable($updateable);		\
    setParameters();\
    startup();\
    getParameters();\
    }\
 ~DataFunc_$Lib_$Name(){cleanup(); } \
 \
void setParameters(){\
SET_FUNC_VECPARAMETER_AWK($*VecPar);\
SET_FUNC_PARAMETER_AWK($*Par);\
};\
 \
template <class T> void process(F_PARAMETERS) {\  
  GET_FUNC_VECPARAMETER_AWK($*VecPar);\
  GET_FUNC_PARAMETER_AWK($*Par);

$END: Function }; DATAFUNC_CONSTRUCTOR($Name,$Lib,"$Info",$Type,$buffered);
$PUBLISH: Function PUBLISH_OBJECT_FUNCTION($Lib,$Name); 
------------------------------------------------------------------------------*/


//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Sys
Name: Unit
Info: Multiplies the data with a unit scale factor and also returns the apropriate unit string.
Type: NUMBER
buffered: false
updateable: false
Par: UnitName, HString, ""
Par: UnitPrefix, HString,""
Par: UnitScaleFactor, HNumber, 1.0
------------------------------------------------------------------------------*/
$$ { 
  dp->getFirstFromVector(*vp,vs);
  INIT_FUNC_ITERATORS(it,end);
  while (it!=end) {*it=hf_div(*it,UnitScaleFactor);++it;};
}
//$END Function -----------------------------------------------------------------



//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Sys
Name: copy
Info: Copies the content of one object to the next.
Type: NUMBER
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${
  dp->getFirstFromVector(*vp,vs);
 }
//$END Function -----------------------------------------------------------------



//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Sys
Name: Neighbours
Info: Returns a list of names of all neighbour objects, first in To direction and the in FROM direction separated by an empty string.
Type: STRING
buffered: false
updateable: true
------------------------------------------------------------------------------*/
$${
  vector<HString> vec;
  vec=dp->getNeighbourNames(DIR_TO);
  vec.push_back("");
  vec_append(vec,dp->getNeighbourNames(DIR_FROM));
  copycast_vec<HString,T>(&vec,vp);
}
//$END Function -----------------------------------------------------------------

//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: RunningAverage
Info: Calculates the running averages over multiple samples of an vector using flat, linear, or Gaussian weighting 
Type: NUMBER
buffered: false
updateable: false
Par: RunningAverageWeightType, HString, "GAUSSIAN"
Par: RunningAverageLength, HInteger, 7
------------------------------------------------------------------------------*/
$$ { 
  vector<T> tmpvec;
  hWEIGHTS wtype=WEIGHTS_LINEAR;
  if (RunningAverageWeightType=="GAUSSIAN") wtype=WEIGHTS_GAUSSIAN;
  else if (RunningAverageWeightType=="FLAT") wtype=WEIGHTS_FLAT;
  dp->getFirstFromVector(tmpvec,vs);
  hRunningAverageVec(tmpvec,*vp, RunningAverageLength, wtype);
}
//$END Function -----------------------------------------------------------------



//$ITERATE MFUNC square,negative,ssqrt,abs,acos,asin,atan,ceil,cos,cosh,exp,floor,log,log10,sin,sinh,sqrt,tan,tanh
//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: $MFUNC
Info: Takes the $MFUNC of the first object vector connected to the object.
Type: NUMBER
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${ 
  dp->getFirstFromVector(*vp,vs);

  INIT_FUNC_ITERATORS(it,end);
 
  while (it!=end) {
   *it=hf_$MFUNC(*it);
   ++it;
  };
}
//$END Function -----------------------------------------------------------------
//$ENDITERATE

//$ITERATE MFUNC phase,imag,conj,real

//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: $MFUNC
Info: Retrieves $MFUNC of the first object vector connected to the object.
Type: COMPLEX
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${ 
  vector<HComplex>* cvp;
  if (WhichType<T>()==COMPLEX) cvp=reinterpret_cast<vector<HComplex>*>(vp); // no need for new vector
  else cvp=new vector<HComplex>;

  dp->getFirstFromVector(*cvp,vs);
  vp->resize(cvp->size(),hfnull<T>());

  ITERATORS(HComplex,cvp,it1,end1);    
  ITERATORS_T(vp,it2,end2);    
 
  while (it1!=end1 && it2!=end2) {
    *it2=mycast<T>($MFUNC(*it1));
    it1++; it2++; 
  };
}
//$END Function -----------------------------------------------------------------
//$ENDITERATE

//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: degree
Info: Converts radians to degrees
Type: NUMBER
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${ 
  dp->getFirstFromVector(*vp,vs);

  INIT_FUNC_ITERATORS(it,end);
  //  HNumber fac=180.0/UnitScaleFactor/M_PI;
  HNumber fac=180.0/M_PI;
  while (it!=end) {
    *it=hf_mul(*it,fac);
   ++it;
  };
}
//$END Function -----------------------------------------------------------------


//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: casatest
Info: Test conversion to casa arrays and squaring operations therewith
Type: NUMBER
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${ 
  dp->getFirstFromVector(*vp,vs);

  T * storage = &((*vp)[0]);
  casa::IPosition shape(1,vp->size()); //tell casa the size of the vector
  casa::Vector<T> cvec(shape,storage,casa::SHARE); //Create casa Vector with data storage identical to the stl vector

  //  INIT_FUNC_ITERATORS(it,end);

  typedef typename casa::Vector<T>::contiter iterator_T;
  iterator_T it=cvec.cbegin(); 
  iterator_T end=cvec.cend();  

  if (it==end) {return;};  

  while (it!=end) {
    *it=hf_mul(*it,*it);
   ++it;
  };

}
//$END Function -----------------------------------------------------------------





//$ITERATE MFUNC sub,mul,add,div,pow
//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: $MFUNC
Info: Performs the operation $MFUNC between the data vector and a reference object
Type: NUMBER
buffered: false
updateable: false
------------------------------------------------------------------------------*/
$${ 
  dp->getFirstFromVector(*vp,vs);
  vector<T> rvec;
  
  Data* robj=dp->Ptr("'Reference");
  if (!isDataObject(robj)) {
    ERROR("Math Function $MFUNC (" << dp->getName(true) << ") - No Reference object found, no calculation performed.");
    return;
  }

  robj->get(rvec,vs);
  
  if (rvec.size()==0) {
    ERROR("Math Function $MFUNC (" << dp->getName(true) << ") - No data in Reference object found, no calculation performed.");
    return;
  }

  INIT_FUNC_ITERATORS(it,end);

  if (rvec.size()==1) {
    T val = rvec[0];
    while (it!=end) {
      *it=hf_$MFUNC(*it,val);
      ++it;
    };
  } else {
    typedef typename vector<T>::iterator iterator_TT;
    iterator_TT it2=rvec.begin();
    iterator_TT end2=rvec.end();
    iterator_TT beg2=it2;
    while (it!=end) {
      *it=hf_$MFUNC(*it,*it2);
      ++it;it2++;
      if (it2==end2) {it2=beg2;};
    };
  };
}
//$END Function -----------------------------------------------------------------
//$ENDITERATE

//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Math
Name: SubtractOffset
Info: Subtracts the first elements in the data vector from the entire data vector.
Type: NUMBER
buffered: false
updateable: false
Par: OffsetValue, HNumber, 0.0
Par: OffsetFixed, HInteger, int(false)
------------------------------------------------------------------------------*/
// The following line with $$ is a comment for the preprocessor, but
// the bracket makes life easier for the editor since we have to add
// one at the end of this block as well
$$ { 
dp->getFirstFromVector(*vp,vs);

 INIT_FUNC_ITERATORS(it,end);

T off;
if (bool(OffsetFixed)) {
  off=mycast<T>(OffsetValue);
 } else {
  off=*it;
  putParameter("OffsetValue", off);
 };

while (it!=end) {
  *it=hf_sub(*it,off);
  ++it;
 };
}
//$END Function -----------------------------------------------------------------


//========================================================================
// END MATH LIBRARY FUNCTIONS
//========================================================================


//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Sys
Name: print
Info: Prints the contents of the data vector to stdout.
Type: STRING
buffered: false
updateable: true
------------------------------------------------------------------------------*/
$${
  dp->getFirstFromVector(*vp,vs);    //copy data vector from predecessor 
  cout << dp->getName() << ": ";  //and print
  printvec(*vp);
}
//$END Function -----------------------------------------------------------------



//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Sys
Name: range
Info: Return a range of numbers (0,1,2,3,4,...N). Parameter objects: start=0, end=0, inc=1
Type: INTEGER
buffered: false
updateable: true
Par: start,HInteger,0
Par: end,HInteger,1
Par: inc,HInteger,1
------------------------------------------------------------------------------*/
$${
  HInteger i,size;

  if (inc==0) {
    ERROR("DataFunc_Sys_Range: Increment inc=0!");
    return;
  };

  MSG("Range: vs1 VectorSelector not yet implemented!"); 

  size=floor(abs((end-start)/inc))+1;
  if (vp->capacity()<size) {vp->reserve(size);};

  //Fill the return data vector and make sure it is casted to the right output type
  for (i=0;i<size;i++) {vp->push_back(mycast<T>(i*inc+start));};
    
}
//$END Function -----------------------------------------------------------------



//------------------------------------------------------------------------
//End Sys Library
//------------------------------------------------------------------------


/*========================================================================
  Define DataFunc Object Library "CR"
  ========================================================================

  This library allows one to access the CR IO and analysis functions

*/


//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: CR
Name: dataReaderObject
Info: Creates a DataReader object for reading CR data and stores its pointer.
Type: POINTER
buffered: true
updateable: true
Par: Filename, HString, dataset_lopes
------------------------------------------------------------------------------*/
$$ { 

  DBG("dataReaderObject: Retrieved filename parameter =" << Filename); // gets filename from object set in hfnet.py

  //Results Objects
  HInteger date;
  HString observatory;
  HInteger filesize;
  HInteger nofAntennas;
  address maxBlocksize;
  HString Filetype;
  vector<HInteger> AntennaIDs; 
  vector<HInteger> Offsets;
  
  //Create the a pointer to the DataReader object and store the pointer
  //Here we could have if statements depending on data types

  union{HIntPointer iptr; HPointer ptr; CR::DataReader* drp;};

  //Close previous file if it was stored in the vector
  if (vp->size()>0) hCloseFile(AsIPtr(vp->at(0)));
  vp->clear();

  //Open the file and retrieve pointer to DataReader object
  iptr=hOpenFile(Filename, Offsets);

  //Store the pointer in the object, so that other objects can access
  //it. The object should actually me made read-only, since the pointer is
  //not to be changed by put ever again until the window is deleted

  vp->push_back(mycast<T>(ptr)); 

  //Read out some header information ...
  CasaRecord hdr=drp->headerRecord();
  date=hdr.asuInt("Date");
  observatory=hdr.asString("Observatory");
  filesize=hdr.asInt("Filesize");
  hdr.asArrayInt("AntennaIDs").tovector(AntennaIDs);
  nofAntennas=drp->nofAntennas();
  Filetype = determine_filetype(Filename);
  maxBlocksize=min(filesize, 1048576);

  //... and store it in results objects
  putResult("Date",date);
  putResult("Observatory",observatory);
  putResult("Filesize",filesize);
  putResult("nofAntennas",nofAntennas);
  putResult("maxBlocksize",maxBlocksize);
  putResult("Filetype",Filetype);
  putResult("AntennaIDs",AntennaIDs);
  putResult("Offsets",Offsets);
  DBG("DataFunc_CR_dataReaderObject: Success.");
}

void cleanup(){
  delete reinterpret_cast<CR::DataReader*>(data_pointer->getOne<HPointer>()); 
}


//$END Function ----------------------------------------------------------------




//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: CR
Name: dataRead
Info: Function retrieving a vector from the dataReader.
Type: NUMBER
buffered: true
updateable: true
Par: File, HPointer, NULL
Par: Antenna, HInteger, 0
Par: Blocksize, HInteger, -1
Par: maxBlocksize, HInteger, 1048576
Par: Block, HInteger, 0
Par: Filesize, HInteger, -1
Par: Stride, HInteger, 0
Par: Shift, HInteger, 0
Par: Datatype, HString, "Fx"
VecPar: Offsets, HInteger
------------------------------------------------------------------------------*/
$$ { 
  if (Blocksize<1) Blocksize=maxBlocksize;
  hReadFile(*vp,AsIPtr(File),Datatype,Antenna,Blocksize,Block,Stride,Shift,Offsets); 
  HInteger maxBlock=Filesize/Blocksize-1;	
  putResult("maxBlock",maxBlock);
}
//$END Function ----------------------------------------------------------------


/*------------------------------------------------------------------------
End DataFunc Object Library "CR"
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
Python Interface Functions
------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//$NEW: Function
/*------------------------------------------------------------------------------
Lib: Py
Name: PyFunc
Info: Function to call a user-defined python object of type hffunc
Type: POINTER
buffered: false
updateable: true
------------------------------------------------------------------------------*/
$${
  data_pointer->callSimplePyObjectMethod(data_pointer->retrievePyFunc(), "hfprocess");
}

void startup(){
  data_pointer->callSimplePyObjectMethod(data_pointer->retrievePyFunc(), "hfstartup");
}

void cleanup(){
  data_pointer->callSimplePyObjectMethod(data_pointer->retrievePyFunc(), "hfcleanup");
}
 
//$END Function ----------------------------------------------------------------

//------------------------------------------------------------------------
//Network functions
//------------------------------------------------------------------------

/* 
These functions can be called from each object to perform certain functions
 */



/*template <class T>
static void * addvecs(data_field * d) {
  vector<T> v0,vi;
  objectid i,j;
  objectid sizei, size0, sizej;
  vector<Data*>* inp;
  inp = reinterpret_cast<vector<Data*>*>(&in_v);

  v0 = (*inp).at(0)->get<T>();
  size0 = v0.size();
  sizei=(*inp).size();
  for (i=1; i<sizei; i++) {
    vi = (*inp).at(i)->get<T>();
    sizej = vi.size();
    for (j=0; j<min(size0,sizej); j++) {
      v0[j] = v0[j] + vi[j];
    };
  };
  return v0;
}

*/


int ReadTextFile(string filename)
{
  string line,name;
  int nfield=0,i,j;
  int nline=0;
  vector<string> fieldname;
  map<string,int> fieldnum;
  vector<vector<double> > fields;


  ifstream f(filename.c_str());
  if (!f)
    {
      cout << "ERROR (ReadText): File " << filename.c_str() << " not found!";
      return 1;
    }
  else
    {cout << "Opening data file " << filename << ".\n";}

  //Read first line and parse the field names
  getline(f,line);
  istringstream sline(line);
  while (sline >> name){
    fieldname.push_back(name);
    cout << "\n" << fieldname.size() << ": " << fieldname.at(fieldname.size()-1);
    fieldnum[name]=nfield;
    nfield++;
  };
  cout << "\n";

  vector<double> row(nfield);
  nline=0;
  //Read data lines
  while (getline(f,line)) {
    istringstream sline(line);
    i=0;
    while (sline >> row[i] && i<nfield){i++;};
    fields.push_back(row);
    nline++;
  };
  f.close();
  for (i=0;i<nline;i++) {
    cout << i << ": ";
    for (j=0;j<nfield;j++) {
      cout << fields[i][j]<<" ";
        };
    cout << "\n";
  };
}

#undef HF_PP_FILETYPE
#define HF_PP_FILETYPE() (GUI)  // Tell the preprocessor (for generating wrappers) that from here on this is the part where the atomatic function wrappers are generated. 
#include "hfwrappers-hfanalysis.cc.h"  // include wrapper definitions generated automatically from source


//------------------------------------------------------------------------
//Publish the Libraries - used in Data object constructor
//------------------------------------------------------------------------
//Note that this will be filled in by the preprocessor 
void DataFunc_Library_publish(DataFuncLibraryClass* library_ptr){
  //$PUBLISH Function
#undef HF_PP_FILETYPE
#define HF_PP_FILETYPE() (GUIPUBLISH)  // Tell the preprocessor (for generating wrappers) that from here on this is the part where the functions are published (and hence the WRAPPER generation macro is identified as such).
#include "hfwrappers-hfanalysis.cc.h"  // include wrapper definitions generated automatically from source
};
 
void dummy_instantiate(){
  vector<HInteger> Offsets;
  CasaVector<int> OffsetsCasa;
  aipsvec2stlvec(OffsetsCasa, Offsets);
}
#undef HF_PP_FILETYPE
