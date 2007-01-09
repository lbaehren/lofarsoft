// ArrayMath_1000.cc -- Mon Nov 13 14:21:06 CET 2006 -- root
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Quanta/Quantum.h>
namespace casa { //# NAMESPACE - BEGIN
template Array<Quantum<Double> > operator*(Quantum<Double> const &, Array<Quantum<Double> > const &);
template Array<Quantum<Double> > operator-(Array<Quantum<Double> > const &);
template void operator*=(Array<Quantum<Double> > &, Array<Quantum<Double> > const &);
} //# NAMESPACE - END
