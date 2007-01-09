// QMath_1000.cc -- Mon Nov 13 14:21:06 CET 2006 -- root
#include <casa/Quanta/QMath.cc>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template Quantum<Vector<Double> > operator*(Vector<Double> const &, Quantum<Vector<Double> > const &);
template Quantum<Complex> operator*(Complex const &, Quantum<Complex> const &);
template Quantum<Complex> ceil(Quantum<Complex> const &);
template Quantum<Complex> floor(Quantum<Complex> const &);
template Quantum<Complex> abs(Quantum<Complex> const &);
} //# NAMESPACE - END
