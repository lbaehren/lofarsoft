
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>

#include <casa/BasicMath/Functional.cc> 

namespace casa{
  template class Functional<Complex, Complex>;
  template class Functional<DComplex, DComplex>;
  template class Functional<Double, Array<Double> >;
  template class Functional<uInt, Array<Double> >;
  template class Functional<Vector<Complex>, Complex>;
  template class Functional<Vector<DComplex>, DComplex>;
  template class Functional<Vector<Double>, Array<Double> >;
  template class Functional<Vector<Double>, Double>;
  template class Functional<Vector<Double>, Float>;
  template class Functional<Vector<Float>, Float>;
  template class Functional<Double, Double>;
  template class Functional<Double, Float>;
  template class Functional<Float, Float>;
  template class Functional<uInt, Double>;
  template class Functional<uInt, Float>;
}
