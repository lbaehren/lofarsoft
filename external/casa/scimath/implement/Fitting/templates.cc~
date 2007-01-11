
// include of header files

#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

// include of implementation files

#include <casa/Arrays/ArrayMath.cc>
#include <casa/BasicMath/Functional.cc>
#include <scimath/Fitting/FitGaussian.cc>

namespace casa {

  template void indgen(Array<Complex> &, Complex);
  template void indgen(Array<Complex> &, Complex, Complex);

  template class Functional<Vector<Complex>, Vector<Complex> >;
  template class Functional<Vector<DComplex>, Vector<DComplex> >;

  template class FitGaussian<Double>;
}
