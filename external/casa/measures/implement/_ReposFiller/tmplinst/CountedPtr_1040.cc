// CountedPtr_1040.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <measures/Measures/MPosition.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedConstPtr<Block<MPosition> >;
template class CountedPtr<Block<MPosition> >;
template class PtrRep<Block<MPosition> >;
template class SimpleCountedConstPtr<Block<MPosition> >;
template class SimpleCountedPtr<Block<MPosition> >;
} //# NAMESPACE - END
