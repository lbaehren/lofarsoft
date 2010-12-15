/**************************************************************************
 *  Header file of Array module for CR Pipeline Python bindings.          *
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

#ifndef CR_PIPELINE_ARRAY_H
#define CR_PIPELINE_ARRAY_H

#include "core.h"
#include "mModule.h"


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
    std::map<HString,HNumber> prefix_to_factor;
  };

  struct storage_container {
    HInteger *ndims_p;
    HInteger *size_p;
    std::vector<HInteger> * dimensions_p;
    std::vector<HInteger> * slice_sizes_p;
    std::vector<T>* vec_p;
    hArray* parent;
    bool vector_is_shared;
    unit_container unit;
    bool trackHistory;
    std::vector<HString> history;
    std::map<HString,HString> keywords;
    std::map<HString,HPyObjectPtr> keywords_py;
  };

    void init();
    void initialize_storage();
    hArray();
    hArray(std::vector<T> & vec);
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
    hArray<T> & setSliceVector(std::vector<HInteger> & index_vector);
    HInteger getSubSliceEnd();
    HInteger getSubSliceStart();
    hArray<T> & setSubSlice(HInteger start, HInteger end, HInteger level);
    hArray<T> & setSubSliceVec(std::vector<HInteger> & start, std::vector<HInteger> & end, HInteger level);
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
    hArray<T> & loop(std::vector<HInteger> & start_element_index, HInteger start=0, HInteger end=-1, HInteger increment=1);
    hArray<T> & loopVector(std::vector<HInteger> & start_element_index, std::vector<HInteger> & vec);
    HInteger setLoopSlice(std::vector<HInteger> & start_element_index);
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
    std::vector<HString> & getHistory();
    void printHistory();
    HString getKey(HString key);
    hArray<T> & setKey(HString key, HString contents);
    HString writeRaw();
    void readRaw(HString raw);
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
    std::vector<HInteger> subslice_vec_start,subslice_vec_end;
    std::vector<HInteger>::iterator subslice_start_it,subslice_end_it;
    std::vector<HInteger> index_vector;
    bool loop_over_indexvector;
    bool loop_next;
    bool doiterate;
  };

// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

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
    .def("getKey",&hArray<TYPE>::getKey)                                   \
    .def("writeRaw",&hArray<TYPE>::writeRaw)                               \
    .def("readRaw",&hArray<TYPE>::readRaw)
;

template <class T> HString pretty_vec(hArray<T> & a, const HInteger maxlen);
void hArray_trackHistory(HBool on);

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
//#include "../../../../build/cr/implement/Pypeline/mArray.def.h"

#undef HFPP_FILETYPE


#endif /* CR_PIPELINE_ARRAY_H */
