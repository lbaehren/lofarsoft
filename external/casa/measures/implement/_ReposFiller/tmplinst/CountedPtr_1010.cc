// CountedPtr_1010.cc -- Mon Nov 13 14:23:21 CET 2006 -- root
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Containers/Block.h>
#include <measures/Measures/Stokes.h>
namespace casa { //# NAMESPACE - BEGIN
template class CountedPtr<Block<Stokes::StokesTypes> >;
template class CountedConstPtr<Block<Stokes::StokesTypes> >;
template class SimpleCountedPtr<Block<Stokes::StokesTypes> >;
template class SimpleCountedConstPtr<Block<Stokes::StokesTypes> >;
template class PtrRep<Block<Stokes::StokesTypes> >;
} //# NAMESPACE - END
