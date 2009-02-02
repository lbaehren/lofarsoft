//#define DBG_MODE 0
//#define DBG_MODE 1

// CASA/casacore header files

//using casa::Bool;
//using casa::Double;
//using casa::Matrix;
//using casa::String;
//using casa::Vector;

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

#include <QtGui/QApplication>
#include <QtGui/QtGui>

#include <mgl/mgl_qt.h>

#include <GUI/mainwindow.h>
#include <GUI/hfdefs.h>
#include <GUI/hfcast.h> 
#include <GUI/hfget.h>
#include <GUI/hffuncs.h>  


#include "Data/LopesEventIn.h"
//#include "Data/LOFAR_TBB.h"


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



//This is a dummy funciton we need in order to fool the compiler
//so that templated methods of all typese are created ...
//This calls all templated methods in the class
//I currently don't know of a smater way doing this, after all the actual typing
//is only done at run time and by using void pointers, there is no other way
//the compiler knows which methods to create.
template <class T>
void ObjectFunctionClass::instantiate_one(){
  T val; HString s = "";
  setParameter(s,val);
  getParameterDefault<T>(s);
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

I have to use explicit overloading, since 
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
  map<HString,parameter_item>::iterator it=parameter_list.find(internal_name);
  parameter_item p_i;
  DBG("ObjectFunctionClass::setParameter: internal_name=" << internal_name 
      << ", ext_name=" << ext_name << ", default_value=" << default_value 
      << " Type=" << datatype_txt(type));
  if (it != parameter_list.end()) {  //Name already exists
    if ((it->second).type==type) {  // and type is the same
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
    it++;
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
    it++;
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
Used to describe the defaul behaviour during creation of function. Is a vector buffer present? And what is its type
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

/* 
Here provide the library with a pointer to the function constructing the object. 
The constructor function will be called with construct=false, which means that
only the Function Descriptor is returned, but no actual instance is created. 

When a function is later assigned to an object, then the call to the
constructor fucntion stored in the libray with construct=true is made, 
an instance is created on the heap, and the Function Descriptor will contain 
an actual pointer to the class.
*/
void DataFuncLibraryClass::add(ConstructorFunctionPointer c_ptr){
  DBG ("FuncLibrary: add");
  DataFuncDescriptor fd=(*c_ptr)(false);
  HString fname=fd.getName(true);
  DBG ("FuncLibrary: add, name=" << fname);
  func_library[fname]=fd;
  DBG("FuncLibrary: size=" << func_library.size());
}

//Retrieves the pointer to the function class
ObjectFunctionClass* DataFuncLibraryClass::f_ptr(HString name, HString library){
  it=func_library.find(library+":"+name);
  if (it !=func_library.end()) {
    return (it->second).getFunction();
  } else {
    return NULL;
  };
}

//Retrieves the struct describing the function class 
DataFuncDescriptor* DataFuncLibraryClass::FuncDescriptor(HString name, HString library){
  it=func_library.find(library+":"+name);
  if (it !=func_library.end()) {
    return &(it->second);
  } else {
    return NULL;
  };
}

//Checks whether a function is in the library or not
bool DataFuncLibraryClass::inLibrary(HString name, HString library){
  it=func_library.find(library+":"+name);
  return (it !=func_library.end());
}

//prints a directory of all functions in the Library
void DataFuncLibraryClass::dir(){
  for (it=func_library.begin();it!=func_library.end();it++) {
    MSG((it->second).getName(true) << " - " << (it->second).getDocstring()); 
  };
}

//End DataFuncLibraryClass 
//........................................................................


/* Tests whether a pointer to a Data Object is valid */

bool test_data_object_ptr(Data * dp){
  if (dp!=NULL && dp!=&NullObject) {
    if ((*dp).magiccode==MAGICCODE) {return true;};
  };
  ERROR("Error: Innvalid data object pointer.");
  return false;
}


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

//!!!This should be automized by a #define command or nawk preprocesser
//otherwise it becomes a nightmare. So, let's not add too many 
//math functions at this point.

//---square
template<class T> inline T hf_square(const T v){return v*v;};
template<>        inline HString hf_square<HString>(const HString v){return mycast<HString>(hf_square(mycast<HNumber>(v)));};
template<>        inline HPointer hf_square<HPointer>(const HPointer v){return v;};

//---sqrt - square root
template<class T> inline T hf_sqrt(const T v){return sqrt(v);};
template<>        inline HString hf_sqrt<HString>(const HString v){return mycast<HString>(hf_sqrt(mycast<HNumber>(v)));};
template<>        inline HPointer hf_sqrt<HPointer>(const HPointer v){return v;};


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
*/


class DataFunc_Sys_Unit : public ObjectFunctionClass {
public:  
  DEFINE_PROCESS_CALLS_NUMONLY 

  DataFunc_Sys_Unit(Data * dp){ 
    //set the default parameters, which are by default integers - perhaps change to float numbers later??
    DBG("DataFunc_Sys_Unit: initialization called.");
    setParameter("UnitName", "");
    setParameter("ScalePrefix", "");
    setParameter("ScaleFactor", 1.0);
  }

  template <class T>
  void process(F_PARAMETERS) {
    GET_FUNC_PARAMETER_T(ScaleFactor,HNumber);
    dp->getFirstFromVector(*vp,vs);
    address i,size=vp->size();
    for (i=0; i<size;i++) {
      (*vp)[i] = (*vp)[i]/ScaleFactor;
    };
  }

  void process_S(F_PARAMETERS_T(HString)) {
    vp->clear();
    GET_FUNC_PARAMETER_T(ScalePrefix,HString);
    GET_FUNC_PARAMETER_T(UnitName,HString);
    vp->push_back(ScalePrefix + UnitName);
  }
};
DATAFUNC_CONSTRUCTOR(Unit,Sys,"Multiplies the data with a unit scale factor and also returns the apropriate unit string.",NUMBER, false)

class DataFunc_Sys_Copy : public ObjectFunctionClass {
public:  
  DEFINE_PROCESS_CALLS 

  DataFunc_Sys_Copy(Data*dp){dp->setUpdateable(false);};
  
  template <class T>
  void process(F_PARAMETERS) {
    (*dp).getFirstFromVector(*vp,vs);
    //This is bad - better use the setParameter method to also allow other than the first object to be accessed ...
    if (vs != NULL) {(*vp) = (*vs).get(*vp);};
  }
};
DATAFUNC_CONSTRUCTOR(Copy,Sys,"Copies the content of one object to the next.",INTEGER, false)

class DataFunc_Sys_Neighbours : public ObjectFunctionClass {
public:  
  DEFINE_PROCESS_CALLS 
  
  DataFunc_Sys_Neighbours(Data*dp){dp->setUpdateable(true);};

  template <class T>
  void process(F_PARAMETERS) {
    vector<HString> vec;
    vec=dp->getNeighbours(DIR_TO);
    vec.push_back("");
    vec_append(vec,dp->getNeighbours(DIR_FROM));
    copycast_vec<T,HString>(vp,&vec);
    if (vs != NULL) {(*vp) = (*vs).get(*vp);};
  }
};

DATAFUNC_CONSTRUCTOR(Neighbours,Sys,"Returns a list of names of all neighbour objects, first in To direction and the in FROM direction separated by an empty string.",STRING, true)
//Name, Library, Description, Default Type, Buffered or not


class DataFunc_Sys_Square : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS 

  DataFunc_Sys_Square(Data*dp){dp->setUpdateable(false);};
 
  template <class T>
  void process(F_PARAMETERS) {
    address i,size;
    
    /*

    !!!Actually the treatment of vector selection here and in the
    following is wrong/inefficient!!
    
    The function will only be called by get with a vector selector
    that is independent of the value (the vs1,vs2 split is already
    done within get), so selection could be done before calculation
    (and hence reduce processing time).
    
    */
 
    (*dp).getFirstFromVector(*vp,vs);
    //This is bad - better use the setParameter method to also allow other than the first object to be accessed ...
    size=(*vp).size();
    for (i=0; i<size; i++) {(*vp)[i]=hf_square((*vp)[i]);};
    
    if (vs != NULL) {(*vp) = (*vs).get(*vp);};
  }
};
DATAFUNC_CONSTRUCTOR(Square,Sys,"Squares the elements in the data vector.",INTEGER, false)
 


class DataFunc_Sys_Print : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS 

  DataFunc_Sys_Print(Data*dp){};

  //Needs treatment of vector selector ...
  template <class T>
  void process(F_PARAMETERS) {
    DBG("DataFunc_Sys_Print: process, data object pointer dp=" << dp);
    (*dp).getFirstFromVector(*vp,vs);    //copy data vector from predecessor 
    //This is bad - better use the setParameter method to also allow other than the first object to be accessed ...
    cout << (*dp).getName() << ": ";  //and print
    printvec(*vp);
  }
};
DATAFUNC_CONSTRUCTOR(Print,Sys,"Prints contents of data vector to stdout.",STRING, false);



class DataFunc_Sys_Range : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS_NUMONLY

  //  Define your own constructor to set the default parameters for the function
  DataFunc_Sys_Range (Data * dp){
    //set the default parameters, which are by default integers - perhaps change to float numbers later??
    DBG("DataFunc_Sys_Range: initialization called.");
    setParameter("start", 0);
    setParameter("end", 0);
    setParameter("inc", 1);
  }

  template <class T>
  void process(F_PARAMETERS) {
    HInteger i,size;

    //This macro will define and obtain set local variables with
    //values from corresponding parameter objects (set by
    //setParameter) or their default values, if they are not
    //present. See init for those values. If one wants a
    //specific/fixed datatype then use GET_FUNC_PARAMETER_T(NAME,TYPE)
    //instead. This may only be called once in the execution of a
    //function.

    GET_FUNC_PARAMETER(start);
    GET_FUNC_PARAMETER(end);
    GET_FUNC_PARAMETER(inc);
    if (inc==static_cast<T>(0)) {
      ERROR("DataFunc_Sys_Range: Increment inc=0!");
      return;
    }
    //Create the data vector to be returned
    vp->clear(); 
    size=floor(abs((end-start)/inc))+1;
    if (vp->capacity()<size) {vp->reserve(size);};

    //Fill the return data vector and make sure it is casted to the right output type
    for (i=0;i<size;i++) {vp->push_back(static_cast<T>(i)*inc+start);};
    
    //Perform a selection on the data vector, if necessary.  This
    //could be done smarter for some selections, i.e., by splitting
    //the vector selector into vs1 and vs2 as in Sys_Square ....
    if (vs!=NULL) {(*vp)=vs->get(*vp);};
  }

  //Now we have to do some specialization for non-numeric datataypes.
  //Again, this is not really nice, if we ever need to add an
  //additional, non-numeric type. However, that could/should also be
  //done with a macro once we have more than one of these cases!!  I
  //tried this with an explicit specialization of the method process,
  //using template<>, but the compiler didn't like that for some
  //reasons.
  void process_S(F_PARAMETERS_T(HString)) {
    vector<HNumber> vn;
    process<HNumber>(&vn, F_PARAMETERS_CALL_NOVEC);
    copycast_vec<HNumber,HString>(&vn,vp);
  }
};
DATAFUNC_CONSTRUCTOR(Range,Sys,"Return a range of numbers (0,1,2,3,4,...N). Parameter objects: start=0, end=0, inc=1",INTEGER, false);


