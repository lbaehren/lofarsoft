// Array_1000.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/Array.cc>
#include <measures/Measures/MDirection.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<MDirection>;
#ifdef AIPS_SUN_NATIVE
template class Array<MDirection>::ConstIteratorSTL;
#endif
} //# NAMESPACE - END
