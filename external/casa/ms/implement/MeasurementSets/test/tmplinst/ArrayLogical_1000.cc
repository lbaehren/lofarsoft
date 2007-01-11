// ArrayLogical_1000.cc -- Mon Nov 13 14:55:01 CET 2006 -- root
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template Bool allNearAbs(Array<Double> const &, Double const &, Double);
template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
} //# NAMESPACE - END
