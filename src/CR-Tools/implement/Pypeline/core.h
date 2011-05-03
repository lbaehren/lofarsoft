/**************************************************************************
 *  Core header files for CR Pipeline Python bindings                     *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef CR_PIPELINE_CORE_H
#define CR_PIPELINE_CORE_H

// ________________________________________________________________________
//                                            Standard library header files

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <complex>
#include <cmath>
#include <cctype>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <typeinfo>
#include <ctime>

// ________________________________________________________________________
//                                                CASA library header files

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

// ________________________________________________________________________
//                                        Boost Python library header files

#include <boost/python/class.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
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
#include <boost/python/str.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/object_operators.hpp>
#include <boost/thread/thread.hpp>
#include <boost/python/has_back_reference.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/exception_translator.hpp>


// ________________________________________________________________________
//                                                       LOFAR header files
#include <crtools.h>

#include "Math/Constants.h"

// ________________________________________________________________________
//                                              Common settings for hftools

// Replacement of "hfcommon.h"

#define ERROR( txt ) ( std::cout << std::endl << "ERROR in file " << __FILE__ << " line " << __LINE__ << ": "  << txt << std::endl )
#define ERROR_RETURN( txt ) throw PyCR::ValueError((HString) "ERROR [" + __FILE__ + ", l." + BOOST_PP_STRINGIZE(__LINE__) + ", " + BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) + "]: " + txt)

#define MESSAGE( txt ) ( std::cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt << std::endl )
#define PRINT( txt ) ( std::cout << txt << std::endl )
#define MSG MESSAGE

//------------------------------------------
//Determines the architecture: 32 vs 64 bit
//------------------------------------------
//In the following we will assume that pointers
//and HIntegers (i.e. my ints) are of that size.

#if LONG_MAX > 2147483647
#define H_OS64BIT 1
#define H_OS32BIT 0
#define H_NATIVE_WORD_SIZE int64_t
#else
#define H_OS32BIT 1
#define H_OS64BIT 0
#define H_NATIVE_WORD_SIZE int32_t
#endif

//-------------------------------------------

typedef int64_t longint; /* should be at least 64 bits */
typedef H_NATIVE_WORD_SIZE HIntPointer; /* This should be an integer that has the same length as a Pointer*/


typedef HIntPointer address ;
//This is used to identify each data field uniquely
typedef HIntPointer objectid;
//counts to keep track of whether an object was modified - 0 if not modified
typedef HIntPointer modval;


#define MAXINT INT_MAX


//Define Types of Data to be stored
//typedef intptr_t HInteger;
typedef HIntPointer HInteger;
typedef double HNumber;
typedef std::complex<HNumber> HComplex;
typedef bool HBool;
typedef std::string HString;
typedef void* HPointer;

typedef boost::python::object HPyObject;
//typedef boost::python::object* HPyObjectPtr;
typedef PyObject* HPyObjectPtr;

//Define types that are being used for the data reader and the casa arrays
typedef double CasaNumber;
typedef casa::DComplex CasaComplex;
typedef int CasaInteger;
typedef casa::Record CasaRecord ;

#define CasaMatrix casa::Matrix
#define CasaArray casa::Array
#define CasaVector casa::Vector
#define IterValueType typename Iter::value_type

#define STL2CASA_SHARED(TYPE,STLVEC,CASAVEC) TYPE * STL_storage = &(STLVEC[0]); casa::IPosition CASA_shape(1,STLVEC.size()); casa::Vector<TYPE> CASAVEC(CASA_shape,STL_storage,casa::SHARE);


//For some reasons NULL is primarily interpreted as Integer by the
//compiler (e.g., when it needs to determine the type of templated functions).
//So, this forces NULL to really be a NULL pointer ...
#define Null_p reinterpret_cast<HPointer>(NULL)

#define IntAsPtr(INT) reinterpret_cast<HPointer>(INT)
#define PtrAsInt(PTR) reinterpret_cast<HInteger>(PTR)

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
  BOOLEAN,
  //! Data type is a boolean
  UNDEF
};

enum VECTORCLASS {
  //! Data vector class is standard c++ (STL)
  VEC_STL,
  //! Data vector class is CASA (aips++)
  VEC_CASA,
  //! Data vector class is numpy
  VEC_NUMPY,
  //! Data vector class is undefined
  VEC_UNDEF
};


//========================================================================
//                    Casting & Conversion Functions
//========================================================================

template <class S, class T>
void aipsvec2stlvec(CasaVector<S>& data, std::vector<T>& stlvec);

