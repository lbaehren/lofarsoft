// MeasBase_1020.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVDoppler.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVDoppler, MeasRef<MDoppler> >;
} //# NAMESPACE - END
