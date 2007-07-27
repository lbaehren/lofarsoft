/*-------------------------------------------------------------------------*
 | $Id:: tUseCASA.cc 392 2007-06-13 10:38:12Z baehren                    $ |
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
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Math.h>

#include <../implement/templates.h>

using std::cout;
using std::cerr;
using std::endl;

using casa::Array;
using casa::Cube;
using casa::IPosition;
using casa::Matrix;
using casa::Slice;
using casa::Slicer;
using casa::Vector;

/*!
  \file tUseArrays.cc

  \ingroup CR-Pipeline

  \brief Test various operations on CASA arrays

  \author Lars B&auml;hren

  \date 2007/07/26

  The code for the cosmic rays data analysis makes extensive use of the CASA
  array classes a prominent data container during computation.
 
  <h3>Prerequisite</h3>
 
  
*/

// ----------------------------------------------------------- test_construction

/*!
  \brief A number of simple tests for cunstructing arrays

  \return nofFailedTests -- The number of failed tests
*/

int test_construction ()
{
  cout << "\n[test_construction]\n" << endl;

  int nofFailedTests (0);
  int nelem (10);
  
  cout << "[1] Construct Array via IPosition shape ..." << endl;
  try {
    Array<double> arr1D (IPosition(1,nelem));
    cout << " --> Array<double> = " << arr1D.shape() << endl;
    Array<double> arr2D (IPosition(2,nelem));
    cout << " --> Array<double> = " << arr2D.shape() << endl;
    Array<double> arr3D (IPosition(3,nelem));
    cout << " --> Array<double> = " << arr3D.shape() << endl;
    Array<double> arr4D (IPosition(4,nelem));
    cout << " --> Array<double> = " << arr4D.shape() << endl;
    Array<double> arr5D (IPosition(5,nelem));
    cout << " --> Array<double> = " << arr5D.shape() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Construct Array from specialized objects ..." << endl;
  try {
    Vector<double> vector (nelem);
    Matrix<double> matrix (nelem,nelem);
    Cube<double> cube (nelem,nelem,nelem);
    cout << " -- Creating Array from Vector ..." << endl;
    Array<double> arr1D (vector);
    cout << " -- Creating Array from Matrix ..." << endl;
    Array<double> arr2D (matrix);
    cout << " -- Creating Array from Cube ..." << endl;
    Array<double> arr3D (cube);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Construct Array from slicing another one ..." << endl;
  try {
    // the original array
    Array<double> arr5D (IPosition(5,nelem));
    // the slicer
    Slicer slice (IPosition(5,0),
		  IPosition(5,1,1,nelem,nelem,nelem),
		  IPosition(5,1),
		  Slicer::endIsLength);
    // the new array obtained from slicing through the original
    Array<double> slice5D(arr5D(slice));
    Array<double> slice3D(arr5D(slice).nonDegenerate(0));
    // Summary of the array properties
    cout << " -- original array                     = " << arr5D.shape()   << endl;
    cout << " -- slicing result w/o axis removal    = " << slice5D.shape() << endl;
    cout << " -- slicing result w/o degenerate axes = " << slice3D.shape() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ---------------------------------------------------------------- test_slicing

/*!
  \brief Test various slicing operations on arrays

  Instead of iterating through array on a per-pixel basis, one can use a Slicer
  in order to address/manipulate/extract a subset of an array. Typical cases for
  such operations are e.g. the access to a row/column of a matrix or to a plane
  of a cube.

  \return nofFailedTests -- The number of failed tests
*/

int test_slicing ()
{
  int nofFailedTests (0);
  int nelem (10);

  cout << "[1] Write vector into rows of a matrix ..." << endl;
  try {
    Vector<double> vec (nelem);
    Matrix<double> mat (nelem,nelem);

    for (int n(0); n<nelem; n++) {
      vec = double(n);
      //
      cout << " --> " << vec << endl;
      //
      mat.row(n) = vec;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Write Matrix into plane of a cube ..." << endl;
  try {
    int n(0);
    Matrix<double> mat (nelem,nelem);
    Cube<double> cube (nelem,nelem,nelem);

    cout << " --> writing x-y planes via xyPlane() function ..." << endl;
    for (n=0; n<nelem; n++) {
      mat = double(n);
      cube.xyPlane(n) = mat;
    }

    cout << " --> writing x-y planes via array Slicer ..." << endl;
    for (n=0; n<nelem; n++) {
      mat = double(n);
      cube (Slice(), Slice(), n) = mat;
    }

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
//   try {
//   } catch (std::string message) {
//     std::cerr << message << endl;
//     nofFailedTests++;
//   }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------ main

/*!
  \brief The main routine of the test program
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);

  nofFailedTests += test_construction();
  nofFailedTests += test_slicing();
  
  return nofFailedTests;
}
