//#define DBG_MODE 0
//#define DBG_MODE 1

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

#include <GUI/mainwindow.h>
#include <GUI/hfdefs.h>
#include <GUI/hfcast.h> 
#include <GUI/hfget.h>
#include <GUI/hffuncs.h>  


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
void ObjectFunctionClass::process_P (F_PARAMETERS_T(Pointer)) {
  ERROR("ObjectFunctionClass: generic process call of function"
	<< getName(true)
	<< ", Call of Type Pointer not defined");
};
void ObjectFunctionClass::process_I(F_PARAMETERS_T(Integer)) {
  ERROR("ObjectFunctionClass: generic process call of function"
	<< getName(true)
	<< ", Call of Type Integer not defined");
};
void ObjectFunctionClass::process_N(F_PARAMETERS_T(Number) ) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type Number not defined");};
void ObjectFunctionClass::process_C(F_PARAMETERS_T(Complex)) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type Complex not defined");};
void ObjectFunctionClass::process_S(F_PARAMETERS_T(String) ) {ERROR("ObjectFunctionClass: generic process call of function" << getName(true) << ", Call of Type String not defined");};



//This is a dummy funciton we need in order to fool the compiler
//so that templated methods of all typese are created ...
//This calls all templated methods in the class
//I currently don't know of a smater way doing this, after all the actual typing
//is only done at run time and by using void pointers, there is no other way
//the compiler knows which methods to create.
template <class T>
void ObjectFunctionClass::instantiate_one(){
  T val; String s = "";
  setParameter(s,s,val);
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

/*
Function to set names and default values of parameters used in the function. The names are names of other
objects in the network that contain the parameter values. It is also possible to set a default value in
case that object is not found.
*/
template <class T>  
void ObjectFunctionClass::setParameter(String internal_name, String external_name , T default_value){
  DATATYPE type=WhichType<T>();
  map<String,parameter_item>::iterator it=parameter_list.find(internal_name);
  parameter_item p_i;
  DBG("ObjectFunctionClass::setParameter: internal_name=" << internal_name 
      << ", external_name=" << external_name << ", default_value=" << default_value 
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

  p_i.name=external_name;
  p_i.type=type;
  p_i.ptr=new T;
  set_ptr_to_value(p_i.ptr,p_i.type,default_value);
  parameter_list[internal_name]=p_i;
}

/*
Returns the external name (i.e. the name of the object to be accessed) of an internal parameter
*/
String ObjectFunctionClass::getParameterName(String internal_name) {
  map<String,parameter_item>::iterator it;
  DBG("ObjectFunctionClass::getParameterName: internal_name=" << internal_name);
  it=parameter_list.find(internal_name);

  if (it != parameter_list.end()) {
    return (it->second).name;
  } else {
    ERROR("ObjectFunctionClass::getParameterName: Name " << internal_name << " not found. Error in programming network function.");
    return "";
  };
}

/*
Returns the default value of an internal parameter, typically used  if the external name (object) does not exist. Can also be used as a static variable of an object.
*/
template <class T>
T ObjectFunctionClass::getParameterDefault(String internal_name) {
  map<String,parameter_item>::iterator it;
  it=parameter_list.find(internal_name);
  if (it != parameter_list.end()) {
    DBG("getParameterDefault: Type=" << datatype_txt(WhichType<T>()));
    DBG("getParameterDefault: type=" << datatype_txt((it->second).type));
    DBG("getParameterDefault: ptr=" << (it->second).ptr);
    DBG("getParameterDefault: cast<Int>=" << cast_ptr_to_value<Integer>((it->second).ptr,(it->second).type));
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
  map<String,parameter_item>::iterator it; //Now destroy all the parameter default variables, created with setParameter
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

DataFuncDescriptor::DataFuncDescriptor(){}

DataFuncDescriptor::~DataFuncDescriptor(){}

//Save the basic information about the function class (name, library it belongs to, and a short description)
void DataFuncDescriptor::setInfo(String name, String library, String shortdocstring, String docstring){
  DBG("DataFuncDescriptor.set: name=" << name << " library=" << library);
  fd.name=name;
  fd.library=library;
  fd.docstring=docstring;
  fd.shortdocstring=shortdocstring;
}


/*
Creates a new instance of the class (on the heap) and returns a
  function descriptor, which also contains the pointer to the new
  instance. This instance will be called by "get" later on.
*/

ObjectFunctionClass* DataFuncDescriptor::newFunction(Data *dp){
  DataFuncDescriptor fdnew;
  fdnew=(*getConstructor())(dp);
  fdnew.getFunction()->setInfo(fdnew.getName(false),fdnew.getLibrary(),
			       fdnew.getDocstring(true),fdnew.getDocstring(false));
  fdnew.getFunction()->setConstructor(fdnew.getConstructor());
  fdnew.getFunction()->setFunction(fdnew.getFunction());
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
String DataFuncDescriptor::getLibrary(){return fd.library;};

//Return the name of the function being described 
//(either with (fullname=false) or without the library name attached)
String DataFuncDescriptor::getName(bool fullname){
  String s;
  if (fullname){
    s= fd.library + ":" + fd.name;
  } else {
    s= fd.name;
  };
  DBG("DataFuncDescriptor.getName = " << s);
  return s;
}

//Retrieve the long or short description of the function
String DataFuncDescriptor::getDocstring(bool shortdoc){
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
  String fname=fd.getName(true);
  DBG ("FuncLibrary: add, name=" << fname);
  func_library[fname]=fd;
  DBG("FuncLibrary: size=" << func_library.size());
}

//Retrieves the pointer to the function class
ObjectFunctionClass* DataFuncLibraryClass::f_ptr(String name, String library){
  it=func_library.find(library+":"+name);
  if (it !=func_library.end()) {
    return (it->second).getFunction();
  } else {
    return NULL;
  };
}

//Retrieves the struct describing the function class 
DataFuncDescriptor* DataFuncLibraryClass::FuncDescriptor(String name, String library){
  it=func_library.find(library+":"+name);
  if (it !=func_library.end()) {
    return &(it->second);
  } else {
    return NULL;
  };
}

//Checks whether a function is in the library or not
bool DataFuncLibraryClass::inLibrary(String name, String library){
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
template<>        inline String hf_square<String>(const String v){return mycast<String>(hf_square(mycast<Number>(v)));};
template<>        inline Pointer hf_square<Pointer>(const Pointer v){return v;};

//---sqrt - square root
template<class T> inline T hf_sqrt(const T v){return sqrt(v);};
template<>        inline String hf_sqrt<String>(const String v){return mycast<String>(hf_sqrt(mycast<Number>(v)));};
template<>        inline Pointer hf_sqrt<Pointer>(const Pointer v){return v;};


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


class DataFunc_Sys_Copy : public ObjectFunctionClass {
public:  
  DEFINE_PROCESS_CALLS 

  DataFunc_Sys_Copy(Data*dp){};
  
  template <class T>
  void process(F_PARAMETERS) {
    (*dp).getFirstFromVector(*vp,vs);
    //This is bad - better use the setParameter method to also allow other than the first object to be accessed ...
    if (vs != NULL) {(*vp) = (*vs).get(*vp);};
  }
};
DATAFUNC_CONSTRUCTOR(Copy,Sys,"Copies the content of one object to the next.")


class DataFunc_Sys_Square : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS 

  DataFunc_Sys_Square(Data*dp){};
 
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
DATAFUNC_CONSTRUCTOR(Square,Sys,"Squares the elements in the data vector.")
 

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
DATAFUNC_CONSTRUCTOR(Print,Sys,"Prints contents of data vector to stdout.");


class DataFunc_Sys_Range : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS_NUMONLY

  //  Define your own constructor to set the default parameters for the function
  DataFunc_Sys_Range (Data * dp){
    //set the default parameters, which are by default integers - perhaps change to float numbers later??
    DBG("DataFunc_Sys_Range: initialization called.");
    setParameter("start", "'start", 0);
    setParameter("end", "'end", 0);
    setParameter("inc", "'inc", 1);
  }

  template <class T>
  void process(F_PARAMETERS) {
    Integer i,size;

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
  void process_S(F_PARAMETERS_T(String)) {
    vector<Number> vn;
    process<Number>(&vn, F_PARAMETERS_CALL_NOVEC);
    copycast_vec<Number,String>(&vn,vp);
  }
};
DATAFUNC_CONSTRUCTOR(Range,Sys,"Return a range of numbers (0,1,2,3,4,...N). Parameter objects: start=0, end=0, inc=1");


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

    String s="hfplot";
    setParameter("name", "'Name", s);
    GET_FUNC_PARAMETER_T(name,String);

    //Create the a pointer to the MainWindow object and store a
    //pointer to the pointer ...  this is the only way the two threads
    //can communicate at this point (through shared memory). For
    //non-shared memory in a cluster, some MPI scheme needs to be
    //invented


    MainWindow **mainwin = new (MainWindow*);

    DBG("mainwin=" << mainwin << " = " << reinterpret_cast<Integer>(mainwin));

    *mainwin=NULL;

    //Store the pointer to pointer in the object, so that other
    //objects can access is. The object should actually me made
    //read-only, since the pointer is not to be changed by put ever
    //again until the window is deleted

    dp->putOne(static_cast<Pointer>(mainwin)); 

    //Now we start a thread, with the pointer to the pointer as the
    //only parameter. The function qrun (defined in hfqt.cc) will then
    //launch the Qt window (and go into its own waiting loop).

    DBG("Starting thread.");
    boost::thread my_thread(boost::bind(qrun,mainwin));     
    
    //Ideally we would now have here a waiting loop until the other
    //thread has filled the pointer with a value, but some how that
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
DATAFUNC_CONSTRUCTOR(Mainwindow,Qt,"Creates the main application/plotting window and stores its pointer");


class DataFunc_Qt_QPrint : public ObjectFunctionClass {
public:

  DEFINE_PROCESS_CALLS_IGNORE_DATATYPE
  
  //  Constructor launching the window
  DataFunc_Qt_QPrint (Data * dp){
    DBG("DataFunc_Qt_QPrint: initialization called.");
    Pointer null=NULL;
    setParameter("mainwindow_pointer", "'Mainwindow", null);
    // We assume that the Label already exists in the main window ....
  };

  
  void process(F_PARAMETERS_NOVEC){
    DBG("DataFunc_Qt_QPrint: processing.");

    GET_FUNC_PARAMETER_T(mainwindow_pointer,Pointer);
    /*
    String n=getParameterName("mainwindow_pointer");
    DBG("Name=" << n);

    Pointer p=getParameterDefault<Pointer>("mainwindow_pointer");
    DBG("Pointer=" << p);

    Pointer mainwindow_pointer;
    DBG("mainwindow_pointer");

    mainwindow_pointer=dp->getParameter(n,p);
    DBG("mainwindow_pointer=" << mainwindow_pointer);
    */
    if (mainwindow_pointer != NULL) {
      
      MainWindow ** mainwin_pp = static_cast<MainWindow**>(mainwindow_pointer);
      DBG("mainwin_pp=" << mainwin_pp << "=" << reinterpret_cast<Integer>(mainwin_pp));
      DBG("*mainwin_pp=" << *mainwin_pp);

      vector<String> vec_S;
      (*dp).getFirstFromVector(vec_S,vs);    //copy data vector from predecessor
      String s=(*dp).getName()+":"+vectostring(vec_S);

      DBG(s);

      //Test: QMetaObject::invokeMethod(*mainwin_pp, "open",Qt::QueuedConnection);
      QMetaObject::invokeMethod(*mainwin_pp, "qprint",Qt::QueuedConnection,Q_ARG(QString, QString(s.c_str())));

    } else {
      ERROR("DataFunc_Qt_Print: Data Object did not exist.");
    };
  }
};
DATAFUNC_CONSTRUCTOR(QPrint,Qt,"Allows one to print the contents of an object in a text window");

/*

template <class T>
void DataFunc_Qt_QPrint(F_PARAMETERS) {
  static QLabel* label=NULL;
  String s;
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
    String s;
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
  library_ptr->add(&DataFunc_Sys_Copy_Constructor);
  library_ptr->add(&DataFunc_Sys_Print_Constructor);
  library_ptr->add(&DataFunc_Sys_Square_Constructor);
  library_ptr->add(&DataFunc_Sys_Range_Constructor);
}

void DataFunc_Qt_Library_publish(DataFuncLibraryClass* library_ptr){
  library_ptr->add(&DataFunc_Qt_Mainwindow_Constructor);
  library_ptr->add(&DataFunc_Qt_QPrint_Constructor);
}
 

