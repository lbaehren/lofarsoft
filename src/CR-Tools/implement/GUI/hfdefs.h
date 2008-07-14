#ifndef HFDEFS_H
#define HFDEFS_H

//#define DBG_MODE 0
//#define DBG_MODE 1
#define DBG_MODE 0

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <complex>
#include <cmath>


#define ERROR( txt ) ( cout << endl << "ERROR in file " << __FILE__ << " line " << __LINE__ << ": "  << txt << endl ) 

#define MESSAGE( txt ) ( cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt << endl )

#define MSG MESSAGE

#if DBG_MODE == 0 
#define DBG( T ) 
#define DBG2( T ) 
#define DBG3( T ) 
#else
#define DBG MESSAGE
#define DBG2( txt ) ( cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt )
#define DBG3( txt ) ( txt )
#endif


typedef /*long*/ int address ;
//This is used to identify each data field uniquely
typedef int objectid;
//counts to keep track of whether an object was modified - 0 if not modified
typedef int modval;

//Define Types of Data to be stored
typedef int Integer;
typedef double Number;
typedef std::complex<double> Complex;
typedef std::string String;
typedef void* Pointer;

/*!
  \brief Enumeration list of data types that can be used with data objects
*/
enum DATATYPE {
  //! Data type is a pointer
  POINTER,
  //! Data type is an integer number
  INTEGER,
  //! Data type is a (floating point) number
  NUMBER,
  //! Data type is a complex (floating point) number
  COMPLEX,
  //! Data type is a string (of characters)
  STRING,
  //! Data type is undefined
  UNDEF
};

enum REFTYPE {
    REF_MEMORY,
    REF_FUNCTION,
    REF_FILE,
    REF_NODE,
    REF_WEB,REF_NONE
};

enum DATACLASS {
  DAT_CONTAINER,
  DAT_PARAMETER,
  DAT_FILENAME,
  DAT_VECTOR,
  DAT_ARRAY,
  DAT_NONE
};

enum DIRECTION {
  DIR_FROM,
  DIR_TO,
  DIR_BOTH,
  DIR_NONE
};

enum MSG_CODE {MSG_NONE, MSG_MODIFIED};

char* direction_txt(DIRECTION dir);
char* dataclass_txt(DATACLASS x);
char* reftype_txt(REFTYPE x);
char* datatype_txt(DATATYPE x);
char* tf_txt(bool x);
char* pf_txt(bool x);

DIRECTION inv_dir(DIRECTION dir);

class Data;

#define F_PTR( T ) void (*f_ptr_T)(vector<T> *, Data *, Vector_Selector *) 
#define F_PTR_P void (*f_ptr_P)(vector<Pointer> *, Data *, Vector_Selector *)
#define F_PTR_I void (*f_ptr_I)(vector<Integer> *, Data *, Vector_Selector *)
#define F_PTR_N void (*f_ptr_N)(vector<Number> *, Data *, Vector_Selector *)
#define F_PTR_C void (*f_ptr_C)(vector<Complex> *, Data *, Vector_Selector *)
#define F_PTR_S void (*f_ptr_S)(vector<String> *, Data *, Vector_Selector *)
#define DEF_F_PTR( T ) union {F_PTR(T); F_PTR_P; F_PTR_I; F_PTR_N; F_PTR_C; F_PTR_S; void * f_ptr_v;}

#define D_PTR( T )  vector<T> *d_ptr_T
#define D_PTR_P vector<Pointer> *d_ptr_P
#define D_PTR_I vector<Integer> *d_ptr_I
#define D_PTR_N vector<Number> *d_ptr_N
#define D_PTR_C vector<Complex> *d_ptr_C
#define D_PTR_S vector<String> *d_ptr_S
#define DEF_D_PTR( T ) union {D_PTR(T); D_PTR_P; D_PTR_I; D_PTR_N; D_PTR_C; D_PTR_S; void *d_ptr_v;}


#define DEF_VAL_T( T )  T *val_T
#define DEF_VAL_P Pointer *val_P
#define DEF_VAL_I Integer *val_I
#define DEF_VAL_N Number *val_N
#define DEF_VAL_C Complex *val_C
#define DEF_VAL_S String *val_S
#define DEF_VAL_V void *val_V
#define DEF_VAL( T ) union {DEF_VAL_T(T); DEF_VAL_P; DEF_VAL_I; DEF_VAL_N; DEF_VAL_C; DEF_VAL_S;}

#define CALL_FUNC_D_PTR( FUNC )  switch (data.type) {\
    case POINTER:\
      FUNC(*d_ptr_P);\
      break;\
    case INTEGER:\
      FUNC(*d_ptr_I);\
      break;\
    case NUMBER:\
      FUNC(*d_ptr_N);\
      break;\
    case COMPLEX:\
      FUNC(*d_ptr_C);\
      break;	     \
    case STRING:\
      FUNC(*d_ptr_S);\
      break;\
    default:\
      ERROR("Error: unknown type:" << data.type);\
    }

#endif
