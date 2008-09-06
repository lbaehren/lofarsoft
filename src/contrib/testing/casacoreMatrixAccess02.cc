
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
using casa::Vector;

// ------------------------------------------------------------------------------

/*!
  \file casacoreMatrixAccess02.cc

  \ingroup contrib

  \brief Collection of tests for the various access schemes to casa::Matrix<T>

  Implemented tests:
  <ol>
    <li>Assign value to matrix using method
    \code <tt>Matix<T>::row(n) = T</tt> \endcode
    <li>Assign value to matrix using method
    \code <tt>Matix<T>::column(n) = T</tt> \endcode
    <li>Assign value to matrix using method 
    \code <tt>Matix<T>::row(n) = Vector<T></tt> \endcode
    <li>Assign value to matrix using method 
    \code <tt>Matix<T>::column(n) = Vector<T></tt> \endcode
  </ol>
*/

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  int nelemMin  (50);
  int nelemStep (50);
  int nelemMax  (10000);

  clock_t start;
  clock_t end;
  std::ofstream outfile;
  
  std::cout << "[1] Fill Matrix<double> by row ..." << std::endl;
  try {
    outfile.open("Matrix_double_row.dat");
    int nrow (0);
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (nrow=0; nrow<nelem; nrow++) {
	mat.row(nrow) = 1.0;
      }
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
  
  std::cout << "[2] Fill Matrix<double> by column ..." << std::endl;
  try {
    outfile.open("Matrix_double_col.dat");
    int ncol (0);
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      for (ncol=0; ncol<nelem; ncol++) {
	mat.column(ncol) = 1.0;
      }
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
    
  std::cout << "[3] Fill Matrix<double> by row using Vector ..." << std::endl;
  try {
    outfile.open("Matrix_double_row_Vector.dat");
    int nrow (0);
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      Vector<double> vec (nelem,1.0);
      start = clock();
      /* Test operation -- begin */
      for (nrow=0; nrow<nelem; nrow++) {
	mat.row(nrow) = vec;
      }
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
  
  std::cout << "[4] Fill Matrix<double> by column using Vector ..." << std::endl;
  try {
    outfile.open("Matrix_double_col_Vector.dat");
    int ncol (0);
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      Vector<double> vec (nelem,1.0);
      start = clock();
      /* Test operation -- begin */
      for (ncol=0; ncol<nelem; ncol++) {
	mat.column(ncol) = vec;
      }
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
