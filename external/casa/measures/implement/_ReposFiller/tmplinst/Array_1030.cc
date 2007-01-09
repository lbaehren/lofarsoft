// Array_1030.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/Array.cc>
#include <measures/Measures/MeasureHolder.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<MeasureHolder>;
#ifdef AIPS_SUN_NATIVE
template class Array<MeasureHolder>::ConstIteratorSTL;
#endif
} //# NAMESPACE - END
