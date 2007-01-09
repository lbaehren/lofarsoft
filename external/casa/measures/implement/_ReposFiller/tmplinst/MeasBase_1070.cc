// MeasBase_1070.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVRadialVelocity.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVRadialVelocity, MeasRef<MRadialVelocity> >;
} //# NAMESPACE - END
