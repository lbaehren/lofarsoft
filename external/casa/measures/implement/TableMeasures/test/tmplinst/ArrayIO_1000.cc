// ArrayIO_1000.cc -- Mon Nov 13 14:27:22 CET 2006 -- root
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
namespace casa { //# NAMESPACE - BEGIN
template ostream & operator<<(ostream &, Array<MEpoch> const &);
template ostream & operator<<(ostream &, Array<MFrequency> const &);
} //# NAMESPACE - END
