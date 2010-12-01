#ifndef NUMPY_PTR_H
#define NUMPY_PTR_H

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

/*!
  \Brief Return the pointer to the begin of the storage of a numpy array

  \param pydata   -- boost-style "wrapped" numpy array

  \return the pointer of the desired type

*/
template <class T> T* numpyBeginPtr(boost::python::numeric::array pydata)
{
  // test if the numpy-array has the preperties we need
  // (they throw exceptions if not)
  //num_util::check_type(pydata, num_util::getEnum<T>() );
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
  //num_util::check_type(pydata, num_util::getEnum<T>() );
  num_util::check_contiguous(pydata);

  int size = num_util::size(pydata);

  return (T *)num_util::data(pydata) + size;
}

#endif // NUMPY_PTR_H

