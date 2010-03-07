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

using namespace std;


#undef HFPP_VERBOSE
#include "hftools.h"


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


SECTION: Administrative Vector Functions
------------------------------
hFill(vec, fill_value)              - Fills a vector with a constant value.
hNew(vec)                           - Make and return a new vector of the same size and type as the input vector
hConvert(vec1, vec2)                - Copies and converts a vector to a vector of another type.
hCopy(vec, outvec)                  - Copies a vector to another one.

SECTION: Math Functions
------------------------------
square(val)                         - Returns the squared value of the parameter
hPhase(frequency, time)             - Returns the interferometer phase in radians for a given frequency and time
funcGaussian(x, sigma, mu)          - Implementation of the Gauss function
hiSub(vec1, vec2)                   - Performs a Sub between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiSub(vec1, val)                    - Performs a Sub between the vector and a scalar (applied to each element), which is returned in the first vector.
hSub(vec1, vec2, vec3)              - Performs a Sub between the two vectors, which is returned in the third vector.
hSubAdd(vec1, vec2, vec3)           - Performs a Sub between the two vectors, and adds the result to the output (third) vector.
hSubAddConv(vec1, vec2, vec3)       - Performs a Sub between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hSub(vec1, val, vec2)               - Performs a Sub between the vector and a scalar, where the result is returned in the second vector.
hiMul(vec1, vec2)                   - Performs a Mul between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiMul(vec1, val)                    - Performs a Mul between the vector and a scalar (applied to each element), which is returned in the first vector.
hMul(vec1, vec2, vec3)              - Performs a Mul between the two vectors, which is returned in the third vector.
hMulAdd(vec1, vec2, vec3)           - Performs a Mul between the two vectors, and adds the result to the output (third) vector.
hMulAddConv(vec1, vec2, vec3)       - Performs a Mul between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hMul(vec1, val, vec2)               - Performs a Mul between the vector and a scalar, where the result is returned in the second vector.
hiAdd(vec1, vec2)                   - Performs a Add between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiAdd(vec1, val)                    - Performs a Add between the vector and a scalar (applied to each element), which is returned in the first vector.
hAdd(vec1, vec2, vec3)              - Performs a Add between the two vectors, which is returned in the third vector.
hAddAdd(vec1, vec2, vec3)           - Performs a Add between the two vectors, and adds the result to the output (third) vector.
hAddAddConv(vec1, vec2, vec3)       - Performs a Add between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hAdd(vec1, val, vec2)               - Performs a Add between the vector and a scalar, where the result is returned in the second vector.
hiDiv(vec1, vec2)                   - Performs a Div between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiDiv(vec1, val)                    - Performs a Div between the vector and a scalar (applied to each element), which is returned in the first vector.
hDiv(vec1, vec2, vec3)              - Performs a Div between the two vectors, which is returned in the third vector.
hDivAdd(vec1, vec2, vec3)           - Performs a Div between the two vectors, and adds the result to the output (third) vector.
hDivAddConv(vec1, vec2, vec3)       - Performs a Div between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hDiv(vec1, val, vec2)               - Performs a Div between the vector and a scalar, where the result is returned in the second vector.
hNegate(vec)                        - Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
hSum(vec)                           - Performs a sum over the values in a vector and returns the value
hNorm(vec)                          - Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2)))
hNormalize(vec)                     - Normalizes a vector to length unity
hMean(vec)                          - Returns the mean value of all elements in a vector
hSort(vec)                          - Sorts a vector in place
hSortMedian(vec)                    - Sorts a vector in place and returns the median value of the elements
hMedian(vec)                        - Returns the median value of the elements
hStdDev(vec, mean)                  - Calculates the standard deviation around a mean value.
hStdDev(vec)                        - Calculates the standard deviation of a vector of values
hFindLessEqual(vec, threshold, vecout) - Find the samples that are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThan(vec, threshold, vecout) - Find the samples that are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterEqual(vec, threshold, vecout) - Find the samples that are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThan(vec, threshold, vecout) - Find the samples that are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hDownsample(vec1, vec2)             - Downsample the input vector to a smaller output vector.
hDownsample(vec, downsample_factor) - Downsample the input vector by a cetain factor and return a new vector
hFindLowerBound(vec, value)         - Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
hFlatWeights(wlen)                  - Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
hLinearWeights(wlen)                - Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
hGaussianWeights(wlen)              - Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
hWeights(wlen, wtype)               - Create a normalized weight vector.
hRunningAverage(idata, odata, weights) - Calculate the running average of an input vector using a weight vector.
hRunningAverage(idata, odata, wlen, wtype) - Overloaded function to automatically calculate weights.

