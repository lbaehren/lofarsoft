
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <coordinates/Coordinates/CoordinateSystem.h>

#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <coordinates/Coordinates/FrequencyAligner.cc>
#include <scimath/Mathematics/InterpolateArray1D.cc>

namespace casa {

  // casa/Utilities/CountedPtr.cc coordinates/Coordinates/CoordinateSystem.h 
  template class CountedConstPtr<CoordinateSystem>;
  template class CountedPtr<CoordinateSystem>;
  template class PtrRep<CoordinateSystem>;
  template class SimpleCountedConstPtr<CoordinateSystem>;
  template class SimpleCountedPtr<CoordinateSystem>;

  // casa/Utilities/PtrHolder.cc 
  template class PtrHolder<CoordinateSystem>;

  // ============================================================================
  //
  //  Templates used by test programs
  //
  // ============================================================================

  // casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h
  template Bool allNearAbs(Array<Double> const &, Double const &, Double);

  // coordinates/Coordinates/FrequencyAligner.cc 
  template class FrequencyAligner<Float>;

  // scimath/Mathematics/InterpolateArray1D.cc 
  template class InterpolateArray1D<Double, Float>;
  
  // casa/Utilities/BinarySearch.cc casa/Arrays/Vector.h
  template Int binarySearchBrackets(Bool &, Vector<Double> const &, Double const &, uInt, Int);
  
}
