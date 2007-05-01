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

/*!
  \file tCoordinateConversion.cc

  \brief Collection of tests for the methods in CoordinateConversion

  \author Lars B&auml;hren

  \date 2007/01/22
 */

/*!
  \brief Some basic test to work with Blitz arrays

  Of special use is the application of <i>Range</i> objects to arrays;
  by this we can extracts e.g. rows from a matrix:
  \code
  for (uint n(0); n<nofRows; n++) {
    std::cout << arr(n,Range(Range::all())) << std::endl;
  }
  \endcode
  
  \return nofFailedTests -- The number of failed tests.
*/
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

// -----------------------------------------------------------------------------

/*!
  \brief Test the functions for computation of the vector norms

  For the example vector \f$\vec x = (1,2,3)\f$
  <table>
    <tr>
      <td>Name</td>
      <td>Symbols</td>
      <td>Value</td>
      <td>approx.</td>
    </tr>
    <tr>
      <td>\f$L^1\f$-norm</td>
      <td>\f$|\vec x|_{1}\f$</td>
      <td>6</td>
      <td>6.000</td>
    </tr>
    <tr>
      <td>\f$L^2\f$-norm</td>
      <td>\f$|\vec x|_{2}\f$</td>
      <td>\f$\sqrt{14}\f$</td>
      <td>3.742</td>
    </tr>
    <tr>
      <td>\f$L^3\f$-norm</td>
      <td>\f$|\vec x|_{3}\f$</td>
      <td>\f$6^{2/3}\f$</td>
      <td>3.302</td>
    </tr>
  </table>

  \return nofFailedTests -- The number of failed tests
*/
int test_Norms ()
{
  std::cout << "\n[test_Norms]\n" << std::endl;

  int nofFailedTests (0);

  std::cout << "[1] Computation using Blitz++ arrays" << std::endl;
  try {
    blitz::Array<double,1> vec (3);
    vec = 1,2,3;
    std::cout << "\t" << vec
	      << "\t" << CR::L1Norm(vec)
	      << "\t" << CR::L2Norm(vec)
	      << std::endl;
    vec = 2,0,0;
    std::cout << "\t" << vec
	      << "\t" << CR::L1Norm(vec)
	      << "\t" << CR::L2Norm(vec)
	      << std::endl;
    vec = 2,2,2;
    std::cout << "\t" << vec
	      << "\t" << CR::L1Norm(vec)
	      << "\t" << CR::L2Norm(vec)
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Computation using C++ arrays" << std::endl;
  try {
    unsigned int nelem (3);
    double *vec;
    double l1norm (0.0);
    double l2norm (0.0);

    vec = new double [nelem];
    
    vec[0] = 1;
    vec[1] = 2;
    vec[2] = 3;
    
    l1norm = CR::L1Norm (vec,nelem);
    l2norm = CR::L2Norm (vec,nelem);

    std::cout << "[\t" << vec[0] << "\t" << vec[1] << "\t" << vec[2] << "]"
	      << "\t" << l1norm
	      << "\t" << l2norm
	      << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;  
}

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
  blitz::Array<double,1> xyz (3);

  try {
    double r (1.0);
    double phi (CR::deg2rad(45.0));
    double theta (CR::deg2rad(90));
    double x (0);
    double y (0);
    double z (0);

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

  {
    nofFailedTests += test_blitz ();
  }

  {
    nofFailedTests += test_Norms ();
  }

  {
    nofFailedTests += test_convert2cartesian ();
  }

  return nofFailedTests;  
}
