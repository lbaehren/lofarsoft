// QLogical_1000.cc -- Mon Nov 13 14:21:06 CET 2006 -- root
#include <casa/Quanta/QLogical.cc>
#include <casa/Arrays/Vector.h>
namespace casa { //# NAMESPACE - BEGIN
template Bool operator==(Quantum<Vector<Double> > const &, Quantum<Vector<Double> > const &);
template Bool operator==(Quantum<Vector<Double> > const &, Vector<Double> const &);
} //# NAMESPACE - END