SECTION: RF (Radio Frequency) Functions
------------------------------
hGeometricDelayFarField(antPosition, skyDirection, length) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren)
hGeometricDelayNearField(antPosition, skyPosition, distance) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren)
hGeometricDelays(antPositions, skyPositions, delays, farfield) - Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field
hGeometricPhases(frequencies, antPositions, skyPositions, phases, farfield) - Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hGeometricWeights(frequencies, antPositions, skyPositions, weights, farfield) - Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hSpectralPower(vec, outvec)         - Calculates the power of a complex spectrum and add it to an output vector.

SECTION: I/O Functions (DataReader)
------------------------------
hFileClose(iptr)                    - Function to close a file with a datareader object providing the pointer to the object as an integer.
hFileOpen(Filename)                 - Function to open a file based on a filename and returning a pointer to a datareader object as an integer
hFileGetParameter(iptr, keyword)    - Return information from a data file as a Python object
hFileSetParameter(iptr, keyword, pyob) - Set parameters in a data file with a Python object as input
hFileRead(iptr, Datatype, vec)      - Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
hCalTable(filename, keyword, date, pyob) - Return a list of antenna positions from the CalTables - this is a test

SECTION: Coordinate Conversion (VectorConversion.cc)
------------------------------
hCoordinateConvert(source, sourceCoordinate, target, targetCoordinate, anglesInDegrees) - Converts a 3D spatial vector into a different Coordinate type (e.g. Spherical to Cartesian)
hReadFileOld(vec, iptr, Datatype, Antenna, Blocksize, Block, Stride, Shift) - Read data from a Datareader object (pointer in iptr) into a vector.



========================================================================
Here is an alphabetic list of some of the functions that are available.
========================================================================

