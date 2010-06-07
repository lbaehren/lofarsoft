#ifndef HFTOOLS_H
#define HFTOOLS_H

//=========================================================================================
// Common with hfget ....
//=========================================================================================

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
#include <fftw3.h>

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

//>>>>>>>><boost/python/class.hpp>
#include <boost/python/class.hpp>
//>>>>>>>>#include <boost/python/object.hpp>
#include <boost/python/object.hpp>
//>>>>>>>>#include <boost/python/list.hpp>
#include <boost/python/list.hpp>
//>>>>>>>>#include <boost/python/extract.hpp>
#include <boost/python/extract.hpp>
//>>>>>>>>#include <boost/python/module.hpp>
#include <boost/python/module.hpp>
//>>>>>>>>#include <boost/python/def.hpp>
#include <boost/python/def.hpp>
//>>>>>>>>#include <boost/utility.hpp>
#include <boost/utility.hpp>

//>>>>>>>>#include <boost/python/call_method.hpp>
#include <boost/python/call_method.hpp>
//>>>>>>>>#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
//>>>>>>>>#include <boost/python/implicit.hpp>
#include <boost/python/implicit.hpp>
//>>>>>>>>#include <boost/python/enum.hpp>
#include <boost/python/enum.hpp>
//>>>>>>>>#include <boost/python/overloads.hpp>
#include <boost/python/overloads.hpp>
//>>>>>>>>#include <boost/python/args.hpp>
#include <boost/python/args.hpp>
//>>>>>>>>#include <boost/python/tuple.hpp>
#include <boost/python/tuple.hpp>
//>>>>>>>>#include <boost/python/class.hpp>
#include <boost/python/class.hpp>
//>>>>>>>>#include <boost/python/str.hpp>
#include <boost/python/str.hpp>
//>>>>>>>>#include <boost/python/return_internal_reference.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_value_policy.hpp>
//>>>>>>>>#include <boost/python/operators.hpp>
#include <boost/python/operators.hpp>
//>>>>>>>>#include <boost/python/object_operators.hpp>
#include <boost/python/object_operators.hpp>
//>>>>>>>>#include <boost/thread/thread.hpp>
#include <boost/thread/thread.hpp>
//>>>>>>>>#include <boost/python/has_back_reference.hpp>
#include <boost/python/has_back_reference.hpp>
//>>>>>>>>#include <boost/python/handle.hpp>
#include <boost/python/handle.hpp>

#include <crtools.h>
#include "IO/LopesEventIn.h"
#include "IO/LOFAR_TBB.h"
#include <Coordinates/CoordinateType.h>
#include "Math/VectorConversion.h"
#include "Math/Constants.h"
#include <Pypeline/hfcommon.h>
#include <Pypeline/hfppnew.h>
#include <Calibration/CalTableReader.h>
#include <Calibration/RFIMitigation.h>

#ifdef HAVE_GSL
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_bspline.h>
#endif


using namespace std;

//========================================================================
//                             Array Class
//========================================================================

