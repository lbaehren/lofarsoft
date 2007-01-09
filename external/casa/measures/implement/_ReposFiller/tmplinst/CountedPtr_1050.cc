// CountedPtr_1050.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <measures/TableMeasures/TableMeasDescBase.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<TableMeasDescBase>;
template class CountedConstPtr<TableMeasDescBase>;
template class PtrRep<TableMeasDescBase>;
template class SimpleCountedPtr<TableMeasDescBase>;
template class SimpleCountedConstPtr<TableMeasDescBase>;
} //# NAMESPACE - END
