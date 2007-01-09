// ArrayMeasColumn_1000.cc -- Mon Nov 13 14:23:23 CET 2006 -- root
#include <measures/TableMeasures/ArrayMeasColumn.cc>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/Muvw.h>
#include <measures/Measures/MRadialVelocity.h>
namespace casa { //# NAMESPACE - BEGIN
template class ROArrayMeasColumn<MDirection>;
template class ArrayMeasColumn<MDirection>;
template class ROArrayMeasColumn<MEpoch>;
template class ArrayMeasColumn<MEpoch>;
template class ROArrayMeasColumn<MFrequency>;
template class ArrayMeasColumn<MFrequency>;
template class ROArrayMeasColumn<MPosition>;
template class ArrayMeasColumn<MPosition>;
template class ROArrayMeasColumn<Muvw>;
template class ArrayMeasColumn<Muvw>;
template class ROArrayMeasColumn<MRadialVelocity>;
template class ArrayMeasColumn<MRadialVelocity>;
} //# NAMESPACE - END
