// CountedPtr_1000.cc -- Mon Nov 13 14:21:06 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <casa/Quanta/MVEpoch.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedConstPtr<Block<MVEpoch> >;
template class CountedPtr<Block<MVEpoch> >;
template class PtrRep<Block<MVEpoch> >;
template class SimpleCountedConstPtr<Block<MVEpoch> >;
template class SimpleCountedPtr<Block<MVEpoch> >;
} //# NAMESPACE - END