/*
Ideas to be done:

Object "DataFile": Reads various columns (or Rows) from a data file and makes them available as data Objects.

The Objects are filled by a python script, as are the DataColumn names. Leave the parsing to Python ...

Define Objects. FileName, BlockSize, BlockNumber, BlockOffset, DataColumns, Type=ASCII
 pointing to Object "File", which then points to the data objects in DataColumns.

*/


//------------------------------------------------------------------------
//End Sys Library
//------------------------------------------------------------------------


/*========================================================================
  Define DataFunc Object Library "CR"
  ========================================================================

  This library allows one to access the CR IO and analysis functions

*/


class DataFunc_CR_dataReaderObject : public ObjectFunctionClass {

public:
  //The following line is necessary to define the process methods for
  //the various types which will all call the templated method process<T>.
  //This emulates a templated virtual funcion.

  DEFINE_PROCESS_CALLS
  
  //  The function creates a CR-Tool data reader object and stores a pointer to it
  DataFunc_CR_dataReaderObject (Data* dp){
	DBG("DataFunc_CR_dataReaderObject: initialization called.");
	setParameter("Filename", "/Users/falcke/LOFAR/usg/data/lopes/2007.01.31.23:59:33.960.event");
	setParameter("Filetype", "LOPESEvent");
	DBG("dataReaderObject: Initialization done.");
    }

