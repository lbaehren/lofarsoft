// ScalarQuantColumn_1000.cc -- Mon Nov 13 14:27:22 CET 2006 -- root
#include <measures/TableMeasures/ScalarQuantColumn.cc>
#include <casa/Quanta/Quantum.h>
namespace casa { //# NAMESPACE - BEGIN
template class ROScalarQuantColumn<Complex>;
template class ROScalarQuantColumn<Int>;
template class ScalarQuantColumn<Complex>;
template class ScalarQuantColumn<Int>;
} //# NAMESPACE - END
