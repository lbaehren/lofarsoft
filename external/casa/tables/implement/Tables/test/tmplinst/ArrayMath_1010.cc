// ArrayMath_1010.cc -- Sun Sep 10 22:15:10 CEST 2006 -- root
#include <casa/Arrays/ArrayMath.cc>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template void indgen(Array<DComplex> &);
template void indgen(Array<DComplex> &, DComplex, DComplex);
} //# NAMESPACE - END
