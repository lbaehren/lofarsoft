/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1159 2007-12-21 15:40:14Z baehren                    $ |
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

#include <crtools.h>

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

#ifdef HAVE_HDF5
#include <hdf5.h>
#include <casa/HDF5/HDF5File.h>
#include <casa/HDF5/HDF5Record.h>
#endif

#include <Utilities/TestsCommon.h>
#include "Casacore.h"

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

  \ingroup CR_test

  \brief A number of tests for clases in the casacore casa module

  \author Lars B&auml;hren

  <h3>Notes</h3>

  <ol>
    <li>Due to the split of the code base and the discontinued developement of
    the original CASA libraries, some of the test might work with <i>casacore</i>
    only; this needs to be handled by precompiler statements.
  </ol>
*/

// ==============================================================================
//
//  casa/Arrays
//
// ==============================================================================

/*!
  \brief Tests for classes in casa/Arrays

  \return nofFailedTests -- The number of failed test within this function
*/
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
    
    /* Test comparing of IPositions */ 
    IPosition ip1 (3,nelem,nelem,nelem);
    IPosition ip2 (3,nelem,nelem,nelem+1);
    IPosition ip3 (3,nelem,nelem,nelem+1);

    if (ip1 == ip2) {
      cout << "-- Comparison of IPositions : equal" << endl;
    } else {
      cout << "-- Comparison of IPositions : not equal" << endl;
    }

    if (ip2 == ip3) {
      cout << "-- Comparison of IPositions : equal" << endl;
    } else {
      cout << "-- Comparison of IPositions : not equal" << endl;
    }
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
    uint nofAxes (3);
    int nelem (100);
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

/*!
  \brief A few additional tests for the casa/Arrays/Matrix class
  
  \return nofFailedTests -- The number of failed test within this function
*/
int test_Vector ()
{
  cout << "\nTesting casa/Arrays/Vector ...\n" << endl;

  int nofFailedTests (0);
  int nelem (5);

  cout << "[1] Testing behaviour of copy operation ..." << endl;
  try {
    Vector<double> vec1 (nelem,0);
    Vector<double> vec2 (nelem,0);

    /* Explicit assignment of values in first vector */

    for (int n(0); n<nelem; n++) {
      vec1(n) = 10*n;
    }
    cout << "-- Vectors after creation: " << endl;
    cout << "--> vec1 = " << vec1 << endl;
    cout << "--> vec2 = " << vec2 << endl;

    /* Assign second vector the values of the first one */

    vec2 = vec1;

    cout << "-- Vectors after assignment 1 -> 2: " << endl;
    cout << "--> vec1 = " << vec1 << endl;
    cout << "--> vec2 = " << vec2 << endl;

    /* Modify individual elements in the original vector */

    vec1(0) = 1;
    vec1(1) = 2;

    cout << "-- Vectors after explicit modification of original: " << endl;
    cout << "--> vec1 = " << vec1 << endl;
    cout << "--> vec2 = " << vec2 << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests; 
}

// ------------------------------------------------------------------------------

/*!
  \brief Test for casa/Arrays/Matrix class

  [1] Element-wise writing <br>
  [2] Column-/Row-wise writing <br>
  [3] Column-/Row-wise writing of a vector <br>
  [4] Column-/Row-wise writing using Slicer

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Matrix (std::vector<int> const &nelem)
{
  cout << "\nTesting casa/Arrays/Matrix ...\n" << endl;

  int nofFailedTests (0);
  clock_t start;
  clock_t end;
  double runtimes[2];
  IPosition shape (2);
  IPosition pos(2);

  cout << "[1] Element-wise writing ..." << std::endl;
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
      runtimes[0] = CR::runtime (start,end);
      
      /* Element-wise addressing in opposite axis order */
      
      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	  arr(pos) = 1.0;
	}
      }
      end = clock();
      runtimes[1] = CR::runtime (start,end);
      
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
      runtimes[0] = CR::runtime (start,end);

      // looping over matrix columns

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.column(pos(1)) = 1.0;
      }
      end = clock();
      runtimes[1] = CR::runtime (start,end);

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
      runtimes[0] = CR::runtime (start,end);

      // looping over matrix columns

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.column(pos(1)) = vec;
      }
      end = clock();
      runtimes[1] = CR::runtime (start,end);

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

