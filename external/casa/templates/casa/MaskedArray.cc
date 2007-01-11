
// include of header files 

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayAccessor.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicMath/Random.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/MVPosition.h>
#include <casa/Quanta/MVRadialVelocity.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/QuantumHolder.h>
#include <casa/Quanta/Unit.h>
#include <casa/Utilities/CountedPtr.h>

// include of implementation files

#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/MaskArrMath.cc>
#include <casa/Arrays/MaskArrMath2.cc>
#include <casa/Arrays/MaskArrLogi.cc>

namespace casa {

  template class MaskedArray<Array<Double> >;
  template class MaskedArray<Slicer>;
  template class MaskedArray<Vector<Complex> >;
  template class MaskedArray<Vector<Double> >;
  template class MaskedArray<Vector<Float> >;
  template class MaskedArray<Bool>;
  template class MaskedArray<uChar>;
  template class MaskedArray<Short>;
  template class MaskedArray<uShort>;
  template class MaskedArray<Int>;
  template class MaskedArray<uInt>;
  template class MaskedArray<Float>;
  template class MaskedArray<Double>;
  template class MaskedArray<Complex>;
  template class MaskedArray<DComplex>;
  template class MaskedArray<String>;
  template class MaskedArray<MVDirection>;
  template class MaskedArray<MVFrequency>;
  template class MaskedArray<MVPosition>;
  template class MaskedArray<MVRadialVelocity>;
  template class MaskedArray<MVTime>;
  template class MaskedArray<Quantum<Double> >;
  template class MaskedArray<Quantum<Float> >;
  template class MaskedArray<QuantumHolder>;
  template class MaskedArray<Unit>;
  template class MaskedArray<Matrix<CountedPtr<Random> > >;
  template class MaskedArray<CountedPtr<Random> >;
  template class MaskedArray<Char>;
  template class MaskedArray<Float *>;
  template class MaskedArray<Long>;
  template class MaskedArray<uLong>;

  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator+

  template MaskedArray<int> operator+(MaskedArray<int> const&, Array<int> const&);
  template MaskedArray<uInt> operator+(MaskedArray<uInt> const&, Array<uInt> const&);
  template MaskedArray<Short> operator+(MaskedArray<Short> const&, Array<Short> const&);
  template MaskedArray<uShort> operator+(MaskedArray<uShort> const&, Array<uShort> const&);
  template MaskedArray<Long> operator+(MaskedArray<Long> const&, Array<Long> const&);
  template MaskedArray<uLong> operator+(MaskedArray<uLong> const&, Array<uLong> const&);
  template MaskedArray<Float> operator+(MaskedArray<Float> const&, Array<Float> const&);
  template MaskedArray<Double> operator+(MaskedArray<Double> const&, Array<Double> const&);
  template MaskedArray<Complex> operator+(MaskedArray<Complex> const&, Array<Complex> const&);
  template MaskedArray<DComplex> operator+(MaskedArray<DComplex> const&, Array<DComplex> const&);

