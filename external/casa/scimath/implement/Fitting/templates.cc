
// include of header files

#include <casa/Logging/LogIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

// include of implementation files

#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/Matrix.cc>
#include <casa/BasicMath/Functional.cc>
#include <scimath/Fitting/FitGaussian.cc>

namespace casa {

//   template class Matrix<Int>;
//   template class Matrix<Float>;
//   template class Matrix<Double>;

  template void indgen(Array<Complex> &, Complex);
  template void indgen(Array<Complex> &, Complex, Complex);

  template class Functional<Vector<Complex>, Vector<Complex> >;
  template class Functional<Vector<DComplex>, Vector<DComplex> >;

//   template class FitGaussian<Double>;
}