    ~DataFunc_CR_dataReaderObject (){
      MSG("~DataFunc_CR_dataReaderObject: Deleting DataReader.");
      delete reinterpret_cast<CR::DataReader*>(data_pointer->getOne<HPointer>()); 
      DBG("~DataFunc_CR_dataReaderObject: Deleted.");
  }
  
  template <class T>
  void process(F_PARAMETERS) {
      GET_FUNC_PARAMETER_T(Filename,HString);
      GET_FUNC_PARAMETER_T(Filetype,HString);
      static HString oldfilename="";
      bool opened;
      void* ptr;

      DBG("dataReaderObject: Retrieved filename parameter =" << Filename);

      if (vp->size()>0 && AsPtr(vp->at(0))!=NULL) {
	DBG("dataReaderObject: Delete old data reader object " << AsPtr(vp->at(0)));
	CR::DataReader* drp=reinterpret_cast<DataReader*>(AsPtr(vp->at(0)));
	delete drp;
      }
      vp->clear();

      //Create the a pointer to the DataReader object and store the pointer
      //Here we could have if statements depending on data types
      
      DBG("DataFunc_CR_dataReaderObject: Opening File, Filename=" << Filename);
      if (Filetype=="LOPESEvent") {
	  CR::LopesEventIn* lep = new CR::LopesEventIn; ptr = lep;
	  DBG("DataFunc_CR_dataReaderObject: lep=" << ptr << " = " << reinterpret_cast<HInteger>(ptr));
	  opened=lep->attachFile(Filename);
	  if (oldfilename!=Filename) {lep->summary();};
	  oldfilename=Filename;
      } else if (Filetype=="LOFAR_TBB") {
/*	  CR::LOFAR_TBB* tbb = new CR::LOFAR_TBB(Filename); ptr = tbb;
	  DBG("DataFunc_CR_dataReaderObject: tbb=" << ptr << " = " << reinterpret_cast<HInteger>(ptr));
	  opened=tbb!=NULL;
	  DBG3(tbb->summary());
*/
      } else {
	  ERROR("DataFunc_CR_dataReaderObject: Unknown Filetype = " << Filetype  << ", name=" << dp->getName(true));
	  vp->push_back(mycast<T>(NULL)); 
	  return;
      }
      if (!opened){
	  ERROR("DataFunc_CR_dataReaderObject: Opening file " << Filename << "failed." << ", name=" << dp->getName(true));
	  vp->push_back(mycast<T>(NULL)); 
	  return;
      };

    //Store the pointer in the object, so that other objects can access
    //it. The object should actually me made read-only, since the pointer is
    //not to be changed by put ever again until the window is deleted
      vp->push_back(mycast<T>(ptr)); 
      DBG("DataFunc_CR_dataReaderObject: Success.");
  }
};

