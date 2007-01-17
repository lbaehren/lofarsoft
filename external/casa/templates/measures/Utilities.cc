
// include of header files

//#include <casa/Arrays/Array.h>

#include <casa/Containers/Block.h>
#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MDoppler.h>
#include <measures/Measures/MEarthMagnetic.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/Measure.h>
#include <measures/Measures/MeasureHolder.h>
#include <measures/Measures/Muvw.h>
#include <measures/Measures/Stokes.h>
#include <measures/TableMeasures/TableMeasDescBase.h>

// include of implementation files

#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <casa/Utilities/Register.cc>

namespace casa {

  // -- CountedPtr.cc

  template class CountedConstPtr<Block<MeasureHolder> >;
  template class CountedPtr<Block<MeasureHolder> >;
  template class PtrRep<Block<MeasureHolder> >;
  template class SimpleCountedConstPtr<Block<MeasureHolder> >;
  template class SimpleCountedPtr<Block<MeasureHolder> >;

  template class CountedPtr<Block<Stokes::StokesTypes> >;
  template class CountedConstPtr<Block<Stokes::StokesTypes> >;
  template class SimpleCountedPtr<Block<Stokes::StokesTypes> >;
  template class SimpleCountedConstPtr<Block<Stokes::StokesTypes> >;
  template class PtrRep<Block<Stokes::StokesTypes> >;

  template class CountedConstPtr<Block<MDirection> >;
  template class CountedPtr<Block<MDirection> >;
  template class PtrRep<Block<MDirection> >;
  template class SimpleCountedConstPtr<Block<MDirection> >;
  template class SimpleCountedPtr<Block<MDirection> >;

  template class CountedConstPtr<Block<MFrequency> >;
  template class CountedPtr<Block<MFrequency> >;
  template class PtrRep<Block<MFrequency> >;
  template class SimpleCountedConstPtr<Block<MFrequency> >;
  template class SimpleCountedPtr<Block<MFrequency> >;

  template class CountedConstPtr<Block<MPosition> >;
  template class CountedPtr<Block<MPosition> >;
  template class PtrRep<Block<MPosition> >;
  template class SimpleCountedConstPtr<Block<MPosition> >;
  template class SimpleCountedPtr<Block<MPosition> >;

  template class CountedPtr<TableMeasDescBase>;
  template class CountedConstPtr<TableMeasDescBase>;
  template class PtrRep<TableMeasDescBase>;
  template class SimpleCountedPtr<TableMeasDescBase>;
  template class SimpleCountedConstPtr<TableMeasDescBase>;
  
  // -- PtrHolder.cc

  template class PtrHolder<Measure>;

  // -- Register.cc

  template uInt Register(MBaseline const *);
  template uInt Register(MDirection const *);
  template uInt Register(MDoppler const *);
  template uInt Register(MEarthMagnetic const *);
  template uInt Register(MEpoch const *);
  template uInt Register(MFrequency const *);
  template uInt Register(MPosition const *);
  template uInt Register(MRadialVelocity const *);
  template uInt Register(Muvw const *);
}
