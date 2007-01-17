
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

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>

namespace casa {

  template class Array<MDirection>;
  template class Array<MFrequency>;
  template class Array<MPosition>;
  template class Array<MeasureHolder>;
  template class Array<Stokes::StokesTypes>;

#ifdef AIPS_SUN_NATIVE 
  template class Array<MDirection>::ConstIteratorSTL;
  template class Array<MFrequency>::ConstIteratorSTL;
  template class Array<MPosition>::ConstIteratorSTL;
  template class Array<MeasureHolder>::ConstIteratorSTL;
  template class Array<Stokes::StokesTypes>::ConstIteratorSTL;
#endif 

  template Bool allEQ(Array<Stokes::StokesTypes> const &, Array<Stokes::StokesTypes> const &);
  template Bool anyNE(Array<Stokes::StokesTypes> const &, Array<Stokes::StokesTypes> const &);

  template class MaskedArray<MDirection>;
  template class MaskedArray<MFrequency>;
  template class MaskedArray<MPosition>;
  template class MaskedArray<MeasureHolder>;
  template class MaskedArray<Stokes::StokesTypes>;

  template class Vector<MeasureHolder>;
  template class Vector<Stokes::StokesTypes>;
  template class Vector<MDirection>;
  template class Vector<MFrequency>;
  template class Vector<MPosition>;

}
