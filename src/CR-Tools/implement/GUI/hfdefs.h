#ifndef HFDEFS_H
#define HFDEFS_H

//#define DBG_MODE 0
//#define DBG_MODE 1
#define DBG_MODE 0
#define VERBOSE 1

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <complex>
#include <cmath>

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/List.h>
#include <casa/Containers/ListIO.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Exceptions/Error.h>

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/utility.hpp>

#include <boost/python/call_method.hpp>
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
#include <boost/python/has_back_reference.hpp>
#include <boost/python/handle.hpp>

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

#if DBG_MODE == 2 
#define D2BG MESSAGE
#define D2BG2( txt ) ( cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt )
#define D2BG3( txt ) ( txt )
#else
#define D2BG( T ) 
#define D2BG2( T ) 
#define D2BG3( T ) 
#endif



//Not yet implemented .... should trace the run thropugh the networ in a more readable form.
#if VERBOSE == 0 
#define VERB( T ) 
#define VERB2( T ) 
#define VERB3( T ) 
#else
#define VERB MESSAGE
#define VERB2( txt ) ( cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt )
#define VERB3( txt ) ( txt )
#endif


typedef long int longint; /* should be at least 64 bits */

typedef /*long*/ int address ;
//This is used to identify each data field uniquely
typedef int objectid;
//counts to keep track of whether an object was modified - 0 if not modified
typedef int modval;


#define MAXINT 2147483648

//the maximum version number that is allowed inside an object (should be ~MAXINT)
#define MAXVERSION MAXINT


//Define Types of Data to be stored
typedef int HInteger;
typedef double HNumber;
typedef std::complex<double> HComplex;
typedef std::string HString;
typedef void* HPointer;

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

#define PUSH DIR_TO
#define PULL DIR_FROM

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
#define F_PTR_P void (*f_ptr_P)(vector<HPointer> *, Data *, Vector_Selector *)
#define F_PTR_I void (*f_ptr_I)(vector<HInteger> *, Data *, Vector_Selector *)
#define F_PTR_N void (*f_ptr_N)(vector<HNumber> *, Data *, Vector_Selector *)
#define F_PTR_C void (*f_ptr_C)(vector<HComplex> *, Data *, Vector_Selector *)
#define F_PTR_S void (*f_ptr_S)(vector<HString> *, Data *, Vector_Selector *)
#define DEF_F_PTR( T ) union {F_PTR(T); F_PTR_P; F_PTR_I; F_PTR_N; F_PTR_C; F_PTR_S; void * f_ptr_v;}

#define D_PTR( T )  vector<T> *d_ptr_T
#define D_PTR_P vector<HPointer> *d_ptr_P
#define D_PTR_I vector<HInteger> *d_ptr_I
#define D_PTR_N vector<HNumber> *d_ptr_N
#define D_PTR_C vector<HComplex> *d_ptr_C
#define D_PTR_S vector<HString> *d_ptr_S
#define DEF_D_PTR( T ) union {D_PTR(T); D_PTR_P; D_PTR_I; D_PTR_N; D_PTR_C; D_PTR_S; void *d_ptr_v;}


#define DEF_VAL_T( T )  T *val_T
#define DEF_VAL_P HPointer *val_P
#define DEF_VAL_I HInteger *val_I
#define DEF_VAL_N HNumber *val_N
#define DEF_VAL_C HComplex *val_C
#define DEF_VAL_S HString *val_S
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