//The following macro has to come at the of the definiton. It defines
//a constructor function (no class) with a pointer that is called when
//an object is assigned this function.
DATAFUNC_CONSTRUCTOR(dataReaderObject,CR,"Creates a DataReader object for reading CR data and stores its pointer.",POINTER,true);



/*!
  \brief The function converts a column in an aips++ matrix to an stl vector
 */

template <class S, class T>
void aipscol2stlvec(casa::Matrix<S> data, vector<T>& stlvec, HInteger col){
    HInteger i,nrow,ncol;
//    vector<HNumber>::iterator p;
    
    nrow=data.nrow();
    ncol=data.ncolumn();

    if (col>=ncol) {
	ERROR("aipscol2stlvec: column number col=" << col << " is larger than total number of columns (" << ncol << ") in matrix.");
	stlvec.clear();
	return;
    }

    stlvec.resize(nrow);
    casa::Vector<S> CASAVec = data.column(col);
    
//    p=stlvec.begin();
    
    for (i=0;i<nrow;i++) {
//	*p=mycast<T>(CASAVec[i]); 
	stlvec[i]=mycast<T>(CASAVec[i]); 
//	p++;
    };
}

/*!
  \brief The function converts an aips++ vector to an stl vector
 */

template <class S, class T>
void aipsvec2stlvec(casa::Vector<S> data, vector<T>& stlvec){
    HInteger i,n;
//    vector<R>::iterator p;
    
    n=data.size();
    stlvec.resize(n);
//    p=stlvec.begin();
    for (i=0;i<n;i++) {
//	*p=mycast<T>(data[i]); 
	stlvec[i]=mycast<T>(data[i]); 
//	p++;
    };
}


class DataFunc_CR_dataRead : public ObjectFunctionClass {
public:
  //The following line is necessary to define the process method for this class

  DEFINE_PROCESS_CALLS 

  //  This function reads Data from an DataReader Object to read CR data
  DataFunc_CR_dataRead (Data* dp){
    DBG("DataFunc_CR_dataRead: initialization called.");

    HPointer ptr=NULL;
    setParameter("FileObject",  ptr);
    setParameter("Antenna", 0);
    setParameter("Blocksize", -1);
    setParameter("Block", 0);
    setParameter("Stride", 0);
    setParameter("Shift", 0);
    HString s="Fx"; setParameter("Datatype", s);

    //Now create a new, but empty data vector as buffer
    vector<HNumber> vec;
    dp->noMod(); dp->put(vec);     
  }

  ~DataFunc_CR_dataRead (){
    MSG("~DataFunc_CR_dataRead: Here something probably needs to happen for destroying the plotter");
  }
  
