/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef TESTARRAY_H
#define TESTARRAY_H

#include <iostream>

#ifdef HAVE_CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>

namespace LOPES {  // namespace LOPES -- begin

/*!
  \class TestArray
  
  \brief Test compiling a class depending on CASA array classes
  
  \author Lars B&auml;hren
  
  \test tTestArray.cc
  
  A simple class making use of the CASA array classes, used for testing how 
  to compile such code using CMake.
*/
template <class T> class TestArray {
  
  // The array itself
  casa::Array<T> array_p;
  //! The number of elements in the array
  uint nofElements_p;
  
 public:
  
  /*!
    \brief Default constructor
    
    This will simply create an array of shape [1] and contents zero.
  */
  TestArray ();
  
  /*!
    \brief Argumented constructor
    
    \param vect -- The array to be stored internally
  */
  TestArray (casa::Array<T> & vect);
  
  /*!
    \brief Destructor
  */
  ~TestArray ();
  
  /*!
    \brief Get the stored array
    
    \return vect -- The internally stored array
  */
  casa::Array<T> array () {
    return array_p;
  }

  /*!
    \brief Set the internally stored array
    
    \param vect -- The array to be stored internally
  */
  void setArray (casa::Array<T> & vect);
  
  /*!
    \brief Get the shape of the data array

    \return shape -- Shape of the internally stored array
  */
  casa::IPosition shape() {
    return array_p.shape();
  }

  /*!
    \brief Provide a summary of the stored array
  */
  void summary ();

 };  // class TestArray -- end

}  // namespace LOPES -- end

#endif

#endif // TESTARRAY_H