funcGaussian(x, sigma, mu)          - Implementation of the Gauss function
hAdd(vec1, val, vec2)               - Performs a Add between the vector and a scalar, where the result is returned in the second vector.
hAdd(vec1, vec2, vec3)              - Performs a Add between the two vectors, which is returned in the third vector.
hAddAdd(vec1, vec2, vec3)           - Performs a Add between the two vectors, and adds the result to the output (third) vector.
hAddAddConv(vec1, vec2, vec3)       - Performs a Add between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hCalTable(filename, keyword, date, pyob) - Return a list of antenna positions from the CalTables - this is a test
hConvert(vec1, vec2)                - Copies and converts a vector to a vector of another type.
hCopy(vec, outvec)                  - Copies a vector to another one.
hDiv(vec1, val, vec2)               - Performs a Div between the vector and a scalar, where the result is returned in the second vector.
hDiv(vec1, vec2, vec3)              - Performs a Div between the two vectors, which is returned in the third vector.
hDivAdd(vec1, vec2, vec3)           - Performs a Div between the two vectors, and adds the result to the output (third) vector.
hDivAddConv(vec1, vec2, vec3)       - Performs a Div between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hDownsample(vec, downsample_factor) - Downsample the input vector by a cetain factor and return a new vector
hDownsample(vec1, vec2)             - Downsample the input vector to a smaller output vector.
hFileClose(iptr)                    - Function to close a file with a datareader object providing the pointer to the object as an integer.
hFileGetParameter(iptr, keyword)    - Return information from a data file as a Python object
hFileOpen(Filename)                 - Function to open a file based on a filename and returning a pointer to a datareader object as an integer
hFileRead(iptr, Datatype, vec)      - Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
hFileSetParameter(iptr, keyword, pyob) - Set parameters in a data file with a Python object as input
hFill(vec, fill_value)              - Fills a vector with a constant value.
hFindGreaterEqual(vec, threshold, vecout) - Find the samples that are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThan(vec, threshold, vecout) - Find the samples that are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindGreaterThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are GreaterThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessEqual(vec, threshold, vecout) - Find the samples that are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessEqualAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessEqual a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThan(vec, threshold, vecout) - Find the samples that are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLessThanAbs(vec, threshold, vecout) - Find the samples whose absolute values are LessThan a certain threshold value and returns the number of samples found and the positions of the samples in a second vector
hFindLowerBound(vec, value)         - Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
hFlatWeights(wlen)                  - Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
hGaussianWeights(wlen)              - Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
hGeometricDelayFarField(antPosition, skyDirection, length) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren)
hGeometricDelayNearField(antPosition, skyPosition, distance) - Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren)
hGeometricDelays(antPositions, skyPositions, delays, farfield) - Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field
hGeometricPhases(frequencies, antPositions, skyPositions, phases, farfield) - Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hGeometricWeights(frequencies, antPositions, skyPositions, weights, farfield) - Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
hLinearWeights(wlen)                - Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
hMean(vec)                          - Returns the mean value of all elements in a vector
hMedian(vec)                        - Returns the median value of the elements
hMul(vec1, val, vec2)               - Performs a Mul between the vector and a scalar, where the result is returned in the second vector.
hMul(vec1, vec2, vec3)              - Performs a Mul between the two vectors, which is returned in the third vector.
hMulAdd(vec1, vec2, vec3)           - Performs a Mul between the two vectors, and adds the result to the output (third) vector.
hMulAddConv(vec1, vec2, vec3)       - Performs a Mul between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hNegate(vec)                        - Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
hNew(vec)                           - Make and return a new vector of the same size and type as the input vector
hNorm(vec)                          - Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2)))
hNormalize(vec)                     - Normalizes a vector to length unity
hPhase(frequency, time)             - Returns the interferometer phase in radians for a given frequency and time
hReadFileOld(vec, iptr, Datatype, Antenna, Blocksize, Block, Stride, Shift) - Read data from a Datareader object (pointer in iptr) into a vector.
hRunningAverage(idata, odata, weights) - Calculate the running average of an input vector using a weight vector.
hRunningAverage(idata, odata, wlen, wtype) - Overloaded function to automatically calculate weights.
hSort(vec)                          - Sorts a vector in place
hSortMedian(vec)                    - Sorts a vector in place and returns the median value of the elements
hSpectralPower(vec, outvec)         - Calculates the power of a complex spectrum and add it to an output vector.
hStdDev(vec)                        - Calculates the standard deviation of a vector of values
hStdDev(vec, mean)                  - Calculates the standard deviation around a mean value.
hSub(vec1, val, vec2)               - Performs a Sub between the vector and a scalar, where the result is returned in the second vector.
hSub(vec1, vec2, vec3)              - Performs a Sub between the two vectors, which is returned in the third vector.
hSubAdd(vec1, vec2, vec3)           - Performs a Sub between the two vectors, and adds the result to the output (third) vector.
hSubAddConv(vec1, vec2, vec3)       - Performs a Sub between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
hSum(vec)                           - Performs a sum over the values in a vector and returns the value
hWeights(wlen, wtype)               - Create a normalized weight vector.
hiAdd(vec1, val)                    - Performs a Add between the vector and a scalar (applied to each element), which is returned in the first vector.
hiAdd(vec1, vec2)                   - Performs a Add between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiDiv(vec1, val)                    - Performs a Div between the vector and a scalar (applied to each element), which is returned in the first vector.
hiDiv(vec1, vec2)                   - Performs a Div between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiMul(vec1, val)                    - Performs a Mul between the vector and a scalar (applied to each element), which is returned in the first vector.
hiMul(vec1, vec2)                   - Performs a Mul between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
hiSub(vec1, val)                    - Performs a Sub between the vector and a scalar (applied to each element), which is returned in the first vector.
hiSub(vec1, vec2)                   - Performs a Sub between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
square(val)                         - Returns the squared value of the parameter

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

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(uint v){return static_cast<T>(v);}
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v+0.5));}
template<>  inline HInteger hfcast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)+0.5));}
template<>  inline HNumber hfcast<HNumber>(HComplex v){return real(v);}


inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}


inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}

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
//                   Matrix Class !!!!!IGNORE!!!!!
//========================================================================

// Testing a simple Matrix class that is built upon std::vectors
// we do actually not really want to do this ....
namespace std {

  template <class T, class Alloc>
  class hmatrix : public vector<T,Alloc> {

  public:
    template <class Iter> hmatrix(Iter it1, Iter it2);
    hmatrix(const char* buf);
    hmatrix();
    ~hmatrix();

    void setNumberOfDimensions(HInteger newdim);
    HInteger getNumberOfDimensions();

    void setDimensionN(HInteger n, HInteger size);

    void setDimensions(HInteger dim0);
    void setDimensions(HInteger dim0, HInteger dim1);
    void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3);


  private:
    HInteger ndim;
    vector<HInteger>* dimensions_p;
  };


  template <class T, class Alloc> template <class Iter> hmatrix<T,Alloc>::hmatrix(Iter it1, Iter it2) : vector<T,Alloc>(it1,it2) {};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(const char* buf) : vector<T,Alloc>(buf){};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(){
    ndim=0;
    dimensions_p=NULL;
    //    setDimension(2);
  };
  template <class T, class Alloc> hmatrix<T,Alloc>::~hmatrix(){};

  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setNumberOfDimensions(HInteger newdim){
    if (newdim==ndim) return;
    if (dimensions_p != NULL) delete dimensions_p;
    ndim=newdim;
    dimensions_p = new vector<HString>(ndim);
  };

  template <class T, class Alloc>
  HInteger hmatrix<T,Alloc>::getNumberOfDimensions(){return ndim;};

  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setDimensionN(HInteger n, HInteger size){
  }

  template <class T, class Alloc>
  void setDimensions(HInteger dim0){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3){}
};

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