/*!
  \brief Test for casa/Arrays/Cube class

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Cube (std::vector<int> const &nelem)
{
  cout << "\nTesting casa/Arrays/Cube ...\n" << endl;

  int nofFailedTests (0);
  uint nelemMax (8);
  clock_t start;
  clock_t end;
  double runtimes[6];
  IPosition shape (3);
  IPosition pos(3);

  std::cout << "[1] Element-wise writing ..." << std::endl;
  std::cout << "\tnelem\t" 
	    << "\t(0,1,2)\t(0,2,1)"
	    << "\t(1,0,2)\t(1,2,0)"
	    << "\t(2,0,1)\t(2,1,0)"
	    << std::endl;
  try {
    for (uint n(2); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n-2],nelem[n-1],nelem[n]);
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
      runtimes[0] = CR::runtime (start,end);
      
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
      runtimes[1] = CR::runtime (start,end);
      
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
      runtimes[2] = CR::runtime (start,end);
      
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
      runtimes[3] = CR::runtime (start,end);
      
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
      runtimes[4] = CR::runtime (start,end);
      
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
      runtimes[5] = CR::runtime (start,end);
      
      /* Show timing information */
      
      std::cout << "\t" << shape
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
  std::cout << "\tnelem\t"
	    << "\t(y-z)"
	    << "\t(x-z)"
	    << "\t(x-y)"
	    << std::endl;
  try {
    int i (0);
    for (uint n(2); n<nelemMax; n++) {
      /* Create the matrix */
      shape = IPosition (3,nelem[n-2],nelem[n-1],nelem[n]);
      Cube<double> arr (shape);

      /* Write values y-z-planes */

      start = clock();
      for (i=0; i<shape(0); i++) {
	arr.yzPlane(i) = 1.0;
      }
      end = clock();
      runtimes[0] = CR::runtime(start,end);

      /* Write values x-z-planes */

      start = clock();
      for (i=0; i<shape(1); i++) {
	arr.xzPlane(i) = 1.0;
      }
      end = clock();
      runtimes[1] = CR::runtime(start,end);

      /* Write values x-y-planes */

      start = clock();
      for (i=0; i<shape(2); i++) {
	arr.xyPlane(i) = 1.0;
      }
      end = clock();
      runtimes[2] = CR::runtime(start,end);

      /* Summary */
      
      std::cout << "\t" << shape
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
  std::cout << "\tnelem\t"
	    << "\t(2,1,0)"
	    << "\t(0,2,1)"
	    << "\t(y-z)"
	    << std::endl;
  try {

    for (uint n(2); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n-2],nelem[n-1],nelem[n]);
      /* Create Matrix object */
      Matrix<double> mat;
      /* Create Cube object */
      Cube<double> arr (shape); 
      
      /* write values into y-z planes */

      mat.resize(shape(1),shape(2));
      mat = 0.0;
      
      start = clock();
      for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	  for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	    arr(pos) = mat(pos(1),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[0] = CR::runtime (start,end);
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	for (pos(2)=0; pos(2)<shape(2); pos(2)++) {
	  for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	    arr(pos) = mat(pos(1),pos(2));
	  }
	}
      }
      end = clock();
      runtimes[1] = CR::runtime (start,end);
      
      start = clock();
      for (pos(0)=0; pos(0)<shape(0); pos(0)++) {
	arr.yzPlane(pos(0)) = mat;
      }
      end = clock();
      runtimes[2] = CR::runtime (start,end);

      /* Summary */
      
      std::cout << "\t" << shape
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
  std::cout << "\tnelem\t"
	    << "\t(2,0,1)"
	    << "\t(1,2,0)"
	    << "\t(x-z)"
	    << std::endl;
  try {

    for (uint n(2); n<nelemMax; n++) {
      /* Set shape of the array */
      shape = IPosition (3,nelem[n-2],nelem[n-1],nelem[n]);
      /* Create Cube object */
      Cube<double> arr (shape); 
      /* Create Matrix object */
      Matrix<double> mat (shape(0),shape(2));
      
      mat = 0.0;
      
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
      runtimes[0] = CR::runtime (start,end);
      
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
      runtimes[1] = CR::runtime (start,end);
      
      /* write values using Plane() method */

      start = clock();
      for (pos(1)=0; pos(1)<shape(1); pos(1)++) {
	arr.xzPlane(pos(1)) = mat;
      }
      end = clock();
      runtimes[2] = CR::runtime (start,end);

      /* Summary */
      
      std::cout << "\t" << shape
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

/*!
  \brief Test for casa/Arrays/Array class

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Array (std::vector<int> const &nelem)
{
    cout << "\nTesting casa/Arrays/Array ...\n" << endl;

  int nofFailedTests (0);
  uint maxDimensions (6);
  uint dim (0);
  uint n(0);
  clock_t start;
  clock_t end;
  double runtimes[2];

  cout << "[1] Construct Array via IPosition shape and assign value ..." << endl;
  try {
    
    for (dim=1; dim<maxDimensions; dim++) {
      for (n=0; n<4; n++) {
	start = clock();
	Array<double> arr (IPosition(dim,nelem[n]));
	end = clock();
	runtimes[0] = CR::runtime (start,end);
	//
	start = clock();
	arr = 1.0;
	end = clock();
	runtimes[1] = CR::runtime (start,end);
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

/*!
  \brief Test for casa/Arrays/Slice class

  While sliced reading turns out to be rather straight-forward, sliced
  writes prove to be more troublesome; as we want to be able to do both this
  routines goes through a number of forseeable test-cases:
  <ol>
    <li>Extract a sequence of values from a vector
    <li>Extract smaller sub-array from larger original array, keeping the
    rank of the array (i.e. the number of axes).
    <li>Extract smaller sub-array from larger original array, decreasing the
    rank of the array (e.g. extracting a plane from a cube into a matrix).
    <li>Insert a lower-dimensional array into a higher-dimensional one, e.g.
    writing the contents of a matrix into planes of a cube.
  </ol>

  To fully specify a subarray, one must supply three pieces of information for
  each axis of the subarray:
  <ol>
    <li>where to start
    <li>how many elements to extract
    <li>what stride (or "increment" or "interval") to use: a stride of "n"
    means pick extract only every "nth" element along an axis 
  </ol>

  \return nofFailedTests -- The number of failed test within this function
*/
int test_Arrays_Slice (std::vector<int> const &nelem)
{
  cout << "\nTesting casa/Arrays/Slice ...\n" << endl;

  int nofFailedTests (0);
  int k (0);
  int n(0);
  uint maxDimension (8);
  clock_t start;
  clock_t end;
  double runtimes[4];

  std::cout << "[1] Assign values to parts of a matrix ..." << std::endl;
  try {
    Matrix<int> mat (nelem[0],nelem[1],0);

    std::cout << "-- writing sub-matrices ..." << std::endl;

    for (n=0; n<nelem[0]; n+=2) {
      mat (Slice(n,2,1),Slice(n,2,1)) = n+1;
    }
#ifdef DEBUGGING_MESSAGES
    std::cout << mat << std::endl;
#endif

    std::cout << "-- write vectors per row ..." << std::endl;

    mat = 0;
    for (n=0; n<nelem[0]; n++) {
      mat (Slice(n,1,1),Slice(0,n+1,1)) = n+10;
    }
#ifdef DEBUGGING_MESSAGES
    std::cout << mat << std::endl;
#endif
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
 
  std::cout << "[2] Extract planes from a Cube ..." << std::endl;
  try {
    IPosition shape(3,nelem[0],nelem[1],nelem[2]);
    Cube<double> arr (shape,0.0);
    Matrix<double> mat;

    /* Extract planes orthogonal to the x-axis (y-z-planes) */

    mat.resize(nelem[1],nelem[2]);
    for (k=0; k<nelem[0]; k++) {
      mat = arr (k,Slice(),Slice()).nonDegenerate();
    }
    std::cout << "\t" << shape << "\t->\t(y,z) = " << mat.shape() << std::endl;
    
    /* Extract planes orthogonal to the y-axis (y-z-planes) */

    mat.resize(nelem[0],nelem[2]);
    for (k=0; k<nelem[1]; k++) {
      mat = arr (Slice(),k,Slice()).nonDegenerate();
    }
    std::cout << "\t" << shape << "\t->\t(x,z) = " << mat.shape() << std::endl;
    
    /* Extract planes orthogonal to the z-axis (x-y-planes) */

    mat.resize(nelem[0],nelem[1]);
    for (k=0; k<nelem[2]; k++) {
      mat = arr (Slice(),Slice(),k).nonDegenerate();
    }
    std::cout << "\t" << shape << "\t->\t(x,y) = " << mat.shape() << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
 
  std::cout << "[3] Assign value to planes of a cube ..." << std::endl;
  try {
    IPosition shape(3,nelem[0],nelem[1],nelem[2]);
    Cube<double> arr (shape,0.0);

    /* Assign value per (y,z)-plane */

    for (k=0; k<nelem[0]; k++) {
      arr (k,Slice(),Slice()) = 1.0*k;
    }

    /* Assign value per (x,z)-plane */

    for (k=0; k<nelem[1]; k++) {
      arr (Slice(),k,Slice()) = 1.0*k;
    }

    /* Assign value per (x,y)-plane */

    for (k=0; k<nelem[2]; k++) {
      arr (Slice(),Slice(),k) = 1.0*k;
    }

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[4] Write Matrix to planes of a Cube ..." << std::endl;
  try {
    std::cout << "\t" << "shape\t"
	      << "\t" << "xyPlane"
	      << "\t" << "Slice"
	      << "\t" << "xzPlane"
	      << "\t" << "Slice"
	      << std::endl;
    for (uint ndim(2); ndim<maxDimension; ndim++) {
      /* shape of the target cube */
      IPosition shape (3,nelem[ndim-2],nelem[ndim-1],nelem[ndim]);
      /* create cube */
      Cube<double> cube (shape);
      /* create matrix */
      Matrix<double> mat;

      /* write (y,z)-planes */
      
      mat.resize(shape(1),shape(2));

      start = clock();
      for (n=0; n<shape(0); n++) {
	mat = 1.0*n;
	cube.yzPlane(n) = mat;
      }
      end = clock();
      runtimes[0] = CR::runtime (start,end);
      
      start = clock();
      for (n=0; n<shape(0); n++) {
	mat = 1.0*n;
	cube (n,Slice(),Slice()).nonDegenerate() = mat;
      }
      end = clock();
      runtimes[1] = CR::runtime (start,end);

      /* write (x,z)-planes */
      
      mat.resize(shape(0),shape(2));

      start = clock();
      for (n=0; n<shape(1); n++) {
	mat = 1.0*n;
	cube.xzPlane(n) = mat;
      }
      end = clock();
      runtimes[2] = CR::runtime (start,end);
      
      start = clock();
      for (n=0; n<shape(1); n++) {
	mat = 1.0*n;
	cube (Slice(),n,Slice()).nonDegenerate() = mat;
      }
      end = clock();
      runtimes[3] = CR::runtime (start,end);

      /* print summary */
      
      std::cout << "\t" << shape
		<< "\t" << runtimes[0]
		<< "\t" << runtimes[1]
		<< "\t" << runtimes[2]
		<< "\t" << runtimes[3]
		<< std::endl;
      
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[5] Write Matrix to planes of a Cube - special cases"
	    << std::endl;
  try {
    IPosition shape(3,5,10,20);
    Cube<double> cube;
    Matrix<double> mat;
    
    std::cout << "-- write (y,z)-plane ..." << std::endl;
    // all axes > 1
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(1),shape(2));
    mat  = 1.0;
    cube (1,Slice(),Slice()).nonDegenerate() = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;
    // last two axes of equal length, but still > 1
    shape = IPosition (3,5,10,10);
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(1),shape(2));
    mat  = 1.0;
    cube (1,Slice(),Slice()).nonDegenerate() = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;
    // length of last axis = 1
    shape = IPosition (3,5,10,1);
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(1),shape(2));
    mat  = 1.0;
    cube.yzPlane(1) = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;
    // set both of the last last axes = 1
    shape = IPosition (3,5,1,1);
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(1),shape(2));
    mat  = 1.0;
    cube.yzPlane(1) = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;

    std::cout << "-- write (x,z)-plane ..." << std::endl;
    shape = IPosition (3,5,10,20);
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(0),shape(2));
    mat  = 1.0;
    cube (Slice(),1,Slice()).nonDegenerate() = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;
    
    std::cout << "-- write (x,y)-plane ..." << std::endl;
    cube.resize(shape);
    cube = 0.0;
    mat.resize(shape(0),shape(1));
    mat  = 1.0;
    cube (Slice(),Slice(),1).nonDegenerate() = mat;
    std::cout << "\t" << mat.shape() << "  ->  " << shape << std::endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ==============================================================================
//
//  casa/BasicMath
//
// ==============================================================================

/*!
  \brief Tests for classes in casa/BasicMath

  \return nofFailedTests -- The number of failed test within this function
*/
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

/*!
  \brief Tests for classes in casa/BasicSL

  \return nofFailedTests -- The number of failed test within this function
*/
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

#ifdef HAVE_HDF5

/*!
  \brief Tests for classes in casa/HDF5

  \return nofFailedTests -- The number of failed test within this function
*/
int test_HDF5 ()
{
  cout << "\n[test_HDF5]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Test writing a set of records to a HDF5 file ..." << endl;
  try {
    // Top-level
    casa::Record rec;
    rec.define ("telescope", "TELESCOPE");
    rec.define ("observer", "OBSERVER");
    rec.define ("project", "PROJECT");
    rec.define ("observation_id", "OBSERVATION_ID");
    rec.define ("observation_mode", "OBSERVATION_MODE");
    // Station-level
    casa::Record station1;
    station1.define ("station_position_value", "STATION_POSITION_VALUE");
    station1.define ("station_position_unit", "STATION_POSITION_UNIT");
    station1.define ("station_position_frame", "STATION_POSITION_FRAME");
    station1.define ("beam_direction_value", "BEAM_DIRECTION_VALUE");
    station1.define ("beam_direction_unit", "BEAM_DIRECTION_UNIT");
    station1.define ("beam_direction_frame", "BEAM_DIRECTION_FRAME");
    rec.defineRecord ("station1",station1);
    casa::Record station2;
    station2.define ("station_position_value", "STATION_POSITION_VALUE");
    station2.define ("station_position_unit", "STATION_POSITION_UNIT");
    station2.define ("station_position_frame", "STATION_POSITION_FRAME");
    station2.define ("beam_direction_value", "BEAM_DIRECTION_VALUE");
    station2.define ("beam_direction_unit", "BEAM_DIRECTION_UNIT");
    station2.define ("beam_direction_frame", "BEAM_DIRECTION_FRAME");
    rec.defineRecord ("station2",station2);
    casa::Record station3;
    station3.define ("station_position_value", "STATION_POSITION_VALUE");
    station3.define ("station_position_unit", "STATION_POSITION_UNIT");
    station3.define ("station_position_frame", "STATION_POSITION_FRAME");
    station3.define ("beam_direction_value", "BEAM_DIRECTION_VALUE");
    station3.define ("beam_direction_unit", "BEAM_DIRECTION_UNIT");
    station3.define ("beam_direction_frame", "BEAM_DIRECTION_FRAME");
    rec.defineRecord ("station3",station3);
    // Create HDF5 file and write the record to it
    casa::HDF5File file("tHDF5Record.h5", casa::ByteIO::New);
    casa::HDF5Record::writeRecord (file, "test", rec);
    file.flush();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

#endif

// ------------------------------------------------------------------------------

/*!
  \brief Main method

  \return nofFailedTests -- The number of failed tests we have encountered
*/
int main ()
{
  int nofFailedTests (0);
  std::vector<int> nelem = CR::number_sequence ();

  // ----------------------------------------------------------------------------
  // Test for the various casacore classes and modules

  nofFailedTests += test_Arrays();
  nofFailedTests += test_Vector();
//   nofFailedTests += test_Arrays_Matrix (nelem);
  nofFailedTests += test_Arrays_Cube (nelem);
//   nofFailedTests += test_Arrays_Array (nelem);
  nofFailedTests += test_Arrays_Slice (nelem);

  nofFailedTests += test_BasicMath();
  nofFailedTests += test_BasicSL();

#ifdef HAVE_HDF5
  nofFailedTests += test_HDF5 ();
#endif 

  // ----------------------------------------------------------------------------
  // additional test using casacore classes as private data of a new class

  try {
    casa_casa<int> myclassInt;
    casa_casa<float> myclassFloat;
    casa_casa<double> myclassDouble;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}