  template <class T>
  void process(F_PARAMETERS) {

    //First retrieve the pointer to the pointer to the dataRead and check whether it is non-NULL.
    GET_FUNC_PARAMETER_T(FileObject,HPointer);
    DBG("FileObject=" << FileObject);
    if (FileObject==NULL){
	ERROR("dataRead: pointer to FileObject is NULL, DataReader not found." << ", name=" << dp->getName(true)); 
	return;
    };
    DataReader *drp=reinterpret_cast<DataReader*>(FileObject); 


//!!!One Needs to verify somehow that the parameters make sense !!!
    GET_FUNC_PARAMETER_T(Antenna, HInteger);
    GET_FUNC_PARAMETER_T(Blocksize,  HInteger);
    GET_FUNC_PARAMETER_T(Block,  HInteger);
    GET_FUNC_PARAMETER_T(Stride,  HInteger);
    GET_FUNC_PARAMETER_T(Shift, HInteger);
    GET_FUNC_PARAMETER_T(Datatype, HString);

//    MSG("Antenna=" << Antenna);

    drp->setBlocksize(Blocksize);
    drp->setBlock(Block);
    drp->setStride(Stride);
    drp->setShift(Shift);

    Vector<uint> antennas(1,Antenna);
    drp->setSelectedAntennas(antennas);
//    Vector<uint> selantennas=drp->selectedAntennas();
//    MSG("No of Selected Antennas" << drp->nofSelectedAntennas ()<< " SelectedAntennas[0]=" <<selantennas[0]);

    if (Datatype=="Time") {aipsvec2stlvec(drp->timeValues(),*vp);}
    else if (Datatype=="Frequency") {aipsvec2stlvec(drp->frequencyValues(),*vp);}
    else if (Datatype=="Fx") {aipscol2stlvec(drp->fx(),*vp,0);}
    else if (Datatype=="Voltage") {aipscol2stlvec(drp->voltage(),*vp,0);}
    else if (Datatype=="invFFT") {aipscol2stlvec(drp->invfft(),*vp,0);}
    else if (Datatype=="FFT") {aipscol2stlvec(drp->fft(),*vp,0);}
    else if (Datatype=="CalFFT") {aipscol2stlvec(drp->calfft(),*vp,0);}
    else {
	ERROR("DataFunc_CR_dataRead: Datatype=" << Datatype << " is unknown." << ", name=" << dp->getName(true));
	vp->clear();
	return;
    };
  }
};

//The following macro has to come at the of the definiton. It defines
//a constructor function (no class) with a pointer that is called when
//an object is assigned this function.
DATAFUNC_CONSTRUCTOR(dataRead,CR,"Function retrieving a vector from the dataReader.",NUMBER, true);

/*------------------------------------------------------------------------
End DataFunc Object Library "CR"
------------------------------------------------------------------------*/


/*========================================================================
  Define DataFunc Object Library "Qt"
  ========================================================================

  This library allows one to assign plotting and Qt windows actions to 
  an object.

*/


class DataFunc_Qt_Mainwindow : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  //  Launch main window here and store pointer
  DataFunc_Qt_Mainwindow (Data* dp){
    //set the default parameters, which are by default integers - perhaps change to float numbers later??
    DBG("DataFunc_Qt_Mainwindow: initialization called.");

    HString s="hfplot";
    setParameter("Name",  s);
    GET_FUNC_PARAMETER_T(Name,HString);

    //Create the a pointer to the MainWindow object and store a
    //pointer to the pointer ...  this is the only way the two threads
    //can communicate at this point (through shared memory). For
    //non-shared memory in a cluster, some MPI scheme needs to be
    //invented


    MainWindow **mainwin = new (MainWindow*);

    DBG("mainwin=" << mainwin << " = " << reinterpret_cast<HInteger>(mainwin));

    *mainwin=NULL;

    //Store the pointer to pointer in the object, so that other
    //objects can access is. The object should actually me made
    //read-only, since the pointer is not to be changed by put ever
    //again until the window is deleted

    dp->putOne(static_cast<HPointer>(mainwin)); 

    //Now we start a thread, with the pointer to the pointer as the
    //only parameter. The function qrun (defined in hfqt.cc) will then
    //launch the Qt window (and go into its own waiting loop).

    DBG("Starting thread.");
    boost::thread my_thread(boost::bind(qrun,mainwin));     
    
    //Ideally we would now have here a waiting loop until the other
    //thread has filled the pointer with a value, but somehow that
    //didn't work yet. 

    // while (label==NULL) {i++;};

    //Here we would then call a function to set the textlabel and
    //other things

  }

  ~DataFunc_Qt_Mainwindow (){
    MSG("~DataFunc_Qt_Mainwindow: Here still need a function to destroy the window and delete the pointer to it");
  }
  
  void process(F_PARAMETERS_NOVEC){
    //The actual function doesn't do anything, since the pointer
    //location stored in the buffer will not be changed.
  }
};
DATAFUNC_CONSTRUCTOR(Mainwindow,Qt,"Creates the main application/plotting window and stores its pointer",POINTER, true);

