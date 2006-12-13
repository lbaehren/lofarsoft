// ArrayLogical_1000.cc -- Sun Sep 10 22:15:10 CEST 2006 -- root
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template Bool allEQ(Array<Complex> const &, Complex const &);
template Bool allEQ(Array<DComplex> const &, DComplex const &);
template Bool allEQ(Array<Short> const &, Short const &);
template Bool allEQ(Array<uShort> const &, Array<uShort> const &);
template Bool allNear(Array<Complex> const &, Array<Complex> const &, Double);
template LogicalArray operator<(Array<Double> const &, Double const &);
} //# NAMESPACE - END
