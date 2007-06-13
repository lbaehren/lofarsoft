/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <iostream>
#include <string>

#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Math.h>

#include <../implement/templates.h>

/*!
  \file tUseCasa.cc

  \ingroup CR-Pipeline

  \brief A simple test for external build against libcasa

  \author Lars B&auml;hren

  \date 2007/01/26

  This is the first part in a series of test to explore building against an
  existing CASA installation using CMake
*/

// ------------------------------------------------------------------ test_Arrays

/*!
  \brief Test working with the array classes

  \return nofFailedTests -- The number of failed tests
*/

int test_Arrays ()
{
  int nofFailedTests (0);

  std::cout << "[1] Testing IPosition class..." << std::endl;
  try {
    casa::IPosition shape (2,10,20);
    //
    std::cout << " - shape = " << shape << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing Array class..." << std::endl;
  try {
    casa::IPosition shape (2,10,20);
    casa::Array<double> arr (shape);
    //
    std::cout << " - shape(arr) = " << arr.shape() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing Array class..." << std::endl;
  try {
    casa::Vector<double> vec (10);
    casa::Matrix<double> mat (10,10);
    //
    std::cout << " - shape(vec) = " << vec.shape() << std::endl;
    std::cout << " - shape(mat) = " << mat.shape() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] Test slicing arrays..." << std:: endl;
  try {
    unsigned int n(0);
    unsigned int nelem (10);
    casa::Vector<double> vect (nelem);

    // fill the vector
    for (n=0; n<nelem; n++) {
      vect(n) = double(n);
    }

    // display slices of the vector
    for (n=0; n<nelem/2+1; n++) {
      std::cout << "\t" << vect(casa::Slice(n,nelem/2)) << std::endl;
    }

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// --------------------------------------------------------------- test_BasicMath

/*!
  \brief Test working with basic math related classes

  return nofFailedTests -- The number of failed tests
*/
int test_BasicMath ()
{
  int nofFailedTests (0);


  return nofFailedTests;
}

// ------------------------------------------------------------------------- main

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_Arrays();
  }
  
  return nofFailedTests;
}