class DataFunc_Qt_mglWindow : public ObjectFunctionClass {
public:
  //This is necessary to define the process method for this function
  //which in this case ignores the input vector.

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  //  This function launches a MathGL QT window and stores a pointer to the pointer of the window class
  DataFunc_Qt_mglWindow (Data* dp){
    DBG("DataFunc_Qt_mglWindow: initialization called.");


    HString s="hfplot";
    setParameter("WindowName",  s);

    //Create the a pointer to the MainWindow object and store a
    //pointer to the pointer ...  this is the only way the two threads
    //can communicate at this point (through shared memory). For
    //non-shared memory in a cluster, some MPI scheme needs to be
    //invented

    mglGraphQT **mglwin = new (mglGraphQT*);

    DBG("mglwin=" << mglwin << " = " << reinterpret_cast<HInteger>(mglwin));

    *mglwin=NULL;

    //Store the pointer to pointer in the object, so that other
    //objects can access is. The object should actually me made
    //read-only, since the pointer is not to be changed by put ever
    //again until the window is deleted

    dp->putOne(static_cast<HPointer>(mglwin)); 

    //Now we start a thread, with the pointer to the pointer as the
    //only parameter. The function qrun (defined in hfqt.cc) will then
    //launch the Qt window (and go into its own waiting loop).

    DBG("Starting thread.");
    boost::thread my_thread(boost::bind(mglrun,mglwin));     
    
    //Ideally we would now have here a waiting loop until the other
    //thread has filled the pointer with a value, but somehow that
    //didn't work yet. 

    // while (*mglwin==NULL) {i++;};

    //Here we would then call a function to set the textlabel and
    //other things

  }

  ~DataFunc_Qt_mglWindow (){
    MSG("~DataFunc_Qt_mglWindow: Here still need a function to destroy the window and delete the pointer to it.");
  }
  
  void process(F_PARAMETERS_NOVEC){
    //No Processing needed here
  }
};

//The following macro has to come at the of the definiton. It defines
//a constructor function (no class) with a pointer that is called when
//an object is assigned this function.
DATAFUNC_CONSTRUCTOR(mglWindow,Qt,"Creates a MathGL plotter window under QT and stores its pointer",POINTER, true);

class DataFunc_Qt_mglPlot : public ObjectFunctionClass {
public:
  //This is necessary to define the process method for this function
  //which in this case ignores the input vector:

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  //  This function initializes the Plotter. It assumes that a MathGL
  //  QT window was already launched with Qt_mglWindow.
  DataFunc_Qt_mglPlot (Data* dp){
    DBG("DataFunc_Qt_mglPlot: initialization called.");

    HString s="1D-Plotter";
    HPointer ptr=NULL;
    setParameter("mglWindow",  ptr);
    setParameter("PlotName",  s);
    setParameter("NPanelsX",  1);
    setParameter("NPanelsY",  1);
    s="x-Axis"; setParameter("XAxisLabel", s);
    s="y-Axis"; setParameter("YAxisLabel", s);
  }

  ~DataFunc_Qt_mglPlot (){
    MSG("~DataFunc_Qt_mglPlot: Here something probably needs to happen for destroying the plotter");
  }
  
  void process(F_PARAMETERS_NOVEC){

    //First retrieve the pointer to the pointer to the mglWindow and check whether it is non-NULL.
    GET_FUNC_PARAMETER_T(mglWindow,HPointer);
    if (mglWindow==NULL){MSG("mglWindow: pointer to pointer is NULL, window not yet launched"); return;}
    else {MSG("mglWindow=" << mglWindow);};
    mglGraphQT **gp=static_cast<mglGraphQT**>(mglWindow); 

    GET_FUNC_PARAMETER_T(PlotName,HString);
    MSG("PlotName:" << PlotName);
    GET_FUNC_PARAMETER_T(XAxisLabel,HString);
    MSG("XAxisLabel:" << XAxisLabel);
    GET_FUNC_PARAMETER_T(YAxisLabel,HString);
    MSG("YAxisLabel:" << YAxisLabel);
    GET_FUNC_PARAMETER_T(NPanelsX,HInteger);
    MSG("NPanelsX:" << NPanelsX);
    GET_FUNC_PARAMETER_T(NPanelsY,HInteger);
    MSG("NPanelsY:" << NPanelsY);


    mglData  d(50);
    d.Modify("0.7*sin(2*pi*x) + 0.5*cos(3*pi*x) + 0.2*sin(pi*x)");
    (*gp)->NewFrame();
    (*gp)->Box();	
    (*gp)->Axis("xy");	
    (*gp)->Label('x',XAxisLabel.c_str());	
    (*gp)->Label('y',YAxisLabel.c_str());
    (*gp)->Text(mglPoint(0,1.2,1),PlotName.c_str());
    (*gp)->Plot(d);
    (*gp)->EndFrame();
  }
};

//The following macro has to come at the of the definiton. It defines
//a constructor function (no class) with a pointer that is called when
//an object is assigned this function.
DATAFUNC_CONSTRUCTOR(mglPlot,Qt,"Function which plots data in a MathGL plotter window under QT",POINTER,false);

