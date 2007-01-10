
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>

using casa::IPosition;
using casa::Array;
using casa::Cube;
using casa::Matrix;
using casa::Vector;

int main () {

  uint nofAxes (1);
  
  {
    IPosition shape (nofAxes);
    Vector<casa::Int>     vectorInt     (nofAxes);
    Vector<casa::Float>   vectorFloat   (nofAxes);
    Vector<casa::Double>  vectorDouble  (nofAxes);
    Vector<casa::Complex> vectorComplex (nofAxes);
  }
  
  {
    nofAxes = 2;
    IPosition shape (nofAxes);
    Matrix<casa::Int>     matrixInt    (shape);
    Matrix<casa::Float>   matrixFloat  (shape);
    Matrix<casa::Double>  matrixDouble (shape);
    Matrix<casa::Complex> matrixComplex (shape);
  }
  
  {
    nofAxes = 3;
    IPosition shape (nofAxes);
    Cube<casa::Int>     cubeInt    (shape);
    Cube<casa::Float>   cubeFloat  (shape);
    Cube<casa::Double>  cubeDouble (shape);
    Cube<casa::Complex> cubeComplex (shape);
  }
  
  {
    nofAxes = 4;
    IPosition shape (nofAxes);
    Vector<casa::Int>     vectorInt     (shape);
    Vector<casa::Float>   vectorFloat   (shape);
    Vector<casa::Double>  vectorDouble  (shape);
    Vector<casa::Complex> vectorComplex (shape);
  }
  
  return 0;
}
