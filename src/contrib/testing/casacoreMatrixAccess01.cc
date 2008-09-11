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

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>

#include "testing_common.h"

using casa::Array;
using casa::IPosition;
using casa::Matrix;

// ------------------------------------------------------------------------------

/*!
  \file casacoreMatrixAccess01.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Matrix<T>

  Implemented tests:
  <ol>
    <li>Create object <tt>Matrix<float></tt>
    <li>Assign single value to all elements of the matrix through
    \code
    Matrix<T> = T
    \endcode
  </ol>
*/

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  int nelemMin  (100);
  int nelemStep (100);
  int nelemMax  (10000);

  clock_t start;
  clock_t end;
  std::ofstream outfile;
  
  std::cout << "[1] Create object of type Matrix<float> ..." << std::endl;
  try {
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      start = clock();
      /* Test operation -- begin */
      Matrix<float> mat (nelem,nelem);
      /* Test operation -- end */
      end = clock();
      std::cout << "\t" << nelem
		<< "\t" << start
		<< "\t" << end
		<< "\t" << runtime(start,end)
		<< std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Fill object of type Matrix<int> ..." << std::endl;
  try {
    outfile.open("Matrix_int.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<int> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << std::endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Fill object of type Matrix<float> ..." << std::endl;
  try {
    outfile.open("Matrix_float.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<float> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << std::endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Fill object of type Matrix<double> ..." << std::endl;
  try {
    outfile.open("Matrix_double.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << std::endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
    
  return nofFailedTests;
}
