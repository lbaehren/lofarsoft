/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <Math/LinearTransform.h>

using CR::LinearTransform;  // Namespace usage

/*!
  \file tLinearTransform.cc

  \ingroup Math

  \brief A collection of test routines for the LinearTransform class
 
  \author Lars B&auml;hren
 
  \date 2007/08/20
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LinearTransform object

  \todo Still need to test constructors using Blitz++ and CASA array classes

  \return nofFailedTests -- The number of failed tests.
*/
int test_constructors ()
{
  std::cout << "\n[test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LinearTransform xform;
    xform.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simplest argumented constructor ..." << std::endl;
  try {
    uint rank (3);
    LinearTransform xform (rank);
    xform.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing fully argumented constructor ..." << std::endl;
  try {
    uint rank (2);
    std::vector<int> shape (rank);
    double *matrix;
    double *shift;

    shape[0] = shape[1] = 3;

    // Assign some values to the array elements

    std::cout << " -- allocating memory for input arrays ..." << std::endl;
    matrix = new double [shape[0]*shape[1]];
    shift  = new double [shape[0]];

    uint row (0);
    uint col (0);
    uint n (0);

    std::cout << " -- assigning values to input arrays ..." << std::endl;
    for (row=0; row<shape[0]; row++) {
      shift[row] = row;
      for (col=0; col<shape[1]; col++) {
	matrix[n] = n;
	n++;
      }
    }

    std::cout << " -- constructing new object ..." << std::endl;
    LinearTransform xform (shape,matrix,shift);
    xform.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing copy constructor ..." << std::endl;
  try {
    uint rank (3);
    std::cout << " -- constructing original object ..." << std::endl;
    LinearTransform xform1 (rank);
    xform1.summary();
    std::cout << " -- constructing new object via copy ..." << std::endl;
    LinearTransform xform2 (xform1);
    xform2.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
#ifdef HAVE_BLITZ
  std::cout << "[5] Testing constructor using Blitz++ arrays ..." << std::endl;
  try {
    uint rank (3);
    blitz::Array<double,2> matrix (rank,rank);
    blitz::Array<double,1> shift (rank);

    matrix = 0,1,2,3,4,5,6,7,8;
    shift  = 0.5,0.5,0.5;

    std::cout << " -- Transformation matrix : " << matrix << std::endl;
    std::cout << " -- Translation vector    : " << shift  << std::endl;
    
    LinearTransform xform (matrix,shift);
    xform.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test application of the transform

  \return nofFailedTests -- The number of failed tests within this function
*/
int test_transform ()
{
  std::cout << "\n[test_transform]\n" << std::endl;

  int nofFailedTests (0);

  uint rank (3);
  LinearTransform xform (rank);
  double *vectIn;
  double *vectOut;
  
  vectIn  = new double[rank];
  vectOut = new double[rank];
  
  std::cout << "[1] Forward transform using default parameters ..." << std::endl;
  try {
    vectIn[0] = 0.;
    vectIn[1] = 1.;
    vectIn[2] = 2.;
    //
    xform.forward (vectOut,vectIn);
    //
    std::cout << "\t[" << vectIn[0]
	      << " " << vectIn[1]
	      << " " << vectIn[2]
	      << "]  ->  "
	      << "[" << vectOut[0]
	      << " " << vectOut[1]
	      << " " << vectOut[2]
	      << "]"
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Forward transform using non-zero shift ..." << std::endl;
  try {
    std::vector<double> shift (rank);

    shift[0] = shift[1] = shift[2] = 0.5;
    
    xform.setShift(shift);
    xform.forward (vectOut,vectIn);
    //
    std::cout << "\t[" << vectIn[0]
	      << " " << vectIn[1]
	      << " " << vectIn[2]
	      << "]  ->  "
	      << "[" << vectOut[0]
	      << " " << vectOut[1]
	      << " " << vectOut[2]
	      << "]"
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors ();
  // Test routines for carrying out the transform
  nofFailedTests += test_transform ();
  
  return nofFailedTests;
}
