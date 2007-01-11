// CountedPtr_1000.cc -- Mon Nov 13 15:01:16 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <ms/MeasurementSets/MSSelection.h>
#include <casa/Containers/Block.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<Block<MSSelection> >;
template class CountedConstPtr<Block<MSSelection> >;
template class SimpleCountedPtr<Block<MSSelection> >;
template class SimpleCountedConstPtr<Block<MSSelection> >;
template class PtrRep<Block<MSSelection> >;
} //# NAMESPACE - END
