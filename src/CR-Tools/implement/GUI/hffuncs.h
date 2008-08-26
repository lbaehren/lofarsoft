#ifndef HFFUNCS_H
#define HFFUNCS_H



#define F_PARAMETERS        vector<T> *vp, Data *dp, Vector_Selector *vs
#define F_PARAMETERS_NOVEC                 Data *dp, Vector_Selector *vs                       
#define F_PARAMETERS_DEF    vector<T> *vp; Data *dp; Vector_Selector *vs
#define F_PARAMETERS_CALL              vp,       dp,                  vs
#define F_PARAMETERS_CALL_NOVEC                  dp,                  vs                     
#define F_PARAMETERS_T( T ) vector<T> *vp, Data *dp, Vector_Selector *vs
#define F_PARAMETERS_h      F_PARAMETERS=NULL

#define DEFINE_PROCESS_CALLS_NUMONLY\
  void process_I(F_PARAMETERS_T(Integer)) {process<Integer>(F_PARAMETERS_CALL);}\
  void process_N(F_PARAMETERS_T(Number) ) {process<Number>(F_PARAMETERS_CALL);}\
  void process_C(F_PARAMETERS_T(Complex)) {process<Complex>(F_PARAMETERS_CALL);}

#define DEFINE_PROCESS_CALLS\
  DEFINE_PROCESS_CALLS_NUMONLY \
  void process_P(F_PARAMETERS_T(Pointer)) {process<Pointer>(F_PARAMETERS_CALL);}\
  void process_S(F_PARAMETERS_T(String) ) {process<String>(F_PARAMETERS_CALL);}

#define DEFINE_PROCESS_CALLS_h\
  void process_P(F_PARAMETERS_T(Pointer));\
  void process_I(F_PARAMETERS_T(Integer));\
  void process_N(F_PARAMETERS_T(Number) );\
  void process_C(F_PARAMETERS_T(Complex));\
  void process_S(F_PARAMETERS_T(String) )

#define DEFINE_PROCESS_CALLS_IGNORE_DATATYPE\
  void process_P(F_PARAMETERS_T(Pointer)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_I(F_PARAMETERS_T(Integer)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_N(F_PARAMETERS_T(Number) ) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_C(F_PARAMETERS_T(Complex)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_S(F_PARAMETERS_T(String) ) {process(F_PARAMETERS_CALL_NOVEC);}


void qrun(MainWindow **label);
void qrun2(QLabel **label);


/*========================================================================
  class DataFuncDescriptor
  ========================================================================

This class stores informations about the functions that are being used in an object.

*/
#define DATAFUNC_CONSTRUCTOR( NAME, LIB, INFO) \
DataFuncDescriptor DataFunc_##LIB##_##NAME##_Constructor(Data * dp=NULL){\
  DataFuncDescriptor fd;\
  fd.setInfo(#NAME,#LIB,INFO); \
  fd.setConstructor(&DataFunc_##LIB##_##NAME##_Constructor);\
  if (dp!=NULL) {\
    fd.setFunction(new DataFunc_##LIB##_##NAME(dp));\
    fd.getFunction()->setInfo(#NAME,#LIB,INFO); \
  } \
  else {fd.setFunction(NULL);};\
  return fd;\
}

typedef DataFuncDescriptor (*ConstructorFunctionPointer)(Data*);

class DataFuncDescriptor {
  
 public:

  /*!
    \param name
    \param library
    \param shortdocstring
    \param docstring
   */
  void setInfo (String name,
		String library="Sys",
		String shortdocstring="",
		String docstring="");
  
  void setFunction(ObjectFunctionClass* f_ptr);

  ObjectFunctionClass* getFunction();

  ObjectFunctionClass* newFunction(Data*);
  
  void setConstructor(ConstructorFunctionPointer);
  ConstructorFunctionPointer getConstructor();
  
  String getName(bool fullname=false);
  String getLibrary();
  String getDocstring(bool shortdoc=true);
  
  DataFuncDescriptor();
  ~DataFuncDescriptor();
  
 private:
  
  struct function_descr { 
    ObjectFunctionClass *f_ptr;
    ConstructorFunctionPointer constructor;
    String name;
    String library;
    String docstring;
    String shortdocstring;
  } fd;
};


//------------------------------------------------------------------------
//Object Function - generic class that object functions are based on
//------------------------------------------------------------------------

#define GET_FUNC_PARAMETER( NAME ) T NAME=(dp->getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));
#define GET_FUNC_PARAMETER_T( NAME, T ) T NAME=(dp->getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));

class ObjectFunctionClass : public DataFuncDescriptor {

 public:

  /* Unfortunately one can't used virtual templated methods. So, these will call
     the non-virtual tempated method process<T> */
  virtual void process_P(F_PARAMETERS_T(Pointer));
  virtual void process_I(F_PARAMETERS_T(Integer));
  virtual void process_N(F_PARAMETERS_T(Number) );
  virtual void process_C(F_PARAMETERS_T(Complex));
  virtual void process_S(F_PARAMETERS_T(String) );
  
  template <class T>
    void setParameter(String, String, T);
  
  template <class T>
    T getParameterDefault(String);
  
  String getParameterName(String);
  
  template <class T>
    void instantiate_one();
  
  virtual void instantiate();
  
  ObjectFunctionClass(Data* dp=NULL);
  virtual ~ObjectFunctionClass();
  
 private:
  /*
    This is a list of parameters that are being used internally for calculations
    in the function.  The value is either filled by a network object of a
    re-definable name or default parameter.
  */
  struct parameter_item {
    /*! Name of the parameter */
    String name;
    /*! Data type of the parameter */
    DATATYPE type;
    /*! Pointer to the actual parameter value */
    void* ptr;
  };
  map<String,parameter_item> parameter_list;
  Data * data_pointer;
};

//------------------------------------------------------------------------

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

//---square
template<class T> inline T hf_square(const T v);
template<>        inline Pointer hf_square<Pointer>(const Pointer v);
template<>        inline String  hf_square<String>(const String v);


//--End Math functions-----------------------------------------------------


/*========================================================================
  class DataFuncLibraryClass 
  ========================================================================*/

/*
  This class stores Libraries of pointers to functions that an object can use.
*/

class DataFuncLibraryClass {
 public:
  
  void add(ConstructorFunctionPointer c_ptr);
  
  ObjectFunctionClass* f_ptr(String name,  String library="Sys");
  
  bool inLibrary(String name,  String library="Sys");
  
  DataFuncDescriptor* FuncDescriptor(String name, String library="Sys");
  
  void dir();
  
  DataFuncLibraryClass();
  ~DataFuncLibraryClass();
  
 private:
  map<String,DataFuncDescriptor> func_library;
  map<String,DataFuncDescriptor>::iterator it;
};

//End DataFuncLibraryClass 
//........................................................................


//------------------------------------------------------------------------
// Netwerk Object and Library functions
//------------------------------------------------------------------------
void DataFunc_Sys_Library_publish(DataFuncLibraryClass*);
void DataFunc_Qt_Library_publish(DataFuncLibraryClass*);


// End netwerk object functions
//------------------------------------------------------------------------



int ReadTextFile(string filename);


#endif
