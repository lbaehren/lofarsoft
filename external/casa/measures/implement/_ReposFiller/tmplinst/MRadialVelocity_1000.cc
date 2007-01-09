// MRadialVelocity_1000.cc -- Mon Nov 13 14:23:22 CET 2006 -- root
#include <measures/Measures/MRadialVelocity.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/ValTypeId.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<Block<MRadialVelocity> >;
template class CountedConstPtr<Block<MRadialVelocity> >;
template class PtrRep<Block<MRadialVelocity> >;
template class SimpleCountedPtr<Block<MRadialVelocity> >;
template class SimpleCountedConstPtr<Block<MRadialVelocity> >;
template class Array<MRadialVelocity>;
template class Vector<MRadialVelocity>;
template class MaskedArray<MRadialVelocity>;
} //# NAMESPACE - END
