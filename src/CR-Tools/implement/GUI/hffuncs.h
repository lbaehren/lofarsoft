#ifndef HFFUNCS_H
#define HFFUNCS_H

#include <mgl/mgl_qt.h>

#define F_PARAMETERS        vector<T> *vp, Data *dp, Vector_Selector *vs
#define F_PARAMETERS_NOVEC                 Data *dp, Vector_Selector *vs                       
#define F_PARAMETERS_DEF    vector<T> *vp; Data *dp; Vector_Selector *vs
#define F_PARAMETERS_CALL              vp,       dp,                  vs
#define F_PARAMETERS_CALL_NOVEC                  dp,                  vs                     
#define F_PARAMETERS_T( T ) vector<T> *vp, Data *dp, Vector_Selector *vs
#define F_PARAMETERS_h      F_PARAMETERS=NULL

#define DEFINE_PROCESS_CALLS_NUMONLY\
  void process_I(F_PARAMETERS_T(HInteger)) {process<HInteger>(F_PARAMETERS_CALL);process_end();} \
  void process_N(F_PARAMETERS_T(HNumber) ) {process<HNumber>(F_PARAMETERS_CALL);process_end();}\
  void process_C(F_PARAMETERS_T(HComplex)) {process<HComplex>(F_PARAMETERS_CALL);process_end();}

#define DEFINE_PROCESS_CALLS_WITHOUT_POINTER\
  DEFINE_PROCESS_CALLS_NUMONLY\
  void process_S(F_PARAMETERS_T(HString) ) {process<HString>(F_PARAMETERS_CALL);process_end();}

#define DEFINE_PROCESS_CALLS\
  DEFINE_PROCESS_CALLS_WITHOUT_POINTER\
  void process_P(F_PARAMETERS_T(HPointer)) {process<HPointer>(F_PARAMETERS_CALL);process_end();}

#define DEFINE_PROCESS_CALLS_h\
  void process_P(F_PARAMETERS_T(HPointer));\
  void process_I(F_PARAMETERS_T(HInteger));\
  void process_N(F_PARAMETERS_T(HNumber) );\
  void process_C(F_PARAMETERS_T(HComplex));\
  void process_S(F_PARAMETERS_T(HString) )

#define DEFINE_PROCESS_CALLS_IGNORE_DATATYPE\
  void process_P(F_PARAMETERS_T(HPointer)) {process(F_PARAMETERS_CALL_NOVEC);process_end();}\
  void process_I(F_PARAMETERS_T(HInteger)) {process(F_PARAMETERS_CALL_NOVEC);process_end();}\
  void process_N(F_PARAMETERS_T(HNumber) ) {process(F_PARAMETERS_CALL_NOVEC);process_end();}\
  void process_C(F_PARAMETERS_T(HComplex)) {process(F_PARAMETERS_CALL_NOVEC);process_end();}\
  void process_S(F_PARAMETERS_T(HString) ) {process(F_PARAMETERS_CALL_NOVEC);process_end();}


void qrun(MainWindow **label);
void qrun2(QLabel **label);
void mglrun(mglGraphQT **mglwin);

/*------------------------------------------------------------------------
Some file utilities
------------------------------------------------------------------------*/
HString file_get_extension(HString filename);
HString determine_filetype(HString filename);

/*------------------------------------------------------------------------
Some aips++ to stl vector conversions
------------------------------------------------------------------------*/
template <class S, class T>
    void aipscol2stlvec(casa::Matrix<S>& data, vector<T>& stlvec, HInteger col);

template <class S, class T>
    void aipsvec2stlvec(casa::Vector<S>& data, vector<T>& stlvec);