template <class S, class Iter>
void aipsvec2stdit(CasaVector<S>& data, const Iter stlvec, const Iter stlvec_end);

std::vector<HNumber> PyList2STLFloatVec(PyObject* pyob);
std::vector<HInteger> PyList2STLIntVec(PyObject* pyob);
std::vector<int> PyList2STLInt32Vec(PyObject* pyob);
std::vector<uint> PyList2STLuIntVec(PyObject* pyob);

template<class T> T hfnull(){};
template<> inline HString  hfnull<HString>(){HString null=""; return null;}
template<> inline HPointer hfnull<HPointer>(){return NULL;}
template<> inline HInteger hfnull<HInteger>(){return 0;}
template<> inline HNumber  hfnull<HNumber>(){return 0.0;}
template<> inline HComplex hfnull<HComplex>(){return 0.0;}

//Identity
template<class T> inline T hfcast(/*const*/ T v);

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(uint v);
#if H_OS64BIT
template<class T> inline T hfcast(int v);
#endif
template<class T> inline T hfcast(ptrdiff_t v);
template<class T> inline T hfcast(HInteger v);
template<class T> inline T hfcast(HNumber v);
template<class T> inline T hfcast(HComplex v);
template<class T> inline T hfcast(HPointer v);
template<class T> inline T hfcast(HString v);



//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(int v);
template<>  inline HInteger hfcast<HInteger>(HNumber v);
template<>  inline HInteger hfcast<HInteger>(HComplex v);
template<>  inline HNumber hfcast<HNumber>(HComplex v);

template<> inline HString hfcast<HString>(HPointer v);
template<> inline HString hfcast<HString>(HInteger v);
template<> inline HString hfcast<HString>(HNumber v);
template<> inline HString hfcast<HString>(HComplex v);

template<class T> inline T hfcast(HString v);
template<> inline HPointer hfcast(HString v);
template<> inline HInteger hfcast(HString v);
template<> inline HNumber hfcast(HString v);
template<> inline HComplex hfcast(HString v);

template<class T> inline T hfcast(HBool v);
template<>  inline HBool hfcast<HBool>(HInteger v);
template<>  inline HBool hfcast<HBool>(HNumber v);
template<>  inline HBool hfcast<HBool>(HComplex v);
template<>  inline HBool hfcast<HBool>(HString v);
template<>  inline HBool hfcast<HBool>(HPointer v);


template<class T> inline HString hf2string(T v);

template <class T>
HString pretty_vec(std::vector<T> & v,const HInteger maxlen=8);



inline HInteger ptr2int(HPointer v);
inline HPointer int2ptr(HInteger v);


inline HComplex operator*(HInteger i, HComplex c);
inline HComplex operator*(HComplex c, HInteger i);
inline HComplex operator+(HInteger i, HComplex c);
inline HComplex operator+(HComplex c, HInteger i);
inline HComplex operator-(HInteger i, HComplex c);
inline HComplex operator-(HComplex c, HInteger i);
inline HComplex operator/(HInteger i, HComplex c);
inline HComplex operator/(HComplex c, HInteger i);

casa::Int hSum(const casa::Int*, const casa::Int*);
casa::Int hSum(casa::Int* const&, casa::Int* const&);

#if H_OS64BIT
long int hSum(const long int*, long int*);
#endif

#define A_LOW_NUMBER 1.0e-290
#define A_HIGH_NUMBER 1.0e290

//Smallest DOUBLE value: -1.79769E+308
//Largest DOUBLE value: 1.79769E+308


#define hfmax(a,b) (((b)<(a))?(a):(b))
#define hfmin(a,b) (((b)>(a))?(a):(b))

#define hfeven(a)  ((a)%2==0)
#define hfodd(a)  ((a)%2!=0)



//Identity
template<class T> inline T hfcast(/*const*/ T v){return v;}


//Convert Numbers to Numbers and loose information (round float, absolute of complex)
#if H_OS64BIT
template<>  inline HInteger hfcast<HInteger>(int v){return static_cast<HInteger>(v);}
#endif
template<>  inline HInteger hfcast<HInteger>(ptrdiff_t v){return static_cast<HInteger>(v);}
template<>  inline HInteger hfcast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v));}
template<>  inline HInteger hfcast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)));}
template<>  inline HNumber hfcast<HNumber>(HComplex v){return real(v);}

