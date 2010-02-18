#ifndef HFANALYSIS_H
#define HFANALYSIS_H

#include <cmath>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>


#include <GUI/hfdefs.h>
#include <GUI/hfcast.h>
#include <GUI/hffuncs.awk.h>
#include <crtools.h>

#include "IO/LopesEventIn.h"
#include "IO/LOFAR_TBB.h"

#include <GUI/hfpp.h>

//========================================================================
//End Preprocessor Definitions
//========================================================================

#define STLVectorIteratorT typename vector<T>::iterator
#define STLVectorIteratorI typename vector<HInteger>::iterator
#define STLVectorIteratorN typename vector<HNumber>::iterator
#define STLVectorIteratorC typename vector<HComplex>::iterator
#define IterValueType typename iterator_traits<Iter>::value_type
//typename Iter::value_type

//________________________________________________________________________
//                                                     Function Definition

template <class Iter>
HInteger hFindLowerBound(const Iter data_start,
			 const Iter data_end,
			 const typename iterator_traits<Iter>::value_type value);

//Testing c-style arrays and pointers wrapper ...
HInteger hFindLowerBound(const HNumber* data_start,
			 const HInteger len,
			 const HNumber value);

template <class Iter>
void hNegate(const Iter data_start,
	     const Iter data_end);

template <class Iter>
void hFill(const Iter data_start,
	   const Iter data_end,
	   const IterValueType fill_value);

template <class Iter>
IterValueType hSum(const Iter data_start,
		   const Iter data_end);

template <class Iter>
IterValueType hMean (const Iter data_start,
		     const Iter data_end);

template <class Iter>
IterValueType hMedian(const Iter data_start,
		      const Iter data_end);

template <class Iter>
IterValueType hStdDev(const Iter data_start,
		      const Iter data_end,
		      const IterValueType mean);

template <class Iter>
IterValueType hStdDev(const Iter data_start,
		      const Iter data_end);

template <class Iter>
void hSquareVec (const Iter idata_start,
		 const Iter idata_end);

template <class Iter>
void hAbsVec (const Iter idata_start,
	      const Iter idata_end);

template <class Iter>
void hAddVecs  (const Iter idata_start,
		const Iter idata_end,
		const Iter odata_start,
		const Iter odata_end);

template <class Iter>
void hDownsample (const Iter idata_start,
		  const Iter idata_end,
		  const Iter odata_start,
		  const Iter odata_end);




//________________________________________________________________________
//                                                 Preprocessor Definition

#undef HF_PP_FILETYPE
#define HF_PP_FILETYPE() (hFILE)  // Tell the preprocessor (for generating wrappers) that this is a c++ header file (brackets are crucial)
#include "hfwrappers-hfanalysis.cc.h"  // include wrapper definitions generated automatically from source

