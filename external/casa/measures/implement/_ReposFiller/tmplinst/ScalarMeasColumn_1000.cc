// ScalarMeasColumn_1000.cc -- Mon Nov 13 14:23:23 CET 2006 -- root
#include <measures/TableMeasures/ScalarMeasColumn.cc>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/Muvw.h>
namespace casa { //# NAMESPACE - BEGIN
template class ROScalarMeasColumn<MDirection>;
template class ScalarMeasColumn<MDirection>;
template class ROScalarMeasColumn<MDoppler>;
template class ScalarMeasColumn<MDoppler>;
template class ROScalarMeasColumn<MEpoch>;
template class ScalarMeasColumn<MEpoch>;
template class ROScalarMeasColumn<MFrequency>;
template class ScalarMeasColumn<MFrequency>;
template class ROScalarMeasColumn<MPosition>;
template class ScalarMeasColumn<MPosition>;
template class ROScalarMeasColumn<MRadialVelocity>;
template class ScalarMeasColumn<MRadialVelocity>;
template class ROScalarMeasColumn<Muvw>;
template class ScalarMeasColumn<Muvw>;
} //# NAMESPACE - END
