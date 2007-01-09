// Muvw_1000.cc -- Mon Nov 13 14:23:23 CET 2006 -- root
#include <measures/Measures/Muvw.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/ValTypeId.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<Block<Muvw> >;
template class CountedConstPtr<Block<Muvw> >;
template class PtrRep<Block<Muvw> >;
template class SimpleCountedPtr<Block<Muvw> >;
template class SimpleCountedConstPtr<Block<Muvw> >;
template class Array<Muvw>;
template class Vector<Muvw>;
template class MaskedArray<Muvw>;
} //# NAMESPACE - END