//$DOCSTRING: Fills a vector with a constant value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hFill)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hFill(const Iter vec,const Iter vec_end, const IterValueType fill_value)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=fill_value;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Make and return a new vector of the same size and type as the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNew
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size of vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
 */
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize a vector to a new length and fill new elements in vector with a specific value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(fill)()("Value to fill new vector elements with")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize an STL vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class T, class S>
void hResize(std::vector<T> & vec1,std::vector<S> & vec2)
{
  vec1.resize(vec2.size());
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Resize a casa vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T, class S>
void hResize(casa::Vector<T> & vec1,casa::Vector<S> & vec2)
{
  vec1.resize(*(vec2.shape().begin()));
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvert
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_CODE_PRE hResize(vec2,vec1); //code will be inserted in wrapper generation in cc file
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec1,const Iterin vec1_end, const Iter vec2,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin it=vec1;
  Iter itout=vec2;
  while ((it!=vec1_end) && (itout !=vec2_end)) {
    *itout=hfcast<T>(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies a vector to another one.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(outvec)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec,const Iterin vec_end, const Iter out,const Iter out_end)
{
  typedef IterValueType T;
  Iterin it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=*it;
    ++it; ++itout;
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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

//$DOCSTRING: Returns the interferometer phase in radians for a given frequency and time.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhase
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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


//$DOCSTRING: Implementation of the Gauss function.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME funcGaussian
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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

//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,sin,sinh,sqrt,square,tan,tanh
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
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}2(const Iter vec,const Iter vec_end, const Iter out,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
} 
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor
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
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter1,class Iter2>
void h{$MFUNC!CAPS}2(const Iter1 vec,const Iter1 vec_end, const Iter2 out,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================


//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef IterValueType T;
  Iter it1=vec1;
  Iterin it2=vec2;
  while (it1!=vec1_end) {
    *it1 HFPP_OPERATOR_INPLACE_$MFUNC hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the vector and a scalar (applied to each element), which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(hi$MFUNC)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class S>
void hi{$MFUNC}2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef IterValueType T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it HFPP_OPERATOR_INPLACE_$MFUNC val_t;
    ++it;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the third vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec3)()("Vector containing the results")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin1, class Iterin2, class Iter>
void HFPP_FUNC_NAME(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end)  && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the two vectors, and adds the result to the output (third) vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Add
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_1)(vec3)()("Vector containing the results")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef IterValueType T;
  Iter it1(vec1), it2(vec2), itout(vec3);
  while ((it1!=vec1_end)  && (itout !=vec3_end)) {
    *itout += (*it1) HFPP_OPERATOR_$MFUNC  (*it2);
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the two vectors, and adds the result to the output (third) vector - automatic casting is done.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}AddConv
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec3)()("Vector containing the results")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin1, class Iterin2, class Iter>
void HFPP_FUNC_NAME(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3,const Iter vec3_end)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3;
  while ((it1!=vec1_end)  && (itout !=vec3_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the vector and a scalar, where the result is returned in the second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(h$MFUNC)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)(STDIT)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin1, class S, class Iter>
void h{$MFUNC}2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) HFPP_OPERATOR_$MFUNC  val_t;
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE



////////////////////
//COMPLEX FUNCTIONS
///////////////////

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

//$DOCSTRING: Take the $MFUNC of all the elements in the complex vector and return results in a float vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Complex input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecout)()("Float output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

The following functions are available for getting real values from
complex numbers:
  abs - absolute value of a complex number
  norm - magnitude of a complex number squared, i.e. c * conj(c) 
  arg - phase angle of a complex number
  imag - imaginary part of a complex number
  real - real part of a complex number
*/
template <class Iter, class Iterout>
void h{$MFUNC!CAPS}(const Iter vec,const Iter vec_end, const Iterout vecout,const Iterout vecout_end)
{
  Iter it=vec;
  Iterout itout=vecout;
  while ((it!=vec_end) && (itout !=vecout_end)) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
} 
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE



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
IterValueType HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=*it; ++it;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2))).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNorm
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
  HNumber norm=hNorm(vec,vec_end);
  Iter it=vec;
  while (it!=vec_end) {*it=(*it)/norm; ++it;};

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


