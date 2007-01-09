// CountedPtr_1030.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <measures/Measures/MFrequency.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedConstPtr<Block<MFrequency> >;
template class CountedPtr<Block<MFrequency> >;
template class PtrRep<Block<MFrequency> >;
template class SimpleCountedConstPtr<Block<MFrequency> >;
template class SimpleCountedPtr<Block<MFrequency> >;
} //# NAMESPACE - END
