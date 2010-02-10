#ifndef HFTOOLS_H
#define HFTOOLS_H

//=========================================================================================
// Common with hfget ....
//=========================================================================================

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <complex>
#include <cmath>
#include <stdint.h>
#include <limits.h>
#include <typeinfo>

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

#include <Pypeline/hfcommon.h>
#include <Pypeline/hfppnew.h>

//========================================================================
//                    Casting & Conversion Functions
//========================================================================

//Identity
template<class T> inline T hfcast(/*const*/ T v);

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(HInteger v);
template<class T> inline T hfcast(HNumber v);
template<class T> inline T hfcast(HComplex v);

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(HNumber v);
template<>  inline HInteger hfcast<HInteger>(HComplex v);
template<>  inline HNumber hfcast<HNumber>(HComplex v);


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

//========================================================================
//                        Helper Functions
//========================================================================

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
#include "hftools.def.h"
#undef HFPP_FILETYPE
//=========================================================================================

#endif
