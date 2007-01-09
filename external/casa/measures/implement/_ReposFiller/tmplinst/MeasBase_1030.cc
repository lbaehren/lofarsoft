// MeasBase_1030.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVEarthMagnetic.h>
#include <measures/Measures/MEarthMagnetic.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVEarthMagnetic, MeasRef<MEarthMagnetic> >;
} //# NAMESPACE - END
