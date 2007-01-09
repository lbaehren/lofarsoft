// MEpoch_1000.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MEpoch.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/ValTypeId.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<Block<MEpoch> >;
template class CountedConstPtr<Block<MEpoch> >;
template class PtrRep<Block<MEpoch> >;
template class SimpleCountedPtr<Block<MEpoch> >;
template class SimpleCountedConstPtr<Block<MEpoch> >;
template class Array<MEpoch>;
template class Vector<MEpoch>;
template class MaskedArray<MEpoch>;
} //# NAMESPACE - END
