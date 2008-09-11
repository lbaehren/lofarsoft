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
  \file casacoreMatrixAccess05.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Matrix<T>

  <h3>Usage</h3>

  \verbatim
  casacoreMatrixAccess05 test1
  \endverbatim
*/

// ------------------------------------------------------------------------------

/*!
  \brief Assign individual elements of Matrix<double> within col-row nested loop

  \code
      for (ncol=0; ncol<nelem; ncol++) {
	for (nrow=0; nrow<nelem; nrow++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
  \endcode
*/
int run_test1 (int const &min,
	       int const &step,
	       int const &max)
{
  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;

  try {
    outfile.open("Matrix_double_col_row.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
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
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Assign individual elements of Matrix<double> within row-col nested loop

  \code
      for (nrow=0; nrow<nelem; nrow++) {
	for (ncol=0; ncol<nelem; ncol++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
  \endcode
*/
int run_test2 (int const &min,
	       int const &step,
	       int const &max)
{
  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;

  try {
    outfile.open("Matrix_double_row_col.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
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
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Assign individual elements of Matrix<float> within col-row nested loop

  \code
      for (ncol=0; ncol<nelem; ncol++) {
	for (nrow=0; nrow<nelem; nrow++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
  \endcode
*/
int run_test3 (int const &min,
	       int const &step,
	       int const &max)
{
  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;

  try {
    outfile.open("Matrix_float_col_row.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
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
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Assign individual elements of Matrix<float> within row-col nested loop

  \code
      for (nrow=0; nrow<nelem; nrow++) {
	for (ncol=0; ncol<nelem; ncol++) {
	  mat(ncol,nrow) = 1.0;
	}
      }
  \endcode
*/
int run_test4 (int const &min,
	       int const &step,
	       int const &max)
{
  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;

  try {
    outfile.open("Matrix_float_row_col.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
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

// ------------------------------------------------------------------------------

int main (int argc,char* argv[])
{
  int nofFailedTests (0);

  int nelemMin  (100);
  int nelemStep (100);
  int nelemMax  (10000);
  
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;
  
  if (argv[1] == "test1") {
    cout << "[1] Assign matrix values individually in col-row order ..." << endl;
    nofFailedTests += run_test1 (nelemMin,nelemStep,nelemMax);
  }
  else if (argv[1] == "test2") {
    cout << "[2] Assign matrix values individually in row-col order ..." << endl;
    nofFailedTests += run_test2 (nelemMin,nelemStep,nelemMax);
  }
  else if (argv[1] == "test3") {
    cout << "[3] Assign matrix values individually in col-row order ..." << endl;
    nofFailedTests += run_test3 (nelemMin,nelemStep,nelemMax);
  }
  else if (argv[1] == "test4") {
    cout << "[4] Assign matrix values individually in row-col order ..." << endl;
    nofFailedTests += run_test4 (nelemMin,nelemStep,nelemMax);
  }
  else {
    cout << "[casacoreMatrixAccess05] No test selected." << endl;
  }

  return nofFailedTests;
}
