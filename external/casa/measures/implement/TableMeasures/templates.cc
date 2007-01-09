
// include of header files

#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MEarthMagnetic.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h 
#include <measures/Measures/Muvw.h 
#include <measures/Measures/MRadialVelocity.h 

// include of implementation files

#include <measures/TableMeasures/ArrayMeasColumn.cc>
#include <measures/TableMeasures/ArrayQuantColumn.cc>
#include <measures/TableMeasures/ScalarMeasColumn.cc>
#include <measures/TableMeasures/ScalarQuantColumn.cc>
#include <measures/TableMeasures/TableMeasDesc.cc>

namespace casa {
  
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
  
  template class ROArrayQuantColumn<Double>;
  template class ArrayQuantColumn<Double>;
  template class ROArrayQuantColumn<Float>;
  template class ArrayQuantColumn<Float>;

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

  template class ROScalarQuantColumn<Double>;
  template class ROScalarQuantColumn<Float>;
  template class ScalarQuantColumn<Double>;
  template class ScalarQuantColumn<Float>;

  template class TableMeasDesc<MBaseline>;
  template class TableMeasDesc<MDirection>;
  template class TableMeasDesc<MDoppler>;
  template class TableMeasDesc<MEarthMagnetic>;
  template class TableMeasDesc<MEpoch>;
  template class TableMeasDesc<MFrequency>;
  template class TableMeasDesc<MPosition>;
  template class TableMeasDesc<MRadialVelocity>;
  template class TableMeasDesc<Muvw>;

}
