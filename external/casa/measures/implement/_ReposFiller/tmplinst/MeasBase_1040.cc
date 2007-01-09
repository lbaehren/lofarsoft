// MeasBase_1040.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVEpoch.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVEpoch, MeasRef<MEpoch> >;
} //# NAMESPACE - END