//$DOCSTRING: Sorts a vector in place.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSort
//-----------------------------------------------------------------------
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
#define HFPP_PARDEF_1 (HNumber)(mean)()("The mean value of the vector caluclated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber hStdDev (const Iter vec,const Iter vec_end, const IterValueType mean)
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
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Find the samples that are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are above threshold")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const IterValueType threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (*it HFPP_OPERATOR_$MFUNC threshold) {
      if (itout < vecout_end) {
	*itout=(it-vec);
	++itout;
      };
    };
    ++it;
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
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are above threshold")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const IterValueType threshold, const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end)
{
  Iter it=vec;
  typename vector<HInteger>::iterator itout=vecout;
  while (it<vec_end) {
    if (abs(*it) HFPP_OPERATOR_$MFUNC threshold) {
      if (itout < vecout_end) {
	*itout=(it-vec);
	++itout;
      };
    };
    ++it;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE


//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

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
void HFPP_FUNC_NAME (const Iter vec1,
		  const Iter vec1_end,
		  const Iter vec2,
		  const Iter vec2_end)
{
  if (vec2>=vec2_end) return; //If size 0 do nothing
  if (vec1>=vec1_end) return; //If size 0 do nothing
  HInteger ilen=(vec1_end-vec1);
  HInteger olen=(vec2_end-vec2);
  HInteger blen=max(ilen/(olen-1),1);
  //use max to avoid infinite loops if output vector is too large
  Iter it2,it1=vec1;
  Iter ito=vec2,ito_end=vec2_end-1;
  //only produce the first N-1 blocks in the output vector
  while ((it1<vec1_end) && (ito<ito_end)) {
    it2=min(it1+blen,vec1_end);
    *ito=hMean(it1,it2);
    it1=it2;
    ++ito;
    }
  *ito=hMean(it2,vec1_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector by a cetain factor and return a new vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
  hDownsample(vec,newvec);
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
HInteger HFPP_FUNC_NAME (const Iter vec,
			 const Iter vec_end,
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
vector<HNumber> hWeights(HInteger wlen, hWEIGHTS wtype){
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
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(weights)()("Weight vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class DataIter, class NumVecIter>
void HFPP_FUNC_NAME (const DataIter  idata,
		     const DataIter  idata_end,
		     const DataIter  odata,
		     const DataIter  odata_end,
		     const NumVecIter weights,
		     const NumVecIter weights_end)
{
  HInteger l=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=l/2;
  /* To avoid too many rounding errors with Integers */
  typename DataIter::value_type fac = l*10;

  DataIter  dit;
  DataIter  dit2;
  DataIter  din(idata);
  DataIter  dout(odata);
  NumVecIter wit;

  while (din<idata_end && dout<odata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights; // weight iterators set to beginning of weights
    while (wit<weights_end) {
      if (dit<idata) dit2=idata;
      else if (dit>=idata_end) dit2=idata_end-1;
      else dit2=dit;
      *dout=*dout+((*dit2)*(*wit)*fac);
      ++dit; ++wit;
    };
    *dout/=fac;
    ++dout; ++din; //point to the next element in data input and output vector
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Overloaded function to automatically calculate weights.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter idata,
		     const DataIter idata_end,
		     const DataIter odata,
		     const DataIter odata_end,
		     HInteger wlen,
		     hWEIGHTS wtype)
{
  vector<HNumber> weights = hWeights(wlen, wtype);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (idata,
							idata_end,
							odata,
							odata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//
//========================================================================
//$SECTION:           RF (Radio Frequency) Function
//========================================================================

//$DOCSTRING: Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelayFarField
//-----------------------------------------------------------------------
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
#define HFPP_PARDEF_0 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(delays)()("Output vector containing the delays in seconds for all antennas and positions [antenna index runs fastest: (ant1,pos1),(ant2,pos1),...] - length of vector has to be number of antennas times positions")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (
			const Iter antPositions,
			const Iter antPositions_end,
			const Iter skyPositions,
			const Iter skyPositions_end,
			const Iter delays,
			const Iter delays_end,
			const bool farfield
			)
{
  HNumber distance;
  Iter
    ant,
    sky=skyPositions,
    del=delays,
    ant_end=antPositions_end-2,
    sky_end=skyPositions_end-2;

  if (farfield) {
    while (sky < sky_end && del < delays_end) {
      distance = hNorm(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && del < delays_end) {
	*del=hGeometricDelayFarField(ant,sky,distance);
	ant+=3; ++del;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && del < delays_end) {
      distance = hNorm(sky,sky+3); //distance from phase center
      ant=antPositions;
      while (ant < ant_end && del < delays_end) {
	*del=hGeometricDelayNearField(ant,sky,distance);
	ant+=3; ++del;
      };
      sky+=3;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricPhases
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(frequencies)()("Vector of frequencies (in Hz) to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(phases)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: (nu1,ant1,pos1),(nu2,ant1,pos1),...] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (
			const Iter frequencies,
			const Iter frequencies_end,
			const Iter antPositions,
			const Iter antPositions_end,
			const Iter skyPositions,
			const Iter skyPositions_end,
			const Iter phases,
			const Iter phases_end,
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
      distance = hNorm(sky,sky+3);
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
      distance = hNorm(sky,sky+3);
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
#define HFPP_PARDEF_0 (HNumber)(frequencies)()("Vector of frequencies (in Hz) to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(antPositions)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(skyPositions)()("Vector in Cartesian coordinates (Meters) pointing towards a sky location, relative to phase center - vector of length number of skypositions times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HComplex)(weights)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: (nu1,ant1,pos1),(nu2,ant1,pos1),...] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if true, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter, class CIter>
void HFPP_FUNC_NAME (
			const Iter frequencies,
			const Iter frequencies_end,
			const Iter antPositions,
			const Iter antPositions_end,
			const Iter skyPositions,
			const Iter skyPositions_end,
			const CIter weights,
			const CIter weights_end,
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
      distance = hNorm(sky,sky+3);
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
      distance = hNorm(sky,sky+3);
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
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec,const Iterin vec_end, const Iter out,const Iter out_end)
{
  typedef IterValueType T;
  Iterin it(vec);
  Iter itout(out);
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout+=real((*it)*conj(*it));
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Convert the ADC value to a voltage.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hADC2Voltage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(adc2voltage)()("Scaling factor if the gain")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
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
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (uint)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (uint)(BetaRise)()("Rising slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (uint)(BetaFall)()("Falling slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const double Alpha,
		       const uint Beta,
		       const uint BetaRise,
		       const uint BetaFall) {

  uint blocksize = vec_end - vec;
  CR::HanningFilter<HNumber> hanning_filter(blocksize, Alpha, Beta, BetaRise, BetaFall);
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
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (uint)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const double Alpha,
		       const uint Beta) {
  uint blocksize = vec_end - vec;
  uint BetaRise = (blocksize - Beta)/2;
  uint BetaFall = (blocksize - Beta)/2;

  hGetHanningFilter(vec, vec_end, Alpha, Beta, BetaRise, BetaFall);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const double Alpha) {
  uint Beta = 0;
  hGetHanningFilter(vec, vec_end, Alpha, Beta);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end){
  double Alpha = 0.5;
  hGetHanningFilter(vec, vec_end, Alpha);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Apply a predefined filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyFilter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Vector containing the data on which the filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(filter)()("Vector containing the filter.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*
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
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Input and return vector containing the data on which the Hanning filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end){
  uint blocksize = data_end - data;
  vector<HNumber> filter(blocksize);

  hGetHanningFilter(filter.begin(), filter.end());
  hApplyFilter(data, data_end, filter.begin(), filter.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Apply an FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFT
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_in)()("Vector containing the data on which the FFT will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterIn, class IterOut>
void HFPP_FUNC_NAME(const IterIn  data_in,  const IterIn  data_in_end,
		    const IterOut data_out, const IterOut data_out_end,
		    const HInteger nyquistZone) { 
  uint channel;
  uint blocksize(data_in_end - data_in);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,blocksize);
  IPosition shape_out(1,fftLength);
  FFTServer<Double,DComplex> fftserver(shape_in, FFTEnums::REALTOCOMPLEX);

  Vector<Double> cvec_in(shape_in, reinterpret_cast<Double*>(&(*data_in)), casa::SHARE);
  Vector<DComplex> cvec_out(shape_out, 0.);
  IterOut it_out;

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
  /// TODO: Check if output is correct.
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Apply an Inverse FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFT
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASS (STL)
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data_in)()("Vector containing the input data on which the inverse FFT will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(data_out)()("Return vector in which the inverse FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (uint)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterIn, class IterOut>
void HFPP_FUNC_NAME(const IterIn data_in, const IterIn data_in_end,
		    const IterOut data_out, const IterOut data_out_end,
		    const uint nyquistZone) {
  // uint channel;
  // uint blocksize = data_out_end - data_out;
  // uint fftLength = blocksize/2 + 1;
  // uint nofChannels = fftLength;
  // IPosition shape_in = (1,fftLength);
  // IPosition shape_out = (1, blocksize);

  // Vector<DComplex> cvec_f(shape_in, reinterpret_cast<DComplex*>(data_in), casa::SHARE);
  // Vector<Double> cvec_out(shape_out, 0.);

  // if ((data_in_end - data_in) != fftLength) {
  //   cerr << "[invfft] Bad input: len(data_in) != fftLength" << endl;
  // };

  // try {
  //   Vector<DComplex> cvec_in(fftLength);
  //   FFTServer<Double,DComplex> server(shape_out,
  // 				      FFTEnums::REALTOCOMPLEX);
  //   switch (nyquistZone) {
  //   case 1:
  //   case 3:
  //     for (channel=0; channel<nofChannels; channel++) {
  // 	cvec_in(channel) = cvec_f(channel);
  //     }
  //     break;
  //   case 2:
  //     for (channel=0; channel<nofChannels; channel++) {
  // 	cvec_in(channel) = conj(cvec_f(fftLength - channel - 1));
  //     }
  //     break;
  //   }
  //   server.fft(cvec_out,cvec_in);
  // } catch (AipsError x) {
  //   cerr << "[invfft]" << x.getMesg() << endl;
  // }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//
//========================================================================
//$SECTION:              I/O Function (DataReader)
//========================================================================

//$DOCSTRING: Function to close a file with a datareader object providing the pointer to the object as an integer.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileClose
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HIntPointer)(iptr)()("Pointer to a DataReader object, stored as an integer.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

void HFPP_FUNC_NAME(HIntPointer iptr) {
  union{void* ptr; CR::DataReader* drp;};
  ptr=IntAsPtr(iptr);
  if (ptr!=Null_p) delete drp;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Function to open a file based on a filename and returning a pointer to a datareader object as an integer.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileOpen
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HIntPointer)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to opwn including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/

HIntPointer HFPP_FUNC_NAME(HString Filename) {

  bool opened;
  union{HIntPointer iptr; void* ptr; CR::DataReader* drp; CR::LOFAR_TBB* tbb; CR::LopesEventIn* lep;};

  //Create the a pointer to the DataReader object and store the pointer

  HString Filetype = hgetFiletype(Filename);
  if (Filetype=="LOPESEvent") {
    lep = new CR::LopesEventIn;
    opened=lep->attachFile(Filename);
    MSG("Opening LOPES File="<<Filename); lep->summary();
  } else if (Filetype=="LOFAR_TBB") {
    tbb = new CR::LOFAR_TBB(Filename,1024);
    opened=tbb!=NULL;
    MSG("Opening LOFAR File="<<Filename);tbb->summary();
  } else {
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Unknown Filetype = " << Filetype  << ", Filename=" << Filename);
    opened=false;
  }

  if (!opened){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Opening file " << Filename << " failed.");
    return PtrAsInt(Null_p);
  };

  return iptr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return information from a data file as a Python object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileGetParameter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HPyObject)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HIntPointer)(iptr)()("Integer containing pointer to the datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword ro be read out from the file metadata")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HPyObject HFPP_FUNC_NAME(HIntPointer iptr, HString key)
{
  DataReader *drp(reinterpret_cast<DataReader*>(iptr));
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if (key== #KEY) {TYPE result(drp->KEY ()); HPyObject pyob((TYPE2)result); return pyob;} else
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
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if (key== #KEY) {CasaVector<TYPE> casavec(drp->KEY ()); std::vector<TYPE2> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
    HFPP_REPEAT(uint,HInteger,antennas)
    HFPP_REPEAT(uint,HInteger,selectedAntennas)
    HFPP_REPEAT(uint,HInteger,selectedChannels)
    HFPP_REPEAT(uint,HInteger,positions)
    HFPP_REPEAT(double,HNumber,increment)
    HFPP_REPEAT(double,HNumber,frequencyValues)
    HFPP_REPEAT(double,HNumber,frequencyRange)
#undef HFPP_REPEAT
#define HFPP_REPEAT(TYPE,TYPE2,KEY)  if (key== #KEY) {TYPE result;  drp->headerRecord().get(key,result); HPyObject pyob((TYPE2)result); return pyob;} else
      HFPP_REPEAT(uint,uint,Date)
	HFPP_REPEAT(casa::String,HString,Observatory)
	HFPP_REPEAT(int,int,Filesize)
	HFPP_REPEAT(double,double,dDate)
	HFPP_REPEAT(int,int,presync)
	HFPP_REPEAT(int,int,TL)
	HFPP_REPEAT(int,int,LTL)
	HFPP_REPEAT(int,int,EventClass)
	HFPP_REPEAT(casa::uChar,uint,SampleFreq)
	HFPP_REPEAT(uint,uint,StartSample)
#undef HFPP_REPEAT
	if (key== "AntennaIDs") {CasaVector<int> casavec; drp->headerRecord().get(key,casavec); std::vector<int> result; aipsvec2stlvec(casavec,result); HPyObject pyob(result); return pyob;} else
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
	HFPP_REPEAT(double,double,dDate)
	HFPP_REPEAT(int,int,presync)
	HFPP_REPEAT(int,int,TL)
	HFPP_REPEAT(int,int,LTL)
	HFPP_REPEAT(int,int,EventClass)
	HFPP_REPEAT(casa::uChar,uint,SampleFreq)
	HFPP_REPEAT(uint,uint,StartSample)
//------------------------------------------------------------------------
	HFPP_REPEAT(int,int,AntennaIDs)
#undef HFPP_REPEAT
		     + "help";
      if (key!="help") cout << "Unknown keyword " << key <<"!"<<endl;
      cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< result <<endl;
      HPyObject pyob(result);
      return pyob;
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set parameters in a data file with a Python object as input.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileSetParameter
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HIntPointer)(iptr)()("Integer containing pointer to the datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HString)(keyword)()("Keyword to be set in the file")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HPyObjectPtr)(pyob)()("Input paramter")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
////$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
void HFPP_FUNC_NAME(HIntPointer iptr, HString key, HPyObjectPtr pyob)
{
  DataReader *drp(reinterpret_cast<DataReader*>(iptr));
#define HFPP_REPEAT(TYPE,TYPE2,KEY) if (key== #KEY) {TYPE input(TYPE2 (pyob)); drp->set##KEY (input);} else
  HFPP_REPEAT(uint,PyInt_AsLong,Block)
    HFPP_REPEAT(uint,PyInt_AsLong,Blocksize)
    HFPP_REPEAT(uint,PyInt_AsLong,StartBlock)
    HFPP_REPEAT(uint,PyInt_AsLong,Stride)
    HFPP_REPEAT(uint,PyInt_AsLong,SampleOffset)
    HFPP_REPEAT(uint,PyInt_AsLong,NyquistZone)
    HFPP_REPEAT(double,PyFloat_AsDouble,ReferenceTime)
    HFPP_REPEAT(double,PyFloat_AsDouble,SampleFrequency)
    HFPP_REPEAT(int,PyInt_AsLong,Shift)
    if (key=="SelectedAntennas") {
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
      if (key!="help") cout << "Unknown keyword " << key <<"!"<<endl;
      cout  << BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << " - available keywords: "<< txt <<endl;
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Read data from a Datareader object (pointer in iptr) into a vector, where the size should be pre-allocated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFileRead
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HIntPointer)(iptr)()("Integer containing pointer to the datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
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
void HFPP_FUNC_NAME(
		    HIntPointer iptr,
		    HString Datatype,
		    std::vector<T> & vec
		    )
{

  //Create a DataReader Pointer from an interger variable
  DataReader *drp=reinterpret_cast<DataReader*>(iptr);

  //Check whether it is non-NULL.
  if (drp==Null_p){
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": pointer to FileObject is NULL, DataReader not found.");
    return;
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
  return;
}
#undef HFPP_REPEAT
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
//$DOCSTRING: Return a list of antenna positions from the CalTables - this is a test.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalTable
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
x=hCalTable("~/LOFAR/usg/data/lopes/LOPES-CalTable",obsdate,list(antennaIDs))

*/
HPyObjectPtr HFPP_FUNC_NAME(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob) {
  CR::CalTableReader* CTRead = new CR::CalTableReader(filename);
  HInteger i,ant,size;
  casa::Vector<Double> tmpvec;
  HPyObjectPtr list=PyList_New(0),tuple;
  if (CTRead != NULL && PyList_Check(pyob)) {  //Check if CalTable was opened ... and Python object is a list
    size=PyList_Size(pyob);
    for (i=0;i<size;++i){  //loop over all antennas
      ant=PyInt_AsLong(PyList_GetItem(pyob,i));  //Get the ith element of the list, i.e. the antenna ID
      CTRead->GetData((uint)date, ant, keyword, &tmpvec);
      tuple=PyTuple_Pack(3,PyFloat_FromDouble(tmpvec[0]),PyFloat_FromDouble(tmpvec[1]),PyFloat_FromDouble(tmpvec[2]));
      PyList_Append(list,tuple);
    };
  };
  return list;
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
#define HFPP_WRAPPER_CLASSES HFPP_NONE
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
      hConvert(tmpvec,vec);
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
//hf #define MAKE_UNION_CASA_MATRIX_AND_STLVEC(TYPECASA,STLVEC,CASAVEC) \
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
