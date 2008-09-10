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

#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>

#include "testing_common.h"

using std::cout;
using std::endl;

using casa::Array;
using casa::Matrix;
using casa::Vector;

// ------------------------------------------------------------------------------

/*!
  \file casacoreMatrixAccess04.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Matrix<T>

  Implemented tests:
  <ol>
    <li>Assign individual matrix elements within col-row nested loop
    <li>Assign individual matrix elements within row-col nested loop
  </ol>
*/

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  int nelemMin  (50);
  int nelemStep (50);
  int nelemMax  (10000);
  
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;
  
  cout << "[1] Assign matrix values individually in col-row order ..." << endl;
  try {
    outfile.open("Matrix_double_col_row.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (ncol=0; ncol<nelem; ncol++) {
	for (nrow=0; nrow<nelem; nrow++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Assign matrix values individually in row-col order ..." << endl;
  try {
    outfile.open("Matrix_double_row_col.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (nrow=0; nrow<nelem; nrow++) {
	for (ncol=0; ncol<nelem; ncol++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Assign matrix values individually in col-row order ..." << endl;
  try {
    outfile.open("Matrix_float_col_row.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<float> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (ncol=0; ncol<nelem; ncol++) {
	for (nrow=0; nrow<nelem; nrow++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Assign matrix values individually in row-col order ..." << endl;
  try {
    outfile.open("Matrix_float_row_col.dat");
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<float> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (nrow=0; nrow<nelem; nrow++) {
	for (ncol=0; ncol<nelem; ncol++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
      /* Test operation -- end */
      end = clock();
      outfile << "\t" << nelem
	      << "\t" << start
	      << "\t" << end
	      << "\t" << runtime(start,end)
	      << endl;
    }
    outfile.close();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
