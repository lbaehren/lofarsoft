// ArrayLogical_1000.cc -- Wed Apr  4 17:26:08 CEST 2007 -- lars
#include <casa/Arrays/ArrayLogical.cc>
namespace casa { //# NAMESPACE - BEGIN
template Bool allNearAbs(Array<Double> const &, Double const &, Double);
#include <casa/BasicSL/Complex.h>
template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);
} //# NAMESPACE - END
