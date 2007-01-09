// Array_1010.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/Array.cc>
#include <measures/Measures/MFrequency.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<MFrequency>;
#ifdef AIPS_SUN_NATIVE
template class Array<MFrequency>::ConstIteratorSTL;
#endif
} //# NAMESPACE - END