#define DefPythonWrapper_VecINCS_0_Parameters(FUNC) \
  void FUNC (vector<HPointer > &vec);		    \
  extern void (*FUNC##_I)(vector<HInteger> &vec); \
  extern void (*FUNC##_N)(vector<HNumber > &vec); \
  extern void (*FUNC##_C)(vector<HComplex > &vec); \
  extern void (*FUNC##_S)(vector<HString > &vec)

#define DefPythonWrapper_TVecINC_0_Parameters(FUNC)     \
  HPointer FUNC (vector<HPointer > &vec);		 \
  HString  FUNC (vector<HString > &vec);		 \
  extern HString (*FUNC##_S)(vector<HString > &vec);\
  extern HInteger (*FUNC##_I)(vector<HInteger> &vec);	\
  extern HNumber (*FUNC##_N)(vector<HNumber > &vec) ;	\
  extern HComplex (*FUNC##_C)(vector<HComplex > &vec);

#define DefVecWrappers_TFunc_Vec_0_Parameters(FUNC)	\
  template <class T> inline T FUNC (vector<T> &vec);		\
  template <class T> inline T FUNC (casa::Vector<T> &vec)

#define DefPythonWrapper_VecINCS_1_TParameters(FUNC)   \
  extern void (*FUNC##_I)(vector<HInteger> &vec,HInteger);	   \
  extern void (*FUNC##_N)(vector<HNumber > &vec,HNumber); \
  extern void (*FUNC##_C)(vector<HComplex > &vec,HComplex); \
  extern void (*FUNC##_S)(vector<HString > &vec,HString)

#define DefPythonWrapper_VecINCS_8_Parameters(FUNC,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8)   \
  extern void (*FUNC##_I)(vector<HInteger> &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8);	  \
  extern void (*FUNC##_N)(vector<HNumber > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8); \
  extern void (*FUNC##_C)(vector<HComplex > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8); \
  extern void (*FUNC##_S)(vector<HString > &vec,VAL1,VAL2,VAL3,VAL4,VAL5,VAL6,VAL7,VAL8)



//========================================================================
//End Preprocessor Definitions
//========================================================================

//!Function to close a file with a datareader object providing the pointer to the object as an integer
void hCloseFile(HIntPointer iptr);

//!Function to create a DataReader Object returning an integer containing the pointer (for Python)
HIntPointer hOpenFile(HString Filename, vector<HInteger> & Offsets);

//! Read data from a Datareader object (pointer in iptr) into an stl vector
template <class T>
void hReadFile(vector<T> & vec,
	       const HIntPointer iptr,
	       const HString Datatype,
	       const HInteger Antenna,
	       const HInteger Blocksize,
	       const HInteger Block,
	       const HInteger Stride,
	       const HInteger Shift,
	       vector<HInteger> & Offsets);

DefPythonWrapper_VecINCS_8_Parameters(hReadFile,
				   const HIntPointer iptr,
				   const HString Datatype,
				   const HInteger Antenna,
				   const HInteger Blocksize,
				   const HInteger Block,
				   const HInteger Stride,
				   const HInteger Shift,
				      vector<HInteger> & Offsets);

//! function value of a Gaussian distribution
inline HNumber funcGaussian(HNumber x,
			    HNumber sigma=1.0,
			    HNumber mu=0.0);

//! Calculate the running average of an array of Numbers.
template <class T>
void hRunningAverageT (const STLVectorIteratorT idata_start,
		       const STLVectorIteratorT idata_end,
		       const STLVectorIteratorT odata_start,
		       const STLVectorIteratorN weights_start,
		       const STLVectorIteratorN weights_end);

//! Calculate the running average of an array of Numbers.
template <class T>
void hRunningAverageN (T* start_data_in,
		       T* start_data_out,
		       address datalen,
		       HNumber * start_weights,
		       address weightslen);


enum hWEIGHTS {WEIGHTS_FLAT,WEIGHTS_LINEAR,WEIGHTS_GAUSSIAN};

vector<HNumber> hWeights(address wlen, hWEIGHTS wtype);
vector<HNumber> hFlatWeights(address wlen);
vector<HNumber> hLinearWeights(address wlen);
vector<HNumber> hGaussianWeights(address wlen);

template <class T>
void hRunningAverageVec(vector<T> &vec_in,vector<T> &vec_out, address len=5, hWEIGHTS wtype=WEIGHTS_LINEAR);
void hRunningAverageVec(vector<HString> &vec_in,vector<HString> &vec_out, address wlen, hWEIGHTS wtype);
void hRunningAverageVec(vector<HPointer> &vec_in,vector<HPointer> &vec_out, address wlen, hWEIGHTS wtype);

extern void (*hRunningAverageVec_N)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len, hWEIGHTS wtype);
extern void (*hRunningAverageVec_I)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len, hWEIGHTS wtype);
extern void (*hRunningAverageVec_C)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len, hWEIGHTS wtype);



#endif
#undef HF_PP_FILETYPE
