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
#include <casa/System/ProgressMeter.h>

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

  While plane-wise assignment of e.g. a Cube is working fine using Slice objects
  \code
    for (n=0; n<nelem; n++) {
      cube(Slice(n,1,1),Slice(0,nelem,1),Slice(0,nelem,1)) = double(n);
    }
  \endcode
  the next imaginable operation -- copy values from one plane of a cube to 
  another plane of a second cube -- will fail (in fact the program aborts when
  running it).

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
    
    cout << " --> writing x-y planes via default Slice objects ..." << endl;
    for (n=0; n<nelem; n++) {
      mat = double(n);
      cube (Slice(), Slice(), n) = mat;
    }
    
    cout << " --> writing (x,y)-planes via specified Slice objects ..." << endl;
    for (n=0; n<nelem; n++) {
      mat = double(n);
      cube (Slice(0,nelem,1),Slice(0,nelem,1),Slice(n,1,1)) = mat;
    }

    /* The following operation will not work; though the code compiles and links,
       the program aborts as soon as be start copying the data from the matrix 
       to one of the planes of the cube.
    */
//     cout << " --> writing (y,z)-planes via specified Slice objects ..." << endl;
//     for (n=0; n<nelem; n++) {
//       mat = double(n);
//       cube (Slice(n,1,1),Slice(0,nelem,1),Slice(0,nelem,1)) = mat;
//     }
    
    cout << " --> writing single value into y-z planes via specified Slice objects ..."
	 << endl;
    for (n=0; n<nelem; n++) {
      cube(Slice(n,1,1),Slice(0,nelem,1),Slice(0,nelem,1)) = double(n);
    }
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

//   cout << "[3] Write planes of a Cube into planes of another Cube ..." << endl;
//   try {
//     int n(0);
//     Cube<double> cube1 (nelem,nelem,nelem);
//     Cube<double> cube2 (nelem,nelem,nelem);
    
//     for (n=0; n<nelem; n++) {
//       // fill the n-th (x,y) plane of the first cube ...
//       cube1 (Slice(0,nelem,1),Slice(0,nelem,1),Slice(n,1,1)) = double(n);
//       // .. and copy this plane to the n-th (y,z) plane of the second cube
//       cube2(Slice(n,1,1),Slice(0,nelem,1),Slice(0,nelem,1)).nonDegenerate(0)
// 	= cube1 (Slice(0,nelem,1),Slice(0,nelem,1),Slice(n,1,1));
//     }
    
//   } catch (std::string message) {
//     std::cerr << message << endl;
//     nofFailedTests++;
//   }
  
  cout << "[4] Simulate inserting beamformed data into image's pixel array" << endl;
  try {
    int nofAxes (5);
    int nofLon (100);
    int nofLat (100);
    int nofDist (20);
    int nofTimesteps (10);
    int nofFrequencies (128);
    
    // Pixel array of the image
    IPosition shapePixels (nofAxes,nofLon,nofLat,nofDist,nofTimesteps,nofFrequencies);
    Array<double> pixels (shapePixels);

    /*
      Array with the results of the beamforming; the various pointing positions
      are not decomposed into (lon,lat,dist), so mapping back onto these three 
      coordinates has to be done later.
    */
    int nofSkyPositions (nofLon*nofLat*nofDist);
    IPosition shapeBeam (2,nofSkyPositions,nofFrequencies);
    Array<double> beam (shapeBeam);

    /*
      Temporary array, into which we copy the data before inserting them into
      the pixel array.
    */
    Array<double> tmp (IPosition(5,nofLon,nofLat,nofDist,1,1));

    // summary of array properties
    cout << " -- shape of pixel array     = " << shapePixels     << endl;
    cout << " -- shape of the beam array  = " << shapeBeam       << endl;
    cout << " -- nof. sky positions       = " << nofSkyPositions << endl;
    cout << " -- shape of temporary array = " << tmp.shape()     << endl;
    
    /*
      Insert the data from the beam array to the pixel array; we use the number
      of timestep as outer loop, thereby enulating the subsequent processing 
      of data blocks.
    */
    
    // create meter to show progress on the operation
    int counter (0);
    casa::ProgressMeter meter (0,
			       nofTimesteps*nofFrequencies,
			       "Filling data",
			       "Processing block",
			       "","",true,1);

    for (shapePixels(3)=0; shapePixels(3)<nofTimesteps; shapePixels(3)++) {
      /*
	Below this point is what needs to be done per block of incoming data
      */
      for (shapePixels(4)=0; shapePixels(4)<nofFrequencies; shapePixels(4)++) {
	// Slicer (start,end,stride,Slicer::endIsLength)
	Slicer slice (IPosition(nofAxes,0,0,0,shapePixels(3),shapePixels(4)),
		      IPosition(nofAxes,nofLon,nofLat,nofDist,1,1),
		      IPosition(nofAxes,1),
		      Slicer::endIsLength);
	// assign a new value to the temp array
	tmp = shapePixels(3)*10+shapePixels(4);
	// insert the temporary data into the pixel array
	pixels(slice) = tmp;
	// show progress
	meter.update(counter);
	counter++;
      }
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
