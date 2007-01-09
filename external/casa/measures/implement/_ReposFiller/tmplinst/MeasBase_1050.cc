// MeasBase_1050.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVFrequency.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVFrequency, MeasRef<MFrequency> >;
} //# NAMESPACE - END
