// MeasBase_1010.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVDirection.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVDirection, MeasRef<MDirection> >;
} //# NAMESPACE - END
