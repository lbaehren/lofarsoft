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
 
  \author Lars Baehren
 
  \date 2007/08/20
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LinearTransform object

  \todo Still need to test constructors using Blitz++ and CASA array classes

  \return nofFailedTests -- The number of failed tests.
*/
int test_LinearTransform ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_LinearTransform]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    LinearTransform xform;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simplest argumented constructor ..." << std::endl;
  try {
    uint rank (3);
    LinearTransform xform (rank);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing fully argumented constructor ..." << std::endl;
  try {
    uint rank (3);
    double *matrix;
    double *shift;

    // Assign some values to the array elements

    std::cout << " -- allocating memory for input arrays ..." << std::endl;
    matrix = new double [rank*rank];
    shift  = new double [rank];

    uint row (0);
    uint col (0);
    uint n (0);

    std::cout << " -- assigning values to input arrays ..." << std::endl;
    for (row=0; row<rank; row++) {
      shift[row] = row;
      for (col=0; col<rank; col++) {
	matrix[n] = n;
	n++;
      }
    }

    std::cout << " -- constructing new object ..." << std::endl;
    LinearTransform xform (rank,matrix,shift);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing copy constructor ..." << std::endl;
  try {
    uint rank (3);
    std::cout << " -- constructing original object ..." << std::endl;
    LinearTransform xform1 (rank);
    std::cout << " -- constructiong new object via copy ..." << std::endl;
    LinearTransform xform2 (xform1);
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
    
    LinearTransform xform (matrix,shift);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_LinearTransform ();
  }

  return nofFailedTests;
}
