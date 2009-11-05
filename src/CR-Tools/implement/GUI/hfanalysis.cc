//#define DBG_MODE 0
//#define DBG_MODE 1

// CASA/casacore header files

//using casa::Bool;
//using casa::Double;
//using casa::Matrix;
//using casa::String;
//using casa::Vector;
//using casa::Array;

using namespace std;
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

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
#include <GUI/hfget.h>
#include <GUI/hfanalysis.h>
#include <GUI/hffuncs.h>  
#include <crtools.h>



/*========================================================================
  class ObjectFunctionClass
  ========================================================================

  Generic class that object functions are based on
*/


/*!
 \brief Returns function value of a Gaussian distribution with spread sigma and median value mu at position x. 
 */
inline HNumber funcGaussian(HNumber x, HNumber sigma, HNumber mu){return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));};

/*!
\brief Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
 */
vector<HNumber> hFlatWeights(address wlen){
  vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}

/*!  \brief Returns vector of weights of length wlen with linearly
rising and decreasing weights centered at len/2.

 The vector is normalized to give a sum of unity. Can be used by
hRunningAverageT.
 */
vector<HNumber> hLinearWeights(address wlen){
  vector<HNumber> weights(wlen,0.0);
  address i,middle=wlen/2;
  HNumber f,sum=0.0;

  //This is not the most effective way of doing things for linear
  //weights but that allows one to adapt this easily to other functions
  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

/*!  \brief Returns vector of weights of length wlen with Gaussian
     distribution centered at len/2 and width sigma.

 The vector is normalized to give a sum of unity. Can be used by hRunningAverageT.
 */
vector<HNumber> hGaussianWeights(address wlen){
  vector<HNumber> weights(wlen,0.0);
  address i,middle=wlen/2;
  HNumber f,sum=0.0;

  //This is not the most effective way of doing things for linear
  //weights but that allows one to adapt this easily to other functions
  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(middle/3.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

vector<HNumber> hWeights(address wlen, hWEIGHTS wtype){
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


/*!

\brief Calculate the running average of an array of Numbers.

idata_start: STL Iterator pointing to the first element of an array with input values.

idata_end: STL Iterator pointing to the end of the input vector

odata_start: STL Iterator pointing to the first element of an array
which will contain input values. The array must have the same size as
*start_data_in and be initialized to zero.


weights_start: STL Iterator pointing to the first element of an array with weights
values that allows one to specify weights to be applied to the
neighbouring samples before averaging. The weights should be
normalized to unity.

weights_end: STL Iterator pointing to the end (last plus one element!) of an array with weights


Attention: Note that it is assumed that the output array is
initialized, has the same length as the input array, and is
initialized with zeros!! If the array values are nonzero, they will
simply be added to the input vector. This is done so that the memory
allocation of a vector/array is handled by the user, hence different
types of arrays/vectors can be used.
*/

template <class T> 
void hRunningAverageT(const VecTit idata_start, const VecTit idata_end,const VecTit odata_start, const VecNit weights_start,const VecNit weights_end){

  address l=(weights_end-weights_start);
  address middle=l/2;  //index of the central element of the weights vector (i.e., where it typically would peak)

  T fac = l*10; //to avoid too many rounding errors with Integers

  typename vector<T>::iterator dit,dit2,din=idata_start,dout=odata_start;
  vector<HNumber>::iterator wit;

  while (din!=idata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights_start; // weight iterators set to beginning of weights
    while (wit!=weights_end) {
      if (dit<idata_start) dit2=idata_start;
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

/*!

\brief Overloaded function to automatically calculate weights

 */
template <class T> 
void hRunningAverageT(const VecTit idata_start, const VecTit idata_end,const VecTit odata_start, address wlen, hWEIGHTS wtype){
  vector<HNumber> weights=hWeights(wlen,wtype);
  hRunningAverageT<T>(idata_start,idata_end,odata_start, weights.begin(),weights.end());
}

/*!
\brief Wrapper to hRunningAverageT for CASA Vectors
 */
template <class T>
void hRunningAverageCasaVec(casa::Vector<T> &vec_in,casa::Vector<T> &vec_out, address wlen, hWEIGHTS wtype) {
  //  vec_out.assign(vec_in.size(),mycast<T>(0.0));
  typedef typename vector<T>::iterator Tit;
  hRunningAverageT<T>(static_cast<Tit>(vec_in.cbegin()),static_cast<Tit>(vec_in.cend()),static_cast<Tit>(vec_in.cend()),wlen,wtype);
  return;
} 

/*!
\brief Wrapper to hRunningAverageT for STL Vectors
 */
template <class T>
void hRunningAverageVec(vector<T> &vec_in,vector<T> &vec_out, address wlen, hWEIGHTS wtype) {
  vec_out.assign(vec_in.size(),mycast<T>(0.0));
  hRunningAverageT<T>(vec_in.begin(),vec_in.end(),vec_out.begin(),wlen,wtype);
  return;
} 

/*!
\brief Dummy function used for formal reasons by the GUI
 */
void hRunningAverageVec(vector<HString> &vec_in,vector<HString> &vec_out, address wlen, hWEIGHTS wtype) {
  vec_out=vec_in;
  MSG("hRunningAverageVec<HString>: not yet implemented");
}
/*!
\brief Dummy function used for formal reasons by the GUI
 */
void hRunningAverageVec(vector<HPointer> &vec_in,vector<HPointer> &vec_out, address wlen, hWEIGHTS wtype) {
  vec_out=vec_in;
  MSG("hRunningAverageVec<HPointer>: not implemented");
}

/*!

\brief Calculate the running average of an array of Numbers. Wrapper
for hRunningAverageT using pointers instead of iterators.

start_data_in: Pointer to the first element of an array with input values.

start_data_out: Pointer to the first element of an array which will contain input values. The array must have the same size as *start_data_in and be initialized to zero.

datalen: length of input and output array

start_weights: Pointer to the first element of an array with weights
values that allows one to specify weights to be applied to the
neighbouring samples before averaging. The weights should be
normalized to unity.

weightslen: number of weights

Attention: Note that it is assumed that the output array is
initialized, has the same length as the input array, and is
initialized with zeros!! If the array values are nonzero, they will
simply be added to the input vector. This is done so that the memory
allocation of a vector/array is handled by the user, hence different
types of arrays/vectors can be used.
*/

template <class T>
void hRunningAverageN(T * start_data_in, T * start_data_out, address datalen, HNumber * start_weights, address weightslen) {
  typedef typename vector<T>::iterator Tit;
  typedef typename vector<HNumber>::iterator Nit;
  Tit idata_start=static_cast<Tit>(start_data_in);
  Tit odata_start=static_cast<Tit>(start_data_out);
  Nit weights_start=static_cast<Nit>(start_weights);
  hRunningAverageT(idata_start, idata_start+datalen, odata_start, weights_start, weights_start+weightslen);
}

/*!
\brief This is needed to produce a Boost Python wrapper to the templated function hRunningAverage for a specific type. 

Those definitions should actually better go into the .hpp file
 */

void (*hRunningAverageVec_N)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;
void (*hRunningAverageVec_I)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;
void (*hRunningAverageVec_C)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address wlen, hWEIGHTS wtype) = &hRunningAverageVec;

/*
Actually you need to define new functions that take 2 or 3 parameters only to makes use of parameter defaulting in Python...

void (*hRunningAverageVec_N_3)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len) = &hRunningAverageVec;
void (*hRunningAverageVec_I_3)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len) = &hRunningAverageVec;
void (*hRunningAverageVec_C_3)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len) = &hRunningAverageVec;
void (*hRunningAverageVec_N_2)(vector<HNumber > &vec_in,vector<HNumber > &vec_out) = &hRunningAverageVec;
void (*hRunningAverageVec_I_2)(vector<HInteger> &vec_in,vector<HInteger> &vec_out) = &hRunningAverageVec;
void (*hRunningAverageVec_C_2)(vector<HComplex> &vec_in,vector<HComplex> &vec_out) = &hRunningAverageVec;
*/


void dummy_instantitate_templates(){
  casa::Vector<HNumber> v;
  hRunningAverageCasaVec(v, v,5,WEIGHTS_GAUSSIAN);
}


