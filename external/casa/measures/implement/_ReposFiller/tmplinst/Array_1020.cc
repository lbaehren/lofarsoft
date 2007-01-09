// Array_1020.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/Array.cc>
#include <measures/Measures/MPosition.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<MPosition>;
#ifdef AIPS_SUN_NATIVE
template class Array<MPosition>::ConstIteratorSTL;
#endif
} //# NAMESPACE - END
