// ArrayMath_1000.cc -- Sun Sep 10 22:15:10 CEST 2006 -- root
#include <casa/Arrays/ArrayMath.cc>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
namespace casa { //# NAMESPACE - BEGIN
template void indgen(Array<Complex> &);
template void indgen(Array<Complex> &, Complex, Complex);
template void operator+=(Array<uChar> &, uChar const &);
template void operator+=(Array<uShort> &, uShort const &);
template Array<Complex> operator+(Array<Complex> const &, Complex const &);
template Array<String> operator+(Array<String> const &, String const &);
template Double stddev(Array<Double> const &, Double);
template Array<Float> fmod(Array<Float> const &, Float const &);
} //# NAMESPACE - END
