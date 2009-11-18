#ifndef HFANALYSIS_H
#define HFANALYSIS_H

#define STLVectorIteratorT typename vector<T>::iterator
#define STLVectorIteratorI typename vector<HInteger>::iterator
#define STLVectorIteratorN typename vector<HNumber>::iterator
#define STLVectorIteratorC typename vector<HComplex>::iterator

#define DefPythonWrapper_VecINCS_0_Parameters(FUNC) \
  extern void (*FUNC##_I)(vector<HInteger> &vec); \
  extern void (*FUNC##_N)(vector<HNumber > &vec); \
  extern void (*FUNC##_C)(vector<HComplex > &vec); \
  extern void (*FUNC##_S)(vector<HString > &vec)
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

//!Negate a vector (multiply with -1) 
template <class T> 
void hNegate (const STLVectorIteratorT data_start,
	      const STLVectorIteratorT data_end);
template <class T> inline void hNegate (vector<T> &vec);
template <class T> inline void hNegate (casa::Vector<T> &vec);
DefPythonWrapper_VecINCS_0_Parameters(hNegate);

//!Fill a vector with a constant value
template <class T> 
void hNegate (const STLVectorIteratorT data_start,
	      const STLVectorIteratorT data_end,
	      T val);
template <class T> inline void hNegate (vector<T> &vec, T val);
template <class T> inline void hNegate (casa::Vector<T> &vec, T val);
DefPythonWrapper_VecINCS_1_TParameters(hFill);


/*
extern void (*hRunningAverageVec_N_3)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len);
extern void (*hRunningAverageVec_I_3)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len);
extern void (*hRunningAverageVec_C_3)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len);
extern void (*hRunningAverageVec_N_2)(vector<HNumber > &vec_in,vector<HNumber > &vec_out);
extern void (*hRunningAverageVec_I_2)(vector<HInteger> &vec_in,vector<HInteger> &vec_out);
extern void (*hRunningAverageVec_C_2)(vector<HComplex> &vec_in,vector<HComplex> &vec_out);
*/





#endif
