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
using casa::Cube;
using casa::Matrix;
using casa::Vector;

// ------------------------------------------------------------------------------

/*!
  \file casacoreCubeAccess.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Matrix<T>

  <h3>Usage</h3>

  \verbatim
  casacoreCubeAccess test1
  \endverbatim
*/

// ------------------------------------------------------------------------------

/*!
  \brief Create new object of type Matrix<T>

  \code
      Cube<T> cube (nelem,nelem,nelem);
  \endcode
 */
int run_test1 (int const &min,
	       int const &step,
	       int const &max)
{
  cout << "[run_test1] -- Create object of type Cube<T> ..." << endl;

  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;
  
  try {
    std::cout << "-- Cube<int> ..." << std::endl;
    outfile.open("Cube_int_create.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      start = clock();
      /* Test operation -- begin */
      Cube<float> cube (nelem,nelem,nelem);
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
  
  try {
    std::cout << "-- Cube<float> ..." << std::endl;
    outfile.open("Cube_float_create.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      start = clock();
      /* Test operation -- begin */
      Cube<float> cube (nelem,nelem,nelem);
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
  
  try {
    std::cout << "-- Cube<double> ..." << std::endl;
    outfile.open("Cube_double_create.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      start = clock();
      /* Test operation -- begin */
      Cube<double> cube (nelem,nelem,nelem);
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

// ------------------------------------------------------------------------------

/*!
  \brief Create new object of type Cube<T>

  \code
      cube = 1;
  \endcode
 */
int run_test2 (int const &min,
	       int const &step,
	       int const &max)
{
  cout << "[run_test2] -- Create object of type Cube<T> ..." << endl;

  int nofFailedTests (0);
  int nrow (0);
  int ncol (0);
  clock_t start;
  clock_t end;
  std::ofstream outfile;

  try {
    std::cout << "-- Cube<int> ..." << std::endl;
    outfile.open("Cube_int_assign.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      Cube<float> cube (nelem,nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      cube = 1;
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
  
  try {
    std::cout << "-- Cube<float> ..." << std::endl;
    outfile.open("Cube_float_assign.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      Cube<float> cube (nelem,nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      cube = 1.0;
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
  
  try {
    std::cout << "-- Cube<double> ..." << std::endl;
    outfile.open("Cube_double_assign.dat");
    for (int nelem(min); nelem<max; nelem+=step) {
      Cube<double> cube (nelem,nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      cube = 1.0;
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

// ------------------------------------------------------------------------------

int main (int argc,char* argv[])
{
  int nofFailedTests (0);

  /* Check input from the command line */
  
  if (argc < 2) {
    std::cerr << "[casacoreCubeAccess] Missing parameter to select test."
	      << std::endl;
    return 1;
  }

  int nelemMin  (10);
  int nelemStep (10);
  int nelemMax  (1000);
  std::string testName (argv[1]);
  
  if (testName == "test1") {
    nofFailedTests += run_test1 (nelemMin,nelemStep,nelemMax);
  }
  else if (testName == "test2") {
    nofFailedTests += run_test2 (nelemMin,nelemStep,nelemMax);
  }
  else {
    std::cout << "No test selected." << std::endl;
  }

  return nofFailedTests;
}
