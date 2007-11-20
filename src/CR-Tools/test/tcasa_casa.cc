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

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include <assert.h>
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Slice.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/List.h>
#include <casa/Containers/ListIO.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

#include "tests_common.h"

using casa::Array;
using casa::Block;
using casa::IPosition;
using casa::Vector;
using casa::Matrix;
using casa::Cube;
using casa::Slice;

using std::cout;
using std::endl;

/*!
  \file tcasa_casa.cc

  \brief A number of tests for clases in the casacore casa module

  \ingroup CR_test

  \author Lars B&auml;hren

  <u>Note:</u> A number of the tests performed will not work with the original CASA
  libraries.
*/

// ==============================================================================
//
//  Function declarations
//
// ==============================================================================

/*!
  \brief Tests for classes in casa/Arrays

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays ();

/*!
  \brief Test for casa/Arrays/Matrix class

  [1] Element-wise writing <br>
  [2] Column-/Row-wise writing <br>
  [3] Column-/Row-wise writing of a vector <br>
  [4] Column-/Row-wise writing using Slicer

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Matrix (std::vector<int> const &nelem);

/*!
  \brief Test for casa/Arrays/Cube class

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Cube (std::vector<int> const &nelem);

/*!
  \brief Test for casa/Arrays/Array class

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Array (std::vector<int> const &nelem);

/*!
  \brief Tests for classes in casa/BasicMath

  \return nofFailedTests -- The number of failed test within this function
*/
int test_BasicMath ();

/*!
  \brief Tests for classes in casa/BasicSL

  \return nofFailedTests -- The number of failed test within this function
*/
int test_BasicSL ();

