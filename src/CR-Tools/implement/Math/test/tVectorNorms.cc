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
#include <iostream>
#include <string>
#include <Math/VectorNorms.h>

using std::cerr;
using std::cout;
using std::endl;

/*!
  \file tVectorNorms.cc

  \brief Collection of tests for the methods in VectorNorms

  \author Lars B&auml;hren

  \date 2007/01/22
 */

// -----------------------------------------------------------------------------

#ifdef HAVE_BLITZ

/*!
  \brief Some basic test to work with Blitz arrays

  Of special use is the application of <i>Range</i> objects to arrays;
  by this we can extracts e.g. rows from a matrix:
  \code
  for (uint n(0); n<nofRows; n++) {
    cout << arr(n,Range(Range::all())) << endl;
  }
  \endcode
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_blitz_functions ()
{
  cout << "\n[test_blitz_functions]\n" << endl;
  
  int nofFailedTests (0);
  
  int nofAntennas (5);
  int nofCoordinates (3);
  blitz::Array<double,2> arr (nofAntennas,nofCoordinates);
  
  arr = 11,12,13,21,22,23,31,32,33,41,42,43,51,52,53;
  
  uint nofRows = arr.rows();
  uint nofCols = arr.cols();
  
  cout << "[1] Basic properties of an array" << endl;
  try {
    cout                           << arr               << endl;
    cout << "arr.shape()       = " << arr.shape()       << endl;
    cout << "arr.rows()        = " << arr.rows()        << endl;
    cout << "arr.cols()        = " << arr.cols()        << endl;
    cout << "arr.numElements() = " << arr.numElements() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Extract rows using the Range operator" << endl;
  try {
    for (uint n(0); n<nofRows; n++) {
      cout << arr(n,blitz::Range(blitz::Range::all())) << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Extract columns using the Range operator" << endl;
  try {
    for (uint n(0); n<nofCols; n++) {
      cout << arr(blitz::Range(blitz::Range::all()),n) << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

#endif

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
  cout << "\n[test_Norms]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Computation using Blitz++ arrays" << endl;
  try {
    blitz::Array<double,1> vec (3);
    vec = 1,2,3;
    cout << "\t" << vec
	 << "\t" << CR::L1Norm(vec)
	 << "\t" << CR::L2Norm(vec)
	 << endl;
    vec = 2,0,0;
    cout << "\t" << vec
	 << "\t" << CR::L1Norm(vec)
	 << "\t" << CR::L2Norm(vec)
	 << endl;
    vec = 2,2,2;
    cout << "\t" << vec
	 << "\t" << CR::L1Norm(vec)
	 << "\t" << CR::L2Norm(vec)
	 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Computation using C++ arrays" << endl;
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
    
    cout << "[\t" << vec[0] << "\t" << vec[1] << "\t" << vec[2] << "]"
	 << "\t" << l1norm
	 << "\t" << l2norm
	 << endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;  
}

// -----------------------------------------------------------------------------

#ifdef HAVE_CASA

int test_casa_functions ()
{
  cout << "\n[test_casa_functions]\n" << endl;

  int nofFailedTests (0);
  int nelem (10);

  cout << "[1] Sign of vector elements..." << endl;
  try {
    casa::Vector<int> vecInt (nelem,1);
    casa::Vector<float> vecFloat (nelem,1);
    casa::Vector<double> vecDouble (nelem,1);

    casa::Vector<int> signVecInt = CR::sign (vecInt);
    casa::Vector<float> signVecFloat = CR::sign (vecFloat);
    casa::Vector<double> signVecDouble = CR::sign (vecDouble);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }

  cout << "[2] Inversion of vector elements order..." << endl;
  try {
    casa::Vector<int> vecInt (nelem,1);
    casa::Vector<float> vecFloat (nelem,1);
    casa::Vector<double> vecDouble (nelem,1);

    casa::Vector<int> invVecInt = CR::invertOrder (vecInt);
    casa::Vector<float> invVecFloat = CR::invertOrder (vecFloat);
    casa::Vector<double> invVecDouble = CR::invertOrder (vecDouble);
  } catch (std::string err) {
    cerr << err << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

#endif

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);

  {
    nofFailedTests += test_Norms ();
  }

#ifdef HAVE_BLITZ

  {
    nofFailedTests += test_blitz_functions ();
  }

#endif

#ifdef HAVE_CASA

  {
    nofFailedTests += test_casa_functions();
  }

#endif

  return nofFailedTests;  
}
