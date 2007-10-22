/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <cmath>
#include <string>

#include <Imaging/CoordinateConversion.h>
#include <Math/VectorConversion.h>

/*!
  \file tCoordinateConversion.cc

  \brief Collection of tests for the methods in CoordinateConversion

  \author Lars B&auml;hren

  \date 2007/01/22
 */

/*!
  \brief Some basic test to work with Blitz++ arrays

  Of special use is the application of <i>Range</i> objects to arrays;
  by this we can extracts e.g. rows from a matrix:
  \code
  for (uint n(0); n<nofRows; n++) {
    std::cout << arr(n,Range(Range::all())) << std::endl;
  }
  \endcode
  
  \return nofFailedTests -- The number of failed tests.
*/
#ifdef HAVE_BLITZ
int test_blitz ()
{
  std::cout << "\n[test_blitz]\n" << std::endl;

  int nofFailedTests (0);

  int nofAntennas (5);
  int nofCoordinates (3);
  blitz::Array<double,2> arr (nofAntennas,nofCoordinates);

  arr = 11,12,13,21,22,23,31,32,33,41,42,43,51,52,53;
  
  uint nofRows = arr.rows();
  uint nofCols = arr.cols();

  std::cout << "[1] Basic properties of an array" << std::endl;
  try {
    std::cout                           << arr               << std::endl;
    std::cout << "arr.shape()       = " << arr.shape()       << std::endl;
    std::cout << "arr.rows()        = " << arr.rows()        << std::endl;
    std::cout << "arr.cols()        = " << arr.cols()        << std::endl;
    std::cout << "arr.numElements() = " << arr.numElements() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Extract rows using the Range operator" << std::endl;
  try {
    for (uint n(0); n<nofRows; n++) {
      std::cout << arr(n,blitz::Range(blitz::Range::all())) << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Extract columns using the Range operator" << std::endl;
  try {
    for (uint n(0); n<nofCols; n++) {
      std::cout << arr(blitz::Range(blitz::Range::all()),n) << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from other to cartesian coordinates

  Test for the routines handling the conversions: \f$(r,\theta,\varphi)
  \rightarrow (x,y,z)\f$

  \return nofFailedTests -- The number of failed tests
*/
int test_convert2cartesian ()
{
  std::cout << "\n[test_convert2cartesian]\n" << std::endl;

  int nofFailedTests (0);
  
  double r (1.0);
  double phi (CR::deg2rad(45.0));
  double theta (CR::deg2rad(90));
  double x (0);
  double y (0);
  double z (0);

  std::cout << "[1] Test working with Blitz++ arrays ..." << std::endl;
#ifdef HAVE_BLITZ
  try {
    blitz::Array<double,1> xyz (3);

    CR::spherical2cartesian(x,y,z,r,phi,theta,false);

    std::cout << "[" << r << "," << phi << "," << theta << "]\t->\t"
	      << "[" << x << "," << y << "," << z << "]" << std::endl;

    blitz::Array<double,1> spherical (3);
    blitz::Array<double,1> cartesian (3);

    spherical = r,phi,theta;

    CR::spherical2cartesian (cartesian,spherical);

    std::cout << spherical << std::endl;
    std::cout << cartesian << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

#ifdef HAVE_BLITZ
  nofFailedTests += test_blitz ();
#endif

  nofFailedTests += test_convert2cartesian ();

  return nofFailedTests;  
}
