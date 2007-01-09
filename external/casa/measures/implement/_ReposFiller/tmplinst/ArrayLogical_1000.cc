// ArrayLogical_1000.cc -- Mon Nov 13 14:23:20 CET 2006 -- root
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/Array.h>
#include <measures/Measures/Stokes.h>
namespace casa { //# NAMESPACE - BEGIN
template Bool allEQ(Array<Stokes::StokesTypes> const &, Array<Stokes::StokesTypes> const &);
template Bool anyNE(Array<Stokes::StokesTypes> const &, Array<Stokes::StokesTypes> const &);
} //# NAMESPACE - END
