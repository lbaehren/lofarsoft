
// include of header files

#include <casa/Containers/Block.h>
#include <casa/Quanta/MVBaseline.h>
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/MVDoppler.h>
#include <casa/Quanta/MVEarthMagnetic.h>
#include <casa/Quanta/MVEpoch.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/MVPosition.h>
#include <casa/Quanta/MVRadialVelocity.h>
#include <casa/Quanta/MVuvw.h>
#include <casa/Utilities/ValTypeId.h>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MEarthMagnetic.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/Muvw.h>
#include <measures/Measures/MCBaseline.h>
#include <measures/Measures/MCDirection.h>
#include <measures/Measures/MCDoppler.h>
#include <measures/Measures/MCEarthMagnetic.h>
#include <measures/Measures/MCEpoch.h>
#include <measures/Measures/MCFrequency.h>
#include <measures/Measures/MCPosition.h>
#include <measures/Measures/MCRadialVelocity.h>
#include <measures/Measures/MCuvw.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <measures/Measures/MeasBase.cc>
#include <measures/Measures/MeasConvert.cc>
#include <measures/Measures/MeasRef.cc>

namespace casa {

  template class CountedPtr<Block<MEpoch> >;
  template class CountedConstPtr<Block<MEpoch> >;
  template class PtrRep<Block<MEpoch> >;
  template class SimpleCountedPtr<Block<MEpoch> >;
  template class SimpleCountedConstPtr<Block<MEpoch> >;
  template class Array<MEpoch>;
  template class Vector<MEpoch>;
  template class MaskedArray<MEpoch>;

  template class CountedPtr<Block<MRadialVelocity> >;
  template class CountedConstPtr<Block<MRadialVelocity> >;
  template class PtrRep<Block<MRadialVelocity> >;
  template class SimpleCountedPtr<Block<MRadialVelocity> >;
  template class SimpleCountedConstPtr<Block<MRadialVelocity> >;
  template class Array<MRadialVelocity>;
  template class Vector<MRadialVelocity>;
  template class MaskedArray<MRadialVelocity>;
  template class MeasBase<MVBaseline, MeasRef<MBaseline> >;

  template class MeasBase<MVDirection, MeasRef<MDirection> >;
  template class MeasBase<MVDoppler, MeasRef<MDoppler> >;
  template class MeasBase<MVEarthMagnetic, MeasRef<MEarthMagnetic> >;
  template class MeasBase<MVEpoch, MeasRef<MEpoch> >;
  template class MeasBase<MVFrequency, MeasRef<MFrequency> >;
  template class MeasBase<MVPosition, MeasRef<MPosition> >;
  template class MeasBase<MVRadialVelocity, MeasRef<MRadialVelocity> >;
  template class MeasBase<MVuvw, MeasRef<Muvw> >;

  template class MeasConvert<MBaseline>;
  template class MeasConvert<MDirection>;
  template class MeasConvert<MDoppler>;
  template class MeasConvert<MEarthMagnetic>;
  template class MeasConvert<MEpoch>;
  template class MeasConvert<MFrequency>;
  template class MeasConvert<MPosition>;
  template class MeasConvert<MRadialVelocity>;
  template class MeasConvert<Muvw>;

  template class MeasRef<MBaseline>;
  template class MeasRef<MDirection>;
  template class MeasRef<MDoppler>;
  template class MeasRef<MEarthMagnetic>;
  template class MeasRef<MEpoch>;
  template class MeasRef<MFrequency>;
  template class MeasRef<MPosition>;
  template class MeasRef<MRadialVelocity>;
  template class MeasRef<Muvw>;

  template class CountedPtr<Block<Muvw> >;
  template class CountedConstPtr<Block<Muvw> >;
  template class PtrRep<Block<Muvw> >;
  template class SimpleCountedPtr<Block<Muvw> >;
  template class SimpleCountedConstPtr<Block<Muvw> >;
  template class Array<Muvw>;
  template class Vector<Muvw>;
  template class MaskedArray<Muvw>;

}
