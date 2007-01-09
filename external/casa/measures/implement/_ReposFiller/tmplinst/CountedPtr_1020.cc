// CountedPtr_1020.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <measures/Measures/MDirection.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedConstPtr<Block<MDirection> >;
template class CountedPtr<Block<MDirection> >;
template class PtrRep<Block<MDirection> >;
template class SimpleCountedConstPtr<Block<MDirection> >;
template class SimpleCountedPtr<Block<MDirection> >;
} //# NAMESPACE - END
