
// include of header files

#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Array.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta/QMath.h>
#include <casa/Quanta/Quantum.h>

// include of implementation files

#include <casa/Quanta/QLogical.cc>
#include <casa/Quanta/QMath.cc>
#include <casa/Quanta/Quantum.cc>

namespace casa {

  template Bool near(Quantum<Double> const &, Quantum<Double> const &);
  template Bool nearAbs(Quantum<Double> const &, Quantum<Double> const &, Double);
  template Bool operator<=(Quantum<Double> const &, Quantum<Double> const &);
  template Bool operator>(Quantum<Double> const &, Quantum<Double> const &);
  template Bool operator>=(Quantum<Double> const &, Quantum<Double> const &);
  template Bool operator<(Quantum<Double> const &, Quantum<Double> const &);
  template Bool operator!=(Quantum<Double> const &, Quantum<Double> const &);
  template Bool operator==(Quantum<Double> const &, Quantum<Double> const &);
  
  template Quantum<Double> operator*(Double const &, Quantum<Double> const &);
  template Quantum<Double> operator*(Quantum<Double> const &, Double const &);
  template Quantum<Double> abs(Quantum<Double> const &);
  template Quantum<Double> acos(Quantum<Double> const &);
  template Quantum<Double> asin(Quantum<Double> const &);
  template Quantum<Double> atan(Quantum<Double> const &);
  template Quantum<Double> atan2(Quantum<Double> const &, Quantum<Double> const &);
  template Quantum<Double> ceil(Quantum<Double> const &);
  template Quantum<Double> cos(Quantum<Double> const &);
  template Quantum<Double> floor(Quantum<Double> const &);
  template Quantum<Double> operator/(Quantum<Double> const &, Double const &);
  template Quantum<Double> pow(Quantum<Double> const &, Int);
  template Quantum<Double> sin(Quantum<Double> const &);
  template Quantum<Double> tan(Quantum<Double> const &);
  template Quantum<Double> log(Quantum<Double> const &);
  template Quantum<Double> log10(Quantum<Double> const &);
  template Quantum<Double> exp(Quantum<Double> const &);
  template Quantum<Double> root(Quantum<Double> const &, Int);
  template Quantum<Double> sqrt(Quantum<Double> const &);
  
  template Quantum<Array<Double> > abs(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > acos(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > asin(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > atan(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > atan2(Quantum<Array<Double> > const &, Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > ceil(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > cos(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > floor(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > sin(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > tan(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > log(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > log10(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > exp(Quantum<Array<Double> > const &);
  template Quantum<Array<Double> > root(Quantum<Array<Double> > const &, Int);
  template Quantum<Array<Double> > sqrt(Quantum<Array<Double> > const &);
  
  template Quantum<Vector<Double> > abs(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > acos(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > asin(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > atan(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > atan2(Quantum<Vector<Double> > const &, Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > ceil(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > cos(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > floor(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > sin(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > tan(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > log(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > log10(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > exp(Quantum<Vector<Double> > const &);
  template Quantum<Vector<Double> > root(Quantum<Vector<Double> > const &, Int);
  template Quantum<Vector<Double> > sqrt(Quantum<Vector<Double> > const &);

  template class Quantum<Array<Complex> >;
  template class Quantum<Array<DComplex> >;
  template class Quantum<Array<Float> >;
  template class Quantum<Array<Int> >;
  template class Quantum<Array<Double> >;
  template class Quantum<Matrix<Double> >;
  template class Quantum<Vector<Complex> >;
  template class Quantum<Vector<DComplex> >;
  template class Quantum<Vector<Float> >;
  template class Quantum<Vector<Int> >;
  template class Quantum<Vector<Double> >;
  template class Quantum<Complex>;
  template class Quantum<DComplex>;
  template class Quantum<Double>;
  template class Quantum<Float>;
  template class Quantum<Int>;
}