class DataFunc_Qt_QPrint : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  //  Constructor launching the window
  DataFunc_Qt_QPrint (Data * dp){
    DBG("DataFunc_Qt_QPrint: initialization called.");
    HPointer null=NULL;
    setParameter("mainwindow_pointer", null,"'","Mainwindow");
    // We assume that the Label already exists in the main window ....
  };

  
  void process(F_PARAMETERS_NOVEC){
    DBG("DataFunc_Qt_QPrint: processing.");

    GET_FUNC_PARAMETER_T(mainwindow_pointer,HPointer);
    /*
    HString n=getParameterName("mainwindow_pointer");
    DBG("Name=" << n);

    HPointer p=getParameterDefault<HPointer>("mainwindow_pointer");
    DBG("HPointer=" << p);

    HPointer mainwindow_pointer;
    DBG("mainwindow_pointer");

    mainwindow_pointer=dp->getParameter(n,p);
    DBG("mainwindow_pointer=" << mainwindow_pointer);
    */
    if (mainwindow_pointer != NULL) {
      
      MainWindow ** mainwin_pp = static_cast<MainWindow**>(mainwindow_pointer);
      DBG("mainwin_pp=" << mainwin_pp << "=" << reinterpret_cast<HInteger>(mainwin_pp));
      DBG("*mainwin_pp=" << *mainwin_pp);

      vector<HString> vec_S;
      (*dp).getFirstFromVector(vec_S,vs);    //copy data vector from predecessor
      HString s=(*dp).getName()+":"+vectostring(vec_S);

      DBG(s);

      //Test: QMetaObject::invokeMethod(*mainwin_pp, "open",Qt::QueuedConnection);
      QMetaObject::invokeMethod(*mainwin_pp, "qprint",Qt::QueuedConnection,Q_ARG(QString, QString(s.c_str())));

    } else {
      ERROR("DataFunc_Qt_Print: Data Object did not exist.");
    };
  }
};
DATAFUNC_CONSTRUCTOR(QPrint,Qt,"Allows one to print the contents of an object in a text window",INTEGER, false);


/*------------------------------------------------------------------------
Python Interface Functions
------------------------------------------------------------------------*/


class DataFunc_Py_PyFunc : public ObjectFunctionClass {
public:
  //This is necessary to define the process method for this function
  //which in this case ignores the input vector:

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  DataFunc_Py_PyFunc (Data* dp){
    PyObject* pyobj = dp->retrievePyFunc();

    DBG("DataFunc_Py_PyFunc: initialization called.");
    if (pyobj==NULL){
      ERROR("PyFunc: pointer to PythonObject is NULL. Object PythonObject does not exist. Define PythonObject and use pytore before assigning PyFunc to this Object." << ", name=" << dp->getName(true)); 
      return;
    };

    //Call the startup method if present
    char AttribStr[] = "hfstartup";
    //first we need to check if attribute is present in the Python Object
    if (!PyObject_HasAttrString(pyobj, AttribStr)) {
	ERROR("PyFunc: Object does not have Attribute " << AttribStr << ", name=" << dp->getName(true)); return;};
    int ret=boost::python::call_method<int>(pyobj,AttribStr,boost::ref(*dp));
    if (ret!=0) {
	ERROR("PyFunc - startup method returned user-defined error code" << ret << ", name=" << dp->getName(true));
    };
  }

    ~DataFunc_Py_PyFunc (){
      Data * dp = data_pointer;
      PyObject* pyobj = dp->retrievePyFunc();
	
	if (pyobj==NULL){ERROR("PyFunc: pointer to Python Object is NULL." << ", name=" << dp->getName(true)); return;};

	char AttribStr[] = "hfcleanup";
	//we need to check if the attribute is present in the Python Object
	if (!PyObject_HasAttrString(pyobj, AttribStr)) {
	    ERROR("PyFunc: Object does not have Attribute " << AttribStr << ", name=" << dp->getName(true) << "."); return;};
	int ret=boost::python::call_method<int>(pyobj,AttribStr);
	if (ret!=0) {
	    ERROR("PyFunc - process method returned user-defined error code" << ret  << ", name=" << dp->getName(true));
	};
    }
 

    void process(F_PARAMETERS_NOVEC){
      PyObject* pyobj = dp->retrievePyFunc();

	DBG("DataFunc_Py_PyFunc.process: pyobj=" << pyobj << " name=" << dp->getName(true));
	if (pyobj==NULL){ERROR("PyFunc: pointer to Python Object is NULL."  << ", name=" << dp->getName(true)); return;};

	char AttribStr[] = "hfprocess";
	//we need to check if the process attribute is present in the Python Object
	if (!PyObject_HasAttrString(pyobj, AttribStr)) {
	    ERROR("PyFunc: Object does not have Attribute " << AttribStr << "."  << ", name=" << dp->getName(true)); return;};
	DBG("DataFunc_Py_PyFunc.process: Call Python Object");
	int ret=boost::python::call_method<int>(pyobj,AttribStr,boost::ref(*dp));
	if (ret!=0) {
	    ERROR("PyFunc - process method returned user-defined error code" << ret << ", name=" << dp->getName(true));
	};
    }
};
//The following macro has to come at the of the definiton. It defines
//a constructor function (no class) with a pointer that is called when
//an object is assigned this function.
DATAFUNC_CONSTRUCTOR(PyFunc,Py,"Function to call a user-defined python object of type hffunc",POINTER,false);

