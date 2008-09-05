
#include <ctime>
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
  \brief Test access schemes to casacore Matrix<T>

  Implemented tests:
  <ol>
    <li>Assign value to matrix using <tt>Matix<T>::row(n) = T</tt> method
    <li>Assign value to matrix using <tt>Matix<T>::column(n) = T</tt> method
  </ol>

  \return nofFailedTests -- The number of failed tests encountered while running
          the program.
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
  
  std::cout << "[1] Fill Matrix<double> by row ..." << std::endl;
  try {
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
  
  std::cout << "[2] Fill Matrix<double> by column ..." << std::endl;
  try {
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
    
  return nofFailedTests;
}
