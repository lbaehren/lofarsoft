// Array_1040.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/Array.cc>
#include <measures/Measures/Stokes.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<Stokes::StokesTypes>;
#ifdef AIPS_SUN_NATIVE
template class Array<Stokes::StokesTypes>::ConstIteratorSTL;
#endif
} //# NAMESPACE - END