/*

template <class T>
void DataFunc_Qt_QPrint(F_PARAMETERS) {
  static QLabel* label=NULL;
  HString s;
  int i=0;
  //Define local static variables
  //End definition of local static variables

  //Begin main body of function, split in three parts: init, run, and destruct.
  if (test_data_object_ptr(dp)) {
    calltype=dp->getFunc()->getCalltype();
    if (calltype==FUNC_CALLTYPE_DESTRUCT) {return;};
    if (calltype==FUNC_CALLTYPE_INIT) {
      dp->getFunc()->setCalltype(FUNC_CALLTYPE_RUN);
      MSG("Starting thread.");
      cout << "Label=" << label;
      boost::thread my_thread(boost::bind(qrun2,&label));     
      return; //while (label==NULL) {i++;};
    };
    (*dp).getFirstFromVector(*vp,vs);    //copy data vector from predecessor
    s=(*dp).getName()+":"+vectostring(*vp);
    cout << "Wait cycles: " << i << " label=" << label;
    MSG("Communicating with thread: " << s);
    cout << "ReturnCode=" << QMetaObject::invokeMethod(label, "setText",Qt::QueuedConnection,Q_ARG(QString, QString(s.c_str()))) << endl;
  } else {
    ERROR("DataFunc_Qt_Print: Data Object did not exist.");
  };

}


template <class T>
void DataFunc_Qt_Main(F_PARAMETERS) {
    HString s;
  int i=0;
  FUNC_CALLTYPE calltype;
  //Define local static variables
  //End definition of local static variables

  //Begin main body of function, split in three parts: init, run, and destruct.
  if (test_data_object_ptr(dp)) {
    calltype=dp->getFunc()->getCalltype();
    if (calltype==FUNC_CALLTYPE_DESTRUCT) {return;}
    else if (calltype==FUNC_CALLTYPE_INIT) {
      ls = new localstatics;
      dp->getFunc()->setStatics(ls);
      dp->getFunc()->setCalltype(FUNC_CALLTYPE_RUN);
      (*dp).getFirstFromVector(*vp,vs);    //copy data vector from predecessor
      ls->s=(*dp).getName()+":"+vectostring(*vp);
      ls->mainwin = new MainWindow;
      MSG("Starting thread.");
      DBG("mainwinl=" << ls->mainwin);
      boost::thread my_thread(boost::bind(qrun,&(mainwin)));     
      return; //while (label==NULL) {i++;};
    };
    dp->getFunc()->setStatics(ls);
    (*dp).getFirstFromVector(*vp,vs);    //copy data vector from predecessor
    s=(*dp).getName()+":"+vectostring(*vp);
    DBG("Wait cycles: " << i << " mainwin=" << mainwin);
    MSG("Communicating with thread: " << s);
    //    cout << "ReturnCode=" << QMetaObject::invokeMethod(label, "setText",Qt::QueuedConnection,Q_ARG(QString, QString(s.c_str()))) << endl;
  } else {
    ERROR("DataFunc_Qt_Print: Data Object did not exist.");
  };
}
*/




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

//------------------------------------------------------------------------
//Publish the Libraries - used in Data object constructor
//------------------------------------------------------------------------

void DataFunc_Sys_Library_publish(DataFuncLibraryClass* library_ptr){
  library_ptr->add(&DataFunc_Sys_Neighbours_Constructor);
  library_ptr->add(&DataFunc_Sys_Copy_Constructor);
  library_ptr->add(&DataFunc_Sys_Print_Constructor);
  library_ptr->add(&DataFunc_Sys_Square_Constructor);
  library_ptr->add(&DataFunc_Sys_Range_Constructor);
  library_ptr->add(&DataFunc_Sys_Unit_Constructor);
}

void DataFunc_Qt_Library_publish(DataFuncLibraryClass* library_ptr){
  library_ptr->add(&DataFunc_Qt_Mainwindow_Constructor);
  library_ptr->add(&DataFunc_Qt_mglWindow_Constructor);
  library_ptr->add(&DataFunc_Qt_mglPlot_Constructor);
  library_ptr->add(&DataFunc_Qt_QPrint_Constructor);
}

void DataFunc_CR_Library_publish(DataFuncLibraryClass* library_ptr){
  library_ptr->add(&DataFunc_CR_dataReaderObject_Constructor);
  library_ptr->add(&DataFunc_CR_dataRead_Constructor);
}

void DataFunc_Py_Library_publish(DataFuncLibraryClass* library_ptr){
  library_ptr->add(&DataFunc_Py_PyFunc_Constructor);
}
 
