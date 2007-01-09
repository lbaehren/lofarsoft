// ScalarMeasColumn_1000.cc -- Mon Nov 13 14:27:22 CET 2006 -- root
#include <measures/TableMeasures/ScalarMeasColumn.cc>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MBaseline.h>
namespace casa { //# NAMESPACE - BEGIN
template class ROScalarMeasColumn<MBaseline>;
template class ScalarMeasColumn<MBaseline>;
} //# NAMESPACE - END
