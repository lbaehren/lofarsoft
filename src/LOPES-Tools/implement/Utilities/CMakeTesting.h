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

#ifndef CMAKETESTING_H
#define CMAKETESTING_H

#include <iostream>

/*!
  \class MyArray
  
  \brief Test compiling a class depending on CASA array classes
  
  \author Lars B&auml;hren

  A simple class making use of the CASA array classes, used for testing how 
  to compile such code using CMake.
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

  /*!
    \brief Destructor
  */
  ~MyArray ();
  
  /*!
    \brief Get the stored vector
    
    \return vect -- The internally stored vector
  */
  casa::Array<T> vector () {
    return vect_p;
  }

  /*!
    \brief Set the internally stored array
    
    \param vect -- The vector to be stored internally
  */
  void setArray (casa::Array<T> & vect);
  
  /*!
    \brief Get the shape of the data array

    \return shape -- Shape of the internally stored array
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

#endif // CMAKETESTING_H
