
// include of header files 

#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>

// include of implementation files

#include <casa/BasicMath/Functional.cc>

namespace casa {

  // ============================================================================
  //
  //  ReposFiller/templates
  //
  // ============================================================================

  // Functional

  template class Functional<AutoDiffA<DComplex>, AutoDiffA<DComplex> >;
  template class Functional<AutoDiffA<Complex>, AutoDiffA<Complex> >;
  template class Functional<Vector<AutoDiffA<Complex> >, AutoDiffA<Complex> >;
  template class Functional<Vector<AutoDiffA<DComplex> >, AutoDiffA<DComplex> >;
  template class Functional<Vector<AutoDiff<Float> >, AutoDiff<Float> >;
  template class Functional<Complex, AutoDiff<Complex> >;
  template class Functional<DComplex, AutoDiff<DComplex> >;
  template class Functional<Vector<Complex>, AutoDiff<Complex> >;
  template class Functional<Vector<DComplex>, AutoDiff<DComplex> >;
  template class Functional<AutoDiff<Double>, AutoDiff<Double> >;
  template class Functional<AutoDiff<Float>, AutoDiff<Float> >;
  template class Functional<Double, AutoDiff<Double> >;
  template class Functional<Float, AutoDiff<Float> >;
  template class Functional<Vector<AutoDiff<Double> >, AutoDiff<Double> >;
  template class Functional<Vector<Double>, AutoDiff<Double> >;
  template class Functional<Vector<Float>, AutoDiff<Float> >;
  template class Functional<AutoDiffA<Double>, AutoDiffA<Double> >;
  template class Functional<AutoDiffA<Float>, AutoDiffA<Float> >;
  template class Functional<Vector<AutoDiffA<Double> >, AutoDiffA<Double> >;
  template class Functional<Vector<AutoDiffA<Float> >, AutoDiffA<Float> >;

  // ============================================================================
  //
  //  test/templates
  //
  // ============================================================================

  template class Functional<Vector<Complex>, Vector<Complex> >;
  template class Functional<Vector<DComplex>, Vector<DComplex> >;

}