//Convert Numbers to Strings
template<> inline HString hfcast<HString>(HPointer v){std::ostringstream os; os << v; return os.str();}
template<> inline HString hfcast<HString>(uint v){std::ostringstream os; os << v; return os.str();}
template<> inline HString hfcast<HString>(HInteger v){std::ostringstream os; os << v; return os.str();}
template<> inline HString hfcast<HString>(HNumber v){std::ostringstream os; os << v; return os.str();}
template<> inline HString hfcast<HString>(HComplex v){std::ostringstream os; os << v; return os.str();}
template<> inline HPointer hfcast(HString v){HPointer t=NULL; std::istringstream is(v); is >> t; return t;}
template<> inline HInteger hfcast(HString v){HInteger t=0; std::istringstream is(v); is >> t; return t;}
template<> inline HNumber hfcast(HString v){HNumber t=0.0; std::istringstream is(v); is >> t; return t;}
template<> inline HComplex hfcast(HString v){HComplex t=0.0; std::istringstream is(v); is >> t; return t;}

//Bools
template<class T> inline T hfcast(HBool v){return hfcast<T>(hfcast<HInteger>(v));}
template<>  inline HBool hfcast<HBool>(HInteger v){return (HInteger)v;}
template<>  inline HBool hfcast<HBool>(HNumber v){return hfcast<HBool>((HInteger)v);}
template<>  inline HBool hfcast<HBool>(HComplex v){return hfcast<HBool>(hfcast<HInteger>(v));}
template<>  inline HBool hfcast<HBool>(HString v){return hfcast<HBool>(hfcast<HInteger>(v));}
template<>  inline HBool hfcast<HBool>(HPointer v){return hfcast<HBool>(hfcast<HInteger>(v));}

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(uint v){return static_cast<T>(v);}
#if H_OS64BIT
template<class T> inline T hfcast(int v){return static_cast<T>(v);}
#endif
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}
template<class T> inline T hfcast(HPointer v){return hfcast<T>(reinterpret_cast<HInteger>(v));}
template<class T> inline T hfcast(HString v){T t; std::istringstream is(v); is >> t; return t;}


inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}

template<class T> inline HString hf2string(T v){std::ostringstream os; os << v; return os.str();}

inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}

inline bool operator< (HInteger i, HComplex c) {return (i <  real(c));}
inline bool operator> (HInteger i, HComplex c) {return (i >  real(c));}
inline bool operator>=(HInteger i, HComplex c) {return (i >= real(c));}
inline bool operator<=(HInteger i, HComplex c) {return (i <= real(c));}
inline bool operator< (HNumber  n, HComplex c) {return (n <  real(c));}
inline bool operator> (HNumber  n, HComplex c) {return (n >  real(c));}
inline bool operator>=(HNumber  n, HComplex c) {return (n >= real(c));}
inline bool operator<=(HNumber  n, HComplex c) {return (n <= real(c));}

inline bool operator< (HComplex c, HInteger i) {return ( real(c) <  i);}
inline bool operator> (HComplex c, HInteger i) {return ( real(c) >  i);}
inline bool operator>=(HComplex c, HInteger i) {return ( real(c) >= i);}
inline bool operator<=(HComplex c, HInteger i) {return ( real(c) <= i);}
inline bool operator< (HComplex c, HNumber  n) {return ( real(c) <  n);}
inline bool operator> (HComplex c, HNumber  n) {return ( real(c) >  n);}
inline bool operator>=(HComplex c, HNumber  n) {return ( real(c) >= n);}
inline bool operator<=(HComplex c, HNumber  n) {return ( real(c) <= n);}


//========================================================================
//                        Helper Functions
//========================================================================

void hInit();
HString hgetFiletype(HString filename);
HString hgetFileExtension(HString filename);



//========================================================================
//                 Casting & Conversion Functions
//========================================================================

template <class T>
HString pretty_vec(std::vector<T> & v,const HInteger maxlen){
  if (v.size()==0) return "[]";
  int i=1;
  typename std::vector<T>::iterator it=v.begin();
  HString s=("[");
  s+=hf2string(*it); ++it; ++i;
  while (it!=v.end() && i< maxlen) {
    s+=","+hf2string(*it); ++it;  ++i;
  };
  if ((HInteger)v.size()>maxlen) s+=",...]";
  else s+="]";
  return s;
}


template <class T>
CasaVector<T> stl2casa(std::vector<T>& stlvec)
{
  T * storage = &(stlvec[0]);
  casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
  CasaVector<T> casavec(shape,storage,casa::SHARE);
  return casavec;
}

