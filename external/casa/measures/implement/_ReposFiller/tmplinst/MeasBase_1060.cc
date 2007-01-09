// MeasBase_1060.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVPosition.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVPosition, MeasRef<MPosition> >;
} //# NAMESPACE - END