/*!
  \brief Tests for classes in casa/Containers

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Containers ();

// ==============================================================================
//
//  Implementation of the functions
//
// ==============================================================================

// ------------------------------------------------------------------------------

int test_Arrays ()
{
  cout << "\nTesting classes in casa/Arrays ...\n" << endl;

  int nofFailedTests (0);
  uint nelem (20);

  cout << "[1] Testing casa/Arrays/IPosition ..." << endl;
  try {
    IPosition shapeVector (1,nelem);
    IPosition shapeMatrix (2,nelem,nelem);
    IPosition shapeCube (3,nelem,nelem,nelem);
    // show IPosition vectors
    cout << " -- IPosition(1) = " << shapeVector << endl;
    cout << " -- IPosition(2) = " << shapeMatrix << endl;
    cout << " -- IPosition(3) = " << shapeCube   << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Testing casa/Arrays/Vector ..." << endl;
  try {
    IPosition shape (1,nelem);
    // Vector
    Vector<int> vectorInt (shape);
    Vector<float> vectorFloat (shape);
    Vector<double> vectorDouble (shape);
    // show array shapes
    cout << " -- shape(Vector<int>)    = " << vectorInt.shape()    << endl;
    cout << " -- shape(Vector<float>)  = " << vectorFloat.shape()  << endl;
    cout << " -- shape(Vector<double>) = " << vectorDouble.shape() << endl;
    // assign values to individual positions
    for (uint n(0); n<nelem; n++) {
      vectorInt(n)   = n;
      vectorFloat(n) = 0.5*n;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Testing casa/Arrays/Matrix ..." << endl;
  try {
    IPosition shape (2,nelem,nelem);
    // Matrix
    Matrix<int> matrixInt (shape);
    Matrix<float> matrixFloat (shape);
    Matrix<double> matrixDouble (shape);
    // show array shapes
    cout << " -- shape(Matrix<int>)    = " << matrixInt.shape()    << endl;
    cout << " -- shape(Matrix<float>)  = " << matrixFloat.shape()  << endl;
    cout << " -- shape(Matrix<double>) = " << matrixDouble.shape() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Testing casa/Arrays/Cube ..." << endl;
  try {
    IPosition shape (3,nelem,nelem,nelem);
    // Cube
    Cube<int> cubeInt (shape);
    Cube<float> cubeFloat (shape);
    Cube<double> cubeDouble (shape);
    // show array shapes
    cout << " -- shape(Cube<int>)    = " << cubeInt.shape()    << endl;
    cout << " -- shape(Cube<float>)  = " << cubeFloat.shape()  << endl;
    cout << " -- shape(Cube<double>) = " << cubeDouble.shape() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Testing casa/Arrays/Array ..." << endl;
  try {
    int nofAxes (5);
    IPosition shape (nofAxes);

    for (int n(0); n<nofAxes; n++) {
      shape(n) = (n+1)*5;
    }

    Array<int> arrayInt (shape);

    // show array shapes
    cout << " -- shape(Array<int>) = " << arrayInt.shape()    << endl;
    cout << " -- nof elements      = " << arrayInt.nelements() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[6] Testing access to array elements ..." << endl;
  try {
    uint n(0);
    uint nofAxes (3);
    uint nelem (100);
    IPosition shape (nofAxes,nelem,nelem,nelem);
    Array<double> arr (shape);

    /* nest the loops in the order of the array axes */
    for (shape(0)=0; shape(0)<nelem; shape(0)++) {
      for (shape(1)=0; shape(1)<nelem; shape(1)++) {
	for (shape(2)=0; shape(2)<nelem; shape(2)++) {
	  arr (shape) = 1.0;
	}
      }
    }

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[7] Test slicing of arrays ..." << endl;
  try {
    IPosition lengths(3,10,20,30);
    Array<int> ai(lengths);
    Array<int> subArray;
    IPosition blc(3,0,0,0);
    IPosition trc(3,5,5,5);
    // set up the sub-array as reference to a subset of the original array
    subArray.reference(ai(blc, trc));
    // Feedback
    cout << "-- shape of the original array = " << ai.shape() << endl;
    cout << "-- shape of the sub array      = " << subArray.shape() << endl;
    // manipulate the selected elements in the original array
    subArray = 10;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_Arrays_Matrix (std::vector<int> const &nelem)
{
  cout << "\nTesting casa/Arrays/Matrix ...\n" << endl;

  int nofFailedTests (0);
  clock_t start;
  clock_t end;
  double runtimes[2];
  IPosition shape (2);
  IPosition pos(2);

  std::cout << "[1] Element-wise writing ..." << std::endl;
  try {
    for (uint n(0); n<nelem.size(); n++) {
      /* Create the matrix */
      shape = IPosition (2,nelem[n],nelem[n]);
      Matrix<double> arr (shape);
      
      /* Element-wise addressing in axis order */
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  arr(pos) = 1.0;
	}
      }
      end = clock();
      runtimes[0] = runtime (start,end);
      
      /* Element-wise addressing in opposite axis order */
      
      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  arr(pos) = 1.0;
	}
      }
      end = clock();
      runtimes[1] = runtime (start,end);
      
      /* Show timing information */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1] << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Column-/Row-wise writing ..." << std::endl;
  try {
    for (uint n(0); n<nelem.size(); n++) {
      /* Create the matrix */
      shape = IPosition (2,nelem[n],nelem[n]);
      Matrix<double> arr (shape);

      // looping over matrix rows

      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	arr.row(pos(0)) = 1.0;
      }
      end = clock();
      runtimes[0] = runtime (start,end);

      // looping over matrix columns

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.column(pos(1)) = 1.0;
      }
      end = clock();
      runtimes[1] = runtime (start,end);

      /* Show timing information */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1] << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Column-/Row-wise writing of a vector ..." << std::endl;
  try {
    for (uint n(0); n<nelem.size(); n++) {
      /* Create the matrix */
      shape = IPosition (2,nelem[n],nelem[n]);
      Matrix<double> arr (shape);
      Vector<double> vec (nelem[n],1.0);

      // looping over matrix rows

      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	arr.row(pos(0)) = vec;
      }
      end = clock();
      runtimes[0] = runtime (start,end);

      // looping over matrix columns

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.column(pos(1)) = vec;
      }
      end = clock();
      runtimes[1] = runtime (start,end);

      /* Show timing information */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1] << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[4] Column-/Row-wise writing using Slicer ..." << std::endl;
  try {
    for (uint n(0); n<nelem.size(); n++) {
      /* Set matrix shape */
      shape = IPosition (2,nelem[n],nelem[n]);
      /* Create the matrix */
      Matrix<double> arr (shape);
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_Arrays_Cube (std::vector<int> const &nelem)
{
  cout << "\nTesting casa/Arrays/Cube ...\n" << endl;

  int nofFailedTests (0);
  uint nelemMax (6);
  clock_t start;
  clock_t end;
  double runtimes[6];
  IPosition shape (3);
  IPosition pos(3);

  std::cout << "[1] Element-wise writing ..." << std::endl;
  std::cout << "\tnelem" 
	    << "\t(0,1,2)\t(0,2,1)"
	    << "\t(1,0,2)\t(1,2,0)"
	    << "\t(2,0,1)\t(2,1,0)"
	    << std::endl;
  try {
    for (uint n(0); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n],nelem[n],nelem[n]);
      /* Create Cube object */
      Cube<double> arr (shape);
      
      /* Axis/Loop order: 0,1,2 -- loops in order of array axes */
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[0] = runtime (start,end);
      
      /* Axis/Loop order: 0,2,1 */
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[1] = runtime (start,end);
      
      /* Axis/Loop order: 1,0,2 */
      
      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[2] = runtime (start,end);
      
      /* Axis/Loop order: 1,2,0 */
      
      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[3] = runtime (start,end);
      
      /* Axis/Loop order: 2,0,1 */
      
      start = clock();
      for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[4] = runtime (start,end);
      
      /* Axis/Loop order: 2,1,0 -- loops in inverted order of array axes */
      
      start = clock();
      for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    arr(pos) = 1.0;
	  }
	}
      }
      end = clock();
      runtimes[5] = runtime (start,end);
      
      /* Show timing information */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1]
		<< "\t" << runtimes[2]
		<< "\t" << runtimes[3]
		<< "\t" << runtimes[4]
		<< "\t" << runtimes[5]
		<< std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Write values to planes of the cube ..." << std::endl;
  std::cout << "\tnelem"
	    << "\t(y-z)"
	    << "\t(x-z)"
	    << "\t(x-y)"
	    << std::endl;
  try {
    uint i (0);
    for (uint n(0); n<nelemMax; n++) {
      /* Create the matrix */
      shape = IPosition (3,nelem[n],nelem[n],nelem[n]);
      Cube<double> arr (shape);

      /* Write values y-z-planes */

      start = clock();
      for (i=0; i<nelem[n]; i++) {
	arr.yzPlane(i) = 1.0;
      }
      end = clock();
      runtimes[0] = runtime(start,end);

      /* Write values x-z-planes */

      start = clock();
      for (i=0; i<nelem[n]; i++) {
	arr.xzPlane(i) = 1.0;
      }
      end = clock();
      runtimes[1] = runtime(start,end);

      /* Write values x-y-planes */

      start = clock();
      for (i=0; i<nelem[n]; i++) {
	arr.xyPlane(i) = 1.0;
      }
      end = clock();
      runtimes[2] = runtime(start,end);

      /* Summary */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1]
		<< "\t" << runtimes[2]
		<< std::endl;

    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[3] Insert values of Matrix into y-z planes ..." << std::endl;
  std::cout << "\tnelem"
	    << "\t(2,1,0)"
	    << "\t(0,2,1)"
	    << "\t(y-z)"
	    << std::endl;
  try {

    for (uint n(0); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n],nelem[n],nelem[n]);
      /* Create Matrix object */
      Matrix<double> mat (nelem[n],nelem[n],0.0);
      /* Create Cube object */
      Cube<double> arr (shape); 
      
      /* write values into y-z planes */
      
      start = clock();
      for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    arr(pos) = mat(pos(1),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[0] = runtime (start,end);
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    arr(pos) = mat(pos(1),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[1] = runtime (start,end);
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	arr.yzPlane(pos(0)) = mat;
      }
      end = clock();
      runtimes[2] = runtime (start,end);

      /* Summary */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1]
		<< "\t" << runtimes[2]
		<< std::endl;

    }
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Insert values of Matrix into x-z planes ..." << std::endl;
  std::cout << "\tnelem"
	    << "\t(2,0,1)"
	    << "\t(1,2,0)"
	    << "\t(x-z)"
	    << std::endl;
  try {

    for (uint n(0); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n],nelem[n],nelem[n]);
      /* Create Matrix object */
      Matrix<double> mat (nelem[n],nelem[n],0.0);
      /* Create Cube object */
      Cube<double> arr (shape); 
      
      /* write values into x-z planes - iteration axis last */
      
      start = clock();
      for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    arr(pos) = mat(pos(0),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[0] = runtime (start,end);
      
      /* write values into x-z planes - iteration axis first */
      
      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    arr(pos) = mat(pos(0),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[1] = runtime (start,end);
      
      /* write values using Plane() method */

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.xzPlane(pos(1)) = mat;
      }
      end = clock();
      runtimes[2] = runtime (start,end);

      /* Summary */
      
      std::cout << "\t" << nelem[n]
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1]
		<< "\t" << runtimes[2]
		<< std::endl;

    }
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_Arrays_Array (std::vector<int> const &nelem)
{
    cout << "\nTesting casa/Arrays/Cube ...\n" << endl;

  int nofFailedTests (0);
  uint maxDimensions (5);
  uint dim (0);
  uint n(0);
  clock_t start;
  clock_t end;
  double runtimes[2];

  cout << "[1] Construct Array via IPosition shape ..." << endl;
  try {
    
    for (dim=1; dim<maxDimensions; dim++) {
      for (n=0; n<4; n++) {
	start = clock();
	Array<double> arr (IPosition(dim,nelem[n]));
	end = clock();
	runtimes[0] = runtime (start,end);
	//
	start = clock();
	arr = 1.0;
	end = clock();
	runtimes[1] = runtime (start,end);
	//
	std::cout << "\t" << runtimes[0]
		  << "\t" << runtimes[1]
		  << "\t" << arr.shape()
		  << std::endl;
      }
    }
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_BasicMath ()
{
  cout << "\nTesting classes in casa/BasicMath ...\n" << endl;

  int nofFailedTests (0);
  double x (1.);
  double y;
  
  cout << "[1] Testing triangular functions ..." << endl;
  try {
    y = casa::sin(x);
    y = casa::cos(x);
    y = casa::tan(x);
    y = casa::asin(x);
    y = casa::acos(x);
    y = casa::atan(x);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2]" << endl;
  try {
    y = casa::sqrt(x);
    y = cbrt(x);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_BasicSL ()
{
  cout << "\nTesting classes in casa/BasicSL ...\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Testing String class ..." << endl;
  try {
    casa::String greetings ("Hello");
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Testing complex numbers ..." << endl;
  try {
    casa::Complex complexFloat (1.);
    casa::DComplex complexDouble (1.);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Testing arrays of complex numbers ..." << endl;
  try {
    int nelem (10);
    Vector<casa::Complex> vectorComplex (nelem);
    Vector<casa::DComplex> vectorDComplex (nelem);
    Matrix<casa::Complex> matrixComplex (nelem,nelem);
    Matrix<casa::DComplex> matrixDComplex (nelem,nelem);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int test_Containers ()
{
  int nofFailedTests (0);

    cout << "[1] Testing casa/Containers/Block ..." << endl;
  try {
    Block<int> bi1;                   // Block::Block()
    assert(bi1.nelements() == 0);     // Block::nelements()
    Block<int> bi2(100);              // Block::Block(uInt)
    assert(bi2.nelements() == 100);
    Block<int> bi7(0);
    assert(bi7.nelements() == 0);
    Block<int> bi3(200,5);            // Block::Block(uInt, T)
    assert(bi3.nelements() == 200);
    Block<int> bi6(0, 5);
    assert(bi6.nelements() == 0);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Testing casa/Containers/Record ..." << endl;
  try {
    int extraArgument=0;
    casa::RecordDesc rd;
    rd.addField ("TpBool", casa::TpBool);
    rd.setComment (0, "comment for field TpBool");
    rd.addField ("TpUChar", casa::TpUChar);
    rd.addField ("TpShort", casa::TpShort);
    rd.addField ("TpInt", casa::TpInt);
    rd.addField ("TpUInt", casa::TpUInt);
    rd.addField ("TpFloat", casa::TpFloat);
    rd.addField ("TpDouble", casa::TpDouble);
    rd.addField ("TpComplex", casa::TpComplex);
    rd.addField ("TpDComplex", casa::TpDComplex);
    rd.addField ("TpString", casa::TpString);
    rd.addField ("TpArrayBool", casa::TpArrayBool, IPosition(1,1));
    rd.addField ("TpArrayUChar", casa::TpArrayUChar, IPosition(1,1));
    rd.addField ("TpArrayShort", casa::TpArrayShort, IPosition(1,1));
    rd.addField ("TpArrayInt", casa::TpArrayInt, IPosition(1,1));
    rd.addField ("TpArrayUInt", casa::TpArrayUInt, IPosition(1,1));
    rd.addField ("TpArrayFloat", casa::TpArrayFloat, IPosition(1,1));
    rd.addField ("TpArrayDouble", casa::TpArrayDouble, IPosition(1,1));
    rd.addField ("TpArrayComplex", casa::TpArrayComplex, IPosition(1,1));
    rd.addField ("TpArrayDComplex", casa::TpArrayDComplex, IPosition(1,1));
    rd.addField ("TpArrayString", casa::TpArrayString);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing casa/Containers/List ..." << endl;
  try {
    casa::List<int> one;
    casa::ListIter<int> onePa(one);
    casa::ListIter<int> onePb(one);
    //
    onePa.addRight(3);
    onePa.addRight(72);
    onePa.addRight(16);
    onePa.addRight(7);
    onePa.addRight(31);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Testing casa/Containers/OrderedMap ..." << endl;
  try {
    casa::OrderedMap<int,int> map(-1);
    
    map(2)  = 90;
    map(8)  = 82;
    map(7)  = 72;
    map(3)  = 51;
    map(10) = 34;
    map(15) = 79;
    map(21) = map(27) = map(24) = 104;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \return nofFailedTests -- The number of failed tests we havve encountered
*/
int main ()
{
  int nofFailedTests (0);
  std::vector<int> nelem = nof_array_elements ();

  nofFailedTests += test_Arrays();
  nofFailedTests += test_Arrays_Matrix (nelem);
  nofFailedTests += test_Arrays_Cube (nelem);
  nofFailedTests += test_Arrays_Array (nelem);

  nofFailedTests += test_BasicMath();
  nofFailedTests += test_BasicSL();
  nofFailedTests += test_Containers();
//   nofFailedTests += test_IO();
//   nofFailedTests += test_Inputs();
//   nofFailedTests += test_Logging();

  return nofFailedTests;
}
