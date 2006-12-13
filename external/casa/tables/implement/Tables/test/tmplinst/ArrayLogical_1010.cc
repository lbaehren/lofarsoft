// ArrayLogical_1010.cc -- Sun Sep 10 22:15:10 CEST 2006 -- root
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
namespace casa { //# NAMESPACE - BEGIN
template LogicalArray operator<(Array<Double> const &, Array<Double> const &);
template LogicalArray operator<(Double const &, Array<Double> const &);
template LogicalArray operator<=(Array<Double> const &, Array<Double> const &);
template LogicalArray operator<=(Double const &, Array<Double> const &);
template LogicalArray operator<(Array<DComplex> const &, Array<DComplex> const &);
template LogicalArray operator<(Array<DComplex> const &, DComplex const &);
template LogicalArray operator<(DComplex const &, Array<DComplex> const &);
template LogicalArray operator<=(Array<DComplex> const &, Array<DComplex> const &);
template LogicalArray operator<=(Array<DComplex> const &, DComplex const &);
template LogicalArray operator<=(DComplex const &, Array<DComplex> const &);
template LogicalArray operator<(Array<String> const &, Array<String> const &);
template LogicalArray operator<(Array<String> const &, String const &);
template LogicalArray operator<(String const &, Array<String> const &);
template LogicalArray operator<=(Array<String> const &, Array<String> const &);
template LogicalArray operator<=(Array<String> const &, String const &);
template LogicalArray operator<=(String const &, Array<String> const &);
template Bool allNear(Array<DComplex> const &, Array<DComplex> const &, Double);
} //# NAMESPACE - END