template <class S, class T>
void aipscol2stlvec(casa::Matrix<S> &data, std::vector<T>& stlvec, const HInteger col){
    HInteger i,nrow,ncol;

    nrow=data.nrow();
    ncol=data.ncolumn();
    //    if (ncol>1) {MSG("aipscol2stlvec: ncol="<<ncol <<" (nrow="<<nrow<<")");};
    if (col>=ncol) {
	ERROR("aipscol2stlvec: column number col=" << col << " is larger than total number of columns (" << ncol << ") in matrix.");
	stlvec.clear();
	return;
    }

    stlvec.resize(nrow);
    CasaVector<S> CASAVec = data.column(col);

//    p=stlvec.begin();

    for (i=0;i<nrow;i++) {
//	*p=hfcast<T>(CASAVec[i]);
	stlvec[i]=hfcast<T>(CASAVec[i]);
//	p++;
    };
}

/*!
  \brief The function converts an aips++ vector to an stl vector
 */

template <class S, class T>
void aipsvec2stlvec(CasaVector<S>& data, std::vector<T>& stlvec){
    HInteger i,n=data.size();
    stlvec.resize(n);
    for (i=0;i<n;i++) {
	stlvec[i]=hfcast<T>(data[i]);
    };
}

/*!
  \brief Copies a aips++ vector to a vector of a different type specified by iterators or pointers.
 */
template <class S, class Iter>
void aipsvec2stdit(CasaVector<S>& data, const Iter stlvec, const Iter stlvec_end){
  typedef IterValueType T;
  HInteger i,n=data.size();
  Iter vec=stlvec;
  for (i=0; (i<n) && (vec!=stlvec_end); i++) {
    *vec=hfcast<T>(data[i]);
    ++vec;
  };
}


//This function is copied from hfget.cc since I don't know a better
//way to do a proper instantiation of a templated function across two
//cpp files ..
template <class T, class S>
void copycast_vec(std::vector<T> &vi, std::vector<S> & vo) {
  typedef typename std::vector<T>::iterator Tit;
  typedef typename std::vector<S>::iterator Sit;
  Tit it1=vi.begin();
  Tit end=vi.end();
  if (it1==end) {vo.clear();}
  else {
    vo.assign(vi.size(),hfnull<S>()); //make the new vector equal in size and initialize with proper Null values
    Sit it2=vo.begin();
    while (it1!=end) {
      *it2=hfcast<S>(*it1);
      it1++; it2++;
    };
  };
}

// ========================================================================
//  Additional string operation functions
// ========================================================================

bool stringToLower(std::string& ioString);
bool stringToUpper(std::string& ioString);
vector<HString> stringSplit(const HString& str_const);
vector<HString> stringSplit(const HString& str_const, const HString& delim);


// ========================================================================
//
//  Exception classes
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  /*!
    \brief Exception base class
  */
  class Exception
  {
  public:
    Exception(std::string m)
    {
      message = m;
    };

    ~Exception(){};

    std::string message;
  };

  // Derived errors
  class ValueError: public Exception
  {
  public:
    ValueError(std::string m) : Exception(m) {};
  };

  class TypeError: public Exception
  {
  public:
    TypeError(std::string m) : Exception(m) {};
  };

  class KeyError: public Exception
  {
  public:
    KeyError(std::string m) : Exception(m) {};
  };

  class IndexError: public Exception
  {
  public:
    IndexError(std::string m) : Exception(m) {};
  };

  class MemoryError: public Exception
  {
  public:
    MemoryError(std::string m) : Exception(m) {};
  };

  class ArithmeticError: public Exception
  {
  public:
    ArithmeticError(std::string m) : Exception(m) {};
  };

  class EOFError: public Exception
  {
  public:
    EOFError(std::string m) : Exception(m) {};
  };

  class FloatingPointError: public Exception
  {
  public:
    FloatingPointError(std::string m) : Exception(m) {};
  };

  class OverflowError: public Exception
  {
  public:
    OverflowError(std::string m) : Exception(m) {};
  };

  class ZeroDivisionError: public Exception
  {
  public:
    ZeroDivisionError(std::string m) : Exception(m) {};
  };

  class NameError: public Exception
  {
  public:
    NameError(std::string m) : Exception(m) {};
  };

  class NotImplementedError: public Exception
  {
  public:
    NotImplementedError(std::string m) : Exception(m) {};
  };

  class IOError: public Exception
  {
  public:
    IOError(std::string m): Exception(m) {};
  };

} // Namespace PyCR -- end

#endif /* CR_PIPELINE_CORE_H */

