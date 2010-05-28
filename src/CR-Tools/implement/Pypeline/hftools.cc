/*-------------------------------------------------------------------------*
 | $Id:: hftools.cc 3995 2010-01-18 10:09:09Z falcke                       $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Heino Falcke (www.astro.ru.nl/~falcke)                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
 HFTOOLS Version 0.2 - Basic algorithms for the CR pipeline with
  automatic wrapper generation for python.  Includes various tools to
  manipulate stl vectors in python and wrapper function for CR Tool classes.
*/
#include <memory>

#undef HFPP_VERBOSE
#include "hftools.h"

#define hfmax(a,b) (((b)<(a))?(a):(b))
#define hfmin(a,b) (((b)>(a))?(a):(b))

#define hfeven(a)  (a%2==0)
#define hfodd(a)  (a%2!=0)

//Some definitions needed for the preprosessor programming:
//Copy source code (where specified with COPY_TO HFILE START) to the file hwrappers-hftools.iter.cc.h
//-----------------------------------------------------------------------
//$FILENAME: HFILE=hftools.def.h
//-----------------------------------------------------------------------

// Tell the preprocessor (for generating wrappers) that this is a c++
// source code file (brackets are crucial)


#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE CC
//-----------------------

//HFDOCSTART: Marks begin of file hftools.doc.cc which is being read by doxygen--------------

/*!
 HFTOOLS Version 0.2 - Basic algorithms for the CR pipeline with
  automatic wrapper generation for python.  Includes various tools to
  manipulate stl vectors in python and wrapper function for CR Tool classes.

========================================================================
Here is a list of the available functions:
========================================================================

SECTION: Administrative Vector Function
--------------------------------------------------
hFill(vec, fill_value) - Fills a vector with a constant value.

hNew(vec)              - Make and return a new vector of the same size and
                         type as the input vector.

hConvert(vec1, vec2)   - Copies and converts a vector to a vector of another
                         type.

hCopy(vec, outvec)     - Copies a vector to another one.


SECTION: Math Function
--------------------------------------------------
square(val)            - Returns the squared value of the parameter.

hPhase(frequency, time) - Returns the interferometer phase in radians for a
                         given frequency and time.

funcGaussian(x, sigma, mu) - Implementation of the Gauss function.

hExp(vec)              - Take the exp of all the elements in the vector.

hExp(vecout, vecin)    - Take the exp of all the elements in the vector and
                         return results in a second vector.

hLog(vec)              - Take the log of all the elements in the vector.

hLog(vecout, vecin)    - Take the log of all the elements in the vector and
                         return results in a second vector.

hLog10(vec)            - Take the log10 of all the elements in the vector.

hLog10(vecout, vecin)  - Take the log10 of all the elements in the vector and
                         return results in a second vector.

hSin(vec)              - Take the sin of all the elements in the vector.

hSin(vecout, vecin)    - Take the sin of all the elements in the vector and
                         return results in a second vector.

hSinh(vec)             - Take the sinh of all the elements in the vector.

hSinh(vecout, vecin)   - Take the sinh of all the elements in the vector and
                         return results in a second vector.

hSqrt(vec)             - Take the sqrt of all the elements in the vector.

hSqrt(vecout, vecin)   - Take the sqrt of all the elements in the vector and
                         return results in a second vector.

hSquare(vec)           - Take the square of all the elements in the vector.

hSquare(vecout, vecin) - Take the square of all the elements in the vector
                         and return results in a second vector.

hTan(vec)              - Take the tan of all the elements in the vector.

hTan(vecout, vecin)    - Take the tan of all the elements in the vector and
                         return results in a second vector.

hTanh(vec)             - Take the tanh of all the elements in the vector.

hTanh(vecout, vecin)   - Take the tanh of all the elements in the vector and
                         return results in a second vector.

hAbs(vec)              - Take the abs of all the elements in the vector.

hAbs(vecout, vecin)    - Take the abs of all the elements in the vector and
                         return results in a second vector.

hCos(vec)              - Take the cos of all the elements in the vector.

hCos(vecout, vecin)    - Take the cos of all the elements in the vector and
                         return results in a second vector.

hCosh(vec)             - Take the cosh of all the elements in the vector.

hCosh(vecout, vecin)   - Take the cosh of all the elements in the vector and
                         return results in a second vector.

hCeil(vec)             - Take the ceil of all the elements in the vector.

hCeil(vecout vecin)    - Take the ceil of all the elements in the vector and
                         return results in a second vector.

hFloor(vec)            - Take the floor of all the elements in the vector.

hFloor(vecout, vecin)  - Take the floor of all the elements in the vector and
                         return results in a second vector.

hAcos(vec)             - Take the acos of all the elements in the vector.

hAcos(vecout, vecin)   - Take the acos of all the elements in the vector and
                         return results in a second vector.

hAsin(vec)             - Take the asin of all the elements in the vector.

hAsin(vecout, vecin)   - Take the asin of all the elements in the vector and
                         return results in a second vector.

hAtan(vec)             - Take the atan of all the elements in the vector.

hAtan(vecout, vecin)   - Take the atan of all the elements in the vector and
                         return results in a second vector.

hSub(vec1, vec2)       - Performs a Sub between the two vectors, which is
                         returned in the first vector. If the second vector
                         is shorter it will be applied multiple times.

hSub(vec1, val)        - Performs a Sub between the vector and a scalar
                         (applied to each element), which is returned in the
                         first vector.

hSub(vec1, vec2, vec3) - Performs a Sub between the two vectors, which is
                         returned in the third vector.

hSubAdd(vec1, vec2, vec3) - Performs a Sub between the two vectors, and adds
                         the result to the output (third) vector.

hSubAddConv(vec1, vec2, vec3) - Performs a Sub between the two vectors, and
                         adds the result to the output (third) vector -
                         automatic casting is done.

hSub(vec1, val, vec2)  - Performs a Sub between the vector and a scalar,
                         where the result is returned in the second vector.

hMul(vec1, vec2)       - Performs a Mul between the two vectors, which is
                         returned in the first vector. If the second vector
                         is shorter it will be applied multiple times.

hMul(vec1, val)        - Performs a Mul between the vector and a scalar
                         (applied to each element), which is returned in the
                         first vector.

hMul(vec1, vec2, vec3) - Performs a Mul between the two vectors, which is
                         returned in the third vector.

hMulAdd(vec1, vec2, vec3) - Performs a Mul between the two vectors, and adds
                         the result to the output (third) vector.

hMulAddConv(vec1, vec2, vec3) - Performs a Mul between the two vectors, and
                         adds the result to the output (third) vector -
                         automatic casting is done.

hMul(vec1, val, vec2)  - Performs a Mul between the vector and a scalar,
                         where the result is returned in the second vector.

hAdd(vec1, vec2)       - Performs a Add between the two vectors, which is
                         returned in the first vector. If the second vector
                         is shorter it will be applied multiple times.

hAdd(vec1, val)        - Performs a Add between the vector and a scalar
                         (applied to each element), which is returned in the
                         first vector.

hAdd(vec1, vec2, vec3) - Performs a Add between the two vectors, which is
                         returned in the third vector.

hAddAdd(vec1, vec2, vec3) - Performs a Add between the two vectors, and adds
                         the result to the output (third) vector.

hAddAddConv(vec1, vec2, vec3) - Performs a Add between the two vectors, and
                         adds the result to the output (third) vector -
                         automatic casting is done.

hAdd(vec1, val, vec2)  - Performs a Add between the vector and a scalar,
                         where the result is returned in the second vector.

hDiv(vec1, vec2)       - Performs a Div between the two vectors, which is
                         returned in the first vector. If the second vector
                         is shorter it will be applied multiple times.

hDiv(vec1, val)        - Performs a Div between the vector and a scalar
                         (applied to each element), which is returned in the
                         first vector.

hDiv(vec1, vec2, vec3) - Performs a Div between the two vectors, which is
                         returned in the third vector.

hDivAdd(vec1, vec2, vec3) - Performs a Div between the two vectors, and adds
                         the result to the output (third) vector.

hDivAddConv(vec1, vec2, vec3) - Performs a Div between the two vectors, and
                         adds the result to the output (third) vector -
                         automatic casting is done.

hDiv(vec1, val, vec2)  - Performs a Div between the vector and a scalar,
                         where the result is returned in the second vector.

hConj(vec)             - Calculate the complex conjugate of all elements in
                         the complex vector.

hCrossCorrelateComplex(vec1, vec2) - Multiplies the elements of the first
                         vector with the complex conjugate of the elements in
                         the second and returns the results in the first.

hReal(vecout, vecin)   - Take the real of all the elements in the complex
                         vector and return results in a float vector.

hArg(vecout, vecin)    - Take the arg of all the elements in the complex
                         vector and return results in a float vector.

hImag(vecout, vecin)   - Take the imag of all the elements in the complex
                         vector and return results in a float vector.

hNorm(vecout, vecin)   - Take the norm of all the elements in the complex
                         vector and return results in a float vector.

hNegate(vec)           - Multiplies each element in the vector with -1 in
                         place, i.e. the input vector is also the output
                         vector.

hSum(vec)              - Performs a sum over the values in a vector and
                         returns the value.

hProduct(vec)          - Multiplies all elements in the vector with each
                         other and return the result

hVectorLength(vec)     - Returns the lengths or norm of a vector (i.e.
                         Sqrt(Sum_i(xi*+2))).

hNormalize(vec)        - Normalizes a vector to length unity.

hMean(vec)             - Returns the mean value of all elements in a vector.

hSort(vec)             - Sorts a vector in place.

hSortMedian(vec)       - Sorts a vector in place and returns the median value
                         of the elements.

hMedian(vec)           - Returns the median value of the elements.

hStdDev(vec, mean)     - Calculates the standard deviation around a mean
                         value.

hStdDev(vec)           - Calculates the standard deviation of a vector of
                         values.

hFindLessEqual(vecout, vec, threshold) - Find the samples that are LessEqual
                         a certain threshold value and returns the number of
                         samples found and the positions of the samples in a
                         second vector.

hFindLessEqualAbs(vecout, vec, threshold) - Find the samples whose absolute
                         values are LessEqual a certain threshold value and
                         returns the number of samples found and the
                         positions of the samples in a second vector.

hFindGreaterThan(vecout, vec, threshold) - Find the samples that are
                         GreaterThan a certain threshold value and returns
                         the number of samples found and the positions of the
                         samples in a second vector.

hFindGreaterThanAbs(vecout, vec, threshold) - Find the samples whose absolute
                         values are GreaterThan a certain threshold value and
                         returns the number of samples found and the
                         positions of the samples in a second vector.

hFindGreaterEqual(vecout, vec, threshold) - Find the samples that are
                         GreaterEqual a certain threshold value and returns
                         the number of samples found and the positions of the
                         samples in a second vector.

hFindGreaterEqualAbs(vecout, vec, threshold) - Find the samples whose
                         absolute values are GreaterEqual a certain threshold
                         value and returns the number of samples found and
                         the positions of the samples in a second vector.

hFindLessThan(vecout, vec, threshold) - Find the samples that are LessThan a
                         certain threshold value and returns the number of
                         samples found and the positions of the samples in a
                         second vector.

hFindLessThanAbs(vecout, vec, threshold) - Find the samples whose absolute
                         values are LessThan a certain threshold value and
                         returns the number of samples found and the
                         positions of the samples in a second vector.

hDownsample(vec1, vec2) - Downsample the input vector to a smaller output
                         vector.

hDownsample(vec, downsample_factor) - Downsample the input vector by a cetain
                         factor and return a new vector.

hFlatWeights(wlen)     - Returns vector of weights of length len with
                         constant weights normalized to give a sum of unity.
                         Can be used by hRunningAverageT.

hLinearWeights(wlen)   - Returns vector of weights of length wlen with
                         linearly rising and decreasing weights centered at
                         len/2.

hGaussianWeights(wlen) - Returns vector of weights of length wlen with
                         Gaussian distribution centered at len/2 and
                         sigma=len/4 (i.e. the Gaussian extends over 2 sigma
                         in both directions).

hWeights(wlen, wtype)  - Create a normalized weight vector.

hRunningAverage(odata, idata, weights) - Calculate the running average of an
                         input vector using a weight vector.

hRunningAverage(odata, idata, wlen, wtype) - Overloaded function to
                         automatically calculate weights.


SECTION: RF (Radio Frequency) Function
--------------------------------------------------
hGeometricDelayFarField(antPosition, skyDirection, length) - Calculates the
                         time delay in seconds for a signal received at an
                         antenna position relative to a phase center from a
                         source located in a certain direction in farfield
                         (based on L. Bahren).

hGeometricDelayNearField(antPosition, skyPosition, distance) - Calculates the
                         time delay in seconds for a signal received at an
                         antenna position relative to a phase center from a
                         source located at a certain 3D space coordinate in
                         nearfield (based on L. Bahren).

hGeometricDelays(delays, antPositions, skyPositions, farfield) - Calculates
                         the time delay in seconds for signals received at
                         various antenna positions relative to a phase center
                         from sources located at certain 3D space coordinates
                         in near or far field.

hGeometricPhases(phases, frequencies, antPositions, skyPositions, farfield) -
                         Calculates the phase gradients for signals received
                         at various antenna positions relative to a phase
                         center from sources located at certain 3D space
                         coordinates in near or far field and for different
                         frequencies.

hGeometricWeights(weights, frequencies, antPositions, skyPositions, farfield)
                         - Calculates the phase gradients as complex weights
                         for signals received at various antenna positions
                         relative to a phase center from sources located at
                         certain 3D space coordinates in near or far field
                         and for different frequencies.

hSpectralPower(vecout, vecin) - Calculates the power of a complex spectrum and
                         add it to an output vector.

hADC2Voltage(vec, adc2voltage) - Convert the ADC value to a voltage.

hGetHanningFilter(vec, Alpha, Beta, BetaRise, BetaFall) - Create a Hanning
                         filter.

hGetHanningFilter(vec, Alpha, Beta) - Create a Hanning filter.

hGetHanningFilter(vec, Alpha) - Create a Hanning filter.

hGetHanningFilter(vec) - Create a Hanning filter.

hApplyFilter(data, filter) - Apply a predefined filter on a vector.

hApplyHanningFilter(data) - Apply a Hanning filter on a vector.

hFFTCasa(data_out, data_in, nyquistZone) - Apply an FFT on a vector.

hInvFFTCasa(data_out, data_in, nyquistZone) - Apply an Inverse FFT on a vector.


SECTION: I/O Function (DataReader)
--------------------------------------------------
hFileSummary(dr)       - Print a brief summary of the file contents and
                         current settings.

hFileOpen(Filename)    - Function to open a file based on a filename and
                         returning a datareader object.

hFileGetParameter(dr, keyword) - Return information from a data file as a
                         Python object.

hFileSetParameter(dr, keyword, pyob) - Set parameters in a data file with a
                         Python object as input.

hFileRead(dr, Datatype, vec) - Read data from a Datareader object (pointer in
                         iptr) into a vector, where the size should be
                         pre-allocated.

hCalTable(filename, keyword, date, pyob) - Return a list of antenna positions
                         from the CalTables - this is a test.


SECTION: Coordinate Conversion (VectorConversion.cc)
--------------------------------------------------
hCoordinateConvert(source, sourceCoordinate, target, targetCoordinate,
                         anglesInDegrees) - Converts a 3D spatial vector into
                         a different Coordinate type (e.g. Spherical to
                         Cartesian).

hReadFileOld(vec, iptr, Datatype, Antenna, Blocksize, Block, Stride, Shift) -
                         Read data from a Datareader object (pointer in iptr)
                         into a vector.


*/


//========================================================================
//                 Casting & Conversion Functions
//========================================================================
template<class T> T hfnull(){};
template<> inline HString  hfnull<HString>(){HString null=""; return null;}
template<> inline HPointer hfnull<HPointer>(){return NULL;}
template<> inline HInteger hfnull<HInteger>(){return 0;}
template<> inline HNumber  hfnull<HNumber>(){return 0.0;}
template<> inline HComplex hfnull<HComplex>(){return 0.0;}

//Identity
template<class T> inline T hfcast(/*const*/ T v){return v;}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
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
template<> inline HPointer hfcast(/*const*/ HString v){HPointer t=NULL; std::istringstream is(v); is >> t; return t;}
template<> inline HInteger hfcast(/*const*/ HString v){HInteger t=0; std::istringstream is(v); is >> t; return t;}
template<> inline HNumber hfcast(/*const*/ HString v){HNumber t=0.0; std::istringstream is(v); is >> t; return t;}
template<> inline HComplex hfcast(/*const*/ HString v){HComplex t=0.0; std::istringstream is(v); is >> t; return t;}

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(uint v){return static_cast<T>(v);}
#if H_OS64BIT
template<class T> inline T hfcast(int v){return static_cast<T>(v);}
#endif
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}
template<class T> inline T hfcast(/*const*/ HPointer v){return hfcast<T>(reinterpret_cast<HInteger>(v));}
template<class T> inline T hfcast(/*const*/ HString v){T t; std::istringstream is(v); is >> t; return t;}

template<class T> inline T hfcast(CR::DataReader v){return hfcast<T>((HInteger)((void*)&v));}



inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}

template<class T> inline HString hf2string(T v){std::ostringstream os; os << v; return os.str();}
template<> inline HString hf2string(CR::DataReader v){return hf2string((HInteger)((void*)&v));}


inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}


template <class T>
HString pretty_vec(vector<T> & v,const HInteger maxlen){
  if (v.size()==0) return "[]";
  int i=1;
  typename vector<T>::iterator it=v.begin();
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
HString pretty_vec(hArray<T> & a,const HInteger maxlen){
  vector<T>* v = &a.getVector();
  if (v->size()==0) return "[]";
  int i=1;
  typename vector<T>::iterator it=v->begin();
  HString s=("[");
  s+=hf2string(*it); ++it;  ++i;
  while (it!=v->end() && i< maxlen) {
    s+=","+hf2string(*it); ++it; ++i;
  };
  if ((HInteger)v->size() > maxlen) s+=",...]";
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

vector<HNumber> PyList2STLFloatVec(PyObject* pyob){
  std::vector<HNumber> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyFloat_AsDouble(PyList_GetItem(pyob,i)));
  }
  return vec;
}