  template MaskedArray<Int> operator+(Array<Int> const&, MaskedArray<Int> const&);
  template MaskedArray<uInt> operator+(Array<uInt> const&, MaskedArray<uInt> const&);
  template MaskedArray<Short> operator+(Array<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<uShort> operator+(Array<uShort> const&, MaskedArray<uShort> const&);
  template MaskedArray<Long> operator+(Array<Long> const&, MaskedArray<Long> const&);
  template MaskedArray<uLong> operator+(Array<uLong> const&, MaskedArray<uLong> const&);
  template MaskedArray<Float> operator+(Array<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> operator+(Array<Double> const&, MaskedArray<Double> const&);
  template MaskedArray<Complex> operator+(Array<Complex> const&, MaskedArray<Complex> const&);
  template MaskedArray<DComplex> operator+(Array<DComplex> const&, MaskedArray<DComplex> const&);

  template MaskedArray<Int> operator+(MaskedArray<Int> const&, MaskedArray<Int> const&);
  template MaskedArray<uInt> operator+(MaskedArray<uInt> const&, MaskedArray<uInt> const&);
  template MaskedArray<Short> operator+(MaskedArray<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<uShort> operator+(MaskedArray<uShort> const&, MaskedArray<uShort> const&);
  template MaskedArray<Long> operator+(MaskedArray<Long> const&, MaskedArray<Long> const&);
  template MaskedArray<uLong> operator+(MaskedArray<uLong> const&, MaskedArray<uLong> const&);
  template MaskedArray<Float> operator+(MaskedArray<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> operator+(MaskedArray<Double> const&, MaskedArray<Double> const&);
  template MaskedArray<Complex> operator+(MaskedArray<Complex> const&, MaskedArray<Complex> const&);
  template MaskedArray<DComplex> operator+(MaskedArray<DComplex> const&, MaskedArray<DComplex> const&);

  // ------------------------------------------------------------------ operator-

  template MaskedArray<Int> operator-(MaskedArray<Int> const &);
  template MaskedArray<uInt> operator-(MaskedArray<uInt> const &);
  template MaskedArray<Short> operator-(MaskedArray<Short> const &);
  template MaskedArray<uShort> operator-(MaskedArray<uShort> const &);
  template MaskedArray<Long> operator-(MaskedArray<Long> const &);
  template MaskedArray<uLong> operator-(MaskedArray<uLong> const &);
  template MaskedArray<Float> operator-(MaskedArray<Float> const &);
  template MaskedArray<Double> operator-(MaskedArray<Double> const &);
  template MaskedArray<Complex> operator-(MaskedArray<Complex> const &);
  template MaskedArray<DComplex> operator-(MaskedArray<DComplex> const &);

  template MaskedArray<Int> operator-(MaskedArray<Int> const &, Int const &);
  template MaskedArray<uInt> operator-(MaskedArray<uInt> const &, uInt const &);
  template MaskedArray<Short> operator-(MaskedArray<Short> const &, Short const &);
  template MaskedArray<uShort> operator-(MaskedArray<uShort> const &, uShort const &);
  template MaskedArray<Long> operator-(MaskedArray<Long> const &, Long const &);
  template MaskedArray<uLong> operator-(MaskedArray<uLong> const &, uLong const &);
  template MaskedArray<Float> operator-(MaskedArray<Float> const &, Float const &);
  template MaskedArray<Double> operator-(MaskedArray<Double> const &, Double const &);
  template MaskedArray<Complex> operator-(MaskedArray<Complex> const &, Complex const &);
  template MaskedArray<DComplex> operator-(MaskedArray<DComplex> const &, DComplex const &);

  // ----------------------------------------------------------------- operator+=

  template Array<Int>& operator+=(Array<Int>&, MaskedArray<Int> const&);
  template Array<uInt>& operator+=(Array<uInt>&, MaskedArray<uInt> const&);
  template Array<Short>& operator+=(Array<Short>&, MaskedArray<Short> const&);
  template Array<uShort>& operator+=(Array<uShort>&, MaskedArray<uShort> const&);
  template Array<Long>& operator+=(Array<Long>&, MaskedArray<Long> const&);
  template Array<uLong>& operator+=(Array<uLong>&, MaskedArray<uLong> const&);
  template Array<Float>& operator+=(Array<Float>&, MaskedArray<Float> const&);
  template Array<Double>& operator+=(Array<Double>&, MaskedArray<Double> const&);
  template Array<Complex>& operator+=(Array<Complex>&, MaskedArray<Complex> const&);
  template Array<DComplex>& operator+=(Array<DComplex>&, MaskedArray<DComplex> const&);

  template const MaskedArray<Int> & operator+=(MaskedArray<Int> const &, Int const &);
  template const MaskedArray<Short> & operator+=(MaskedArray<Short> const &, Short const &);
  template const MaskedArray<Long> & operator+=(MaskedArray<Long> const &, Long const &);
  template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, Float const &);
  template const MaskedArray<Double> & operator+=(MaskedArray<Double> const &, Double const &);
  template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, Complex const &);
  template const MaskedArray<DComplex> & operator+=(MaskedArray<DComplex> const &, DComplex const &);

  template MaskedArray<Int> const& operator+=(MaskedArray<Int> const&, Array<Int> const&);
  template MaskedArray<uInt> const& operator+=(MaskedArray<uInt> const&, Array<uInt> const&);
  template MaskedArray<Short> const& operator+=(MaskedArray<Short> const&, Array<Short> const&);
  template MaskedArray<uShort> const& operator+=(MaskedArray<uShort> const&, Array<uShort> const&);
  template MaskedArray<Long> const& operator+=(MaskedArray<Long> const&, Array<Long> const&);
  template MaskedArray<uLong> const& operator+=(MaskedArray<uLong> const&, Array<uLong> const&);
  template MaskedArray<Float> const& operator+=(MaskedArray<Float> const&, Array<Float> const&);
  template MaskedArray<Double> const& operator+=(MaskedArray<Double> const&, Array<Double> const&);
  template MaskedArray<Complex> const& operator+=(MaskedArray<Complex> const&, Array<Complex> const&);
  template MaskedArray<DComplex> const& operator+=(MaskedArray<DComplex> const&, Array<DComplex> const&);

  template MaskedArray<Int> const& operator+=(MaskedArray<Int> const&, MaskedArray<Int> const&);
  template MaskedArray<uInt> const& operator+=(MaskedArray<uInt> const&, MaskedArray<uInt> const&);
  template MaskedArray<Short> const& operator+=(MaskedArray<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<uShort> const& operator+=(MaskedArray<uShort> const&, MaskedArray<uShort> const&);
  template MaskedArray<Long> const& operator+=(MaskedArray<Long> const&, MaskedArray<Long> const&);
  template MaskedArray<uLong> const& operator+=(MaskedArray<uLong> const&, MaskedArray<uLong> const&);
  template MaskedArray<Float> const& operator+=(MaskedArray<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> const& operator+=(MaskedArray<Double> const&, MaskedArray<Double> const&);
  template MaskedArray<Complex> const& operator+=(MaskedArray<Complex> const&, MaskedArray<Complex> const&);
  template MaskedArray<DComplex> const& operator+=(MaskedArray<DComplex> const&, MaskedArray<DComplex> const&);

  // ----------------------------------------------------------------- operator-=

  template const MaskedArray<Int> & operator-=(MaskedArray<Int> const &, Int const &);
  template const MaskedArray<Short> & operator-=(MaskedArray<Short> const &, Short const &);
  template const MaskedArray<Long> & operator-=(MaskedArray<Long> const &, Long const &);
  template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, Float const &);
  template const MaskedArray<Double> & operator-=(MaskedArray<Double> const &, Double const &);
  template const MaskedArray<Complex> & operator-=(MaskedArray<Complex> const &, Complex const &);
  template const MaskedArray<DComplex> & operator-=(MaskedArray<DComplex> const &, DComplex const &);

  // ----------------------------------------------------------------- operator/=

  template Array<Int> & operator/=(Array<Int> &, MaskedArray<Int> const &);
  template Array<uInt> & operator/=(Array<uInt> &, MaskedArray<uInt> const &);
  template Array<Short> & operator/=(Array<Short> &, MaskedArray<Short> const &);
  template Array<uShort> & operator/=(Array<uShort> &, MaskedArray<uShort> const &);
  template Array<Long> & operator/=(Array<Long> &, MaskedArray<Long> const &);
  template Array<uLong> & operator/=(Array<uLong> &, MaskedArray<uLong> const &);
  template Array<Float> & operator/=(Array<Float> &, MaskedArray<Float> const &);
  template Array<Double> & operator/=(Array<Double> &, MaskedArray<Double> const &);
  template Array<Complex> & operator/=(Array<Complex> &, MaskedArray<Complex> const &);
  template Array<DComplex> & operator/=(Array<DComplex> &, MaskedArray<DComplex> const &);

  // ----------------------------------------------------------------- operator<=

  template MaskedArray<bool> operator<= (Array<int> const&, MaskedArray<int> const&);
  template MaskedArray<bool> operator<= (Array<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<bool> operator<= (Array<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<bool> operator<= (Array<Double> const&, MaskedArray<Double> const&);

  template MaskedArray<bool> operator<= (MaskedArray<int> const&, Array<int> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Short> const&, Array<Short> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Float> const&, Array<Float> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Double> const&, Array<Double> const&);

  template MaskedArray<bool> operator<= (MaskedArray<int> const&, MaskedArray<int> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<bool> operator<= (MaskedArray<Double> const&, MaskedArray<Double> const&);

  // ============================================================================
  //
  //  Mathematical operations
  //
  // ============================================================================

  // ------------------------------------------------------------------------ abs

  template MaskedArray<Float> abs(MaskedArray<Float> const &);
  template MaskedArray<Double> abs(MaskedArray<Double> const &);

  // ---------------------------------------------------------------------- avdev

  template int avdev (MaskedArray<int> const&, int);

  // ---------------------------------------------------------------------- atan2

  template MaskedArray<int> atan2 (Array<int> const&, MaskedArray<int> const&);
  template MaskedArray<Short> atan2 (Array<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<Float> atan2 (Array<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> atan2 (Array<Double> const&, MaskedArray<Double> const&);

  template MaskedArray<int> atan2 (MaskedArray<int> const&, Array<int> const&);
  template MaskedArray<Short> atan2 (MaskedArray<Short> const&, Array<Short> const&);
  template MaskedArray<Float> atan2 (MaskedArray<Float> const&, Array<Float> const&);
  template MaskedArray<Double> atan2 (MaskedArray<Double> const&, Array<Double> const&);

  template MaskedArray<int> atan2 (MaskedArray<int> const&, MaskedArray<int> const&);
  template MaskedArray<Short> atan2 (MaskedArray<Short> const&, MaskedArray<Short> const&);
  template MaskedArray<Float> atan2 (MaskedArray<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> atan2 (MaskedArray<Double> const&, MaskedArray<Double> const&);

  // ------------------------------------------------------------------------ max

  template Int max(MaskedArray<Int> const &);
  template uInt max(MaskedArray<uInt> const &);
  template Short max(MaskedArray<Short> const &);
  template uShort max(MaskedArray<uShort> const &);
  template Long max(MaskedArray<Long> const &);
  template uLong max(MaskedArray<uLong> const &);
  template Float max(MaskedArray<Float> const &);
  template Double max(MaskedArray<Double> const &);
  template Complex max(MaskedArray<Complex> const &);
  template DComplex max(MaskedArray<DComplex> const &);

  // ------------------------------------------------------------------------ min

  template Int min(MaskedArray<Int> const &);
  template uInt min(MaskedArray<uInt> const &);
  template Short min(MaskedArray<Short> const &);
  template uShort min(MaskedArray<uShort> const &);
  template Long min(MaskedArray<Long> const &);
  template uLong min(MaskedArray<uLong> const &);
  template Float min(MaskedArray<Float> const &);
  template Double min(MaskedArray<Double> const &);
  template Complex min(MaskedArray<Complex> const &);
  template DComplex min(MaskedArray<DComplex> const &);

  template MaskedArray<int> min(Array<int> const&, MaskedArray<int> const&);
  template MaskedArray<uInt> min(Array<uInt> const&, MaskedArray<uInt> const&);
  template MaskedArray<Float> min(Array<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> min(Array<Double> const&, MaskedArray<Double> const&);

  template MaskedArray<int> min(MaskedArray<int> const&, Array<int> const&);
  template MaskedArray<uInt> min(MaskedArray<uInt> const&, Array<uInt> const&);
  template MaskedArray<Float> min(MaskedArray<Float> const&, Array<Float> const&);
  template MaskedArray<Double> min(MaskedArray<Double> const&, Array<Double> const&);

  template MaskedArray<int> min(MaskedArray<int> const&, MaskedArray<int> const&);
  template MaskedArray<uInt> min(MaskedArray<uInt> const&, MaskedArray<uInt> const&);
  template MaskedArray<Float> min(MaskedArray<Float> const&, MaskedArray<Float> const&);
  template MaskedArray<Double> min(MaskedArray<Double> const&, MaskedArray<Double> const&);

  // --------------------------------------------------------------------- minMax

  template void minMax(Int &, Int &, MaskedArray<Int> const &);
  template void minMax(uInt &, uInt &, MaskedArray<uInt> const &);
  template void minMax(Short &, Short &, MaskedArray<Short> const &);
  template void minMax(uShort &, uShort &, MaskedArray<uShort> const &);
  template void minMax(Long &, Long &, MaskedArray<Long> const &);
  template void minMax(uLong &, uLong &, MaskedArray<uLong> const &);
  template void minMax(Float &, Float &, MaskedArray<Float> const &);
  template void minMax(Double &, Double &, MaskedArray<Double> const &);

  template void minMax(Int &, Int &, IPosition &, IPosition &, MaskedArray<Int> const &);
  template void minMax(uInt &, uInt &, IPosition &, IPosition &, MaskedArray<uInt> const &);
  template void minMax(Short &, Short &, IPosition &, IPosition &, MaskedArray<Short> const &);
  template void minMax(uShort &, uShort &, IPosition &, IPosition &, MaskedArray<uShort> const &);
  template void minMax(Long &, Long &, IPosition &, IPosition &, MaskedArray<Long> const &);
  template void minMax(uLong &, uLong &, IPosition &, IPosition &, MaskedArray<uLong> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, MaskedArray<Float> const &);
  template void minMax(Double &, Double &, IPosition &, IPosition &, MaskedArray<Double> const &);

  // ----------------------------------------------------------------------- mean

  template Int mean(MaskedArray<Int> const &);
  template uInt mean(MaskedArray<uInt> const &);
  template Short mean(MaskedArray<Short> const &);
  template uShort mean(MaskedArray<uShort> const &);
  template Long mean(MaskedArray<Long> const &);
  template uLong mean(MaskedArray<uLong> const &);
  template Float mean(MaskedArray<Float> const &);
  template Double mean(MaskedArray<Double> const &);
//   template Complex mean(MaskedArray<Complex> const &);
//   template DComplex mean(MaskedArray<DComplex> const &);

  // --------------------------------------------------------------------- median

  template int median (MaskedArray<int> const&, bool, bool);
  template Short median (MaskedArray<Short> const&, bool, bool);
  template Long median (MaskedArray<Long> const&, bool, bool);
  template Float median (MaskedArray<Float> const&, bool, bool);
  template Double median (MaskedArray<Double> const&, bool, bool);

  // ------------------------------------------------------------------------ pow

  template MaskedArray<int> pow<int, int>(Array<int> const&, MaskedArray<int> const&);

  template MaskedArray<int> pow<int, int> (MaskedArray<int> const&, Array<int> const&);

  template MaskedArray<int> pow<int, int> (MaskedArray<int> const&, MaskedArray<int> const&);

  // -------------------------------------------------------------------- product

  template int product (MaskedArray<int> const&);
  template uInt product (MaskedArray<uInt> const&);
  template Float product (MaskedArray<Float> const&);
  template Double product (MaskedArray<Double> const&);

  // ------------------------------------------------------------------------ sum

  template Int sum(MaskedArray<Int> const &);
  template uInt sum(MaskedArray<uInt> const &);
  template Short sum(MaskedArray<Short> const &);
  template uShort sum(MaskedArray<uShort> const &);
  template Long sum(MaskedArray<Long> const &);
  template uLong sum(MaskedArray<uLong> const &);
  template Float sum(MaskedArray<Float> const &);
  template Double sum(MaskedArray<Double> const &);
//   template Complex sum(MaskedArray<Complex> const &);
//   template DComplex sum(MaskedArray<DComplex> const &);

  // ----------------------------------------------------------------- sumsquares

  template Int sumsquares(MaskedArray<Int> const &);
  template uInt sumsquares(MaskedArray<uInt> const &);
  template Short sumsquares(MaskedArray<Short> const &);
  template uShort sumsquares(MaskedArray<uShort> const &);
  template Long sumsquares(MaskedArray<Long> const &);
  template uLong sumsquares(MaskedArray<uLong> const &);
  template Float sumsquares(MaskedArray<Float> const &);
  template Double sumsquares(MaskedArray<Double> const &);

  // ------------------------------------------------------------------- variance

  template Int variance(MaskedArray<Int> const &);
  template uInt variance(MaskedArray<uInt> const &);
  template Short variance(MaskedArray<Short> const &);
  template uShort variance(MaskedArray<uShort> const &);
  template Long variance(MaskedArray<Long> const &);
  template uLong variance(MaskedArray<uLong> const &);
  template Float variance(MaskedArray<Float> const &);
  template Double variance(MaskedArray<Double> const &);
//   template Complex variance(MaskedArray<Complex> const &);
//   template DComplex variance(MaskedArray<DComplex> const &);

  template Int variance(MaskedArray<Int> const &, Int);
  template Float variance(MaskedArray<Float> const &, Float);
  template Double variance(MaskedArray<Double> const &, Double);

  // ----------------------------------------------------------------------------
  // everything below this point hasn't yet been organized...

  template const MaskedArray<Complex> & operator/=(MaskedArray<Complex> const &, MaskedArray<Float> const &);
  template MaskedArray<Float> operator-(Float const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator*=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator/=(MaskedArray<Float> const &, MaskedArray<Float> const &);

  // ============================================================================
  //
  //  Logical operations
  //
  // ============================================================================

  // allAND

  template bool allAND (bool const&, MaskedArray<bool> const&);
  template bool allAND (int const&, MaskedArray<int> const&);
  template bool allAND (uInt const&, MaskedArray<uInt> const&);
  template bool allAND (Short const&, MaskedArray<Short> const&);
  template bool allAND (uShort const&, MaskedArray<uShort> const&);
  template bool allAND (Long const&, MaskedArray<Long> const&);
  template bool allAND (uLong const&, MaskedArray<uLong> const&);
  template bool allAND (Float const&, MaskedArray<Float> const&);
  template bool allAND (Double const&, MaskedArray<Double> const&);

  template bool allAND (MaskedArray<bool> const&, bool const&);
  template bool allAND (MaskedArray<int> const&, int const&);
  template bool allAND (MaskedArray<uInt> const&, uInt const&);
  template bool allAND (MaskedArray<Short> const&, Short const&);
  template bool allAND (MaskedArray<uShort> const&, uShort const&);
  template bool allAND (MaskedArray<Long> const&, Long const&);
  template bool allAND (MaskedArray<uLong> const&, uLong const&);
  template bool allAND (MaskedArray<Float> const&, Float const&);
  template bool allAND (MaskedArray<Double> const&, Double const&);

  template Bool allAND (const Array<Bool> &, const MaskedArray<Bool> &);
  template Bool allAND (const Array<Int> &, const MaskedArray<Int> &);
  template Bool allAND (const Array<uInt> &, const MaskedArray<uInt> &);
  template Bool allAND (const Array<Short> &, const MaskedArray<Short> &);
  template Bool allAND (const Array<uShort> &, const MaskedArray<uShort> &);
  template Bool allAND (const Array<Long> &, const MaskedArray<Long> &);
  template Bool allAND (const Array<uLong> &, const MaskedArray<uLong> &);
  template Bool allAND (const Array<Float> &, const MaskedArray<Float> &);
  template Bool allAND (const Array<Double> &, const MaskedArray<Double> &);

  template Bool allAND (const MaskedArray<Bool> &, const Array<Bool> &);
  template Bool allAND (const MaskedArray<Int> &, const Array<Int> &);
  template Bool allAND (const MaskedArray<uInt> &, const Array<uInt> &);
  template Bool allAND (const MaskedArray<Short> &, const Array<Short> &);
  template Bool allAND (const MaskedArray<uShort> &, const Array<uShort> &);
  template Bool allAND (const MaskedArray<Long> &, const Array<Long> &);
  template Bool allAND (const MaskedArray<uLong> &, const Array<uLong> &);
  template Bool allAND (const MaskedArray<Float> &, const Array<Float> &);
  template Bool allAND (const MaskedArray<Double> &, const Array<Double> &);

  template Bool allAND (const MaskedArray<Bool> &, const MaskedArray<Bool> &);
  template Bool allAND (const MaskedArray<Int> &, const MaskedArray<Int> &);
  template Bool allAND (const MaskedArray<uInt> &, const MaskedArray<uInt> &);
  template Bool allAND (const MaskedArray<Short> &, const MaskedArray<Short> &);
  template Bool allAND (const MaskedArray<uShort> &, const MaskedArray<uShort> &);
  template Bool allAND (const MaskedArray<Long> &, const MaskedArray<Long> &);
  template Bool allAND (const MaskedArray<uLong> &, const MaskedArray<uLong> &);
  template Bool allAND (const MaskedArray<Float> &, const MaskedArray<Float> &);
  template Bool allAND (const MaskedArray<Double> &, const MaskedArray<Double> &);

  // allLE

  template bool allLE (bool const&, MaskedArray<bool> const&);
  template bool allLE (int const&, MaskedArray<int> const&);
  template bool allLE (uInt const&, MaskedArray<uInt> const&);
  template bool allLE (Short const&, MaskedArray<Short> const&);
  template bool allLE (uShort const&, MaskedArray<uShort> const&);
  template bool allLE (Long const&, MaskedArray<Long> const&);
  template bool allLE (uLong const&, MaskedArray<uLong> const&);
  template bool allLE (Float const&, MaskedArray<Float> const&);
  template bool allLE (Double const&, MaskedArray<Double> const&);

  template bool allLE (MaskedArray<bool> const&, bool const&);
  template bool allLE (MaskedArray<int> const&, int const&);
  template bool allLE (MaskedArray<uInt> const&, uInt const&);
  template bool allLE (MaskedArray<Short> const&, Short const&);
  template bool allLE (MaskedArray<uShort> const&, uShort const&);
  template bool allLE (MaskedArray<Long> const&, Long const&);
  template bool allLE (MaskedArray<uLong> const&, uLong const&);
  template bool allLE (MaskedArray<Float> const&, Float const&);
  template bool allLE (MaskedArray<Double> const&, Double const&);

  template Bool allLE (const Array<Bool> &, const MaskedArray<Bool> &);
  template Bool allLE (const Array<Int> &, const MaskedArray<Int> &);
  template Bool allLE (const Array<uInt> &, const MaskedArray<uInt> &);
  template Bool allLE (const Array<Short> &, const MaskedArray<Short> &);
  template Bool allLE (const Array<uShort> &, const MaskedArray<uShort> &);
  template Bool allLE (const Array<Long> &, const MaskedArray<Long> &);
  template Bool allLE (const Array<uLong> &, const MaskedArray<uLong> &);
  template Bool allLE (const Array<Float> &, const MaskedArray<Float> &);
  template Bool allLE (const Array<Double> &, const MaskedArray<Double> &);

  template Bool allLE (const MaskedArray<Bool> &, const Array<Bool> &);
  template Bool allLE (const MaskedArray<Int> &, const Array<Int> &);
  template Bool allLE (const MaskedArray<uInt> &, const Array<uInt> &);
  template Bool allLE (const MaskedArray<Short> &, const Array<Short> &);
  template Bool allLE (const MaskedArray<uShort> &, const Array<uShort> &);
  template Bool allLE (const MaskedArray<Long> &, const Array<Long> &);
  template Bool allLE (const MaskedArray<uLong> &, const Array<uLong> &);
  template Bool allLE (const MaskedArray<Float> &, const Array<Float> &);
  template Bool allLE (const MaskedArray<Double> &, const Array<Double> &);

  template Bool allLE (const MaskedArray<Bool> &, const MaskedArray<Bool> &);
  template Bool allLE (const MaskedArray<Int> &, const MaskedArray<Int> &);
  template Bool allLE (const MaskedArray<uInt> &, const MaskedArray<uInt> &);
  template Bool allLE (const MaskedArray<Short> &, const MaskedArray<Short> &);
  template Bool allLE (const MaskedArray<uShort> &, const MaskedArray<uShort> &);
  template Bool allLE (const MaskedArray<Long> &, const MaskedArray<Long> &);
  template Bool allLE (const MaskedArray<uLong> &, const MaskedArray<uLong> &);
  template Bool allLE (const MaskedArray<Float> &, const MaskedArray<Float> &);
  template Bool allLE (const MaskedArray<Double> &, const MaskedArray<Double> &);

  // allOR

  template bool allOR (bool const&, MaskedArray<bool> const&);
  template bool allOR (int const&, MaskedArray<int> const&);
  template bool allOR (uInt const&, MaskedArray<uInt> const&);
  template bool allOR (Short const&, MaskedArray<Short> const&);
  template bool allOR (uShort const&, MaskedArray<uShort> const&);
  template bool allOR (Long const&, MaskedArray<Long> const&);
  template bool allOR (uLong const&, MaskedArray<uLong> const&);
  template bool allOR (Float const&, MaskedArray<Float> const&);
  template bool allOR (Double const&, MaskedArray<Double> const&);

  template bool allOR (MaskedArray<bool> const&, bool const&);
  template bool allOR (MaskedArray<int> const&, int const&);
  template bool allOR (MaskedArray<uInt> const&, uInt const&);
  template bool allOR (MaskedArray<Short> const&, Short const&);
  template bool allOR (MaskedArray<uShort> const&, uShort const&);
  template bool allOR (MaskedArray<Long> const&, Long const&);
  template bool allOR (MaskedArray<uLong> const&, uLong const&);
  template bool allOR (MaskedArray<Float> const&, Float const&);
  template bool allOR (MaskedArray<Double> const&, Double const&);

  template Bool allOR (const Array<Bool> &, const MaskedArray<Bool> &);
  template Bool allOR (const Array<Int> &, const MaskedArray<Int> &);
  template Bool allOR (const Array<uInt> &, const MaskedArray<uInt> &);
  template Bool allOR (const Array<Short> &, const MaskedArray<Short> &);
  template Bool allOR (const Array<uShort> &, const MaskedArray<uShort> &);
  template Bool allOR (const Array<Long> &, const MaskedArray<Long> &);
  template Bool allOR (const Array<uLong> &, const MaskedArray<uLong> &);
  template Bool allOR (const Array<Float> &, const MaskedArray<Float> &);
  template Bool allOR (const Array<Double> &, const MaskedArray<Double> &);

  template Bool allOR (const MaskedArray<Bool> &, const Array<Bool> &);
  template Bool allOR (const MaskedArray<Int> &, const Array<Int> &);
  template Bool allOR (const MaskedArray<uInt> &, const Array<uInt> &);
  template Bool allOR (const MaskedArray<Short> &, const Array<Short> &);
  template Bool allOR (const MaskedArray<uShort> &, const Array<uShort> &);
  template Bool allOR (const MaskedArray<Long> &, const Array<Long> &);
  template Bool allOR (const MaskedArray<uLong> &, const Array<uLong> &);
  template Bool allOR (const MaskedArray<Float> &, const Array<Float> &);
  template Bool allOR (const MaskedArray<Double> &, const Array<Double> &);

  template Bool allOR (const MaskedArray<Bool> &, const MaskedArray<Bool> &);
  template Bool allOR (const MaskedArray<Int> &, const MaskedArray<Int> &);
  template Bool allOR (const MaskedArray<uInt> &, const MaskedArray<uInt> &);
  template Bool allOR (const MaskedArray<Short> &, const MaskedArray<Short> &);
  template Bool allOR (const MaskedArray<uShort> &, const MaskedArray<uShort> &);
  template Bool allOR (const MaskedArray<Long> &, const MaskedArray<Long> &);
  template Bool allOR (const MaskedArray<uLong> &, const MaskedArray<uLong> &);
  template Bool allOR (const MaskedArray<Float> &, const MaskedArray<Float> &);
  template Bool allOR (const MaskedArray<Double> &, const MaskedArray<Double> &);

  // anyAND

  template bool anyAND (bool const&, MaskedArray<bool> const&);
  template bool anyAND (int const&, MaskedArray<int> const&);
  template bool anyAND (uInt const&, MaskedArray<uInt> const&);
  template bool anyAND (Short const&, MaskedArray<Short> const&);
  template bool anyAND (uShort const&, MaskedArray<uShort> const&);
  template bool anyAND (Long const&, MaskedArray<Long> const&);
  template bool anyAND (uLong const&, MaskedArray<uLong> const&);
  template bool anyAND (Float const&, MaskedArray<Float> const&);
  template bool anyAND (Double const&, MaskedArray<Double> const&);

  template bool anyAND (MaskedArray<bool> const&, bool const&);
  template bool anyAND (MaskedArray<int> const&, int const&);
  template bool anyAND (MaskedArray<uInt> const&, uInt const&);
  template bool anyAND (MaskedArray<Short> const&, Short const&);
  template bool anyAND (MaskedArray<uShort> const&, uShort const&);
  template bool anyAND (MaskedArray<Long> const&, Long const&);
  template bool anyAND (MaskedArray<uLong> const&, uLong const&);
  template bool anyAND (MaskedArray<Float> const&, Float const&);
  template bool anyAND (MaskedArray<Double> const&, Double const&);

  template Bool anyAND (const Array<Bool> &, const MaskedArray<Bool> &);
  template Bool anyAND (const Array<Int> &, const MaskedArray<Int> &);
  template Bool anyAND (const Array<uInt> &, const MaskedArray<uInt> &);
  template Bool anyAND (const Array<Short> &, const MaskedArray<Short> &);
  template Bool anyAND (const Array<uShort> &, const MaskedArray<uShort> &);
  template Bool anyAND (const Array<Long> &, const MaskedArray<Long> &);
  template Bool anyAND (const Array<uLong> &, const MaskedArray<uLong> &);
  template Bool anyAND (const Array<Float> &, const MaskedArray<Float> &);
  template Bool anyAND (const Array<Double> &, const MaskedArray<Double> &);

  template Bool anyAND (const MaskedArray<Bool> &, const Array<Bool> &);
  template Bool anyAND (const MaskedArray<Int> &, const Array<Int> &);
  template Bool anyAND (const MaskedArray<uInt> &, const Array<uInt> &);
  template Bool anyAND (const MaskedArray<Short> &, const Array<Short> &);
  template Bool anyAND (const MaskedArray<uShort> &, const Array<uShort> &);
  template Bool anyAND (const MaskedArray<Long> &, const Array<Long> &);
  template Bool anyAND (const MaskedArray<uLong> &, const Array<uLong> &);
  template Bool anyAND (const MaskedArray<Float> &, const Array<Float> &);
  template Bool anyAND (const MaskedArray<Double> &, const Array<Double> &);

  template Bool anyAND (const MaskedArray<Bool> &, const MaskedArray<Bool> &);
  template Bool anyAND (const MaskedArray<Int> &, const MaskedArray<Int> &);
  template Bool anyAND (const MaskedArray<uInt> &, const MaskedArray<uInt> &);
  template Bool anyAND (const MaskedArray<Short> &, const MaskedArray<Short> &);
  template Bool anyAND (const MaskedArray<uShort> &, const MaskedArray<uShort> &);
  template Bool anyAND (const MaskedArray<Long> &, const MaskedArray<Long> &);
  template Bool anyAND (const MaskedArray<uLong> &, const MaskedArray<uLong> &);
  template Bool anyAND (const MaskedArray<Float> &, const MaskedArray<Float> &);
  template Bool anyAND (const MaskedArray<Double> &, const MaskedArray<Double> &);

  // anyOR

  template bool anyOR (bool const&, MaskedArray<bool> const&);
  template bool anyOR (int const&, MaskedArray<int> const&);
  template bool anyOR (uInt const&, MaskedArray<uInt> const&);
  template bool anyOR (Short const&, MaskedArray<Short> const&);
  template bool anyOR (uShort const&, MaskedArray<uShort> const&);
  template bool anyOR (Float const&, MaskedArray<Float> const&);
  template bool anyOR (Double const&, MaskedArray<Double> const&);

  template bool anyOR (MaskedArray<bool> const&, bool const&);

  template Bool anyOR (const Array<Bool> &, const MaskedArray<Bool> &);
  template Bool anyOR (const Array<Int> &, const MaskedArray<Int> &);
  template Bool anyOR (const Array<uInt> &, const MaskedArray<uInt> &);
  template Bool anyOR (const Array<Short> &, const MaskedArray<Short> &);
  template Bool anyOR (const Array<uShort> &, const MaskedArray<uShort> &);
  template Bool anyOR (const Array<Long> &, const MaskedArray<Long> &);
  template Bool anyOR (const Array<uLong> &, const MaskedArray<uLong> &);
  template Bool anyOR (const Array<Float> &, const MaskedArray<Float> &);
  template Bool anyOR (const Array<Double> &, const MaskedArray<Double> &);

  template Bool anyOR (const MaskedArray<Bool> &, const Array<Bool> &);
  template Bool anyOR (const MaskedArray<Int> &, const Array<Int> &);
  template Bool anyOR (const MaskedArray<uInt> &, const Array<uInt> &);
  template Bool anyOR (const MaskedArray<Short> &, const Array<Short> &);
  template Bool anyOR (const MaskedArray<uShort> &, const Array<uShort> &);
  template Bool anyOR (const MaskedArray<Long> &, const Array<Long> &);
  template Bool anyOR (const MaskedArray<uLong> &, const Array<uLong> &);
  template Bool anyOR (const MaskedArray<Float> &, const Array<Float> &);
  template Bool anyOR (const MaskedArray<Double> &, const Array<Double> &);

  template Bool anyOR (const MaskedArray<Bool> &, const MaskedArray<Bool> &);
  template Bool anyOR (const MaskedArray<Int> &, const MaskedArray<Int> &);
  template Bool anyOR (const MaskedArray<uInt> &, const MaskedArray<uInt> &);
  template Bool anyOR (const MaskedArray<Short> &, const MaskedArray<Short> &);
  template Bool anyOR (const MaskedArray<uShort> &, const MaskedArray<uShort> &);
  template Bool anyOR (const MaskedArray<Long> &, const MaskedArray<Long> &);
  template Bool anyOR (const MaskedArray<uLong> &, const MaskedArray<uLong> &);
  template Bool anyOR (const MaskedArray<Float> &, const MaskedArray<Float> &);
  template Bool anyOR (const MaskedArray<Double> &, const MaskedArray<Double> &);

}
