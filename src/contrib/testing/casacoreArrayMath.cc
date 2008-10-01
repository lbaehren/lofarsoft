/*-------------------------------------------------------------------------*
 | $Id:: TimeFreqSkymap.cc 1967 2008-09-09 13:16:22Z baehren             $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>

using std::cout;
using std::endl;

using casa::Array;
using casa::Matrix;
using casa::Vector;

// ------------------------------------------------------------------------------

/*!
  \file casacoreArrayMath.cc

  \ingroup contrib

  \brief Collection of tests for mathematical operations on casacore arrays

  \author Lars B&auml;hren

  Nothing really exciting here, only a small number of tests on how to
  perform mathematical operations on/with arrays of types casa::Vector<T>,
  casa::Matrix<T>.
*/

// ------------------------------------------------------------------------------

/*!
  \brief Test mathematical operations using object of type casa::Vector<T>
*/
int test_VectorMath (int const &nelem=10)
{
  cout << "\n[casacoreArrayMath::test_VectorMath]\n" << endl;

  int nofFailedTests (0);

  Vector<int> vec_int (nelem);
  Vector<uint> vec_uint (nelem);
  Vector<float> vec_float (nelem);
  Vector<double> vec_double (nelem);

  for (int n(0); n<nelem; n++) {
    vec_int(n)    = n;
    vec_uint(n)   = uint(n);
    vec_float(n)  = float(n);
    vec_double(n) = double(n);
  }

  cout << "[1] Testing operations for Vector<int> ..." << endl;
  try {
    cout << "-- min(x) = " << min(vec_int) << endl;
    cout << "-- max(x) = " << max(vec_int) << endl;
    cout << "-- sin(x) = " << sin(vec_int) << endl;
    cout << "-- cos(x) = " << cos(vec_int) << endl;
    cout << "-- tan(x) = " << tan(vec_int) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Testing operations for Vector<uint> ..." << endl;
  try {
    cout << "-- min(x) = " << min(vec_uint) << endl;
    cout << "-- max(x) = " << max(vec_uint) << endl;
    cout << "-- sin(x) = " << sin(vec_uint) << endl;
    cout << "-- cos(x) = " << cos(vec_uint) << endl;
    cout << "-- tan(x) = " << tan(vec_uint) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Testing operations for Vector<float> ..." << endl;
  try {
    cout << "-- min(x) = " << min(vec_float) << endl;
    cout << "-- max(x) = " << max(vec_float) << endl;
    cout << "-- sin(x) = " << sin(vec_float) << endl;
    cout << "-- cos(x) = " << cos(vec_float) << endl;
    cout << "-- tan(x) = " << tan(vec_float) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[4] Testing operations for Vector<double> ..." << endl;
  try {
    cout << "-- min(x) = " << min(vec_double) << endl;
    cout << "-- max(x) = " << max(vec_double) << endl;
    cout << "-- sin(x) = " << sin(vec_double) << endl;
    cout << "-- cos(x) = " << cos(vec_double) << endl;
    cout << "-- tan(x) = " << tan(vec_double) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test mathematical operations using object of type casa::Matrix<T>
*/
int test_MatrixMath (int const &nelem=10)
{
  cout << "\n[casacoreArrayMath::test_MatrixMath]\n" << endl;

  int nofFailedTests (0);
  int n(0);
  int row (0);
  int col (0);

  Matrix<int> mat_int (nelem,nelem);
  Matrix<float> mat_float (nelem,nelem);
  Matrix<double> mat_double (nelem,nelem);

  for (row=0; row<nelem; row++) {
    for (col=0; col<nelem; col++) {
      mat_int(row,col)    = int (n);
      mat_float(row,col)  = float (n);
      mat_double(row,col) = double (n);
      //
      n++;
    }
  }

  cout << "[1] Testing operations for Matrix<int> ..." << endl;
  try {
    cout << "-- min(x) = " << min(mat_int) << endl;
    cout << "-- max(x) = " << max(mat_int) << endl;
    cout << "-- sin(x) = " << sin(mat_int) << endl;
    cout << "-- cos(x) = " << cos(mat_int) << endl;
    cout << "-- tan(x) = " << tan(mat_int) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Testing operations for Matrix<float> ..." << endl;
  try {
    cout << "-- min(x) = " << min(mat_float) << endl;
    cout << "-- max(x) = " << max(mat_float) << endl;
    cout << "-- sin(x) = " << sin(mat_float) << endl;
    cout << "-- cos(x) = " << cos(mat_float) << endl;
    cout << "-- tan(x) = " << tan(mat_float) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[4] Testing operations for Matrix<double> ..." << endl;
  try {
    cout << "-- min(x) = " << min(mat_double) << endl;
    cout << "-- max(x) = " << max(mat_double) << endl;
    cout << "-- sin(x) = " << sin(mat_double) << endl;
    cout << "-- cos(x) = " << cos(mat_double) << endl;
    cout << "-- tan(x) = " << tan(mat_double) << endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}
// ------------------------------------------------------------------------------

int main (int argc,char* argv[])
{
  int nofFailedTests (0);
  int nelem (10);

  nofFailedTests += test_VectorMath (nelem);
  nofFailedTests += test_MatrixMath (nelem);

  return nofFailedTests;
}
