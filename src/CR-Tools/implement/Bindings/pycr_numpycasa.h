/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 4824 2010-05-06 09:07:23Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
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

#ifndef PYCR_NUMPYCASA_H
#define PYCR_NUMPYCASA_H

// Standard library header files
#include <iostream>
#include <string>

#include <boost/python/numeric.hpp>

#include <casa/Arrays.h>

#include <crtools.h>
#include "IO/DataReader.h"

//needed for num_util
#ifdef PY_ARRAY_UNIQUE_SYMBOL
  #warning "PY_ARRAY_UNIQUE_SYMBOL already defined! Possible mixup of num_util directives!"
#endif
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#define NO_IMPORT_ARRAY
#include <num_util.h>

namespace bpl = boost::python;

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin
    
    /*!
      \ingroup CR
      \ingroup pycr
      
      \brief Template header file with helper functions for numpy arrays
      
      \author Andreas Horneffer

      \date 2010/06/13

      \test tpycr_numpycasa.cc
    
      <h3>Prerequisite</h3>
    
      <ul type="square">
      <li>[start filling in your text here]
      </ul>
    
      <h3>Synopsis</h3>
    
      <h3>Example(s)</h3>
    
    */
  
    // ===========================================================================
    //  Functions 
    // ===========================================================================

    // === num_util replacement functions ========================================

    /** 
     *Function template creates a one-dimensional numpy array of length n containing
     *a copy of data at data*.  See num_util.cpp::getEnum<T>() for list of specializations
     *@param T  C type of data
     *@param T* data pointer to start of data
     *@param n an integer indicates the size of the array.
     *@return a numpy array of size n with elements initialized to data.
     */
    
    template <typename T> boost::python::numeric::array makeNum(const T* data, int n = 0){
      boost::python::object obj(boost::python::handle<>(PyArray_FromDims(1, &n,  num_util::getEnum<T>())));
      void *arr_data = PyArray_DATA((PyArrayObject*) obj.ptr());
      memcpy(arr_data, data, PyArray_ITEMSIZE((PyArrayObject*) obj.ptr()) * n); // copies the input data to 
      return boost::python::extract<boost::python::numeric::array>(obj);
    }
    
    /** 
     *Function template creates an n-dimensional numpy array with dimensions dimens containing
     *a copy of values starting at data.  See num_util.cpp::getEnum<T>() for list of specializations
     *@param T  C type of data
     *@param T*  data pointer to start of data
     *@param n an integer indicates the size of the array.
     *@return a numpy array of size n with elements initialized to data.
     */
    
    
    template <typename T> boost::python::numeric::array makeNum(const T * data, std::vector<int> dims){
      int total = std::accumulate(dims.begin(),dims.end(),1,std::multiplies<int>());
      boost::python::object obj(boost::python::handle<>(PyArray_FromDims(dims.size(),&dims[0],  num_util::getEnum<T>())));
      void *arr_data = PyArray_DATA((PyArrayObject*) obj.ptr());
      memcpy(arr_data, data, PyArray_ITEMSIZE((PyArrayObject*) obj.ptr()) * total);
      return boost::python::extract<boost::python::numeric::array>(obj);
    }
    

    // === Convert/map numpy to casa (no copy) ===================================
  
    /*!
      \brief Get a casa-array that has its storage mapped to a numpy array
    
      \param pydata -- boost-style "wrapped" numpy array
      \param casarray -- casa array that is to be mapped onthe the storage of the numpy array
    
      \return true if everyting worked
    */
    template <class T> bool casaFromNumpy(bpl::numeric::array pydata, 
					  casa::Array<T> &casarray ) {
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_type(pydata, num_util::getEnum<T>() );
      num_util::check_contiguous(pydata);
    
      // get the shape of the numpy-array
      int i,rank = num_util::rank(pydata);
      casa::IPosition shape(rank);
      for (i=0; i<rank; i++){
	shape[(rank-i-1)] = num_util::get_dim(pydata, i);
      };
    
      //now map the data
      casarray.takeStorage(shape, (T *)num_util::data(pydata), casa::SHARE);
    
      return true;
    };
    
    /*!
      \brief Get a casa-array with the storage of a resized numpy array
    
      \param pydata   -- boost-style "wrapped" numpy array
      \param casarray -- casa array that is to be mapped onthe the storage of the numpy array
      \param shape    -- resize the numpy array so that the remapped casa array has this shape
    
      \return true if everyting worked
    
      First the numpy array is resized to the shape given in the "shape" parameter, then the 
      storage is mapped by the casa array.
    */
    template <class T> bool casaFromNumpyResize(bpl::numeric::array &pydata, 
						casa::Array<T> &casarray,
						casa::IPosition &shape){
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_type(pydata, num_util::getEnum<T>() );
      num_util::check_contiguous(pydata);

      /* This doesn't work for the time being...
      // get the shape of the numpy-array
      int i,rank = shape.size();
      std::vector<int> outshape;
      for (i=0; i<rank; i++){
	outshape.push_back(shape[rank-i-1]);
      };
      pydata.resize(outshape);
      */
      // ugly fix to get things working:
      int rank=shape.size();
      if (rank == 1) {
	pydata.resize(shape[0]);
      } else if (rank == 2) {
	pydata.resize(shape[1], shape[0]);
      } else if (rank == 3) {
	pydata.resize(shape[2], shape[1], shape[0]);
      } else if (rank == 4) {
	pydata.resize(shape[3], shape[2], shape[1], shape[0]);
      } else if (rank == 5) {
	pydata.resize(shape[4], shape[3], shape[2], shape[1], shape[0]);
      } else {
	cerr << "casaFromNumpyResize: Arrays with more than 5 (or less than 1) dimensions not supported!" << endl;
      };

      //now map the data
      casarray.takeStorage(shape, (T *)num_util::data(pydata), casa::SHARE);
    
      return true;
    };

    // === Copy casa/stl to numpy =================================================
  
    /*!
      \brief Create a numpy array with the shape and (copied) values of a casa array
    
      \param casarray -- casa array that is to be copied into the new array
    
      \return boost-style "wrapped" numpy array
    */
    template <class T> bpl::numeric::array numpyFromCasa(const casa::Array<T> &casarray){
      casa::IPosition shape(casarray.shape());
      std::vector< int > outshape;
      int i,rank = shape.size();
      for (i=0; i<rank; i++){
	outshape.push_back(shape[rank-i-1]);
      };
      Bool  deleteIt;
      const T * datapointer(casarray.getStorage(deleteIt));
      bpl::numeric::array result(CR::PYCR::makeNum(datapointer,outshape));
      casarray.freeStorage(datapointer, deleteIt);
      return result;
    };


    /*!
      \brief Create a numpy array with the (copied) values of a STL-vector
    
      \param invec -- STL-vector that is to be copied into the new array
    
      \return boost-style "wrapped" numpy array
    */
    template <class T> bpl::numeric::array numpyFromSTL(const std::vector<T> invec){
      int size(invec.size());
      std::vector< int > outshape;
      outshape.push_back(size);
      bpl::numeric::array result(num_util::makeNum(outshape,num_util::getEnum<T>()));
      for (int i=0; i<size; i++){
	result[i] = invec[i];
      }
      return result;
    };


    // === Copy numpy/list to casa =================================================
  
    /*!
      \brief Create a casa vector with the size and (copied) values of a python list
    
      \param pylist -- boost-style wrapped Python list
    
      \return casa vector
    */
    template <class T> casa::Vector<T> casaFromList(bpl::list pylist){
      int i,size=PyList_Size(pylist.ptr());
      std::vector<T> outvec(size);
      for (i=0;i<size;++i) outvec[i] = bpl::extract<T>(pylist[i]);
      return outvec;
    }

    /*!
      \brief Create a casa vector with the size and (copied) values of a python object
    
      \param pyob -- boost-style wrapped python object. Must be a list or a numpy object
    
      \return casa vector
    */
    template <class T> casa::Vector<T> casaFromPyob(bpl::object &pyob){
      bpl::extract<bpl::numeric::array> nparray(pyob);
      if (nparray.check()) {
	casa::Vector<T> outvec;
	casaFromNumpy<T>(nparray,outvec);
	return outvec;
      };
      bpl::extract<bpl::list> listobj(pyob);
      if (listobj.check()) {
	return casaFromList<T>(listobj());
      };
      cout << "casaFromPyob: Unknown type of python object, not np-array or list.!" << endl;
      return casa::Vector<T>();
    }
    
    // === Copy numpy/list to STL-vector =================================================
    
    /*!
      \brief Create a STL vector with the size and (copied) values of a python list
      
      \param pylist -- boost-style wrapped Python list
    
      \return STL vector
    */
    template <class T> std::vector<T> STLVecFromNumpy(bpl::numeric::array nparray){
      std::vector<T> vec;
      int i,size=num_util::size(nparray); 
      vec.reserve(size);
      for (i=0;i<size;++i) vec[i] = bpl::extract<T>(nparray[i]);
      return vec;
    }

    /*!
      \brief Create a STL vector with the size and (copied) values of a python list
      
      \param pylist -- boost-style wrapped Python list
    
      \return STL vector
    */
    template <class T> std::vector<T> STLVecFromList(bpl::list pylist){
      std::vector<T> vec;
      int i,size=PyList_Size(pylist.ptr());
      vec.reserve(size);
      for (i=0;i<size;++i) vec[i] = bpl::extract<T>(pylist[i]);
      return vec;
    }

    /*!
      \brief Create a STL vector with the size and (copied) values of a python list
      
      \param pylist -- boost-style wrapped Python list
    
      \return STL vector
    */
    template <class T> std::vector<T> STLVecFromPyob(bpl::object &pyob){
      bpl::extract<bpl::numeric::array> nparray(pyob);
      if (nparray.check()) {
	return STLVecFromNumpy<T>(nparray());
      };
      bpl::extract<bpl::list> listobj(pyob);
      if (listobj.check()) {
	cout << "STLVecFromPyob: calling STLVecFromList" << endl;
	return STLVecFromList<T>(listobj());
      };
      cout << "STLVecFromPyob: Unknown type of python object, not np-array or list.!" << endl;
      return std::vector<T>();
    }

    // === Get pointers from numpy  =================================================

    /*!
      \Brief Return the pointer to the begin of the storage of a numpy array
    
      \param pydata   -- boost-style "wrapped" numpy array
    
      \return the pointer of the desired type
    
    */
    template <class T> T* numpyBeginPtr(bpl::numeric::array pydata){
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_type(pydata, num_util::getEnum<T>() );
      num_util::check_contiguous(pydata);
    
      return (T *)num_util::data(pydata);
    }
						    
    /*!
      \brief Return the pointer to the end of the storage of a numpy array
    
      \param pydata   -- boost-style "wrapped" numpy array
    
      \return the pointer of the desired type
    
    */
    template <class T> T* numpyEndPtr(bpl::numeric::array pydata){
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_type(pydata, num_util::getEnum<T>() );
      num_util::check_contiguous(pydata);

      int size = num_util::size(pydata);

      return (T *)num_util::data(pydata) + size;
    }
						    

  
  } // Namespace PYCR -- end
 
} // Namespace CR -- end

#endif /* PYCR_NUMPYCASA_H */
  
