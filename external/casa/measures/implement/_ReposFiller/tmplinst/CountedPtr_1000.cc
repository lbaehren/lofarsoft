// CountedPtr_1000.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <measures/Measures/MeasureHolder.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedConstPtr<Block<MeasureHolder> >;
template class CountedPtr<Block<MeasureHolder> >;
template class PtrRep<Block<MeasureHolder> >;
template class SimpleCountedConstPtr<Block<MeasureHolder> >;
template class SimpleCountedPtr<Block<MeasureHolder> >;
} //# NAMESPACE - END