vector<HInteger> PyList2STLIntVec(PyObject* pyob){
  std::vector<HInteger> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

vector<uint> PyList2STLuIntVec(PyObject* pyob){
  std::vector<uint> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
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
    HInteger i,n;
//    std::vector<R>::iterator p;

    n=data.size();
    stlvec.resize(n);
//    p=stlvec.begin();
    for (i=0;i<n;i++) {
//	*p=hfcast<T>(data[i]);
	stlvec[i]=hfcast<T>(data[i]);
//	++p;
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

//========================================================================
//                             Array Class
//========================================================================

//! Testing a rudimentary Array class, that allows contiguous slicing

template <class T> void hArray<T>::init(){
  //  cout << "Creating hArray ptr=" << reinterpret_cast<void*>(this) << endl;
  storage_p = NULL;
  slice_begin=0;
  slice_end=0;
  slice_size=0;
  array_is_shared=false;
  doiterate=false;
  loop_slice_begin=0; loop_slice_end=0; loop_slice_size=0; loop_lower_level_size=0;
  loop_slice_start_offset=0; loop_slice_end_offset=-1;
  loop_i=0; loop_start=0; loop_end=0; loop_increment=1; loop_maxn=0;
  loop_next=false;
  loop_nslice=0;
  loop_over_indexvector=false;
  setSubSlice(0,-1,-1);
}

template <class T> void hArray<T>::new_storage(){
  storage_p=new storage_container;
  storage_p->parent=this;
  storage_p->ndims_p=NULL;
  storage_p->size_p=NULL;
  storage_p->dimensions_p=NULL;
  storage_p->slice_sizes_p=NULL;
  storage_p->vec_p=NULL;
  storage_p->vector_is_shared=false;
  storage_p->unit.prefix_to_factor["f"]=1e-15;
  storage_p->unit.prefix_to_factor["p"]=1e-12;
  storage_p->unit.prefix_to_factor["n"]=1e-9;
  storage_p->unit.prefix_to_factor["micro"]=1e-6;
  storage_p->unit.prefix_to_factor["mu"]=1e-6;
  storage_p->unit.prefix_to_factor["\\mu"]=1e-6;
  storage_p->unit.prefix_to_factor["µ"]=1e-6;
  storage_p->unit.prefix_to_factor["m"]=1e-3;
  storage_p->unit.prefix_to_factor["c"]=1e-2;
  storage_p->unit.prefix_to_factor["d"]=1e-1;
  storage_p->unit.prefix_to_factor[""]=1.0;
  storage_p->unit.prefix_to_factor["D"]=1e1;
  storage_p->unit.prefix_to_factor["h"]=1e2;
  storage_p->unit.prefix_to_factor["k"]=1e3;
  storage_p->unit.prefix_to_factor["M"]=1e6;
  storage_p->unit.prefix_to_factor["G"]=1e9;
  storage_p->unit.prefix_to_factor["T"]=1e12;
  storage_p->unit.prefix_to_factor["P"]=1e15;
  storage_p->unit.prefix_to_factor["E"]=1e18;
  storage_p->unit.prefix_to_factor["Z"]=1e21;
  storage_p->unit.name="";
  storage_p->unit.prefix="";
  storage_p->unit.scale_factor=1.0;
  storage_p->trackHistory=True;
}

template <class T> void hArray<T>::initialize_storage(){
  if (storage_p==NULL) new_storage();
  if (storage_p->ndims_p==NULL) storage_p->ndims_p=new HInteger;
  if (storage_p->size_p==NULL) storage_p->size_p=new HInteger;
  if (storage_p->dimensions_p==NULL) storage_p->dimensions_p=new std::vector<HInteger>;
  if (storage_p->slice_sizes_p==NULL) storage_p->slice_sizes_p=new std::vector<HInteger>;
  if (storage_p->vec_p==NULL) {
    storage_p->vec_p=new std::vector<T>;
    addHistory((HString)"initialize_storage",(HString)"Vector of type "+typeid(*storage_p->vec_p).name()+" created.");
  };
}

template <class T> void hArray<T>::delete_storage(){
  if (!array_is_shared) {
    if (storage_p->ndims_p!=NULL) {delete storage_p->ndims_p; storage_p->ndims_p=NULL;}
    if (storage_p->size_p!=NULL) {delete storage_p->dimensions_p; storage_p->dimensions_p=NULL;}
    if (storage_p->dimensions_p!=NULL) {delete storage_p->dimensions_p; storage_p->dimensions_p=NULL;}
    if (storage_p->slice_sizes_p!=NULL) {delete storage_p->slice_sizes_p; storage_p->slice_sizes_p=NULL;}
    delVector();
    delete storage_p;
  }
  storage_p=NULL;
}

template <class T> hArray<T>::hArray(){
  copycount=0;
  init();
  initialize_storage();
}

template <class T> hArray<T>::hArray(const std::vector<T> & vec){
  copycount=0;
  init();
  storage_p=new storage_container;
  setVector(vec);
  storage_p->vector_is_shared=true;
  initialize_storage();
}


/*!
\brief Creates a new array as a shared copy, i.e. it shares the data vector and dimensions of its parent.

*/

template <class T> hArray<T>::hArray(storage_container * sptr){
  if (sptr==NULL) return;
  copycount=sptr->parent->copycount+1;
  init();
  storage_p=sptr;
  array_is_shared=true;
  initialize_storage();
  setSlice(0,storage_p->vec_p->size());
}

/*! \brief Returns a new hArray object, which shares the same vector and storage description (i.e. dimensions).

The storage will be deleted when the original object is deleted (not
when the shared copy is deleted) - even if the shared copied object is
still around.

 */
template <class T> hArray<T> & hArray<T>::shared_copy(){hArray<T> * ary_p=new hArray<T>(storage_p); return *ary_p;}

template <class T> void hArray<T>::delVector(){
  if (storage_p==NULL) return; //Check if vector was deleted elsewhere
  if ((storage_p->vec_p != NULL) && !storage_p->vector_is_shared) {
    addHistory((HString)"delVector",(HString)"Vector of type"+typeid(*storage_p->vec_p).name()+" and size="+hf2string((HInteger)storage_p->vec_p->size())+" deleted.");
    delete storage_p->vec_p;
  }
  storage_p->vec_p=NULL;
  addHistory((HString)"delVector",(HString)"Reference to Vector of type"+typeid(*storage_p->vec_p).name()+" deleted.");
}

template <class T> hArray<T>::~hArray(){
  //  cout << "Deleting hArray ptr=" << reinterpret_cast<void*>(this) << endl;
  delete_storage();
}

/*
template <class T> hArray::storage_container * hArray<T>::getStorage(){
  return storage_p;
}
*/

/*!

\brief Set the vector to be stored (as reference, hence no copy is
made). Creation and destruction of this vector has to be done outside
this class!!

*/
template <class T> hArray<T> &   hArray<T>::setVector(std::vector<T> & vec){
  delVector();
  initialize_storage();
  storage_p->vec_p=&vec;
  (*storage_p->size_p)=vec.size();
  setDimensions1((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  storage_p->vector_is_shared=true;
  return *this;
}

/*!
\brief Retrieve the stored vector (returned as reference, hence no copy is made).
 */
template <class T> std::vector<T> & hArray<T>::getVector(){
  if (storage_p==NULL) { static vector<T> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->vec_p);
}

/*!
\brief Retrieve the stored vector (returned as reference, hence no copy is made).
 */
template <class T> std::vector<HInteger> & hArray<T>::getDimensions(){
  if (storage_p==NULL) {static vector<HInteger> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->dimensions_p);
}

/*!
\brief Calculate the size of one slice in each dimension, will be called whenever dimensions change.
 */
template <class T> void hArray<T>::calcSizes(){
  if (storage_p==NULL) return; //Check if vector was deleted elsewhere
  (*storage_p->slice_sizes_p).resize((*storage_p->dimensions_p).size());
  (*storage_p->slice_sizes_p)[(*storage_p->dimensions_p).size()-1]=1;
  for (int i=(*storage_p->dimensions_p).size()-1; i>0; --i) (*storage_p->slice_sizes_p)[i-1]=(*storage_p->slice_sizes_p)[i]*(*storage_p->dimensions_p)[i];
  setSubSlice(0,-1,-1);
}

/*!
\brief Calculate the size of one slice in each dimension, will be called whenever dimensions change.
 */
template <class T> std::vector<HInteger> & hArray<T>::getSizes(){
  if (storage_p==NULL) {static vector<HInteger> v; return v;} //Check if vector was deleted elsewhere
  return *(storage_p->slice_sizes_p);
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions1(HInteger dim0){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+"]");
  (*storage_p->ndims_p)=1;
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if (storage_p->vec_p->size() != (uint)(*storage_p->size_p)) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions2(HInteger dim0, HInteger dim1){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+"]");
  (*storage_p->ndims_p)=2;
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions3(HInteger dim0, HInteger dim1, HInteger dim2){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=3;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions4(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=4;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+","+hf2string(dim3)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->dimensions_p)[3]=dim3;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets the dimensions of the array. Last index runs fastest, i.e., from left to right one goes from large to small chunks in memory.
 */
template <class T> void  hArray<T>::setDimensions5(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3, HInteger dim4){
  if (storage_p==NULL) return ; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ; //Check if vector was deleted elsewhere
  (*storage_p->ndims_p)=5;
  addHistory((HString)"setDimension",(HString)"Dimensions set to ["+hf2string(dim0)+","+hf2string(dim1)+","+hf2string(dim2)+","+hf2string(dim3)+","+hf2string(dim4)+"]");
  if ((*storage_p->dimensions_p).size()!=(uint)(*storage_p->ndims_p)) (*storage_p->dimensions_p).resize((*storage_p->ndims_p));
  (*storage_p->dimensions_p)[0]=dim0;
  (*storage_p->dimensions_p)[1]=dim1;
  (*storage_p->dimensions_p)[2]=dim2;
  (*storage_p->dimensions_p)[3]=dim3;
  (*storage_p->dimensions_p)[4]=dim4;
  (*storage_p->size_p)=hProduct<HInteger>((*storage_p->dimensions_p));
  if ((uint)(*storage_p->size_p) != storage_p->vec_p->size()) storage_p->vec_p->resize((*storage_p->size_p));
  setSlice(0,(*storage_p->size_p));
  calcSizes();
  return ;
}

/*!
\brief Sets begin and end the currently active slice using integer offsets from the begin of the stored vector. Use (0,size(vector)) to get the full vector
 */
template <class T> hArray<T> &  hArray<T>::setSlice(HInteger beg, HInteger end){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  slice_begin=hfmax(beg,0);
  if (end==-1) end=storage_p->vec_p->size();
  if (end>=0) slice_end=std::min(end,(*storage_p->size_p));
  else slice_end=(*storage_p->size_p);
  slice_size=slice_end-slice_begin;
  return *this;
}

/*!  \brief Sets begin and end of the currently active slice using an integer array of array indices.

If the length of the index vector is shorter than the actual number of
dimensions, this will provide a slice over the remanining dimensions
(belonging to the element indicated by the index vector)

 */
template <class T> hArray<T> &  hArray<T>::setSliceVector(vector<HInteger> & index_vector){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=index_vector.size();
  if ((level > *storage_p->ndims_p) || (level < 0)) {ERROR("setSliceVector: Dimension wrong"); return *this;};
  if (subslice_level==-1) subslice_level=level;
  slice_begin=hfmax(0,std::min(hMulSum(index_vector,*storage_p->slice_sizes_p)+getSubSliceStart(),(HInteger)storage_p->vec_p->size()));
  slice_end=hfmax(0,std::min(hMulSum(index_vector,*storage_p->slice_sizes_p)+getSubSliceEnd(),(HInteger)storage_p->vec_p->size()));
  slice_size=slice_end-slice_begin;
  return *this;
}

//!Calculate the begin of the current subslice (the last slice) as an offset from the begin of the 2nd to last index. If *subslice_end_it == -1 then calculate from end
template <class T> HInteger hArray<T>::getSubSliceStart(){
  subslice_start=*subslice_start_it;
  //If negative (i.e. -N), then take full slice minus (N-1) number of elements
  if (subslice_start<0) subslice_start=storage_p->dimensions_p->at(subslice_level)+subslice_start;
  subslice_start *= (storage_p->slice_sizes_p->at(subslice_level));
  return subslice_start;
}

//!Calculate the end of the current subslice (the last slice) as an offset from the begin of the 2nd to last index. If *subslice_end_it == -1 then take full slice.
template <class T> HInteger hArray<T>::getSubSliceEnd(){
  subslice_end=*subslice_end_it;
  //If negative (i.e. -N), then take full slice minus (N-1) number of elements
  if (subslice_end<0) subslice_end=storage_p->dimensions_p->at(subslice_level)+subslice_end+1;
  subslice_end *= (storage_p->slice_sizes_p->at(subslice_level));
  return subslice_end;
}


//!Sets a second slice at the end (last index) of an already sliced array (e.g. used for array[0:2,...,0:4] -loops over first index and takes only the first 4 elements of each)
template <class T> hArray<T> &  hArray<T>::setSubSlice(HInteger start, HInteger end, HInteger level)
{
  subslice_vec_start.resize(1);
  subslice_vec_end.resize(1);
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  *subslice_start_it=start;
  *subslice_end_it=end;
  subslice_level=level;
  return *this;
}

//!Sets a second slice at the end (last index) of an already sliced array (e.g. used for array[0:2,...,0:4] -loops over first index and takes only the first 4 elements of each). With each call of next the next slice is taken from the vectors.
template <class T> hArray<T> &  hArray<T>::setSubSliceVec(const vector<HInteger> start, const vector<HInteger> & end, const HInteger level){
  subslice_vec_start=start;
  subslice_vec_end=end;
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  subslice_level=level;
  return *this;
}

/*!
\brief Returns the number of dimensions that have been associated with the current array
 */
template <class T> HInteger hArray<T>::getNumberOfDimensions(){
  if (storage_p==NULL) return 0; //Check if vector was deleted elsewhere
  return (*storage_p->ndims_p);
}

/*!
\brief Returns the begin iterator of the current slice in the stored vector
 */
template <class T> typename std::vector<T>::iterator hArray<T>::begin(){
  return storage_p->vec_p->begin()+getBegin();
}
/*!
\brief Returns the end iterator of the current slice in the stored vector
 */
template <class T> typename std::vector<T>::iterator hArray<T>::end(){
  return storage_p->vec_p->begin()+getEnd();
}

/*!
\brief Returns the offset of the current slice from the begin iterator of the stored vector
 */
template <class T> HInteger hArray<T>::getBegin(){
  if (doiterate) return std::min(loop_slice_begin+loop_nslice*loop_slice_size+getSubSliceStart(),loop_slice_end);
  else return slice_begin;
}

/*!
\brief Returns the offset of the end of the current slice from the begin iterator of the stored vector
 */
template <class T> HInteger hArray<T>::getEnd(){
  if (doiterate) return std::max(loop_slice_begin+loop_nslice*loop_slice_size+getSubSliceEnd(),loop_slice_begin);
  else return slice_end;
}

/*!
\brief Returns the size (length) of the current slice
 */
template <class T> HInteger hArray<T>::getSize(){
  if (doiterate) return loop_slice_size;
  else return slice_size;
}

/*!
\brief Returns the length of the underlying vector
 */
template <class T> HInteger hArray<T>::length(){
  if (storage_p==NULL) return 0; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return 0; //Check if vector was deleted elsewhere
  return storage_p->vec_p->size();
}


/*!  \brief Resizes the vector to the length given as input. If the
new size differs, then the vector will be resized and slices and
dimensions be reset to a flat (one dimensional) vector array.
 */
template <class T> hArray<T> & hArray<T>::resize(HInteger newsize){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (newsize==(HInteger)storage_p->vec_p->size()) return *this;
  storage_p->vec_p->resize(newsize);
  *storage_p->size_p=newsize;
  storage_p->dimensions_p->resize(1);
  storage_p->dimensions_p->at(0)=newsize;
  setSlice(0,newsize);
  calcSizes();
  return *this;
}

/*!
\brief Returns whether or not to iterate over all slices in the vector
 */
template <class T> bool hArray<T>::doLoopAgain(){return doiterate && loop_next;}

/*!
\brief Returns whether or not the  iterate over all slices in the vector
 */
template <class T> bool hArray<T>::loopingMode(){return doiterate;}

/*!  \brief Returns the current loop index (which may be different
from loop_nslice if one loops over an array of indices)
 */
template <class T> HInteger hArray<T>::getLoop_i(){return loop_i;}

/*!
\brief Returns the current loop start
 */
template <class T> HInteger hArray<T>::getLoop_start(){return loop_start;}

/*!
\brief Returns the current loop end
 */
template <class T> HInteger hArray<T>::getLoop_end(){return loop_end;}

/*!
\brief Returns the current loop index increment
 */
template <class T> HInteger hArray<T>::getLoop_increment(){return loop_increment;}

/*!  \brief Returns the number of the currently used slice in the loop
(which may be different from loop_i, if one loops over an array of
indices)
 */
template <class T> HInteger hArray<T>::getLoop_nslice(){return loop_nslice;}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

 */
template <class T> hArray<T> & hArray<T>::loopOn(){
  loop_next=true;
  doiterate=true;
  return *this;
}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

 */
template <class T> hArray<T> & hArray<T>::all(){setSlice(0,-1); return *this;}

/*!
\brief Sets the array to looping mode (i.e. the next function will loop over all slices in the vector).

\param "start_element_index" indicates the array element to start with. Looping
will be done over the next higher dimension.

\param start (starting at 0), end, increment indicate over how
many slices the iteration should proceed.

 */
template <class T> hArray<T> &   hArray<T>::loop(vector<HInteger> & start_element_index, HInteger start, HInteger end, HInteger increment){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=setLoopSlice(start_element_index);
  if ( level <= -1 )  {ERROR("loop: dimensions are wrong!"); return *this;};
  if (end<0) end=storage_p->dimensions_p->at(level);
  start=std::min(hfmax(0,start),loop_maxn);
  end=std::min(hfmax(start,end),loop_maxn);
  increment = hfmax(increment,1);
  loopOn();
  loop_over_indexvector=false;
  loop_i=start; loop_start=start; loop_end=end; loop_increment=increment;
  loop_slice_start_offset=*subslice_start_it; loop_slice_end_offset=*subslice_end_it;
  loop_nslice = loop_i;
  return *this;
}

/*!

 \brief Sets the array to looping mode (i.e. the next function will
loop over all slices in the vector) using and index vector to indicate
over which slices to loop.

 */
template <class T> hArray<T> & hArray<T>::loopVector(vector<HInteger> & start_element_index, vector<HInteger> & vec){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  HInteger level=setLoopSlice(start_element_index);
  if ( level <= -1 ) {ERROR("loopVector: dimensions are wrong!"); return *this;};
  loopOn();
  loop_over_indexvector=true;
  loop_i=0; loop_start=0; loop_end=vec.size(); loop_increment=1;
  loop_slice_start_offset=*subslice_start_it; loop_slice_end_offset=*subslice_end_it;
  index_vector=vec;
  loop_nslice=index_vector[loop_i];
  return *this;
}

/*!
\brief Sets the array to looping mode and resets the loop parameters to its start values

 */
template <class T> hArray<T> & hArray<T>::resetLoop(){
  loop_i=loop_start;
  subslice_start_it=subslice_vec_start.begin();
  subslice_end_it=subslice_vec_end.begin();
  if (loop_over_indexvector) loop_nslice=index_vector[loop_i];
  else loop_nslice = loop_i;
  loopOn();
  return *this;
}


/*!
\brief Sets the slice parameters used by the looping algorithm to calculate the currently worked on slice.

\param start_element_index: a vector of n indices which remain
constant during the looping. Looping will be done over the npipil+1st
index.

 */
template <class T> HInteger hArray<T>::setLoopSlice(vector<HInteger> & start_element_index){
  if (storage_p==NULL) return -1; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return -1; //Check if vector was deleted elsewhere
  HInteger level=start_element_index.size();
  if (level >= *storage_p->ndims_p)  {ERROR("setLoopSlice: dimensions are wrong!"); return -1;};
  loop_maxn = storage_p->dimensions_p->at(level);
  loop_slice_begin=hMulSum(start_element_index,*storage_p->slice_sizes_p); // multiplies the start element indices with the (cummulaitve) sizes of slices per dimension - giving the total offset
  loop_slice_size=storage_p->slice_sizes_p->at(level);
  if (level+1 >= *storage_p->ndims_p) loop_lower_level_size=0;
  else loop_lower_level_size=storage_p->slice_sizes_p->at(level+1);
  loop_slice_end=loop_slice_begin+loop_slice_size*loop_maxn;
  return level;
}

/*!
\brief Switches the looping mode off (i.e. the next function will not loop over all slices in the vector)
 */
template <class T> hArray<T> & hArray<T>::loopOff(){doiterate=false; return *this;}

/*!
\brief Increase the current slice by one, if array is in looping mode.

If the end of the vector is reached, switch looping mode off and reset array to first slice.
 */
template <class T> hArray<T> &  hArray<T>::next(){
  if (!doiterate) return *this;
  if (loop_next==false) loop_next=true;  // start all over again
  loop_i+=loop_increment;
  subslice_start_it+=loop_increment; if (subslice_start_it>=subslice_vec_start.end()) subslice_start_it=subslice_vec_start.begin();
  subslice_end_it+=loop_increment; if (subslice_end_it>=subslice_vec_end.end()) subslice_end_it=subslice_vec_end.begin();
  if (loop_i>=loop_end) { // the end is near, stop looping ...
    resetLoop();
    loop_next=false;
  } else {
    if (loop_over_indexvector) loop_nslice=index_vector[loop_i];
    else loop_nslice = loop_i;
    loop_nslice = std::min(loop_nslice,loop_maxn);
  };
  return *this;
}

/*!
\brief Get the unit name and prefix of the data array
 */
template <class T> HString hArray<T>::getUnit(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.prefix + storage_p->unit.name;
};

/*!
\brief Get the unit name of the data array
 */
template <class T> HString hArray<T>::getUnitName(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.name;
};

/*!
\brief Get the unit prefix of the data array
 */
template <class T> HString hArray<T>::getUnitPrefix(){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return ""; //Check if vector was deleted elsewhere
  return storage_p->unit.prefix;
};

/*!
\brief Take out the unit information
 */
template <class T> hArray<T> &  hArray<T>::clearUnit(){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  setUnit("","");
  storage_p->unit.name="";
  addHistory((HString)"clearUnit",(HString)"");
  return *this;
}

/*!

\brief Set the value of a particular key word.

Inputs are always given as strings (also numerical values). As a
convention, the contents should be formatted as python input style.

 */
template <class T> hArray<T> &  hArray<T>::setKey(HString key, HString contents){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->keywords[key]=contents;
  addHistory("setKey",key + " = " + contents);
  return *this;
}

/*!

\brief Set the value of a particular key word.

Inputs are always given as strings (also numerical values). As a
convention, the contents should be formatted as python input style.

 */
template <class T> HString hArray<T>::getKey(HString key){
  if (storage_p==NULL) return ""; //Check if vector was deleted elsewhere
  map<HString,HString>::iterator it=storage_p->keywords.find(key);
  if (it!=storage_p->keywords.end()) return it->second;
  return "";
}

/*!

\brief Set the value of a particular key word as a Python object.


template <class T> hArray<T> &  hArray<T>::setKeyPy(HString key, HPyObjectPtr pyobj){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->keywords_py[key]=pyobj;
  addHistory("setKey",key + " = ..." );
  return *this;
}
 */

/*!

\brief Set the value of a particular key word from a python object.

template <class T> HPyObject & hArray<T>::getKeyPy(HString key){
  if (storage_p==NULL) return HPyObject(false); //Check if vector was deleted elsewhere
  map<HString,HPyObjectPtr>::iterator it=storage_p->keywords_py.find(key);
  if (it!=storage_p->keywords_py.end()) return *(it->second);
  return HPyObject(false);
}
 */


/*!
\brief Set the unit of the data array and multiply all data with the
apropriate scale factor. Prefix is the
 */
template <class T> hArray<T> &  hArray<T>::setUnit(HString prefix, HString name){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  map<HString,HNumber>::iterator factor_p=storage_p->unit.prefix_to_factor.find(prefix);
  if (factor_p==storage_p->unit.prefix_to_factor.end()) {
    ERROR("setUnit: unit prefix " << prefix << " unknown.");
    return *this;
  };
  HNumber factor = factor_p->second;
  if ((name=="") || (storage_p->unit.name==name)) {
    if (typeid(T)==typeid(HInteger)) {
      hMul(*reinterpret_cast<vector<HInteger>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else if (typeid(T)==typeid(HNumber)) {
      hMul(*reinterpret_cast<vector<HNumber>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else if (typeid(T)==typeid(HComplex)) {
      hMul(*reinterpret_cast<vector<HComplex>*>(storage_p->vec_p),storage_p->unit.scale_factor/factor);
    } else {
      ERROR("setUnit: unit setting not available for type  " << typeid(T).name() << ".");
    }
  } else storage_p->unit.name=name;
  storage_p->unit.scale_factor=factor;
  storage_p->unit.prefix=prefix;
  addHistory((HString)"setUnit",(HString)"Unit set to " + getUnit()+".");
  return *this;
}

/*!
\brief Add a line of history information to the history vector
 */
template <class T> hArray<T> & hArray<T>::addHistory(HString name, HString text){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) return *this;
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [20];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
  HString out(buffer);
  out += " [" + name + "]: " + text;
  storage_p->history.push_back(out);
  return *this;
}

/*!
\brief Return the history information
 */
template <class T> vector<HString> & hArray<T>::getHistory(){
  if (storage_p==NULL) {static vector<HString> h; return h;}; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) {static vector<HString> h; return h;}; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) {static vector<HString> h; return h;};
  return storage_p->history;
}

/*!
\brief Print the history information
 */
template <class T> void hArray<T>::printHistory(){
  if (storage_p==NULL)  return; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL)  return; //Check if vector was deleted elsewhere
  if (!storage_p->trackHistory) return;
  vector<HString>::iterator it=storage_p->history.begin();
  while (it!=storage_p->history.end()) {
    cout << "  " << *it << endl;
    ++it;
  };
  return;
}

/*!
\brief Clear the history information
 */
template <class T> hArray<T> &  hArray<T>::clearHistory(){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  storage_p->history.clear();
  return *this;
}

/*!
\brief Set history tracking on or off
 */
template <class T> hArray<T> &  hArray<T>::setHistory(bool on_or_off){
  if (storage_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (storage_p->vec_p==NULL) return *this; //Check if vector was deleted elsewhere
  if (!on_or_off) addHistory((HString)"setHistory",(HString)"History tracking switched off.");
  storage_p->trackHistory=on_or_off;
  if (on_or_off) addHistory((HString)"setHistory",(HString)"History tracking switched on.");

  return *this;
}

/*!
\brief Check if history tracking is on or off
 */
template <class T> bool hArray<T>::isTrackingHistory(){
  if (storage_p==NULL) return false; //Check if vector was deleted elsewhere
  return(storage_p->trackHistory);
}

    //      .def("setVector",&hArray<TYPE>::setVector,return_value_policy<manage_new_object>()) \
    //      .def("getVector",&hArray<TYPE>::getVector,return_value_policy<manage_new_object>()) \
    //  .def("shared_copy",&hArray<TYPE>::shared_copy,return_value_policy<manage_new_object>()) \

/*
    .def("getVector",&hArray<TYPE>::getVector,return_value_policy<manage_new_object>()) \
    .def("shared_copy",&hArray<TYPE>::shared_copy,return_value_policy<manage_new_object>()) \
    .def("getDimensions",&hArray<TYPE>::getDimensions,return_value_policy<manage_new_object>()) \
    .def("getSizes",&hArray<TYPE>::getSizes,return_value_policy<manage_new_object>()) \
*/

/*
return_internal_reference<1,
        with_custodian_and_ward<1, 2> >
*/

#define HFPP_hARRAY_BOOST_PYTHON_WRAPPER(TYPE,NAME)	\
    class_<hArray<TYPE> >(#NAME) \
    .def("getVector",&hArray<TYPE>::getVector,return_internal_reference<>()) \
    .def("shared_copy",&hArray<TYPE>::shared_copy,return_internal_reference<>()) \
    .def("getDimensions",&hArray<TYPE>::getDimensions,return_internal_reference<>()) \
    .def("getSizes",&hArray<TYPE>::getSizes,return_internal_reference<>()) \
    .def("setVector",&hArray<TYPE>::setVector,return_internal_reference<>())\
    .def("setDimensions",&hArray<TYPE>::setDimensions1)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions2)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions3)			\
    .def("setDimensions",&hArray<TYPE>::setDimensions4)			\
    .def("setSlice",&hArray<TYPE>::setSlice,return_internal_reference<>())				\
    .def("setSliceVector",&hArray<TYPE>::setSliceVector,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSlice,return_internal_reference<>())				\
    .def("setSubSlice",&hArray<TYPE>::setSubSliceVec,return_internal_reference<>())				\
    .def("getNumberOfDimensions",&hArray<TYPE>::getNumberOfDimensions)	\
    .def("getBegin",&hArray<TYPE>::getBegin)				\
    .def("getEnd",&hArray<TYPE>::getEnd)				\
    .def("getSize",&hArray<TYPE>::getSize)				\
    .def("loopingMode",&hArray<TYPE>::loopingMode)			\
    .def("doLoopAgain",&hArray<TYPE>::doLoopAgain)			\
    .def("loop_i",&hArray<TYPE>::getLoop_i)				\
    .def("loop_nslice",&hArray<TYPE>::getLoop_nslice)			\
    .def("loop_start",&hArray<TYPE>::getLoop_start)			\
    .def("loop_end",&hArray<TYPE>::getLoop_end)				\
    .def("loop_increment",&hArray<TYPE>::getLoop_increment)		\
    .def("__len__",&hArray<TYPE>::length)				\
    .def("resize",&hArray<TYPE>::resize,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loop,return_internal_reference<>())					\
    .def("loop",&hArray<TYPE>::loopVector,return_internal_reference<>())					\
    .def("resetLoop",&hArray<TYPE>::resetLoop,return_internal_reference<>())				\
    .def("noOn",&hArray<TYPE>::loopOn,return_internal_reference<>())				\
    .def("noOff",&hArray<TYPE>::loopOff,return_internal_reference<>())				\
    .def("next",&hArray<TYPE>::next,return_internal_reference<>())       \
    .def("setUnit_",&hArray<TYPE>::setUnit,return_internal_reference<>())	\
    .def("getUnit",&hArray<TYPE>::getUnit)	\
    .def("getUnitName",&hArray<TYPE>::getUnitName)	\
    .def("getUnitPrefix",&hArray<TYPE>::getUnitPrefix)	\
    .def("clearUnit",&hArray<TYPE>::clearUnit,return_internal_reference<>())	\
    .def("addHistory",&hArray<TYPE>::addHistory,return_internal_reference<>())	\
    .def("clearHistory",&hArray<TYPE>::clearHistory,return_internal_reference<>())	\
    .def("setHistory",&hArray<TYPE>::setHistory,return_internal_reference<>())	\
    .def("getHistory",&hArray<TYPE>::getHistory,return_internal_reference<>())	\
    .def("isTrackingHistory",&hArray<TYPE>::isTrackingHistory)	\
    .def("history",&hArray<TYPE>::printHistory)	\
    .def("setKey",&hArray<TYPE>::setKey,return_internal_reference<>())	\
    .def("getKey",&hArray<TYPE>::getKey)	\
    //.def("setKeyPy",&hArray<TYPE>::setKeyPy,return_internal_reference<>()) \
    //.def("getKeyPy",&hArray<TYPE>::getKeyPy)				\

//========================================================================
//                        Helper Functions
//========================================================================


/*!
\brief Determine the filetype based on the extension of the filename
 */
HString hgetFiletype(HString filename){
  HString ext=hgetFileExtension(filename);
  HString typ="";
  if (ext=="event") typ="LOPESEvent";
  else if (ext=="h5") typ="LOFAR_TBB";
  return typ;
}

/*!
\brief Get the extension of the filename, i.e. the text after the last dot.
 */
HString hgetFileExtension(HString filename){
  HInteger size=filename.size();
  HInteger pos=filename.rfind('.',size);
  pos++;
  if (pos>=size) {return "";}
  else {return filename.substr(pos,size-pos);};
}


//========================================================================
//$SECTION:           Administrative Vector Function
//========================================================================

//!Initialize functions of the library
void hInit(){
#ifdef HAVE_GSL
  gsl_set_error_handler_off ();
#else
  ERROR("hInit: GSL-Libraries not installed. Some functions are not defined.")
#endif
}


//$DOCSTRING: Fills a vector with a constant value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hFill(vec,0) -> [0,0,0,...]
  vec.fill(0) -> [0,0,0,...]

  \brief $DOCSTRING
  $PARDOCSTRING

See also: hSet
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const IterValueType fill_value)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=fill_value;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Sets certain elements specified in an indexlist to a constant value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSet
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Vector of in which to set elements.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(indexlist)()("Index list containing the positions of the elements to be set, (e.g. [0,2,4,...] will set every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_1)(val)()("Value to assign to the indexed elements.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: hFill, hCopy
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const IterI index, const IterI index_end, const IterValueType val)
{
  Iter it;
  IterI itidx(index);
  if (index >= index_end) return;
  while (itidx != index_end) {
    it = vec + *itidx;
    if (it < vec_end && it >= vec) *it=val;
    ++itidx;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Returns the contents of a vector (up to a maximum length) as a pretty string for printing
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPrettyString
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HString)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(maxlen)()("Maximum length to output.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hPrettyString(vec,3) -> [vec_0,vec_1,vec_3,...]

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HString HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  HString s=("[");
  Iter it(vec), maxit;
  if (maxlen>=0) maxit=(vec+hfmin(maxlen,vec_end-vec));
  else maxit=vec_end;
  if (vec<vec_end) s+=hf2string(*it);
  ++it;
  while (it<maxit) {
    s+=","+hf2string(*it);
    ++it;
  };
  if ( it < vec_end ) s+=",...";
  s+="]";
  return s;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Prints the contents of a vector (up to a maximum length) as a pretty string
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPPrint
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(maxlen)()("Maximum length to output.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hPrettyString(vec,3) -> '[vec_0,vec_1,vec_3,...]'

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  cout << hPrettyString(vec,vec_end,maxlen) << endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Fills a vector with a series of values starting at a start value and then increasing by an increment until it is filled
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFillRange
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(start)()("Start value of the range.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(increment)()("Increment (to multiply loop variable with).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hFillRange(vec,-2.5,2.) -> [-2.5,-0.5,1.5,3.5,...]
  vec.fillrange(start,increment) -> vec=[start, start+1*increment, start+2*increment ...]

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const  IterValueType start,  const IterValueType increment)
{
  Iter it=vec;
  HInteger i=0;
  while (it!=vec_end) {
    *it=start+increment*i;
    ++it; ++i;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Fills a vector with the content of another vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_vec)()("Vector of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hFill(vec,[0,1,2]) -> [0,1,2,0,1,2,...]
  vec.fill([0,1,2]) -> [0,1,2,0,1,2,...]

  \brief $DOCSTRING
  $PARDOCSTRING

If fill_vec is shorther than vec, the procedure will wrap around and
start from the beginning of fill_vec again. Hence, in this case
fill_vec will appear repeated multiple times in vec.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter fill_vec, const Iter fill_vec_end)
{
  Iter it1=vec;
  Iter it2=fill_vec;
  if (it2==vec_end) return;
  while (it1!=vec_end) {
    *it1=*it2;
    ++it1;++it2;
    if (it2==fill_vec_end) it2=fill_vec;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Combines two vectors into one where the elements of the vectors follow each other alternating between the input vectors. Equivalent to python "zip".
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hZipper
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecA)()("Input vector #1 of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecB)()("Input vector #2 of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hZipper(vec,vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]
  vec.zipper(vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vectors are shorther than vec they will wrap around and
  start from the beginning.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter vecA, const Iter vecA_end, const Iter vecB, const Iter vecB_end)
{
  Iter it=vec;
  Iter itA=vecA;
  Iter itB=vecB;
  if (itA>=vecA_end) return;
  if (itB>=vecB_end) return;
  while (it!=vec_end) {
    *it=*itA;
    ++it; if (it==vec_end) return;
    ++itA; if (itA==vecA_end) itA=vecA;
    *it=*itB;
    ++it; if (it==vec_end) return;
    ++itB; if (itB==vecB_end) itB=vecB;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Make and return a new vector of the same size and type as the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNew
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
std::vector<T> HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> vnew(vec.size());
  return vnew;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Resize a vector to a new length.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size of vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
 */
template <class T>
void HFPP_FUNC_NAME (std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize a vector to a new length and fill new elements in vector with a specific value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(fill)()("Value to fill new vector elements with")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
void HFPP_FUNC_NAME (std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize an STL vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class T, class S>
void HFPP_FUNC_NAME (std::vector<T> & vec1,std::vector<S> & vec2)
{
  vec1.resize(vec2.size());
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Resize a casa vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------#
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T, class S>
void HFPP_FUNC_NAME (casa::Vector<T> & vec1,casa::Vector<S> & vec2)
{
  vec1.resize(*(vec2.shape().begin()));
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvert
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Output vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec1.convert(vec2) -> Copy vec1 to vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iter2>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter2 vec2,const Iter2 vec2_end)
{
  typedef IterValueType T;
  Iter it1(vec1);
  Iter2 it2(vec2);
  while (it1!=vec1_end) {
    *it1=hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type and resizes the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvertResize
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL) // since it does memory management, using resize
#define HFPP_PYTHON_WRAPPER_CLASSES (STL) // since it does memory management, using resize
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_CODE_PRE hResize(vecout,vecin); //code will be inserted in wrapper generation in cc file
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec1.convert(vec2) -> Copy vec1 to vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  typedef IterValueType T;
  Iter itout(vecout);
  Iterin itin(vecin);
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout=hfcast<T>(*itin);
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Copies a vector to another one without resizing them.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec1.copy(vec2) -> copy elements of vec2 to vec1

  If the input vector is shorter than the output vector, it will be
  copied mutliple times until the output vector is filled.  Use
  vec.resize first if you want to ensure that both vectors have the
  same size.

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  typedef IterValueType T;
  Iter itout(vecout);
  Iterin itin(vecin);
  if (itin >= vecin_end) return;
  while (itout != vecout_end) {
    *itout=*itin;
    ++itin; ++itout;
    if (itin==vecin_end) itin=vecin;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Copies all elements provided in an indexlist from one vector to another (no resizing is done!).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(indexlist)()("Index list containing the positions of the elements to be copied over, (e.g. [0,2,4,...] will copy every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(number_of_elements)()("Maximum number of elements to be copied, if this value is <0, then copying will be repeated until the output vector is filled.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  If the index vector is shorter than the output vector, the (indexed
  part of the) input vector will be copied mutliple times until the
  output vector is filled.  Use vec.resize first if you want to ensure
  that both vectors have the same size.

  If number_of_elements is larger than zero and smaller than the
  length of the index vector, the remaining elements in the index
  vector are simply ignored.

See also: hFindGreaterThan, etc..
*/
template <class Iter, class Iterin, class IterI>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end, const IterI index, const IterI index_end, const HInteger number_of_elements)
{
  typedef IterValueType T;
  Iter itout(vecout);
  IterI itidx(index);
  Iterin itin;
  HInteger count(number_of_elements);
  if ((index >= index_end) || count==0) return;
  if (vecin >= vecin_end) return;
  if (count<0) count=(vecout_end-vecout);
  while (itout != vecout_end) {
    itin = vecin + *itidx;
    if (itin < vecin_end) *itout=hfcast<T>(*itin);
    --count; if (count==0) return;
    ++itidx; ++itout;
    if (itidx==index_end) itidx=index;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Redistributes the values in one vector sequentially into another vector, given an offset and stride (interval) - can be used for a transpose operation
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRedistribute
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(offset)()("Offset from first element in output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(stride)()("Stride (interval) between subsequent elements in the output Vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  vec=[0,1,2,3]
  vec.redistribute(invec,0,2) -> invec = [0,0,0,1,0,0,2,0,0,3,0,0]

  \brief $DOCSTRING
  $PARDOCSTRING

  Operation will stop whenever the end of one of the vectors is reached.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger offset, HInteger stride)
{
  Iter it(vec+offset);
  Iter itin(invec);
  while ((itin!=invec_end)&&(it <vec_end)) {
    *it=*itin;
    ++itin; it+=stride;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//========================================================================
//$SECTION:                 Math Function
//========================================================================

//$DOCSTRING: Returns the squared value of the parameter.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME square
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Value to be squared")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class T>
inline T square(T val)
{
  return val*val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the natural logarithm of the value if the value is larger than zero, otherwise return a low number.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME logSave
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Value to be squared")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class T>
inline T HFPP_FUNC_NAME(T val)
{
  if (val>0) return log(val);
  else return A_LOW_NUMBER;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the interferometer phase in radians for a given frequency and time.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhase
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(frequency)()("Frequency in Hz")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(time)()("Time in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
inline HNumber hPhase(HNumber frequency, HNumber time)
{
  return CR::_2pi*frequency*time;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Coverts a real phase and amplitude to a complex number
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hAmplitudePhaseToComplex
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HComplex)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(amplitude)()("Amplitude of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(phase)()("Phase of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
inline HComplex HFPP_FUNC_NAME(HNumber amplitude, HNumber phase)
{
  return amplitude*exp(HComplex(0.0,phase));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Coverts a real phase to a complex number (with amplitude of unity)
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhaseToComplex
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HComplex)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phase)()("Phase of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
inline HComplex HFPP_FUNC_NAME(HNumber phase)
{
  return exp(HComplex(0.0,phase));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Implementation of the Gauss function.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME funcGaussian
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(x)()("Position at which the Gaussian is evaluated")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(sigma)()("Width of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(mu)()("Mean value of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
inline HNumber funcGaussian (HNumber x,
			     HNumber sigma,
			     HNumber mu)
{
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));
};
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the maximum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMax
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vec.max() -> maximum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it > val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the minimum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMin
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vec.max() -> minimum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it < val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,logSave,sin,sinh,sqrt,square,tan,tanh
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}2(const Iter vecout,const Iter vecout_end, const Iter vecin,const Iter vecin_end)
{
  Iter itin=vecin;
  Iter itout=vecout;
  while ((itin!=vecin_end) && (itout !=vecout_end)) {
    *itout=$MFUNC(*itin);
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor,round
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec.$MFUNC() -> $MFUNC operation on vec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> return result of $MFUNC operation on vec1 in vec2.
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void h{$MFUNC!CAPS}2(const IterOut vecout, const IterOut vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  IterIn itin=vecin;
  IterOut itout=vecout;
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout=$MFUNC(*itin);
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================


//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the second vector. If the first vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}To
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input  vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operand and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  hMulTo(vecA,vecB) -> vecB=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  hAddTo(vecA,vecB) -> vecB=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  hSubTo(vecA,vecB) -> vecB=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  hDivTo(vecA,vecB) -> vecB=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  \brief $DOCSTRING
  $PARDOCSTRING


  Alternative usages are:

  hMul(vecA,vecB) = vecA.mul(vecB)  etc.

*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec1,const Iterin vec1_end, const Iter vec2,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin it1=vec1;
  Iter it2=vec2;
  while (it2!=vec2_end) {
    *it2 =  hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC (*it2));
    ++it1; ++it2;
    if (it1==vec1_end) it1=vec1;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vecA *= vecB  -> vecA=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  vecA += vecB  -> vecA=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  vecA -= vecB  -> vecA=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  vecA /= vecB  -> vecA=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  \brief $DOCSTRING
  $PARDOCSTRING

  Alternative usages are:

  hMul(vecA,vecB) of vecA.mul(vecB)  etc.

*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef IterValueType T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 =  hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC (*it2));
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the vector and a scalar (applied to each element), which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(scalar1)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class S>
void h{$MFUNC}2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef IterValueType T;
  Iter it=vec1;
  while (it!=vec1_end) {
    *it = hfcast<T>(*it HFPP_OPERATOR_$MFUNC val);
    ++it;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;
  while ((it1!=vec1_end)  && (itout !=vec_end)) {
    *itout = hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different type.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Add
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;
  while ((it1!=vec1_end)  && (itout !=vec_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC!LOW between the vector and a scalar, where the result is returned in the first vector (with automatic casting).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(scalar1)()("Scalar value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar
  vec.$MFUNC(vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin1,  class S>
void h{$MFUNC}2(const Iter vec,const Iter vec_end,  const Iterin1 vec1,const Iterin1 vec1_end, const S scalar1)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iter itout=vec;
  while (itout !=vec_end) {
    *itout = hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC  scalar1);
    ++it1; ++itout;
    if (it1==vec1_end) it1=vec1;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE

////////////////////
//COMPLEX FUNCTIONS
///////////////////


//$DOCSTRING: Coverts a vector of real phase to a vector of corresponding complex numbers (with amplitude of unity).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPhaseToComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector returning complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(phasevec)()("Input vector with real phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hPhaseToComplex(outvec,phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]
  outvec.phasetocomplex(phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If input vector is shorter it will be repeated until output vector is full.

*/
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 vec, const Iter1 vec_end, const Iter2 phasevec, const Iter2 phasevec_end)
{
  Iter1 it1=vec;
  Iter2 it2=phasevec;
  if (it2>=phasevec_end) return;
  while (it1!=vec_end) {
    *it1=hPhaseToComplex(*it2);
    ++it1;
    ++it2; if (it2==phasevec_end) it2=phasevec; // loop over input vector if necessary ...
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Coverts a vector of real phases and amplitudes to a vector of corresponding complex numbers.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAmplitudePhaseToComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector returning complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(ampphase)()("Input vector with real amplitudes and phases (2 numbers per entry: [amp_0, phase_0, amp_1, phase_1, ...])")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hAmplitudePhaseToComplex(vec,ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]
  vec.amplitudephasetocomplex(ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If input vector is shorter it will be repeated until output vector is full.

*/
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 vec, const Iter1 vec_end, const Iter2 ampphase, const Iter2 ampphase_end)
{
  Iter1 it1=vec;
  Iter2 it2=ampphase;
  if (it2+1>=ampphase_end) return;
  while (it1!=vec_end) {
    *it1=hAmplitudePhaseToComplex(*it2,*it2+1);
    ++it1;
    it2+=2; if (it2>=ampphase_end) it2=ampphase; // loop over input vector if necessary ...
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculate the complex conjugate of all elements in the complex vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConj
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void  HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=conj(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Multiplies the elements of the first vector with the complex conjugate of the elements in the second and returns the results in the first.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCrossCorrelateComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec1)()("Complex input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vec2)()("Second complex vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  fftvec1.crosscorrelatecomplex(fftvec2) -> fftvec1 return FFT of cross correlation between

  \brief $DOCSTRING
  $PARDOCSTRING

 If the second vector is shorter than the first one, the second vector
 will simply wrap around and begin from the start until the end of the
 first vector is reached. If the first vector is shorter, then the
 calculation will simply stop.

 Relation to Cross Correlation:
 ------------------------------
 If the complex input vectors are the Fourier transformed data of two
 (real) vector f1 & f2, then vec1*conj(vec2) will be the Fourier
 transform of the crosscorrelation between f1 and f2.

 Hence, in order to calculate a cross correlation between f1 & f2, first do
 f1.fft(vec1) and f2.fft(vec2), then vec1.crosscorrelatecomplex(vec2)
 and FFT back through vec1.invfft(floatvec).
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end)
{
  Iter it1=vec1;
  Iter it2=vec2;
  while (it1!=vec1_end) {
    *it1 *= conj(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//========================================================================
//$ITERATE MFUNC arg,imag,norm,real
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the complex vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Complex input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed. 

  Input and output vector can be identical.

The following functions are available for getting real values from
complex numbers:
  norm - magnitude (length) squared of a complex number, i.e. c * conj(c)
  abs - amplitude of a complex number, i.e. c * conj(c)
  arg - phase angle of a complex number (in radians)
  imag - imaginary part of a complex number
  real - real part of a complex number
*/
template <class Iter, class IterIn>
void h{$MFUNC!CAPS}(const Iter vecout,const Iter vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  IterIn itin(vecin);
  Iter itout(vecout);
  if (itout >= vecout_end) return;
  if (itin >= vecin_end) return;
  while (itout != vecout_end) {
    *itout=(IterValueType)$MFUNC(*itin);
    ++itin; ++itout;
    if (itin == vecin_end) itin=vecin;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Take the $MFUNC of all the elements in the complex vector and return results in the same vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (Complex)(vecout)()("Complex in and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed. 

  Input and output vector can be identical.

The following functions are available for getting real values from
complex numbers:
  norm - magnitude (length) squared of a complex number, i.e. c * conj(c)
  abs - amplitude of a complex number, i.e. c * conj(c)
  arg - phase angle of a complex number (in radians)
  imag - imaginary part of a complex number
  real - real part of a complex number
*/
template <class Iter>
void h{$MFUNC!CAPS}(const Iter vecout,const Iter vecout_end)
{
  Iter itout(vecout);
  if (itout >= vecout_end) return;
  while (itout != vecout_end) {
    *itout=$MFUNC(*itout);
    ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//$DOCSTRING: Set the amplitude of complex numbers to the values provided in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSetAmplitude
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector with amplitudes.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  The function calculates (c.setamplitude(newamplitude)) for each
  element c -> c/|c|*newamplitude.

  If input vector is shorter than output vector it will wrap around
  and start from the beginning until the output vector is fully processed.
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME (const IterOut vecout,const IterOut vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  IterIn itin(vecin);
  IterOut itout(vecout);
  if (itout >= vecout_end) return;
  if (itin >= vecin_end) return;
  while (itout != vecout_end) {
    *itout *= (*itin)/abs(*itout);
    ++itin; ++itout;
    if (itin == vecin_end) itin=vecin;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the amplitude of complex numbers to a particular value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSetAmplitude
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(amp)()("Amplitude")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  The function calculates (c.setamplitude(amp)) for each
  element c -> c/|c|*amp.
*/
template <class IterOut, class Iter>
void HFPP_FUNC_NAME (const IterOut vecout,const IterOut vecout_end, Iter amp)
{
  IterOut itout(vecout);
  if (itout >= vecout_end) return;
  while (itout != vecout_end) {
    *itout *= amp/abs(*itout);
    ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNegate
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  IterValueType fac = -1;
  while (it!=vec_end) {
    *it=(*it) * fac;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=*it; ++it;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the absolute values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSumAbs
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=abs(*it); ++it;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the square values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSumSquare
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=(*it)*(*it); ++it;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Piecewise multiplication of the elements in a vector and summing of the results
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMulSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hMulSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> a*x + b*y + c*z + ....

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it1=vec1,it2=vec2;
  while (it1!=vec1_end && it2!=vec2_end) {sum+=*it1 * *it2; ++it1; ++it2;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

// //$DOCSTRING: Piecewise multiplication of the elements in a vector and summing of the results
// //$COPY_TO HFILE START --------------------------------------------------
// #define HFPP_FUNC_NAME hMulSum
// //-----------------------------------------------------------------------
// #define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE  //Needed due to a bug in hfppnew.h which can't deal with scalar references ....
// #define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
// #define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(sum)()("Output value (sum of product of vectors).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
// //$COPY_TO END --------------------------------------------------
// /*!
//   hMulSum(sum,Vector([a,b,c,...]),Vector([x,y,z,...]) -> sum=a*x + b*y + c*z + ....
//   \brief $DOCSTRING
//  $PARDOCSTRING
// */
// template <class Iter,class T>
// IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end,T & sum)
// {
//   sum=hfnull<T>();
//   Iter it1=vec1,it2=vec2;
//   while (it1!=vec1_end && it2!=vec2_end) {sum+=hfcast<T>(*it1 * *it2); ++it1; ++it2;};
// }
// //$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Multiplies all elements in the vector with each other and return the result
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hProduct
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING

 hProduct(vec) -> vec[0]*vec[1]*vec[2]* ... * vec[N]

*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  typedef IterValueType T;
  T prod=1.0;
  Iter it=vec;
  while (it!=vec_end) {prod *= *it; ++it;};
  return prod;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2))).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hVectorLength
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber sum=0;
  Iter it=vec;
  while (it!=vec_end) {sum += (*it) * (*it); ++it;};
  return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Normalizes a vector to length unity.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNormalize
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber norm=hVectorLength(vec,vec_end);
  Iter it=vec;
  while (it!=vec_end) {*it=(*it)/norm; ++it;};

}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//========================================================================
//$SECTION:           Statistics Functions
//========================================================================

//$DOCSTRING: Fills a vector with random values between minimum and maximum limits
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandom
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector which will be filled with random numbers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(minimum)()("Random numbers will not go below that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(maximum)()("Random numbers will not exceed that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hRandom(vector,-1,1) -> [-0.5,0.3,0.1, ...]
  vector.random(-1,1) -> [-0.5,0.3,0.1, ...]

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const IterValueType minimum, const IterValueType maximum)
{
  Iter it=vec;
  IterValueType scale=(maximum-minimum)/RAND_MAX;
  while (it!=vec_end) {
    *it=rand()*scale+minimum;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Returns the mean value of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber mean=hSum(vec,vec_end);
  if (vec_end>vec) mean/=(vec_end-vec);
  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of the absolue values of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanAbs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber mean=hSumAbs(vec,vec_end);
  if (vec_end>vec) mean/=(vec_end-vec);
  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of the square values of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanSquare
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber mean=hSumSquare(vec,vec_end);
  if (vec_end>vec) mean/=(vec_end-vec);
  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Sorts a vector in place.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSort
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted in place. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Sorts a vector in place and returns the median value of the elements.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSortMedian
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted first. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
  if (vec_end!=vec) return *(vec+(vec_end-vec)/2);
  else return hfnull<IterValueType>();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the median value of the elements.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMedian
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
T HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> scratch(vec);
  return hSortMedian(scratch);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation around a mean value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(mean)()("The mean value of the vector calculated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const IterValueType mean)
{
  typedef IterValueType T;
  HNumber scrt,sum=0.0;
  HInteger len=vec_end-vec;
  Iter it=vec;
  while (it<vec_end) {
    scrt = *it - mean;
    sum += scrt * scrt;
    ++it;
  };
  if (len>1) return sqrt(sum / (len-1));
  else return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Calculates the mean of all values which are $MFUNC a certain threshold value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean{$MFUNC}Threshold
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold above which elements are taken into account")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec, const Iter vec_end, const IterValueType threshold)
{
  Iter it(vec);
  IterValueType sum(0),extremum(*it);
  HInteger n(0);
  while (it<vec_end) {
    if (*it HFPP_OPERATOR_$MFUNC extremum) extremum = *it;
    if (*it HFPP_OPERATOR_$MFUNC threshold) {
      sum += *it; ++n;
    };
    ++it;
  };
  if (n>0) return sum/n;
  else return extremum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


//$DOCSTRING: Calculates the mean of all values which are below or above the mean plus 'nsigma' standard deviations
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanThreshold
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(mean)()("The mean of the values in the input vector provided as input.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(rms)()("The rms value of the vector - also calculated beforehand.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(nsigma)()("Only consider values that are below (positive) or above (negative) mean+nsigma+sigma the mean")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  This simply call hMeanGreaterThanThreshold or hMeanLessThanThreshold with a
  threshold value of mean+nsigma*rms.
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec, const Iter vec_end, HNumber mean, HNumber rms, HNumber nsigma)
{
  if (nsigma>=0) return hMeanLessThanThreshold(vec,vec_end,(IterValueType)(mean+nsigma*rms));
  else return hMeanGreaterThanThreshold(vec,vec_end,(IterValueType)(mean+nsigma*rms));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the mean of the inverse of all values and then return the inverse of that value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanInverse
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  vec.meaninvers() -> N/sum(1/vec_0+1/vec_1+...+1/vec_N)

  This is useful to calculate the mean value of very spiky data. Large
  spikes will appear as zero and hence will not have a strong effect
  on the average (or the sum) if only a small number of channels are
  affected. while a single very large spike could well dominate the
  normal average.
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum(hfnull<T>());
  Iter it(vec);
  while (it!=vec_end) {sum+=1.0/(*it); ++it;};
  return (vec_end-vec)/sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the standard deviation of a vector of values.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber hStdDev (const Iter vec,const Iter vec_end)
{
  return hStdDev(vec,vec_end,hMean(vec,vec_end));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//========================================================================
//$SECTION:           Comparison and Selection
//========================================================================


#define Between(x,lower,upper) ((x > lower) && (x < upper))
#define BetweenOrEqual(x,lower,upper) ((x >= lower) && (x <= upper))
#define Outside(x,lower,upper) ((x < lower) || (x > upper))
#define OutsideOrEqual(x,lower,upper) ((x <= lower) || (x >= upper))

//========================================================================
//$ITERATE MFUNC Between,BetweenOrEqual,Outside,OutsideOrEqual
//========================================================================

//$DOCSTRING: Find the samples that are $MFUNC upper and lower threshold values and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are between the limits.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(lower_limit)()("The lower limit of values to find.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_TYPE)(upper_limit)()("The upper limit of values to find.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType lower_limit,
			 const IterValueType upper_limit)
{
  Iter itin=vecin;
  typename vector<HInteger>::iterator itout=vecout;
  while (itin<vecin_end) {
    if ({$MFUNC}(*itin,lower_limit,upper_limit)) {
      if (itout < vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Find the samples that are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in the input vector which satisfy the threshold condition.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType threshold
			 )
{
  Iter itin=vecin;
  typename vector<HInteger>::iterator itout=vecout;
  while (itin<vecin_end) {
    if (*itin HFPP_OPERATOR_$MFUNC threshold) {
      if (itout < vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples whose absolute values are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}Abs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are above threshold")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType threshold
			 )
{
  Iter itin=vecin;
  typename vector<HInteger>::iterator itout=vecout;
  while (itin<vecin_end) {
    if (abs(*itin) HFPP_OPERATOR_$MFUNC threshold) {
      if (itout < vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Count the samples that are $MFUNC a certain threshold value and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const IterValueType threshold)
{
  Iter it=vec;
  HInteger count=0;
  while (it<vec_end) {
    if (*it HFPP_OPERATOR_$MFUNC threshold) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples whose absolute values are $MFUNC a certain threshold value and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC}Abs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const IterValueType threshold)
{
  Iter it=vec;
  HInteger count=0;
  while (it<vec_end) {
    if (abs(*it) HFPP_OPERATOR_$MFUNC threshold) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE


//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,
		     const Iter vecout_end,
		     const Iter vecin,
		     const Iter vecin_end
		     )
{
  if (vecout>=vecout_end) return; //If size 0 do nothing
  if (vecin>=vecin_end) return; //If size 0 do nothing
  HInteger ilen=(vecin_end-vecin);
  HInteger olen=(vecout_end-vecout);
  HInteger blen=max(ilen/(olen-1),1);
  //use max to avoid infinite loops if output vector is too large
  Iter it1=vecin, it2;
  Iter itout=vecout, itout_end=vecout_end-1;
  //only produce the first N-1 blocks in the output vector
  while ((it1<vecin_end) && (itout<itout_end)) {
    it2=min(it1+blen,vecin_end);
    *itout=hMean(it1,it2);
    it1=it2;
    ++itout;
    }
  *itout=hMean(it2,vecin_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Interpolate a vector between two end points, which are part also start and end points of the new vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInterpolate2P
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Interpolated output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(startvalue)()("Starting value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(endvalue)()("End value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
v=hArray(float, [10])
v.interpolate2p(0.,9.)  #-> [0,1,2,3,4,5,6,7,8,9]
v.interpolate2psubpiece(0.,10.) #-> [0,1,2,3,4,5,6,7,8,9]
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,
		     const Iter vec_end,
		     const IterValueType startvalue,
		     const IterValueType endvalue
		     )
{
  HInteger count(0), len(vec_end-vec);
  if (len==1) {*vec=startvalue; return;};
  HNumber slope((endvalue-startvalue)/(len-1));
  Iter it(vec);
  while (it<vec_end) {
    *it=(IterValueType)(startvalue+count*slope);
    ++it; ++count;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Interpolate a vector between two end points, where the 2nd end point marks the last element of the output vector plus one. Useful for piecing interpolations together.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInterpolate2PSubpiece
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Interpolated output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(startvalue)()("Starting value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(endvalue)()("End value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
v=hArray(float, [10])
v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
v.interpolate2psubpiece(0.,10.) #-> [0,1,2,3,4,5,6,7,8,9]
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,
		     const Iter vec_end,
		     const IterValueType startvalue,
		     const IterValueType endvalue
		     )
{
  HInteger count(0), len(vec_end-vec);
  if (len==1) {*vec=startvalue; return;};
  HNumber slope((endvalue-startvalue)/len);
  Iter it(vec);
  while (it<vec_end) {
    *it=(IterValueType)(startvalue+count*slope);
    ++it; ++count;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecrms)()("Output vector containgin the RMS in each bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.

     As a second output the root mean square (RMS, standard deviation)
     of the mean in each downsampled bin is returned.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,
		     const Iter vecout_end,
		     const Iter vecrms,
		     const Iter vecrms_end,
		     const Iter vecin,
		     const Iter vecin_end
		     )
{
  if (vecout>=vecout_end) return; //If size 0 do nothing
  if (vecin>=vecin_end) return; //If size 0 do nothing
  if (vecrms>=vecrms_end) return; //If size 0 do nothing
  HInteger ilen(vecin_end-vecin);
  HInteger olen(vecout_end-vecout);
  HInteger blen(max(ilen/(olen-1),1));
  //use max to avoid infinite loops if output vector is too large
  Iter it1(vecin), it2;
  Iter itrms(vecrms);
  Iter itout(vecout), itout_end(vecout_end-1);
  //only produce the first N-1 blocks in the output vector
  while ((it1<vecin_end) && (itout<itout_end) && (itrms<vecrms_end)) {
    it2=min(it1+blen,vecin_end);
    *itout=hMean(it1,it2);
    *itrms=hStdDev(it1,it2,*itout);
    it1=it2;
    ++itout;++itrms;
    }
  *itout=hMean(it2,vecin_end);
  *itrms=hStdDev(it2,vecin_end,*itout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Upsample the input vector to a larger output vector by linear interpolation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hUpsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Upsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Shorter vector to be upsampled.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
x=hArray(float, [31])
v=hArray(float, [10])
v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
x.upsample(v)
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,
		     const Iter vecout_end,
		     const Iter vecin,
		     const Iter vecin_end
		     )
{
  if (vecout>=vecout_end) return; //If size 0 do nothing
  if (vecin>=vecin_end) return; //If size 0 do nothing
  HInteger ilen(vecin_end-vecin);
  HInteger olen(vecout_end-vecout);
  HNumber blen(max(olen/(ilen-1.0),1.0));
  HInteger count(1);
  //use max to avoid infinite loops if input vector is too large
  Iter it1(vecout), it2(it1+((HInteger)blen)),  itin(vecin), itin_end(vecin_end-2);
  //only produce the first N-1 blocks in the input vector
  while ((it2<vecout_end) && (itin<itin_end)) {
    hInterpolate2PSubpiece(it1,it2,*itin,*(itin+1));
    ++count; it1=it2; it2=vecout+(HInteger)(count*blen); ++itin;
  }
  hInterpolate2P(it1,vecout_end,*itin,*itin+1);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector to a smaller output vector trying to exclude spikes in the data.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsampleSpikyData
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecrms)()("Output vector containgin the RMS in each bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(nsigma)()("Only consider values for averaging that are nsigma above (positive) or below (negative) the mean.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.

     As a second output the root mean square (RMS, standard deviation)
     of the mean in each downsampled bin is returned.

     The mean here is taken only of data that is nsigma below
     (negative) or above (positive) the RMS in the bin.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,
		     const Iter vecout_end,
		     const Iter vecrms,
		     const Iter vecrms_end,
		     const Iter vecin,
		     const Iter vecin_end,
		     HNumber nsigma
		     )
{
  if (vecout>=vecout_end) return; //If size 0 do nothing
  if (vecin>=vecin_end) return; //If size 0 do nothing
  if (vecrms>=vecrms_end) return; //If size 0 do nothing
  HInteger ilen(vecin_end-vecin);
  HInteger olen(vecout_end-vecout);
  HInteger blen(max(ilen/(olen-1),1));
  //use max to avoid infinite loops if output vector is too large
  Iter it1(vecin), it2;
  Iter itrms(vecrms);
  Iter itout(vecout), itout_end(vecout_end-1);
  //only produce the first N-1 blocks in the output vector
  HNumber mean;
  while ((it1<vecin_end) && (itout<itout_end) && (itrms<vecrms_end)) {
    it2=min(it1+blen,vecin_end);
    mean=hMean(it1,it2);
    *itrms=hStdDev(it1,it2,mean);
    *itout=hMeanThreshold(it1,it2,mean,*itrms,nsigma);
    it1=it2;
    ++itout;++itrms;
  }
  mean=hMean(it2,vecin_end);
  *itrms=hStdDev(it2,vecin_end,mean);
  *itout=hMeanThreshold(it2,vecin_end,mean,*itrms,nsigma);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Downsample the input vector by a cetain factor and return a new vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(downsample_factor)()("Factor by which to reduce original size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a new smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class T>
std::vector<T> HFPP_FUNC_NAME (
		  std::vector<T> & vec,
		  HNumber downsample_factor
		  )
{
  std::vector<T> newvec(floor(vec.size()/downsample_factor+0.5));
  hDownsample(newvec,vec);
  return newvec;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFindLowerBound
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Sorted numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(value)()("value to search for")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

Finds -- through a binary search and interpolation -- the location in
  a monotonically increasing vector, where the search value is just
  above or equal to the value in the vector.

This requires random access iterators, in order to have an optimal search result.

*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec, const Iter vec_end,
			 const IterValueType value)
//iterator_traits<Iter>::value_type value)
{
  HNumber value_n=hfcast<HNumber>(value); //This also works for Complex then
  HInteger niter=0;
  if (vec==vec_end) return 0;
  HInteger maxpos=vec_end-vec-1,posguess;
  Iter it1=vec,it2=vec_end-1,it0;
  if (value_n<=hfcast<HNumber>(*it1)) return 0;
  if (value_n>=hfcast<HNumber>(*it2)) return maxpos;
  posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*maxpos;
  it0=vec+posguess;
  if (it0<it1) return hfcast<HInteger>(it1-vec); //Error, Non monotonic
  if (it0>=it2) return hfcast<HInteger>(it2-vec); //Error, Non monotonic
  //  cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
  while (!((value_n < hfcast<HNumber>(*(it0+1))) && (value_n >= hfcast<HNumber>(*it0)))) {
    if (value_n > hfcast<HNumber>(*it0)) {
      it1=it0;
    } else {
      it2=it0;
    };
    it0=it1+(it2-it1)/2;
    if (*it0>value_n) it2=it0; //Binary search step
    else it1=it0;
    posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*(it2-it1)+(it1-vec);
    it0=vec+posguess;
    ++niter;
    //cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
    if (it0<it1) return it1-vec; //Error, Non monotonic
    if (it0>it2) return it2-vec; //Error, Non monotonic
  };
  return posguess;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

/*
//Wrapper for c-style arrays
HInteger hFindLowerBound(const HNumber* vec,
			 const HInteger len,
			 const HNumber value)
{
  return hFindLowerBound(vec,vec+len,value);
}
*/

//$DOCSTRING: Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFlatWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  std::vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING:  Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

The vector is normalized to give a sum of unity. Can be used by
hRunningAverage.
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  std::vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGaussianWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>


std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(wlen/4.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a normalized weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
vector<HNumber> HFPP_FUNC_NAME (HInteger wlen, hWEIGHTS wtype){
  vector<HNumber> weights;
  if (wlen<1) wlen=1;
  switch (wtype) {
  case WEIGHTS_LINEAR:
    weights=hLinearWeights(wlen);
    break;
  case WEIGHTS_GAUSSIAN:
    weights=hGaussianWeights(wlen);
    break;
  default: //  WEIGHTS_FLAT:
    weights=hFlatWeights(wlen);
  };
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Calculate the running average of an input vector using a weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(weights)()("Weight vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class DataIter, class NumVecIter>
void HFPP_FUNC_NAME (const DataIter  odata,
		     const DataIter  odata_end,
		     const DataIter  idata,
		     const DataIter  idata_end,
		     const NumVecIter weights,
		     const NumVecIter weights_end)
{
  HInteger l=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=l/2;
  /* To avoid too many rounding errors with Integers */
  //  typename DataIter::value_type fac = l*10;
  HNumber temp;

  DataIter  dit;
  DataIter  dit2;
  DataIter  din(idata);
  DataIter  dout(odata);
  NumVecIter wit;

  while (din<idata_end && dout<odata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights; // weight iterators set to beginning of weights
    temp=0.0;
    while (wit<weights_end) {
      if (dit<idata) dit2=idata;
      else if (dit>=idata_end) dit2=idata_end-1;
      else dit2=dit;
      temp=temp+(*dit2)*(*wit);
      ++dit; ++wit;
    };
    *dout=temp;
    ++dout; ++din; //point to the next element in data input and output vector
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Overloaded function to automatically calculate weights.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:

  in_array.runningaverage(array_out,7,hWEIGHTS.GAUSSIAN)

  Available Weights:

  FLAT:  all have the same value
  LINEAR: linearly rising, peaking at the center (i.e., /\)
  GAUSSIAN: Gaussian distribution falling of to 2 sigma at the ends
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter odata,
		     const DataIter odata_end,
		     const DataIter idata,
		     const DataIter idata_end,
		     HInteger wlen,
		     hWEIGHTS wtype)
{
  vector<HNumber> weights = hWeights(wlen, wtype);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (odata,
							odata_end,
							idata,
							idata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//
//========================================================================
//$SECTION:           RF (Radio Frequency) Function
//========================================================================

//$DOCSTRING: Coverts a vector of time delays and a vector of frequencies to a corresponding vector of phases (of a complex number).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDelayToPhase
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Output vector returning real phases of complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Input vector with real delays in units of time [second]")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(delays)()("Input vector with real delays in units of time [second]")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hDelayToPhase(vec,frequencies,delays) -> vec = [phase(frequencies_0,delays_0),phase(frequencies_1,delays_0),...,phase(frequencies_n,delays_0),phase(frequencies_1,delays_1),...]


  \brief $DOCSTRING
  $PARDOCSTRING

  The frequencies always run fastest in the output vector.  If input
  vectors are shorter they will be wrapped until the output vector is
  full.

*/
template <class Iter>
void  HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const Iter frequencies, const Iter frequencies_end, const Iter delays, const Iter delays_end)
{
  Iter it1=vec;
  Iter it2=frequencies;
  Iter it3=delays;
  if (it2>=frequencies_end) return;
  if (it3>=delays_end) return;
  while (it1!=vec_end) {
    *it1=hPhase(*it2,*it3);
    ++it1;
    ++it2; if (it2==frequencies_end) {
      it2=frequencies;
      ++it3; if (it3==delays_end) it3=delays; // loop over input vector if necessary ...
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelayFarField
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(antPosition)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(skyDirection)()("Vector in Cartesian coordinates pointing towards a sky position from the antenna - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(length)()("Length of the skyDirection vector, used for normalization - provided to speed up calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
HNumber HFPP_FUNC_NAME (
			const Iter antPosition,
			const Iter skyDirection,
			HNumber length
			)
{
  return - (*skyDirection * *antPosition
	    + *(skyDirection+1) * *(antPosition+1)
	    + *(skyDirection+2) * *(antPosition+2))/length/CR::lightspeed;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelayNearField
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(antPosition)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(skyPosition)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(distance)()("Distance of source, i.e. the length of skyPosition - provided to speed up calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
HNumber HFPP_FUNC_NAME (
			const Iter antPosition,
			const Iter skyPosition,
			const HNumber distance)
{
  return (
	  sqrt(
	       square(*skyPosition - *antPosition)
	       +square(*(skyPosition+1) - *(antPosition+1))
	       +square(*(skyPosition+2) - *(antPosition+2))
	       ) - distance
	  )/CR::lightspeed;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelays
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(delays)()("Output vector containing the delays in seconds for all antennas and positions [antenna index runs fastest: (ant1,pos1),(ant2,pos1),...] - length of vector has to be number of antennas times positions")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

result=[]
plt.clf()
for j in range(29,30):
    x=[]; y=[]
    for i in range(177,178):
        azel[0]=float(i)
        azel[1]=float(j)
        hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
        hGeometricDelays(delays,antenna_positions,hArray(cartesian),True)
        delays *= 10**6
        deviations=delays-obsdelays
        deviations.abs()
        sum=deviations.vec().sum()
        x.append(i); y.append(sum)
    result.append(y)
    plt.plot(x,y)
b
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter delays,
		     const Iter delays_end,
		     const Iter antPositions,
		     const Iter antPositions_end,
		     const Iter skyPositions,
		     const Iter skyPositions_end,
		     const bool farfield
		     )
{
  HNumber distance;
  Iter
    ant=antPositions,
    sky=skyPositions,
    del=delays,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;

  if (farfield) {
    while (del < delays_end) {
      distance = hVectorLength(sky,sky+3);
      *del=hGeometricDelayFarField(ant,sky,distance);
      ++del;
      ant+=3; if (ant>=ant_end) {
	ant=antPositions;
	sky+=3; if (sky>=sky_end) sky=skyPositions;
      };
    };
  } else {
    while (del < delays_end) {
      distance = hVectorLength(sky,sky+3);
      *del=hGeometricDelayFarField(ant,sky,distance);
      ++del;
      ant+=3; if (ant>=ant_end) {
	ant=antPositions;
	sky+=3; if (sky>=sky_end) sky=skyPositions;
      };
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricPhases
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phases)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: (nu1,ant1,pos1),(nu2,ant1,pos1),...] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Vector of frequencies (in Hz) to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter phases,
		     const Iter phases_end,
		     const Iter frequencies,
		     const Iter frequencies_end,
		     const Iter antPositions,
		     const Iter antPositions_end,
		     const Iter skyPositions,
		     const Iter skyPositions_end,
		     const bool farfield
		     )
{
  HNumber distance;
  Iter
    ant,
    freq,
    sky=skyPositions,
    phase=phases,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;

  if (farfield) {
    while (sky < sky_end && phase < phases_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
	freq=frequencies;
	while (freq < frequencies_end && phase < phases_end) {
	  *phase=hPhase(*freq,hGeometricDelayFarField(ant,sky,distance));
	  ++phase; ++freq;
	};
	ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && phase < phases_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
	freq=frequencies;
	while (freq < frequencies_end && phase < phases_end) {
	  *phase=hPhase(*freq,hGeometricDelayNearField(ant,sky,distance));
	  ++phase; ++freq;
	};
	ant+=3;
      };
      sky+=3;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricWeights
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(weights)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: (nu1,ant1,pos1),(nu2,ant1,pos1),...] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Vector of frequencies (in Hz) to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter weights,
		     const CIter weights_end,
		     const Iter frequencies,
		     const Iter frequencies_end,
		     const Iter antPositions,
		     const Iter antPositions_end,
		     const Iter skyPositions,
		     const Iter skyPositions_end,
		     const bool farfield
		     )
{
  HNumber distance;
  Iter
    ant,
    freq,
    sky=skyPositions,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;
  CIter weight=weights;

  if (farfield) {
    while (sky < sky_end && weight < weights_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
	freq=frequencies;
	while (freq < frequencies_end && weight < weights_end) {
	  *weight=exp(HComplex(0.0,hPhase(*freq,hGeometricDelayFarField(ant,sky,distance))));
	  ++weight; ++freq;
	};
	ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && weight < weights_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
	freq=frequencies;
	while (freq < frequencies_end && weight < weights_end) {
	  *weight=exp(HComplex(0.0,hPhase(*freq,hGeometricDelayNearField(ant,sky,distance))));
	  ++weight; ++freq;
	};
	ant+=3;
      };
      sky+=3;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the power of a complex spectrum and add it to an output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSpectralPower
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Input vector containing the complex spectrum. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an hArray) is here determined by
  the second parameter!

Example:
spectrum=hArray(float,[1,128])
cplxfft=hArray(complex,[10,128],fill=1+0j)
spectrum[...].spectralpower(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  Iterin itin(vecin);
  Iter itout(vecout);
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=real((*itin)*conj(*itin));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the upper half of the cross-correlation matrix of a number of antenna data in the frequency domain and adds it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCrossCorrelationMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(ccm)()("Upper half of the cross-correlation matrix (output) containing complex visibilities as a function of frequency. The ordering is ant0*ant1,ant0*ant2,...,ant1*ant2,... - where each antN contains many frequency bins.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Vector containing the FFTed data of all antennas subsequently.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nfreq)()("Number of frequency bins per antenna.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  hCrossCorrelationMatrix(ccm,fftdata,nfreq) -> ccm = ccm(old) + ccm(fftdata)

  \brief $DOCSTRING
  $PARDOCSTRING

  The length of the ccm vector is N*(N-1)/2 * N_freq, where N is the
  number of antennas and N_freq the number of frequency bins per
  antenna. The length of the (input) vector is then N*N_freq.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter ccm,const Iter ccm_end, const Iter fftdata,const Iter fftdata_end, const HInteger nfreq)
{
  Iter it(ccm);
  Iter it1_start(fftdata);
  Iter it1_end(fftdata+nfreq);
  Iter it1(it1_start);
  Iter it2(it1_end);
  if (it1>=fftdata_end) return;
  if (it2>=fftdata_end) return;
  while (it!=ccm_end) {
    *it+=(*it1)*conj(*it2);
    ++it; ++it1; ++it2;
    if (it1==it1_end) {
      if (it2>=fftdata_end) {
	it1_start+=nfreq;
	it1_end+=nfreq;
	if (it1_end>=fftdata_end) return;
	it2=it1_end;
      };
      it1=it1_start;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates a single complex pixel or a pixel vector (i.e., an image), from an cross correlation matrix and complex geometrical weights for each antenna by multiplying ccm and weights and summing over all baselines.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCImageFromCCM
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(image)()("Image vector containing N_freq frequency bins in the order [power(pix0,bin0),power(pix0,bin1),...,power(pix1,bin0),...]. Size of vector is N_pixel*N_bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(ccm)()("Upper half of the cross-correlation matrix (input) containing complex visibilities as a function of frequency. The ordering is ant0*ant1,ant0*ant2,...,ant1*ant2,... - where each antN contains nfreq frequency bins.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(weights)()("Vector containing the weights to be applied for each pixel, antenna, and frequency bin (in that order, i.e. frequency index runs fastest).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(nofAntennas)()("Number of antennas used for weights and ccm")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(nfreq)()("Number of frequency bins used for weights and ccm")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  hCrossCorrelationMatrix(ccm,fftdata,nfreq) -> ccm = ccm(old) + ccm(fftdata)

  \brief $DOCSTRING
  $PARDOCSTRING

  The image contains the power as a function of pixel and frequency
  bins. Each value is the sum over all baselines of the norm
  of the complex visibilities times complex weights for that pixel and
  frequency bin.

  The results will be added to the image, hence for a new image the
  image must be initialized to zero! Otherwise an old image can be
  provided and the new data will simply be added to it.

  The length of the ccm vector is (N-1)/2*N_freq, where N is the
  number of antennas and N_freq the number of frequency bins per
  antenna. The length of the weights vector is N_pixel*N_ant*N_freq.

Example:
  ccm=Vector(complex,file["nofSelectedAntennas"]*(file["nofSelectedAntennas"]-1)/2*file["fftLength"])
  cimage=Vector(complex,n_pixels*nbins,fill=0)
  image=hArray(float,[n_pixels,file["fftLength"]])

  hCrossCorrelationMatrix(ccm,file_fft.vec(),file["fftLength"])
  hCImageFromCCM(cimage,ccm,weights.vec(),file["nofSelectedAntennas"],file["fftLength"])
  cimage.norm(image.vec())
  intimage=np.array(image[...].sum()) # convert to numpy array
  immax=intimage.max()
  intimage /= immax
  intimage.resize([n_az,n_el])
  plt.imshow(intimage,cmap=plt.cm.hot)

*/
template <class IterC>
void HFPP_FUNC_NAME(
		    const IterC image, const IterC image_end,
		    const IterC ccm,const IterC ccm_end,
		    const IterC weights,const IterC weights_end,
		    const HInteger nofAntennas,
		    const HInteger nfreq
		    )
{
  IterC it_im(image);
  IterC it_im_start=image;
  IterC it_im_end=image+nfreq;
  IterC it_ccm(ccm);
  IterC it_w1_start(weights);
  IterC it_w1_end(weights+nfreq);
  IterC it_w2_end = it_w1_start+nofAntennas*nfreq;
  IterC it_w1(it_w1_start); //start at antenna 0
  IterC it_w2(it_w1_end); //start at antenna 1
  if (it_ccm>=ccm_end) return;
  if (it_w1>=weights_end) return;
  while (it_im_start<image_end) {
    *it_im+=(*it_ccm) * (*it_w1)*conj(*it_w2);
    ++it_w1; ++it_w2; ++it_ccm; ++it_im;
    if (it_w1>=it_w1_end) { //reached the end of frequency bins of first antenna, restart at first bin and advance second antenna by one
      if (it_w2>=it_w2_end) { //the second antenna has reached the last antenna value (last baseline to first antenna), so increase first antenna by one
	it_w1_start+=nfreq; //advance by one antenna (each containing nfreq frequencies), i.e. next baseline
	it_w1_end+=nfreq;
	if (it_w1_end>=it_w2_end) { //has cycled through all antenna pairs/all baselines, restart at beginning and advance to next pixel
	  if (it_im!=it_im_end) {
	    ERROR("hImageFromCCM: Mismatch between image vector, ccm, and weights");
	    return;
	  };
	  if (it_ccm!=ccm_end) {
	    ERROR("Error - hImageFromCCM: Mismatch between ccm and weights");
	    return;
	  };
	  it_im_start=it_im_end;  //first frequency bin of NEXT PIXEL
	  it_im_end=it_im_start+nfreq;
	  it_w1_start=it_w2_end;  // at the end of w2 comes the next sequence fo weights for the next pixel
	  it_w1_end=it_w1_start+nfreq;
	  it_w2_end = it_w1_start+nofAntennas*nfreq;
	  it_ccm=ccm; //ccm begins at first frequency first baseline again
	};
	it_w2=it_w1_end; //start second antenna pair pointer at antenna 1 - start with baseline 0-1
      };
      it_w1=it_w1_start;  //start over at first frequency bin in weights (with next baseline)
      it_im=it_im_start; //start over at first frequency bin in image (with next baseline)
    };
    if (it_im>=it_im_end) {
      ERROR("hImageFromCCM: Mismatch between image vector, ccm, and weights");
      return;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Convert the ADC value to a voltage.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hADC2Voltage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(adc2voltage)()("Scaling factor of the gain")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const HNumber adc2voltage) {
  Iter it = vec;
  while(it != vec_end) {
    *it *= adc2voltage;
    it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Creates one half (i.e. rising or falling part) of a Hanning filter and add an offset
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilterHalf
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(height)()("Height of the Hanning function.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(offset)()("Offset added to the Hanning function.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (bool)(falling)()("Return first (False) or second half (True).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const HNumber height,
		    const HNumber offset,
		    const bool falling
		       ) {
  HInteger width=vec_end-vec;
  CR::HanningFilter<HNumber> hanning_filter(width*2, (double)0.5, 0, (HInteger)width, (HInteger)width);
  Iter it_v(vec);
  CasaVector<HNumber> filter = hanning_filter.weights();
  HNumber * it_f(filter.cbegin());
  if (falling) it_f+=width;
  while (it_v != vec_end) {
    *it_v = (typename Iter::value_type) (*it_f)*height+offset;
    it_v++; it_f++;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(BetaRise)()("Rising slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(BetaFall)()("Falling slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha,
		       const HInteger Beta,
		       const HInteger BetaRise,
		       const HInteger BetaFall) {

  HInteger blocksize = vec_end - vec;
  CR::HanningFilter<HNumber> hanning_filter(blocksize, (double)Alpha, (HInteger)Beta, (HInteger)BetaRise, (HInteger)BetaFall);
  Iter it_v = vec;
  CasaVector<HNumber> filter = hanning_filter.weights();
  CasaVector<HNumber>::iterator it_f = filter.begin();

  while ((it_v != vec_end) && (it_f != filter.end())) {
    *it_v = (IterValueType) *it_f;
    it_v++; it_f++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha,
		       const HInteger Beta) {
  HInteger blocksize = vec_end - vec;
  HInteger BetaRise = (blocksize - Beta)/2;
  HInteger BetaFall = (blocksize - Beta)/2;

  hGetHanningFilter(vec, vec_end, Alpha, Beta, BetaRise, BetaFall);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha) {
  HInteger Beta = 0;
  hGetHanningFilter(vec, vec_end, Alpha, Beta);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end){
  HNumber Alpha = 0.5;
  hGetHanningFilter(vec, vec_end, Alpha);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Apply a predefined filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Vector containing the data on which the filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(filter)()("Vector containing the filter.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class IterFilter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end, const IterFilter filter, IterFilter filter_end){
  Iter       it_d = data;
  IterFilter it_f = filter;

  while ((it_d != data_end) && (it_f != filter_end)) {
    *it_d *= (IterValueType) *it_f;
    it_d++; it_f++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hanning filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Input and return vector containing the data on which the Hanning filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end){
  HInteger blocksize = data_end - data;
  vector<HNumber> filter(blocksize);

  hGetHanningFilter(filter.begin(), filter.end());
  hApplyFilter(data, data_end, filter.begin(), filter.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a forward FFT on a complex vector and return it in a second
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  outvec.fftw(invec) -> return FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

The DFT results are stored in-order in the array out, with the
zero-frequency (DC) component in data_out[0]. If data_in != data_out, the transform
is out-of-place and the input array in is not modified. Otherwise, the
input array is overwritten with the transform.

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by n.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data_out, const Iter data_out_end,
		    const Iter data_in,  const Iter data_in_end)
{
  int N(data_in_end - data_in);
  int Nout(data_out_end - data_out);
  if (N!=Nout) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) <<": input and output vector have different sizes! N=" << N << " Nout=" << Nout);
    return;
  };
  fftw_plan p;
  p = fftw_plan_dft_1d(N, (fftw_complex*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a backward FFT on a complex vector and return it in a second
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!

  outvec.fftw(invec) -> return backward FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

  !!!! Note that the input vector will be modified and scrambled !!!!

The DFT results are stored in-order in the array out, with the
zero-frequency (DC) component in data_out[0]. If data_in != data_out, the transform
is out-of-place and the input array in is not modified. Otherwise, the
input array is overwritten with the transform.

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by n.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data_out, const Iter data_out_end,
		    const Iter data_in,  const Iter data_in_end)
{
  int N(data_in_end - data_in);
  int Nout(data_out_end - data_out);
  if (N!=Nout) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) <<": input and output vector have different sizes! N=" << N << " Nout=" << Nout);
    return;
  };
  fftw_plan p;
  p = fftw_plan_dft_1d(N, (fftw_complex*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a real-to-complex FFT using fftw3
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT (DFT) transformed data is stored. The length is N/2+1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Real input vector of N elements to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fftw(floatvec) -> return FFT of floatvec in complexvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by N.

The size N can be any positive integer, but sizes that are products of
small factors are transformed most efficiently (although prime sizes
still use an O(N log N) algorithm).  The two arguments are input and
output arrays of the transform.  These vectors can be equal,
indicating an in-place transform.

*/
template <class IterOut,class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end)
{
  int N(data_in_end - data_in);
  int Nout(data_out_end - data_out);
  if (Nout != (N/2+1)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": input and output vector have wrong sizes! N(in)=" << N  << " N(out)=" << Nout<< " (should be = " << N/2+1 << ")");
    return;
  };
  fftw_plan p;
  p = fftw_plan_dft_r2c_1d(N, (double*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a complex-to-real (inverse) FFT using fftw3
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector of N real elements in which the inverse FFT (DFT) transformed data is stored. ")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex input vector of The length is N/2+1 elements to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.invfftw(complexvec) -> return (inv) FFT of complexvec in vec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by N.

The size N can be any positive integer, but sizes that are products of
small factors are transformed most efficiently (although prime sizes
still use an O(N log N) algorithm).  The two arguments are input and
output arrays of the transform.  These vectors can be equal,
indicating an in-place transform.

*/
template <class IterOut,class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end)
{
  int Nin(data_in_end - data_in);
  int N(data_out_end - data_out);
  if (Nin != (N/2+1)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": input and output vector have wrong sizes! N(out)=" << N  << " N(in)=" << Nin<< " (should be = " << N/2+1 << ")");
    return;
  };
  fftw_plan p;
  p = fftw_plan_dft_c2r_1d(N, (fftw_complex*) &(*data_in), (double*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Reorders the elements in a complex vector that was created by a FFT according to its Nyquistzone, such that frequencies always increase from left to right
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNyquistSwap
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Complex input and output vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(nyquistZone)()("Nyquist zone. 1: nu= 0 - Nu_max, 2: nu=Nu_max - 2 * Nu_max, etc ...  ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.nyquistswap()

  \brief $DOCSTRING
  $PARDOCSTRING

  The order of the elements in the vector will be reversed and replaced with their negative complex conjugate.

  The operation is only performed for an even Nyquist Zone (2,4,6,..). Otherwise nothing is done.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const HInteger nyquistZone)
{
  if (hfodd(nyquistZone)) return;
  Iter it1(vec),it2(vec_end-1);
  HComplex tmp;
  while (it2 >= it1) {
    tmp=*it1;
    *it1=-conj(*it2);
    *it2=-conj(tmp);
    ++it1; --it2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply an FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTCasa
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fftcasa(floatvec) -> return FFT for floatvec in complexvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end,
		    const HInteger nyquistZone) {
  uint channel;
  uint blocksize(data_in_end - data_in);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,blocksize);
  IPosition shape_out(1,fftLength);
  FFTServer<Double,DComplex> fftserver(shape_in, FFTEnums::REALTOCOMPLEX);

  //  Vector<Double> cvec_in(shape_in, reinterpret_cast<Double*>(&(*data_in)), casa::SHARE);
  Vector<Double> cvec_in(shape_in, 0.);
  Vector<DComplex> cvec_out(shape_out, 0.);

  IterIn it_in = data_in;
  IterOut it_out = data_out;
  Vector<Double>::iterator it_vin=cvec_in.begin();
  // make copy of input vector since fft will also modify the order of the input data.
  while ((it_in != data_in_end) && (it_vin != cvec_in.end())) {
    *it_vin  = *it_in;
    it_in++; it_vin++;
  }

  // Apply FFT

  fftserver.fft(cvec_out, cvec_in);

  // Is there some aftercare needed (checking/setting the size of the in/output vector)
  switch (nyquistZone) {
  case 1:
  case 3:
  case 5:
    {
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
	*it_out = cvec_out(channel);
	it_out++; channel++;
      }
    }
    break;
  case 2:
  case 4:
  case 6:
    {
      /// See datareader for implementation.
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
	*it_out = conj(cvec_out(fftLength - channel - 1));
	it_out++; channel++;
      }
    }
    break;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply an Inverse FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTCasa
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector in which the inverse FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  floatvec.invfftcasa(complexvec) -> return inverse FFT for complexvecvec in floatvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn data_in,   const IterIn data_in_end,
		    const HInteger nyquistZone) {
  uint channel;
  uint blocksize(data_out_end - data_out);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,fftLength);
  IPosition shape_out(1,blocksize);

  Vector<DComplex> cvec_f(shape_in, reinterpret_cast<DComplex*>(&(*data_in)), casa::SHARE);
  Vector<DComplex> cvec_in(fftLength);
  Vector<Double> cvec_out(shape_out, 0.);

  if ((data_in_end - data_in) != (int)fftLength) {
    cerr << "[invfft] Bad input: len(data_in) != fftLength" << endl;
    cerr << "  len(data_in) = " << (data_in_end - data_in) << endl;
    cerr << "  fftLength    = " << fftLength << endl;
  };

  try {
    FFTServer<Double,DComplex> server(shape_out,
				      FFTEnums::REALTOCOMPLEX);
    switch (nyquistZone) {
    case 1:
    case 3:
      for (channel=0; channel<nofChannels; channel++) {
   	cvec_in(channel) = cvec_f(channel);
      }
      break;
    case 2:
      for (channel=0; channel<nofChannels; channel++) {
  	cvec_in(channel) = conj(cvec_f(fftLength - channel - 1));
      }
      break;
    }
    server.fft(cvec_out,cvec_in);

    // Copy result back to data_out
    Vector<Double>::iterator it_vout= cvec_out.begin();
    IterOut it_dout = data_out;
    while ((it_vout != cvec_out.end()) && (it_dout != data_out_end)) {
      *it_dout = *it_vout;
      it_vout++; it_dout++;
    }
  } catch (AipsError x) {
    cerr << "[invfft]" << x.getMesg() << endl;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Apply an FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFT
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fft(floatvec) -> return FFT of floatvec in complexvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end,
		    const HInteger nyquistZone) {

  hFFTCasa(data_out, data_out_end,
	   data_in,  data_in_end,
	   nyquistZone);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Apply an inverse FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFT
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector in which the inverse FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  floatvec.invfft(complexvec) -> return inverse FFT of complexvec in floatvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterIn, class IterOut>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn data_in,   const IterIn data_in_end,
		    const HInteger nyquistZone) {

  hInvFFTCasa(data_out, data_out_end,
	      data_in, data_in_end,
	      nyquistZone);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

// ================================================================================
//$SECTION: GSL Fitting
// ================================================================================

//$COPY_TO HFILE: #ifdef HAVE_GSL
//!Makes a shared memory GSL Matrix. Note that m->block needs to be deleted explicitly afterwards ...
template <class Iter>
gsl_matrix * STL2GSLM(const Iter vec, const HInteger dim1, const HInteger dim2) {
  gsl_matrix * m = new gsl_matrix;
  m->size1 = dim1;
  m->size2 = dim2;
  m->tda = dim2;
  m->data = (double*) &(*vec);
  m->block = NULL;
  //  m->block = new gsl_block;
  //  m->block->data = m->data;
  //  m->block->size=dim1*dim2;
  m->owner = 0;
  return m;
}

template <class Iter>
gsl_vector * STL2GSL(const Iter vec, const Iter vec_end) {
  gsl_vector * m = new gsl_vector;
  m->size = vec_end-vec;
  m->stride = 1;
  m->data = (double*) &(*vec);
  m->block = NULL;
  /*  m->block = new gsl_block;
  m->block->data = m->data;
  m->block->size=m->size;
  */
  m->owner = 0; //owner=0 Means block of memory is not owned by the GSL vector and hence not freed.
  return m;
}

//$DOCSTRING: Calculate the XValues matrix vector for the polynomial (linear-fitting) routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFitPolynomialX
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the x vector. Size is (n+1)*len(xvec), where n=len(powers).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(powers)()("Input vector with the powers to consider in the fitting. For an n-th order polynomial, this is simply [0,1,2,...,n].")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  [x_0,x_1,x_2,...] -> [0,x_0,x_0**2,...,x_0**n,0,x_1,x_1**2,...,x_1**n]

  n is the number of powers of the polynomial to fit,
  i.e. len(powers). The length of the output vector is the length of
  xvec*(n+1).

  \brief $DOCSTRING
  $PARDOCSTRING

See also: LinearFit
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const IterI powers, const IterI powers_end
		       )
{
  Iter itout(vecout), itx(xvec);
  IterI itn(powers);
  if (powers_end<=powers) return;
  if (vecout_end<=vecout) return;
  if (xvec_end<=xvec) return;
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      *itout=1.0;
      break;
    case 1:
      *itout=*itx;
      break;
    case 2:
      *itout=(*itx) * (*itx);
      break;
    default:
      *itout=pow(*itx,(int)*itn);
      break;
    };
    ++itout; ++itn;
    if (itn==powers_end) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers;
      ++itx;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the X-Values matrix vector for the Basis Spline (BSpline) fitting routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFitXValues
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the x vector. Size is Ncoeffs*len(xvec).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xmin)()("Lower end of the x-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(xmax)()("Upper end of the x-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//#define HFPP_PARDEF_2 (HInteger)(bwipointer)()("Pointer to the BSpline workspace returned as Integer number.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(Ncoeffs)()("Number of coefficients to calculate.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const HNumber xmin,
		    const HNumber xmax,
		    //		    const HInteger bwipointer
		    const HInteger Ncoeffs
		       )
{
  ////gsl_bspline_workspace *bw=(gsl_bspline_workspace*)bwipointer;
  ////const HInteger Ncoeffs=bw->n;      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  Iter itout(vecout), itout_end(vecout_end-Ncoeffs+1), itx(xvec);
  gsl_bspline_workspace *bw=gsl_bspline_alloc(4, Ncoeffs-2);
  //  gsl_bspline_knots_uniform(*xvec, *(xvec_end-1), bw);
  gsl_bspline_knots_uniform(xmin, xmax, bw);
  //  gsl_vector *B = gsl_vector_alloc(Ncoeffs);
  gsl_vector *B = STL2GSL(itout, itout+Ncoeffs);
  //  HInteger i;
  //HNumber Bj;
  if (vecout_end<=vecout) return;
  if (xvec_end<=xvec) return;
  while ((itout < itout_end) && (itx != xvec_end)) {
    //    B = STL2GSL(itout, itout+Ncoeffs);
    gsl_bspline_eval(*itx, B, bw);
    // for (i=0; i<Ncoeffs; i++) {
    //   Bj=gsl_vector_get(B, i);
    //   *(itout+i)=Bj;
    // };
    itout+=Ncoeffs; ++itx; B->data=&(*itout);
    //Bj=gsl_vector_get(B, j)1    delete B;
  };
  gsl_bspline_free(bw);
  //  gsl_vector_free(B);
  delete B;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the y-values of the results of a Basis Spline fit
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSpline
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the y-values for the input vector x-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector of Ndata*Ncoeffs X-values produced with hBSplineFitXValues. Size is Ncoeffs*len(xvec).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeffs)()("Input vector with the coefficients calculated by the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also:  BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeffs, const Iter coeffs_end
		    )
{
  HInteger Ncoeffs(coeffs_end-coeffs);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  Iter itout(vecout), itx(xvec), itx_end(itx+Ncoeffs);
  Iter coeffs2(coeffs),  coeffs_end2(coeffs_end2);
  if (vecout_end<=vecout) return;
  if (xvec_end<=xvec) return;
  while ((itout < vecout_end) && (itx_end < xvec_end)) {
    *itout=hMulSum(itx,itx_end,coeffs2,coeffs_end2);
    //    *itout=hMulSum(&(*itx),&(*itx_end),&(*coeffs2),&(*coeffs_end2));
    //    hMulSum(itx,itx_end,coeffs2,coeffs_end2,*itout);
    ++itout; itx=itx_end; itx_end+=Ncoeffs;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a polynomial and adds it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPolynomial
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the polynomial y-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeff)()("Coefficients for each  power in powers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(powers)()("Input vector with the powers to take into account. For an n-th order polynomial, this is simply [0,1,2,...,n].")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  xvec=[x_0,x_1,x_2,...]
  coeff=[C_0,C_1,C_2,...] -> [sum(C_0,C_1*x_0,C_2*x_0**2),...,sum(C_0,C_1*x_1,C_2*x_1**2,...,C_n*x_1**n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  Note that the polynomical is added to the output vector, so you need
  to initialize the output vector to zero!

See also: LinearFit, hLinearFitPolynomialX
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeff, const Iter coeff_end,
		    const IterI powers, const IterI powers_end
		       )
{
  Iter itout(vecout), itx(xvec);
  IterI itn(powers); Iter itc(coeff);
  if (powers_end<=powers) return;
  if (vecout_end<=vecout) return;
  if (xvec_end<=xvec) return;
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      *itout+=*itc;
      break;
    case 1:
      *itout+=*itc * *itx;
      break;
    case 2:
      *itout+=*itc * (*itx) * (*itx);
      break;
    default:
      *itout+=*itc * pow(*itx,(int)*itn);
      break;
    };
    ++itn; ++itc;
    if ((itn==powers_end )|| (itc==coeff_end)) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers; itc=coeff;
      ++itx; ++itout;
    };
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates a weight factor from an error for a fitting routine, i.e. w=1/Error**2.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hErrorsToWeights
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the weights.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecin)()("Input vector with errorvalues.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
[x_0,x_1,x_2,...] -> [1/x_0**2,1/x_1**2,...]

See also: hLinearFit, hLinearFitPolynomialX, hPolynomial
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter vecin, const Iter vecin_end
		    )
{
  Iter itout(vecout), itin(vecin);
  while ((itout != vecout_end) && (itin != vecin_end)) {
    *itout = 1/((*itin) * (*itin));
    ++itout; ++itin;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set provided a vector of weights and return the coefficients and covariance matrix in two vectors. Returns as function value the chi-square value of the fit.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the x values of the data, where each value is actually one row of (n+1) values of x**n (e.g., if x values are [2,3] => xvec=[0,2,4,0,3,9] for a quadratic polynomical (n=2)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(wvec)()("Vector containing the weights of the data (which are 1/Error^2).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HInteger)(ndata)()("Number of data points to take into account (ndata=-1 -> take all elements in yvec, if ndata>0 only take the first ndata). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: hLinearFitPolynomialX,hPolynomial
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end,
		    const Iter wvec, const Iter wvec_end,
		    const HInteger ndata
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeff(vecout_end-vecout);
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  if ((wvec_end-wvec)!=Ndata) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": weights vector has wrong size! len(wvec)=" << (wvec_end-wvec)  <<  ", should be = " << Ndata << ". Ndata=" << Ndata);
    return -1.0;
  };
  if ((xvec_end-xvec)!=(Ndata*Ncoeff)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xvec)=" << (xvec_end-xvec)  <<  ", should be = " << Ndata*Ncoeff << ". Ndata=" << Ndata << ", Ncoeff=" << Ncoeff);
    return -1.0;
  };
  if ((veccov_end-veccov)!=(Ncoeff*Ncoeff)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeff*Ncoeff << ". Ncoeff=" << Ncoeff);
    return -1.0;
  };

  if (ndata>0) Ndata=min(ndata,Ndata);

  X = STL2GSLM(xvec, Ndata, Ncoeff);
  y = STL2GSL(yvec, yvec+Ndata);
  w = STL2GSL(wvec, wvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeff, Ncoeff);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeff);
  error=gsl_multifit_wlinear (X, w, y, c, cov,&chisq, work);
  gsl_multifit_linear_free (work);
  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  //Make sure to delete all GSL vector and matrix structures - this will not
  delete X;
  delete cov;
  delete y;
  delete w;
  delete c;
  return chisq;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set (without weights) and return the coefficients and covariance matrix in two vectors. Returns as function value the chi-square value of the fit.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the x values of the data, where each value is actually one row of (n+1) values of x**n (e.g., if x values are [2,3] => xvec=[0,2,4,0,3,9] for a quadratic polynomical (n=2)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(ndata)()("Number of data points to take into account (ndata=-1 -> take all elements in yvec, if ndata>0 only take the first ndata). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: hLinearFitPolynomialX,hPolynomial
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end,
		    const HInteger ndata
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeff(vecout_end-vecout);
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *c;
  if ((xvec_end-xvec)!=(Ndata*Ncoeff)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xvec)=" << (xvec_end-xvec)  <<  ", should be = " << Ndata*Ncoeff << ". Ndata=" << Ndata << ", Ncoeff=" << Ncoeff);
    return -1.0;
  };
  if ((veccov_end-veccov)!=(Ncoeff*Ncoeff)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeff*Ncoeff << ". Ncoeff=" << Ncoeff);
    return -1.0;
  };

  if (ndata>0) Ndata=min(ndata,Ndata);

  X = STL2GSLM(xvec, Ndata, Ncoeff);
  y = STL2GSL(yvec, yvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeff, Ncoeff);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeff);
  error=gsl_multifit_linear (X, y, c, cov,&chisq, work);

  gsl_multifit_linear_free (work);

  delete X;
  delete cov;
  delete y;
  delete c;

  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  return chisq;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Do a basis spline fit to a data set (without weights) and return the coefficients and covariance matrix in two vectors. Returns the chi-square value of the fit.
// Returns as function value a pointer to the bspline workspace that is needed by other functions.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xmatrix)()("Output vector containing the x values of the data, where each value is actually one row of ncoeff values some power of x. Calculated with hBSplineFitXValues.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(xvec)()("Vector containing the x values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//#define HFPP_PARDEF_2 (HInteger)(bwipointer)()("Pointer to the BSpline workspace returned as Integer number.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also:  BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xmatrix, const Iter xmatrix_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeffs(vecout_end-vecout);
  HInteger Nbreak(Ncoeffs-2);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *c;


  if ((xmatrix_end-xmatrix)!=(Ndata*Ncoeffs)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xmatrix)=" << (xmatrix_end-xmatrix)  <<  ", should be = " << Ndata*Ncoeffs << ". Ndata=" << Ndata << ", Ncoeffs=" << Ncoeffs);
    return -1.0;
  };
  if ((veccov_end-veccov)!=(Ncoeffs*Ncoeffs)) {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeffs*Ncoeffs << ". Ncoeffs=" << Ncoeffs);
    return -1.0;
  };

  gsl_bspline_workspace *bw=gsl_bspline_alloc(4, Nbreak);

  X = STL2GSLM(xmatrix, Ndata, Ncoeffs);
  y = STL2GSL(yvec, yvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeffs, Ncoeffs);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeffs);
  gsl_bspline_knots_uniform(*xvec, *(xvec_end-1), bw);
  //  hBSplineFitXValues(xmatrix,xmatrix_end,xvec,xvec_end,(HInteger)bw);
  hBSplineFitXValues(xmatrix,xmatrix_end,xvec,xvec_end,*xvec,*(xvec_end-1),Ncoeffs);
  error=gsl_multifit_linear(X, y, c, cov, &chisq, work);
  gsl_multifit_linear_free (work);

  gsl_bspline_free(bw);

  delete X;
  delete cov;
  delete y;
  delete c;

  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  return chisq;
  //  return (HInteger) bw;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$COPY_TO HFILE: #endif

// ================================================================================
//$SECTION: RFI mitigation
// ================================================================================


// ___________________________________________________________________
//                                                    hRFIDownsampling

//$DOCSTRING: Generate a downsampled vector containing the mean and rms values of the spectrum amplitudes. This is needed to generate a baseline for the RFI mitigation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIDownsampling
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(amplitudeVec)()("Return vector containing the average value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(rmsVec)()("Return vector containing the root mean square value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(spectrumVec)()("Vector containing the amplitude of the spectrum data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (Iter amplitudeVec, Iter amplitudeVec_end,
		     Iter rmsVec, Iter rmsVec_end,
		     const Iter spectrumVec, const Iter spectrumVec_end
		     ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;
  uint resampledSize = amplitudeVec_end - amplitudeVec;

  uint amplitudeVecSize = amplitudeVec_end - amplitudeVec;
  uint rmsVecSize = rmsVec_end - rmsVec;
  if (amplitudeVecSize != rmsVecSize) {
    cout << "ERROR: Output vectors are not of the specified size." << endl;
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1, fullSize);
  IPosition shape_ResampledSize (1, resampledSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Double> amplitudeVector(shape_ResampledSize, reinterpret_cast<Double*>(&(*amplitudeVec)), casa::SHARE);
  Vector<Double> rmsVector(shape_ResampledSize, reinterpret_cast<Double*>(&(*rmsVec)), casa::SHARE);

  // === Call RM::doDownSampling() ===========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doDownsampling(spectrumVector,
			       resampledSize,
			       amplitudeVector,
			       rmsVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



// ___________________________________________________________________
//                                                 hRFIBaselineFitting

//$DOCSTRING: Perform a baseline fitting on the amplitude of the spectrum vector and use the fit to create an interpolated spectrum.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIBaselineFitting
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(fitVec)()("Output vector containing the baseline fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(amplitudeVec)()("Input vector containing the average value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(rmsVec)()("Input vector containing the root mean square value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(rmsThresholdValue)()("RMS values above this threshold value will be excluded from the fit.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hRFIBaselineFitting(Iter fitVec, Iter fitVec_end,
			 const Iter amplitudeVec, const Iter amplitudeVec_end,
			 const Iter rmsVec, const Iter rmsVec_end,
			 const HNumber rmsThresholdValue
			 ) {
  CR::RFIMitigation rfiMitigation;
  int fitMethodType = 0; // Enumerator for different fit methods will be added

  // === Check vector size ===================================
  uint fullSize = fitVec_end - fitVec;
  uint resampledSize = amplitudeVec_end - amplitudeVec;

  uint amplitudeVecSize = amplitudeVec_end - amplitudeVec;
  uint rmsVecSize = rmsVec_end - rmsVec;
  if (amplitudeVecSize != rmsVecSize) {
    cout << "ERROR: Input vectors are not of the same size." << endl;
    exit(0);
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  IPosition shape_resampledsize(1,resampledSize);
  Vector<Double> amplitudeVector(shape_resampledsize, reinterpret_cast<Double*>(&(*amplitudeVec)), casa::SHARE);
  Vector<Double> rmsVector(shape_resampledsize, reinterpret_cast<Double*>(&(*rmsVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);

  // === call RM::doBaselineFitting() ========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doBaselineFitting(amplitudeVector,
				  rmsVector,
				  rmsThresholdValue,
				  fitVector,
				  fitMethodType);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// ___________________________________________________________________
//                                                        hRFIFlagging

//$DOCSTRING: Create a flag vector defining the regions that need to be mitigated in the spectrum.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIFlagging
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(flagVec)()("Vector describing which parts of the spectrum vector need to be mitigated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(flagThresholdValue)()("Value of the threshold above which a frequency bin will be flagged.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(spectrumVec)()("Input vector containing the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(fitVec)()("Input vector containing the baseline fit of the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterN, class IterI>
void hRFIFlagging(IterI flagVec, IterI flagVec_end,
		  const HNumber flagThresholdValue,
		  const IterN spectrumVec, const IterN spectrumVec_end,
		  const IterN fitVec, const IterN fitVec_end
		  ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;

  uint fitVecSize = fitVec_end - fitVec;
  uint flagVecSize = flagVec_end - flagVec;
  if ((fitVecSize != fullSize) ||
      (flagVecSize != fullSize)) {
    cout << "ERROR: Input vectors are not of the same size." << endl;
    exit(0);
  }

  // === create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);
  Vector<Int> flagVector(shape_FullSize, reinterpret_cast<Int*>(&(*flagVec)), casa::SHARE);

  // === call RM::doFlagging() ===============================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doRFIFlagging(spectrumVector,
			      fitVector,
			      flagThresholdValue,
			      flagVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



// ___________________________________________________________________
//                                                      hRFIMitigation

//$DOCSTRING: Final part of the RFI mitigation: remove the flagged regions from the spectrum
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIMitigation
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(mitigatedSpectrumVec)()("Output vector containing the mitigated spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(spectrumVec)()("Input vector containing the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(fitVec)()("Input vector containing the baseline fit of the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(flagVec)()("Input vector describing which parts of the spectrum vector need to be mitigated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterN, class IterI>
void hRFIMitigation(IterN mitigatedSpectrumVec, IterN mitigatedSpectrumVec_end,
		    const IterN spectrumVec, const IterN spectrumVec_end,
		    const IterN fitVec, const IterN fitVec_end,
		    const IterI flagVec, const IterI flagVec_end
		    ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;

  uint fitVecSize = fitVec_end - fitVec;
  uint flagVecSize = flagVec_end - flagVec;
  uint mitigatedSpectrumVecSize = mitigatedSpectrumVec_end - mitigatedSpectrumVec;

  if ((fitVecSize != fullSize) ||
      (flagVecSize != fullSize) ||
      (mitigatedSpectrumVecSize != fullSize)) {
    cout << "ERROR: Input and output vectors are not of the same size." << endl;
    exit(0);
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Int> flagVector(shape_FullSize, reinterpret_cast<Int*>(&(*flagVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);
  Vector<Double> mitigatedSpectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*mitigatedSpectrumVec)), casa::SHARE);

  // === call RM::doRFIMitigation() ==========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doRFIMitigation(spectrumVector,
				fitVector,
				flagVector,
				mitigatedSpectrumVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//========================================================================
//$SECTION:              I/O Function (DataReader)
//========================================================================

//$DOCSTRING: Print a brief summary of the file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("DataReader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

void HFPP_FUNC_NAME(CRDataReader & dr) {
  dr.summary();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Function to open a file based on a filename and returning a datareader object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileOpen
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (CRDataReader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to opwn including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

CRDataReader & HFPP_FUNC_NAME(HString Filename) {

  bool opened;

  //Create the a pointer to the DataReader object and store the pointer
  CR::DataReader* drp;
  HString Filetype = hgetFiletype(Filename);

  if (Filetype=="LOPESEvent") {
    CR::LopesEventIn* lep = new CR::LopesEventIn;
    opened=lep->attachFile(Filename);
    drp=lep;
    cout << "Opening LOPES File="<<Filename<<endl; drp->summary();
  } else if (Filetype=="LOFAR_TBB") {
    drp = new CR::LOFAR_TBB(Filename,1024);
    opened=drp!=NULL;
    cout << "Opening LOFAR File="<<Filename<<endl; drp->summary();
  } else {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Unknown Filetype = " << Filetype  << ", Filename=" << Filename);
    opened=false;
  }
  if (!opened){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Opening file " << Filename << " failed.");
    CR::LopesEventIn* lep = new CR::LopesEventIn; //Make a dummy data reader ....
    drp=lep;
  };
  return *drp;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return information from a data file as a Python object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileGetParameter
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HPyObject)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object openen, e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword ro be read out from the file metadata")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HPyObject HFPP_FUNC_NAME(CRDataReader &dr, HString key)
{
  HString key2(key);
  key2[0]=(unsigned char)toupper((int)key2[0]);
  key[0]=(unsigned char)tolower((int)key[0]);
  DataReader *drp=&dr;
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if ((key== #KEY) || (key2== #KEY)) {_H_NL_ TYPE result(drp->KEY ()); _H_NL_ HPyObject pyob((TYPE2)result); _H_NL_ return pyob;} else
  HFPP_REPEAT(uint,uint,nofAntennas)
    HFPP_REPEAT(uint,uint,nofSelectedChannels)
    HFPP_REPEAT(uint,uint,nofSelectedAntennas)
    HFPP_REPEAT(uint,uint,blocksize)
    HFPP_REPEAT(uint,uint,fftLength)
    HFPP_REPEAT(uint,uint,block)
    HFPP_REPEAT(uint,uint,stride)
    HFPP_REPEAT(uint,uint,nyquistZone)
    HFPP_REPEAT(double,double,sampleInterval)
    HFPP_REPEAT(double,double,referenceTime)
    HFPP_REPEAT(double,double,sampleFrequency)
    HFPP_REPEAT(uint,uint,nofBaselines)
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if ((key== #KEY) || (key2== #KEY)) {_H_NL_ CasaVector<TYPE> casavec(drp->KEY ()); _H_NL_ std::vector<TYPE2> result; _H_NL_ aipsvec2stlvec(casavec,result); _H_NL_ HPyObject pyob(result); _H_NL_ return pyob;} else
    HFPP_REPEAT(uint,HInteger,antennas)
    HFPP_REPEAT(uint,HInteger,selectedAntennas)
    HFPP_REPEAT(uint,HInteger,selectedChannels)
    HFPP_REPEAT(uint,HInteger,positions)
    HFPP_REPEAT(double,HNumber,increment)
    HFPP_REPEAT(double,HNumber,frequencyValues)
    HFPP_REPEAT(double,HNumber,frequencyRange)
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if ((key== #KEY) || (key2== #KEY)) {_H_NL_ TYPE result;  _H_NL_ drp->headerRecord().get(#KEY,result); _H_NL_ HPyObject pyob((TYPE2)result); _H_NL_ return pyob;} else
      HFPP_REPEAT(uint,uint,Date)
	HFPP_REPEAT(casa::String,HString,Observatory)
	HFPP_REPEAT(int,int,Filesize)
	//	HFPP_REPEAT(double,double,dDate)
	HFPP_REPEAT(int,int,presync)
	HFPP_REPEAT(int,int,TL)
	HFPP_REPEAT(int,int,LTL)
	HFPP_REPEAT(int,int,EventClass)
	HFPP_REPEAT(casa::uChar,uint,SampleFreq)
	HFPP_REPEAT(uint,uint,StartSample)
#undef HFPP_REPEAT
	if ((key== "AntennaIDs") || (key2== "AntennaIDs")) {_H_NL_ CasaVector<int> casavec; _H_NL_ drp->headerRecord().get("AntennaIDs",casavec); _H_NL_ std::vector<HInteger> result; _H_NL_ aipsvec2stlvec(casavec,result); _H_NL_ HPyObject pyob(result); _H_NL_ return pyob;} else
    { HString result; result = result
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  + #KEY + ", "
  HFPP_REPEAT(uint,uint,nofAntennas)
    HFPP_REPEAT(uint,uint,nofSelectedChannels)
    HFPP_REPEAT(uint,uint,nofSelectedAntennas)
    HFPP_REPEAT(uint,uint,nofBaselines)
    HFPP_REPEAT(uint,uint,block)
    HFPP_REPEAT(uint,uint,blocksize)
    HFPP_REPEAT(uint,uint,stride)
    HFPP_REPEAT(uint,uint,fftLength)
    HFPP_REPEAT(uint,uint,nyquistZone)
    HFPP_REPEAT(double,double,sampleInterval)
    HFPP_REPEAT(double,double,referenceTime)
    HFPP_REPEAT(double,double,sampleFrequency)

    HFPP_REPEAT(uint,HInteger,antennas)
    HFPP_REPEAT(uint,HInteger,selectedAntennas)
    HFPP_REPEAT(uint,HInteger,selectedChannels)
    HFPP_REPEAT(uint,HInteger,positions)
    HFPP_REPEAT(double,HNumber,increment)
    HFPP_REPEAT(double,HNumber,frequencyValues)
    HFPP_REPEAT(double,HNumber,frequencyRange)

      HFPP_REPEAT(uint,uint,Date)
	HFPP_REPEAT(casa::String,HString,Observatory)
	HFPP_REPEAT(int,int,Filesize)
			//HFPP_REPEAT(double,double,dDate)
	HFPP_REPEAT(int,int,presync)
	HFPP_REPEAT(int,int,TL)
	HFPP_REPEAT(int,int,LTL)
	HFPP_REPEAT(int,int,EventClass)
	HFPP_REPEAT(casa::uChar,uint,SampleFreq)
	HFPP_REPEAT(uint,uint,StartSample)
//------------------------------------------------------------------------
	HFPP_REPEAT(int,int,AntennaIDs)
#undef HFPP_REPEAT
		     + "keywords, help";
      if ((key!="help") && (key!="keywords")) cout << "Unknown keyword " << key <<"!"<<endl;
      if (key!="keywords") cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< result <<endl;
      HPyObject pyob(result);
      return pyob;
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set parameters in a data file with a Python object as input.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileSetParameter
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object openen, e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be set in the file")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HPyObjectPtr)(pyob)()("Input paramter")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
////$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
bool HFPP_FUNC_NAME(CRDataReader &dr, HString key, HPyObjectPtr pyob)
{
  DataReader *drp=&dr;
  HString key2(key);
  key2[0]=(unsigned char)toupper((int)key2[0]);
  key[0]=(unsigned char)tolower((int)key[0]);
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if ((key== #KEY) || (key2==#KEY)) {TYPE input(TYPE2 (pyob)); drp->set##KEY (input);} else
  HFPP_REPEAT(uint,PyInt_AsLong,Block)
    HFPP_REPEAT(uint,PyInt_AsLong,Blocksize)
    HFPP_REPEAT(uint,PyInt_AsLong,StartBlock)
    HFPP_REPEAT(uint,PyInt_AsLong,Stride)
    HFPP_REPEAT(uint,PyInt_AsLong,SampleOffset)
    HFPP_REPEAT(uint,PyInt_AsLong,NyquistZone)
    HFPP_REPEAT(double,PyFloat_AsDouble,ReferenceTime)
    HFPP_REPEAT(double,PyFloat_AsDouble,SampleFrequency)
    HFPP_REPEAT(int,PyInt_AsLong,Shift)
    if ((key=="selectedAntennas") || (key2=="SelectedAntennas")) {
      vector<uint> stlvec(PyList2STLuIntVec(pyob));
      uint * storage = &(stlvec[0]);
      casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
      CasaVector<uint> casavec(shape,storage,casa::SHARE);
      drp->setSelectedAntennas(casavec);
    } else
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  + #KEY + ", "
    { HString txt; txt = txt
  HFPP_REPEAT(uint,PyInt_AsLong,Blocksize)
    HFPP_REPEAT(uint,PyInt_AsLong,StartBlock)
    HFPP_REPEAT(uint,PyInt_AsLong,Block)
    HFPP_REPEAT(uint,PyInt_AsLong,Stride)
    HFPP_REPEAT(uint,PyInt_AsLong,SampleOffset)
    HFPP_REPEAT(uint,PyInt_AsLong,NyquistZone)
    HFPP_REPEAT(double,PyFloat_AsDouble,ReferenceTime)
    HFPP_REPEAT(double,PyFloat_AsDouble,SampleFrequency)
    HFPP_REPEAT(int,PyInt_AsLong,Shift)
    HFPP_REPEAT(uint,XX,SelectedAntennas)
#undef HFPP_REPEAT
		     + "help";
      if (key!="help") {
	cout << "Unknown keyword " << key <<"!"<<endl;
	return false;
      };
      cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< txt <<endl;
    };
  return true;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileRead
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_hARRAY  // Additional C++ wrapper to generate - STL is not needed, since this is already STL
#define HFPP_PYTHON_WRAPPER_CLASSES HFPP_CLASS_hARRAY HFPP_CLASS_STL //expose STL and hARRAY classes to python
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNC_KEEP_RETURN_TYPE_FIXED HFPP_TRUE //return a single DataReader object and not a vector thereof for array operations
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 2 // Use the third parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (CRDataReader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (CRDataReader)(dr)()("Datareader object, opened e.g. with hFileOpen or crfile.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example on how to use this with the Python wrapper

file=hFileOpen("data/lofar/RS307C-readfullsecond.h5")
file=hFileOpen("/Users/falcke/LOFAR/usg/data/lopes/test.event")

#offsets=IntVec()
idata=IntVec()
hReadFile(file,"Fx",idata)
hCloseFile(file)

The data will then be in the vector idata. You can covert that to a
Python list with [].extend(idata)
*/

template <class T>
CRDataReader & HFPP_FUNC_NAME(
		    CRDataReader &dr,
		    HString Datatype,
		    std::vector<T> & vec
		    )
{

  //Create a DataReader Pointer from an interger variable
  DataReader *drp=&dr;

  //Check whether it is non-NULL.
  if (drp==Null_p){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": pointer to FileObject is NULL, DataReader not found.");
    return dr;
  };

  //------TIME------------------------------
  if (Datatype=="Time") {
    if (typeid(vec) == typeid(vector<double>)) {
      std::vector<double> * vec_p;
      vec_p=reinterpret_cast<vector<double>*>(&vec); //That is just a trick to fool the compiler
      drp->timeValues(*vec_p);
    }  else {
      cout << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl;
    };
  //------FREQUENCY------------------------------
  } else if (Datatype=="Frequency") {
    if (typeid(vec) == typeid(vector<double>)) {
    CasaVector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
    }  else {
      cout << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl;
    };
  }
//..........................................................................................
//Conversion from aips to stl using shared memory space
//..........................................................................................
#define HFPP_REPEAT(TYPESTL,TYPECASA,FIELD,SIZE)				\
  if (typeid(vec)==typeid(std::vector<TYPESTL>)) {	_H_NL_			\
    casa::IPosition shape(2);				_H_NL_		\
    shape(0)=drp->SIZE (); shape(1)=drp->nofSelectedAntennas();	_H_NL_\
    casa::Matrix<TYPECASA> casamtrx(shape,reinterpret_cast<TYPECASA*>(&(vec[0])),casa::SHARE); _H_NL_\
    drp->FIELD (casamtrx);						_H_NL_\
  } else {								_H_NL_\
    cout << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Datatype " << typeid(vec).name() << " not supported for data field = " << Datatype << "." <<endl; _H_NL_\
  }
//..........................................................................................

  //------FX------------------------------
  else if (Datatype=="Fx") {HFPP_REPEAT(HNumber,double,fx,blocksize);}
  //------VOLTAGE------------------------------
  else if (Datatype=="Voltage") {HFPP_REPEAT(HNumber,double,voltage,blocksize);}
  //------FFT------------------------------
  else if (Datatype=="FFT") {HFPP_REPEAT(HComplex,CasaComplex,fft,fftLength);}
  //------CALFFT------------------------------
  else if (Datatype=="CalFFT") {HFPP_REPEAT(HComplex,CasaComplex,calfft,fftLength);}
  else {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Datatype=" << Datatype << " is unknown.");
    vec.clear();
  };
  return dr;
}
#undef HFPP_REPEAT
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return a list of antenna positions from the CalTables - this is a test.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalTable
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HPyObjectPtr)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HString)(filename)()("Filename of the caltable")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be read out from the file metadata (currenly only Position is implemented)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(date)()("Date for which the information is requested")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HPyObjectPtr)(pyob)()("(Python) List  with antenna IDs")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example:
antennaIDs=hFileGetParameter(file,"AntennaIDs")
x=hCalTable("~/LOFAR/usg/data/lopes/LOPES-CalTable","Positions",obsdate,list(antennaIDs))

*/
HPyObjectPtr HFPP_FUNC_NAME(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  Double tmpval;
  HPyObjectPtr list=PyList_New(0),tuple;
  if (CTRead != NULL && PyList_Check(pyob)) {  //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){  //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i));  //Get the ith element of the list, i.e. the antenna ID
      CTRead->GetData((uint)date, ant, keyword, &tmpvec);
      if (keyword=="Position") {
	tuple=PyTuple_Pack(3,PyFloat_FromDouble(tmpvec[0]),PyFloat_FromDouble(tmpvec[1]),PyFloat_FromDouble(tmpvec[2]));
      } else if (keyword=="Delay") {
	tuple=PyTuple_Pack(1,PyFloat_FromDouble(tmpval));
      };
      PyList_Append(list,tuple);
    };
  };
  delete CTRead;
  return list;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return a list of antenna calibration values from the CalTables as a float vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalTableVector
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HString)(filename)()("Filename of the caltable")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be read out from the file metadata (currenly only Position is implemented)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(date)()("Date for which the information is requested (I guess Julian days? As output from DataReader)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HPyObjectPtr)(pyob)()("(Python) List  with antenna IDs")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

 possible keywords: Positions, Delay


Example:
antennaIDs=hFileGetParameter(file,"AntennaIDs")
vec=hCalTablePositions("~/LOFAR/usg/data/lopes/LOPES-CalTable",obsdate,list(antennaIDs))

*/
vector<HNumber> HFPP_FUNC_NAME(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  /*  cout << "summary:" << CTRead.summary() << endl;
  uint t = time();
  cout << "current time:" << t << endl;
  */
  vector<HNumber> outvec;
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  Double tmpval;
  if (CTRead != NULL && PyList_Check(pyob)) {  //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){  //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i));  //Get the ith element of the list, i.e. the antenna ID
      if (keyword=="Position") {
	CTRead->GetData((uint)date, ant, keyword, &tmpvec);
	outvec.push_back(tmpvec[1]); outvec.push_back(tmpvec[0]); outvec.push_back(tmpvec[2]);
      } else if (keyword=="Delay") {
	CTRead->GetData((uint)date, ant, keyword, &tmpval);
	outvec.push_back(tmpval);
      };
    };
  };
  delete CTRead;
  return outvec;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Dump a single vector to a file in binary format (machine dependent)
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteDump
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write file to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  vec.writedump(filename) -> dumps vector to file

Related functions:
  hReadDump, hWriteDump

Python Example:
  >>> hWriteFileBinaryVector
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinaryVector(v,"test.dat")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinaryVector(x,"test.dat")
  >>> x
  Vec(float,10)=[3.0,3.0,3.0,3.0,3.0,...]


*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::out | ios::binary);
  outfile.write(v1, (HInteger)(v2-v1));
  outfile.close();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Read a single vector from a file which was dumped in binary format
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadDump
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write File to. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.readdump(filename) -> reads dumped vector from file

  \brief $DOCSTRING
  $PARDOCSTRING

Related functions:
  hReadDump, hWriteDump

Python Example:
  >>> hWriteFileBinaryVector
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinaryVector(v,"test.dat")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinaryVector(x,"test.dat")
  >>> x
  Vec(float,10)=[3.0,3.0,3.0,3.0,3.0,...]

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::in | ios::binary);
  outfile.read(v1, (HInteger)(v2-v1));
  outfile.close();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//
//========================================================================
//$SECTION:      Coordinate Conversion (VectorConversion.cc)
//========================================================================


//$DOCSTRING: Converts a 3D spatial vector into a different Coordinate type (e.g. Spherical to Cartesian).
//$ORIGIN: Math/VectorConversion.cc
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCoordinateConvert
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(source)()("Coordinates of the source to be converted - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (CRCoordinateType)(sourceCoordinate)()("Type of the coordinates for the source")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(target)()("Coordinates of the source to be converted - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (CRCoordinateType)(targetCoordinate)()("Type of the coordinates for the target (output vector)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (bool)(anglesInDegrees)()("True if the angles are in degree, otherwise in radians")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Available Coordinate Types are:

      -  Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      AzElHeight,
      -  Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      AzElRadius,
      -  Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      Cartesian,
      -  Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      Cylindrical,
      -  Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      Direction,
      -  Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      DirectionRadius,
      -  Frquency
      Frequency,
      -  Longitude-Latitude-Radius
      LongLatRadius,
      -  North-East-Height
      NorthEastHeight,
      -  Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      Spherical,
      - Time
      Time
*/

template <class Iter>
bool HFPP_FUNC_NAME  (Iter source,
		    CR::CoordinateType::Types const &sourceCoordinate,
		    Iter target,
		    CR::CoordinateType::Types const &targetCoordinate,
		    bool anglesInDegrees
		    )
{
  return CR::convertVector(
			   *target,
			   *(target+1),
			   *(target+2),
			   targetCoordinate,
			   *source,
			   *(source+1),
			   *(source+2),
			   sourceCoordinate,
			   anglesInDegrees
			       );
    }
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//========================================================================================
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//========================================================================================

//$DOCSTRING: Read data from a Datareader object (pointer in iptr) into a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadFileOld
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HIntPointer)(iptr)()("Integer containing pointer to the datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(Antenna)()("Antenna number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(Blocksize)()("Length of the data block to read")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(Block)()("Block number to read")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HInteger)(Stride)()("Stride between blocks")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HInteger)(Shift)()("Shift start of first block by n samples")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//#define HFPP_PARDEF_8 (HInteger)(Offsets)()("Offsets per antenna")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------

/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example on how to use this with the Python wrapper

file=hOpenFile("data/lofar/RS307C-readfullsecond.h5")
file=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/test.event")

#offsets=IntVec()
data=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,file,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)
hCloseFile(file)

The data will then be in the vector idata. You can covert that to a
Python list with [].extend(idata)
*/

template <class T>
void HFPP_FUNC_NAME(std::vector<T> & vec,
		    HIntPointer iptr,
		    HString Datatype,
		    HInteger Antenna,
		    HInteger Blocksize,
		    HInteger Block,
		    HInteger Stride,
		    HInteger Shift)
//	       std::vector<HInteger> & Offsets)
{

  DataReader *drp=reinterpret_cast<DataReader*>(iptr);

  //First retrieve the pointer to the pointer to the dataRead and check whether it is non-NULL.
  if (drp==Null_p){
    ERROR("dataRead: pointer to FileObject is NULL, DataReader not found.");
    return;
  };

//!!!One Needs to verify somehow that the parameters make sense !!!
  if (Antenna > static_cast<HInteger>(drp->nofAntennas()-1)) {
    ERROR("Requested Antenna number too large!");
    return;
  };
  drp->setBlocksize(Blocksize);
  drp->setBlock(Block);
  drp->setStride(Stride);
  drp->setShift(Shift);

  CasaVector<uint> antennas(1,Antenna);
  drp->setSelectedAntennas(antennas);

  address ncol;

  #define copy_ary2vp  ncol=ary.ncolumn(); /* MSG("ncol="<<ncol<<", Antenna="<<Antenna); */ if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);


  if (Datatype=="Time") {
    if (typeid(vec) == typeid(vector<double>)) {
      std::vector<double> * vec_p;
      vec_p=reinterpret_cast<vector<double>*>(&vec); //That is just a tr
      drp->timeValues(*vec_p);
    }  else {
      std::vector<double> tmpvec;
      drp->timeValues(tmpvec);
      hConvert(vec,tmpvec);
    };
  }
  else if (Datatype=="Frequency") {
    //vector<HNumber>* vp2; *vp2 = drp->frequencyValues().tovec();
    //    std::vector<double> vals;
    //copycast_vec(vals,vec);
    CasaVector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
  }
  else if (Datatype=="Fx") {
//hf #define MAKE_UNION__MATRIX_AND_STLVEC(TYPECASA,STLVEC,CASAVEC) \
//     casa::IPosition shape(2,STLVEC.size()); shape(1)=1;	\
//     casa::Matrix<TYPECASA> CASAVEC(shape,reinterpret_cast<TYPECASA*>(&(STLVEC[0])),casa::SHARE)
//     if (typeid(vec)==typeid(std::vector<double>)) {

//       vec.resize(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,vec,mtrx);
//       drp->fx(mtrx);
//     } else {
//       std::vector<double> nvec(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,nvec,mtrx);
//       drp->fx(mtrx);
//       copycast_vec(nvec,vec); //Copy back to output vector and convert type
//     };
    CasaMatrix<CasaNumber> ary=drp->fx();
    copy_ary2vp;
  }
  else if (Datatype=="Voltage") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->voltage();
    copy_ary2vp;
  }
  else if (Datatype=="invFFT") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->invfft();
    copy_ary2vp;
  }
  else if (Datatype=="FFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->fft();
    copy_ary2vp;
  }
  else if (Datatype=="CalFFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->calfft();
    copy_ary2vp;
  }
  else {
    ERROR("DataFunc_CR_dataRead: Datatype=" << Datatype << " is unknown.");
    vec.clear();
    return;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

/*
  else if (Datatype=="Fx") {aipscol2stlvec(drp->fx(),*vp,0);}
  else if (Datatype=="Voltage") {aipscol2stlvec(drp->voltage(),*vp,0);}
  else if (Datatype=="invFFT") {aipscol2stlvec(drp->invfft(),*vp,0);}
  else if (Datatype=="FFT") {aipscol2stlvec(drp->fft(),*vp,0);}
  else if (Datatype=="CalFFT") {aipscol2stlvec(drp->calfft(),*vp,0);}
*/


//       std::vector<double> tmpvec;
//       STL2CASA_SHARED(double,tmpvec,casavec); //Create casa vector sharing memory with the stl vector
//       casa::Vector<double> * vec_p;  //Get pointer to casa vector
//       vec_p=reinterpret_cast<casa::Vector<double>*>(&casavec); //That is just a trick to fool the compiler to
//                                                               //compile this section for T!=double (even though it is then never used)
//       *vec_p=drp->frequencyValues(); //read data into the casa and hence also into the stl vector
//       hConvert(tmpvec,vec);
//     }  else {   //Input vector is not of the right format
//       std::vector<double> tmpvec;  //Create temporary stl vector
//       STL2CASA_SHARED(double,tmpvec,casavec);  //Create casa vector sharing memory with the tmp stl vector
//       casavec=drp->frequencyValues(); //read data into the casa vector (hence als tmp stl vector)
//       hConvert(tmpvec,vec); // Copy and convert data from tmp stl (=casa) vector to the output vector.
//     };


//========================================================================================
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!OUTDATED!!!
//========================================================================================

#include <Pypeline/hftools.hpp>


