// MeasBase_1080.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MeasBase.cc>
#include <casa/Quanta/MVuvw.h>
#include <measures/Measures/Muvw.h>
#include <measures/Measures/MeasRef.h>
namespace casa { //# NAMESPACE - BEGIN
template class MeasBase<MVuvw, MeasRef<Muvw> >;
} //# NAMESPACE - END
