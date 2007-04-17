// Function_1000.cc -- Wed Apr  4 17:26:08 CEST 2007 -- lars
#include <scimath/Functionals/Function.cc>
#include <casa/Arrays/Array.h>
namespace casa { //# NAMESPACE - BEGIN
template class Function<Float, Array<Float> >;
template class Function<Float, Double>;
template class Function<Int, Double>;
template ostream & operator<<(ostream &, Function<Double, Double> const &);
#include <casa/BasicSL/Complex.h>
template class Function<Double, DComplex>;
template class Function<Int, Complex>;
} //# NAMESPACE - END
