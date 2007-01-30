
#include <templates.h>

// ==============================================================================
//
//  Template instantiation for CASA classes
//
// ==============================================================================

// ---------------------------------------------------------------------- libcasa

namespace casa { //# NAMESPACE - BEGIN
  template class Array<int>;
  template class Array<float>;
  template class Array<double>;
  template class Array<Complex>;
  template class Array<DComplex>;
  template Bool allEQ(Array<Complex> const &, Array<Complex> const &);
  template Bool allNear(Array<Complex> const &, Complex const &, Double);
  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Complex const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);
} //# NAMESPACE - END

// ------------------------------------------------------------------- libscimath

namespace casa { //# NAMESPACE - BEGIN
  template class FFTServer<Float,Complex>;
  template class FFTServer<Double,DComplex>;
} //# NAMESPACE - END


// ==============================================================================
//
//  Template instantiation for LOPES-Tools classes
//
// ==============================================================================

namespace LOPES {

  template class TemplatesBase<short>;
  template class TemplatesBase<int>;
  template class TemplatesBase<float>;
  template class TemplatesBase<double>;

}
