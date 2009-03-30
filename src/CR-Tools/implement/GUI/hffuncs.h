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
  void process_I(F_PARAMETERS_T(HInteger)) {process<HInteger>(F_PARAMETERS_CALL);}\
  void process_N(F_PARAMETERS_T(HNumber) ) {process<HNumber>(F_PARAMETERS_CALL);}\
  void process_C(F_PARAMETERS_T(HComplex)) {process<HComplex>(F_PARAMETERS_CALL);}

#define DEFINE_PROCESS_CALLS_WITHOUT_POINTER\
  DEFINE_PROCESS_CALLS_NUMONLY\
  void process_S(F_PARAMETERS_T(HString) ) {process<HString>(F_PARAMETERS_CALL);}

#define DEFINE_PROCESS_CALLS\
  DEFINE_PROCESS_CALLS_WITHOUT_POINTER\
  void process_P(F_PARAMETERS_T(HPointer)) {process<HPointer>(F_PARAMETERS_CALL);}

#define DEFINE_PROCESS_CALLS_h\
  void process_P(F_PARAMETERS_T(HPointer));\
  void process_I(F_PARAMETERS_T(HInteger));\
  void process_N(F_PARAMETERS_T(HNumber) );\
  void process_C(F_PARAMETERS_T(HComplex));\
  void process_S(F_PARAMETERS_T(HString) )

#define DEFINE_PROCESS_CALLS_IGNORE_DATATYPE\
  void process_P(F_PARAMETERS_T(HPointer)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_I(F_PARAMETERS_T(HInteger)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_N(F_PARAMETERS_T(HNumber) ) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_C(F_PARAMETERS_T(HComplex)) {process(F_PARAMETERS_CALL_NOVEC);}\
  void process_S(F_PARAMETERS_T(HString) ) {process(F_PARAMETERS_CALL_NOVEC);}


void qrun(MainWindow **label);
void qrun2(QLabel **label);
void mglrun(mglGraphQT **mglwin);

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

#define GET_FUNC_PARAMETER( NAME ) T NAME=(dp->getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));
#define GET_FUNC_PARAMETER_T( NAME, T ) T NAME=(dp->getParameter(getParameterName(#NAME),getParameterDefault<T>(#NAME)));

class ObjectFunctionClass : public DataFuncDescriptor {

public:
  virtual void process_P(F_PARAMETERS_T(HPointer)); //unfortunately one can't used virtual templated methods.
  virtual void process_I(F_PARAMETERS_T(HInteger)); //So, these will call the non-virtual tempated method process<T>
  virtual void process_N(F_PARAMETERS_T(HNumber) );
  virtual void process_C(F_PARAMETERS_T(HComplex));
  virtual void process_S(F_PARAMETERS_T(HString) );

  void setParameter(HString internal_name, HPointer default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HInteger default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HNumber default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HComplex default_value, HString prefix="'", HString external_name="");
  void setParameter(HString internal_name, HString default_value, HString prefix="'", HString external_name="");

  template <class T>
      void setParameterT(HString internal_name, T default_value, HString prefix="'", HString external_name="");

  template <class T>
    T getParameterDefault(HString);
  
  HString getParameterName(HString);

  vector<HString> getParameterList(HString first_element="");

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
template<>        inline HPointer hf_square<HPointer>(const HPointer v);
template<>        inline HString  hf_square<HString>(const HString v);


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
void DataFunc_Sys_Library_publish(DataFuncLibraryClass*);
void DataFunc_Qt_Library_publish(DataFuncLibraryClass*);
void DataFunc_CR_Library_publish(DataFuncLibraryClass*);
void DataFunc_Py_Library_publish(DataFuncLibraryClass*);

// End netwerk object functions
//------------------------------------------------------------------------



int ReadTextFile(string filename);


#endif
