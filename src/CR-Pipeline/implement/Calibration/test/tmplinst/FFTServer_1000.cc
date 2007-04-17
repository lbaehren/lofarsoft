// FFTServer_1000.cc -- Wed Apr  4 17:26:08 CEST 2007 -- lars
#include <scimath/Mathematics/FFTServer.cc>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template class FFTServer<Float,Complex>;
template class FFTServer<Double, DComplex>;
} //# NAMESPACE - END
