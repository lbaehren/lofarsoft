// TableMeasDesc_1000.cc -- Mon Nov 13 14:23:23 CET 2006 -- root
#include <measures/TableMeasures/TableMeasDesc.cc>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MEarthMagnetic.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/Muvw.h>
namespace casa { //# NAMESPACE - BEGIN
template class TableMeasDesc<MBaseline>;
template class TableMeasDesc<MDirection>;
template class TableMeasDesc<MDoppler>;
template class TableMeasDesc<MEarthMagnetic>;
template class TableMeasDesc<MEpoch>;
template class TableMeasDesc<MFrequency>;
template class TableMeasDesc<MPosition>;
template class TableMeasDesc<MRadialVelocity>;
template class TableMeasDesc<Muvw>;
} //# NAMESPACE - END
