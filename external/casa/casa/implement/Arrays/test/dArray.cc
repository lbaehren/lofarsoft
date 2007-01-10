#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>

using casa::IPosition;
using casa::Array;

int main () {

  uint nofAxes (3);
  IPosition shape (nofAxes);

//   Array<Int>    arrayInt    (shape);
//   Array<Float>  arrayFloat  (shape);
  Array<casa::Double> arrayDouble (shape);

  return 0;
}
