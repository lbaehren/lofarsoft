/**************************************************************************
 *  This file is part of the Transient Template Library.                  *
 *  Copyright (C) 2010 Andreas Horneffer <A.Horneffer@astro.ru.nl>        *
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

#ifndef TTL_NUMPY_PTR_H
#define TTL_NUMPY_PTR_H

// SYSTEM INCLUDES
#include <string>

// PROJECT INCLUDES
#include <boost/python/numeric.hpp>

// Needed for num_util
#ifndef PY_ARRAY_UNIQUE_SYMBOL
  #define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#endif

#define NO_IMPORT_ARRAY

#include <num_util.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

namespace ttl
{
  /*!
    \brief Exception for type errors.
   */
  class TypeError
  {
  public:
      TypeError()
      {
        message = "Argument type not implemented.";
      };

      TypeError(std::string m)
      {
        message = m;
      };

      ~TypeError(){};

      std::string message;
  };
  
  /*!
    \brief Exception translator for type errors.
   */
  void exceptionTranslator(TypeError const& e)
  {
    PyErr_SetString(PyExc_UserWarning, e.message.c_str());
  }
  
  /*!
    \Brief Return the pointer to the begin of the storage of a numpy array
  
    \param pydata   -- boost-style "wrapped" numpy array
  
    \return the pointer of the desired type
  
  */
  template <class T> T* numpyBeginPtr(boost::python::numeric::array pydata)
  {
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
  template <class T> T* numpyEndPtr(boost::python::numeric::array pydata)
  {
    // test if the numpy-array has the preperties we need
    // (they throw exceptions if not)
    num_util::check_type(pydata, num_util::getEnum<T>() );
    num_util::check_contiguous(pydata);

    int size = num_util::size(pydata);

    return (T *)num_util::data(pydata) + size;
  }
} // End ttl

#endif // TTL_NUMPY_PTR_H

