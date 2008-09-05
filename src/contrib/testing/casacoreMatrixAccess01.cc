
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
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<int> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
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
  
  std::cout << "[3] Fill object of type Matrix<float> ..." << std::endl;
  try {
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<float> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
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
  
  std::cout << "[4] Fill object of type Matrix<double> ..." << std::endl;
  try {
    for (int nelem(nelemMin); nelem<nelemMax; nelem+=nelemStep) {
      Matrix<double> mat (nelem,nelem);
      start = clock();
      /* Test operation -- begin */
      mat = 1.0;
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