/*========================================================================
  class DataFuncDescriptor
  ========================================================================

This class stores informations about the functions that are being used in an object.

*/
#define DATAFUNC_CONSTRUCTOR( NAME, LIB, INFO, TYP, BUFFERED)			\
DataFuncDescriptor DataFunc_##LIB##_##NAME##_Constructor(Data * dp=NULL){\
  DataFuncDescriptor fd;\
  fd.setInfo(#NAME,#LIB,INFO); \
  fd.setType(TYP);\
  fd.setBuffered(BUFFERED);\
  fd.setConstructor(&DataFunc_##LIB##_##NAME##_Constructor);\
  if (dp!=NULL) {\
    fd.setFunction(new DataFunc_##LIB##_##NAME(dp));\
    fd.getFunction()->setInfo(#NAME,#LIB,INFO); \
  } \
  else {fd.setFunction(NULL);};\
  return fd;\
}

#define BEGIN_OBJECT_FUNCTION(LIB,NAME)		\
class DataFunc_##LIB##_##NAME : public ObjectFunctionClass {	\
public:\
DEFINE_PROCESS_CALLS\
 DataFunc_##LIB##_##NAME (Data* dp) : ObjectFunctionClass(dp){	\
   dp->setUpdateable(O_UPDATEABLE);		\
    setParameters();\
    startup();\
    getParameters();\
    }\
 ~DataFunc_##LIB##_##NAME(){cleanup(); }

 

#define END_OBJECT_FUNCTION(LIB,NAME)  }; DATAFUNC_CONSTRUCTOR( NAME , LIB , O_INFO , O_TYPE , O_BUFFERED );

#define PUBLISH_OBJECT_FUNCTION(LIB,NAME) library_ptr->add(&DataFunc_##LIB##_##NAME##_Constructor)


typedef DataFuncDescriptor (*ConstructorFunctionPointer)(Data*);

/*!
  \class DataFuncDescriptor

  \ingroup CR_GUI

  \author Heino Falcke
*/
class DataFuncDescriptor {

 public:

  /*!
    \brief Set the information on a data function descriptor

    \param name           -- 
    \param library        -- 
    \param shortdocstring -- 
    \param docstring      -- 
  */
  void setInfo (HString name,
		HString library="Sys",
		HString shortdocstring="",
		HString docstring="");

  void setType (DATATYPE typ);

  void setFunction(ObjectFunctionClass* f_ptr);

  ObjectFunctionClass* getFunction();

  ObjectFunctionClass* newFunction(Data*,DATATYPE typ=UNDEF);

  void setConstructor(ConstructorFunctionPointer);
  ConstructorFunctionPointer getConstructor();

  HString getName(bool fullname=false);
  DATATYPE getType();
  bool getBuffered();
  /*!  If a function is buffered, a vector will be created by default
that is attached to the object, where the result of the function is
stored*/
  void setBuffered(bool buf=true); 
  HString getLibrary();
  HString getDocstring(bool shortdoc=true);

  DataFuncDescriptor();
  ~DataFuncDescriptor();

private:
  struct function_descr { 
    ObjectFunctionClass *f_ptr;
    ConstructorFunctionPointer constructor;
    HString name;
    HString library;
    HString docstring;
    HString shortdocstring;
    DATATYPE deftype;
    bool buffered;
  } fd;
};


//------------------------------------------------------------------------
//Object Function - generic class that object functions are based on
//------------------------------------------------------------------------


//#define GET_FUNC_PARAMETER( NAME ) T NAME=(getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));
//#define GET_FUNC_PARAMETER_T( NAME, T ) T NAME=(getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));

#define GET_FUNC_PARAMETER( NAME, TYP ) TYP NAME=(getParameter(getParameterName(#NAME),getParameterDefault<TYP>(#NAME)));
#define SET_FUNC_PARAMETER( NAME, TYP , DEFVAL) setParameter(#NAME,mycast<TYP>(DEFVAL));

#define GET_FUNC_PARAMETER_AWK( NAME, TYP , DEFVAL) TYP NAME=(getParameter(getParameterName(#NAME),getParameterDefault<TYP>(#NAME)));
#define SET_FUNC_PARAMETER_AWK( NAME, TYP , DEFVAL) setParameter(#NAME,mycast<TYP>(DEFVAL));

#define INIT_FUNC_ITERATORS(IT,END)		   \
  typedef typename vector<T>::iterator iterator_T; \
  iterator_T IT=vp->begin(); \
  iterator_T END=vp->end(); \
  if (IT==END) {return;};  




class ObjectFunctionClass : public DataFuncDescriptor {

public:
  virtual void process_P(F_PARAMETERS_T(HPointer)); //unfortunately one can't used virtual templated methods.
  virtual void process_I(F_PARAMETERS_T(HInteger)); //So, these will call the non-virtual tempated method process<T>
  virtual void process_N(F_PARAMETERS_T(HNumber) );
  virtual void process_C(F_PARAMETERS_T(HComplex));
  virtual void process_S(F_PARAMETERS_T(HString) );

  virtual void setParameters();
  virtual void startup();
  virtual void cleanup();
  void process_end();

  void setParameter(HString internal_name, HPointer default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HInteger default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HNumber default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HComplex default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HString default_value, HString prefix="'", HString external_name="");

  template <class T>
      void setParameterT(HString internal_name, T default_value, HString prefix="'", HString external_name="");

  template <class T>
    T getParameterDefault(HString);
  
  void getParameters();
  
  HString getParameterName(HString);

  vector<HString> getParameterList(HString first_element="");

  HString getResultsObjectName();
  Data* getResultsObject();
  Data* getResultObject(HString name);
  HString getParametersObjectName();
  Data* getParametersObject();
  Data* getParameterObject(HString name);

  Data* getParameterObjectPointer(HString);
  void setParameterObjectPointer(HString, Data*);
 
  template <class T>
    T getParameter(const HString name, const T defval);
  template <class T>
    T putParameter(const HString name, const T val);
  template <class T>  
    Data* putResult(HString name, T val);
  template <class T>  
    Data* putVecResult(HString name,vector<T> vec);

  template <class T>
    void instantiate_one();
  
  virtual void instantiate();
  
  ObjectFunctionClass(Data* dp=NULL);
  ~ObjectFunctionClass();

  Data * data_pointer;

 private:
  /*
    This is a list of parameters that are being used internally for calculations in the function.  The value
    is either filled by a network object of a re-definable name or default parameter
   */
  
  struct parameter_item {
    HString name;
    DATATYPE type;
    void* ptr;
  };

  map<HString,parameter_item> parameter_list;
  map<HString,parameter_item>::iterator pit;
  map<HString,Data*> parameter_pointer;
  map<HString,Data*>::iterator ppit;
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
#ifndef HF_MATH_FUNC
#undef HF_MATH_FUNC
#endif
#ifndef HF_MATH_FUNC2
#undef HF_MATH_FUNC2
#endif

#define HF_MATH_FUNC( FUNC ) \
  template<class T> inline T FUNC(const T v);				\
  template<>        inline HString FUNC<HString>(const HString v);	\
  template<>        inline HPointer FUNC<HPointer>(const HPointer v);	

#define HF_MATH_FUNC2( FUNC ) \
  template<class T, class S> inline T FUNC(const T v1, const S v2);\
  inline HString FUNC(const HString v1, const HString v2); \
  template<class S>        inline HString FUNC(const HString v1, const S v2); \
  template<class S>        inline HString FUNC(const S v1, const HString v2); \
  inline HPointer FUNC(const HPointer v1, const HPointer v2); \
  template<class S>        inline HPointer FUNC(const HPointer v1, const S v2); \
  template<class S>        inline HPointer FUNC(const S v1, const HPointer v2); 

/*
  template<>  inline HPointer FUNC<HPointer,HPointer>(const HPointer v1, const HPointer v2); \
  template<>        inline HString FUNC<HString,HString>(const HString v1, const HString v2); \
  */
					

HF_MATH_FUNC(hf_Square)
HF_MATH_FUNC(hf_Sqrt)

HF_MATH_FUNC2(hf_Sub)
HF_MATH_FUNC2(hf_Mul)
HF_MATH_FUNC2(hf_Add)
HF_MATH_FUNC2(hf_Div)
HF_MATH_FUNC2(hf_Pow)


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

  ObjectFunctionClass* f_ptr(HString name,  HString library="Sys");

  bool inLibrary(HString name,  HString library="Sys");

  DataFuncDescriptor* FuncDescriptor(HString name, HString library="Sys");

  void dir();

  DataFuncLibraryClass();
  ~DataFuncLibraryClass();

 private:
  map<HString,DataFuncDescriptor> func_library;
  map<HString,DataFuncDescriptor>::iterator it;
};

//End DataFuncLibraryClass 
//........................................................................


//------------------------------------------------------------------------
// Netwerk Object and Library functions
//------------------------------------------------------------------------
void DataFunc_Library_publish(DataFuncLibraryClass*);

// End netwerk object functions
//------------------------------------------------------------------------



int ReadTextFile(string filename);


#endif
