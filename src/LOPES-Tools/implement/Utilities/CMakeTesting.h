/*!
  \file CMakeTesting.cc

  \brief A collection and classes for testing compiling with CMake

  \author Lars B&auml;hren

  \date 2006/11/20
*/

#include <iostream>

/*!
  \class MyArray
  
  \brief Test compiling a class depending on CASA array classes
  
  \author Lars B&auml;hren
*/
#ifdef HAVE_CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>

template <class T> class MyArray {
  
  // The vector itself
  casa::Array<T> vect_p;
  //! The number of lements in the vector
  uint nofElements_p;
  
 public:
  
  /*!
    \brief Default constructor
  */
  MyArray ();
  
  /*!
    \brief Argumented constructor
    
    \param vect -- The vector to be stored internally
  */
  MyArray (casa::Array<T> & vect);

  ~MyArray ();
  
  /*!
    \brief Get the stored vector

    \return vect -- The internally stored vector
  */
  casa::Array<T> vector () {
    return vect_p;
  }
  
  void setArray (casa::Array<T> & vect);
  
  /*!
    \brief Get the shape of the data array
  */
  casa::IPosition shape() {
    return vect_p.shape();
  }

  /*!
    \brief Provide a summary of the stored vector
  */
  void summary () {
    std::cout << " Number of elements : " << nofElements_p << std::endl;
  }

};

#endif
