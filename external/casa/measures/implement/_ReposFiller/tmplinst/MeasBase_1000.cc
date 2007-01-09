// MeasBase_1000.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVBaseline.h>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVBaseline, MeasRef<MBaseline> >;
} //# NAMESPACE - END
