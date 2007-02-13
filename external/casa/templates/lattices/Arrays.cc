
#include <casa/BasicSL/Complex.h>

#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>

namespace casa {

  // 1000 casa/Arrays/ArrayLogical.cc 
  template Bool allEQ(Array<Complex> const &, Complex const &);
  template Bool allEQ(Array<DComplex> const &, DComplex const &);
  template Bool allNear(Array<DComplex> const &, DComplex const &, Double);
  template Bool allNear(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNear(Array<DComplex> const &, Array<DComplex> const &, Double);

  // 1000 casa/Arrays/ArrayMath.cc casa/BasicSL/Complex.h 
  template Array<Int> operator-(Array<Int> const &, Int const &);
  template void indgen(Array<Complex> &);
  template void indgen(Array<Complex> &, Complex, Complex);
  
}