template <class T>
  class hArray {

  public:

  struct unit_container {
    HNumber scale_factor;
    HString prefix;
    HString name;
    map<HString,HNumber> prefix_to_factor;
  };

  struct storage_container {
    HInteger *ndims_p;
    HInteger *size_p;
    vector<HInteger> * dimensions_p;
    vector<HInteger> * slice_sizes_p;
    vector<T>* vec_p;
    hArray* parent;
    bool vector_is_shared;
    unit_container unit;
    bool trackHistory;
    vector<HString> history;
    map<HString,HString> keywords;
    map<HString,HPyObjectPtr> keywords_py;
  };

    void init();
    void initialize_storage();
    hArray();
    hArray(const std::vector<T> & vec);
    hArray(storage_container * sptr);
    ~hArray();
    void delVector();
    void delete_storage();
    void new_storage();
    hArray<T> & shared_copy();
    hArray<T> & resize(HInteger newsize);
    hArray<T> & setVector(std::vector<T> & vec);
    std::vector<T> & getVector();
    std::vector<HInteger> & getDimensions();
    std::vector<HInteger> & getSizes();
    void calcSizes();
    void setDimensions1(HInteger dim0);
    void setDimensions2(HInteger dim0, HInteger dim1);
    void setDimensions3(HInteger dim0, HInteger dim1, HInteger dim2);
    void setDimensions4(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3);
    void setDimensions5(HInteger dim0, HInteger dim1, HInteger dim2, HInteger dim3, HInteger dim4);
    HInteger getNumberOfDimensions();
    hArray<T> & setSlice(HInteger beg, HInteger end=-1);
    hArray<T> & setSliceVector(vector<HInteger> & index_vector);
    HInteger getSubSliceEnd();
    HInteger getSubSliceStart();
    hArray<T> & setSubSlice(HInteger start, HInteger end, HInteger level);
    hArray<T> & setSubSliceVec(const vector<HInteger> start, const vector<HInteger> & end, const HInteger level);
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    HInteger getLoop_i();
    HInteger getLoop_nslice();
    HInteger getLoop_start();
    HInteger getLoop_end();
    HInteger loop_slice_start_offset;
    HInteger loop_slice_end_offset; 
    HInteger getLoop_increment();
    HInteger getBegin();
    HInteger getEnd();
    HInteger getSize();
    HInteger length();
    bool loopingMode();
    bool doLoopAgain();
    hArray<T> & loop(vector<HInteger> & start_element_index, HInteger start=0, HInteger end=-1, HInteger increment=1);
    hArray<T> & loopVector(vector<HInteger> & start_element_index, vector<HInteger> & vec);
    HInteger setLoopSlice(vector<HInteger> & start_element_index);
    hArray<T> &  next();
    hArray<T> &  loopOn();
    hArray<T> &  loopOff();
    hArray<T> &  resetLoop();
    hArray<T> &  all();
    hArray<T> &  setUnit(HString prefix, HString name);
    hArray<T> &  clearUnit();
    HString  getUnit();
    HString  getUnitName();
    HString  getUnitPrefix();
    hArray<T> &  setHistory(bool on_or_off);
    bool isTrackingHistory();
    hArray<T> &  clearHistory();
    hArray<T> &  addHistory(HString name, HString text);
    vector<HString> & getHistory();
    void printHistory();
    HString getKey(HString key);
    hArray<T> & setKey(HString key, HString contents);

    //hArray<T> & setKeyPy(HString key, HPyObjectPtr pyobj);
    //HPyObject & getKeyPy(HString key);

    //These are the basic parameters describing the data
    //structure. They can be shared

    storage_container * storage_p;
    bool array_is_shared;

    HInteger copycount;

  private:
    HInteger slice_begin, slice_end, slice_size;
    HInteger loop_slice_begin, loop_slice_end, loop_slice_size, loop_lower_level_size;
    HInteger loop_start, loop_end, loop_increment, loop_i, loop_nslice, loop_maxn;
    HInteger subslice_level,subslice_start,subslice_end;
    vector<HInteger> subslice_vec_start,subslice_vec_end;
    vector<HInteger>::iterator subslice_start_it,subslice_end_it;
    vector<HInteger> index_vector;
    bool loop_over_indexvector;
    bool loop_next;
    bool doiterate;
  };

//========================================================================
//                    Casting & Conversion Functions
//========================================================================

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
template<class T> inline T hfcast(/*const*/ HPointer v);
template<class T> inline T hfcast(/*const*/ HString v);

template<class T> inline T hfcast(CR::DataReader v);


//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(HNumber v);
template<>  inline HInteger hfcast<HInteger>(HComplex v);
template<>  inline HNumber hfcast<HNumber>(HComplex v);

template<> inline HString hfcast<HString>(HPointer v);
template<> inline HString hfcast<HString>(HInteger v);
template<> inline HString hfcast<HString>(HNumber v);
template<> inline HString hfcast<HString>(HComplex v);

template<class T> inline T hfcast(/*const*/ HString v);
template<> inline HPointer hfcast(/*const*/ HString v);
template<> inline HInteger hfcast(/*const*/ HString v);
template<> inline HNumber hfcast(/*const*/ HString v);
template<> inline HComplex hfcast(/*const*/ HString v);

template<class T> inline T hfcast(HBool v);
template<>  inline HBool hfcast<HBool>(HInteger v);
template<>  inline HBool hfcast<HBool>(HNumber v);
template<>  inline HBool hfcast<HBool>(HComplex v);
template<>  inline HBool hfcast<HBool>(HString v);
template<>  inline HBool hfcast<HBool>(HPointer v);


template<class T> inline HString hf2string(T v);
template<> inline HString hf2string(CR::DataReader v);

template <class T>
HString pretty_vec(vector<T> & v,const HInteger maxlen=8);

template <class T>
HString pretty_vec(hArray<T> & v,const HInteger maxlen=8);


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

#define A_LOW_NUMBER 1.0e-32

//========================================================================
//                        Helper Functions
//========================================================================

void hInit();
HString hgetFiletype(HString filename);
HString hgetFileExtension(HString filename);

//========================================================================
//                           Math Functions
//========================================================================

enum hWEIGHTS {WEIGHTS_FLAT,WEIGHTS_LINEAR,WEIGHTS_GAUSSIAN};
vector<HNumber> hWeights(HInteger wlen, hWEIGHTS wtype);
template <class T> inline T square(T val);


//========================================================================
//                             I/O Functions
//========================================================================

HIntPointer hOpenFile(HString Filename);
void hCloseFile(HIntPointer iptr);

//=========================================================================================
// Define headers for vector wrappers
//=========================================================================================
#undef HFPP_FILETYPE
// Tell the preprocessor (for generating wrappers) that this is a c++ header file (brackets are crucial)
#define HFPP_FILETYPE hFILE
// include wrapper definitions generated automatically from source
#include "../../../../build/cr/implement/Pypeline/hftools.def.h"
#undef HFPP_FILETYPE
//=========================================================================================

#endif
