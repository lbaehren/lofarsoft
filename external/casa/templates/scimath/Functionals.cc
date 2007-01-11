
// include of header files

#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Block.h>
#include <scimath/Functionals/Function1D.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <scimath/Functionals/SampledFunctional.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/BasicMath/Functional.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/GenSort.cc>
#include <scimath/Functionals/ArraySampledFunctional.cc>
#include <scimath/Functionals/Function.cc>
#include <scimath/Functionals/FunctionOrder.cc>
#include <scimath/Functionals/FunctionParam.cc>
#include <scimath/Functionals/Gaussian3D.cc>
#include <scimath/Functionals/Gaussian3DParam.cc>
#include <scimath/Functionals/GaussianND.cc>
#include <scimath/Functionals/GaussianNDParam.cc>
#include <scimath/Functionals/Interpolate1D.cc>
#include <scimath/Functionals/ScalarSampledFunctional.cc>
#include <scimath/Mathematics/MatrixMathLA.cc>

namespace casa {

  template class Array<Array<Float> >;

  template Bool anyLE(Array<Double> const &, Double const &);

  template Array<Complex> operator*(Array<Complex> const &, Array<Complex> const &);
  template Array<Complex> operator*(Complex const &, Array<Complex> const &);
  template void indgen(Array<Complex> &);
  template void indgen(Array<Complex> &, Complex, Complex);
  template Array<Complex> operator+(Array<Complex> const &, Complex const &);
  template void indgen(Array<DComplex> &);
  template void indgen(Array<DComplex> &, DComplex, DComplex);

  template class MaskedArray<Array<Float> >;

  template class Vector<Array<Float> >;

  template class Functional<uInt, Array<Complex> >;
  template class Functional<Float, Array<Float> >;
  template class Functional<Float, Double>;
  template class Functional<Int, Double>;
  template class Functional<uInt, Int>;
  template class Functional<Double, DComplex>;
  template class Functional<uInt, DComplex>;
  template class Functional<uInt, Array<Float> >;
  template class Functional<uInt, Array<DComplex> >;
  template class Functional<Vector<Float>, Double>;
  template class Functional<Vector<Int>, Double>;
  template class Functional<Vector<Double>, DComplex>;
  template class Functional<Vector<Float>, Array<Float> >;

  template Int binarySearchBrackets(Bool &, Block<Int> const &, Int const &, uInt, Int);

  template class CountedConstPtr<Block<Array<Float> > >;
  template class CountedPtr<Block<Array<Float> > >;
  template class PtrRep<Block<Array<Float> > >;
  template class SimpleCountedConstPtr<Block<Array<Float> > >;
  template class SimpleCountedPtr<Block<Array<Float> > >;

  template uInt genSort(Vector<uInt> &, Block<Float> const &);

  template class ArraySampledFunctional<Array<Complex> >;
  template class ArraySampledFunctional<Array<Double> >;
  template class ArraySampledFunctional<Array<Float> >;

  template class Function<Float, Array<Float> >;
  template class Function<Float, Double>;
  template class Function<Int, Double>;
  template ostream & operator<<(ostream &, Function<Double, Double> const &);
  template class Function<Double, DComplex>;

  template class Function1D<Float, Array<Float> >;
  template class Function1D<Float, Double>;
  template class Function1D<Int, Double>;
  template class Function1D<Double, DComplex>;

  template class FunctionOrder<Double>;
  template ostream & operator<<(ostream &, FunctionOrder<Double> const &);
//   template const String &FunctionOrder<Double>::ident() const;
//   template Bool FunctionOrder<Double>::toRecord(String &, RecordInterface &out) const;
//   template Bool FunctionOrder<Double>::fromString(String &, String const &);
//   template Bool FunctionOrder<Double>::fromRecord(String &, RecordInterface const &);

  template class FunctionParam<Int>;

  template class Gaussian3D<AutoDiffA<Double> >;

  template class Gaussian3DParam<AutoDiffA<Double> >;

  template class GaussianND<Float>;

  template class GaussianNDParam<Float>;

  template class Interpolate1D<Float, Array<Float> >;
  template class Interpolate1D<Float, Double>;
  template class Interpolate1D<Float, Float>;
  template class Interpolate1D<Int, Double>;
  template class Interpolate1D<Double, DComplex>;

  template class SampledFunctional<Array<Complex> >;
  template class SampledFunctional<Array<Float> >;
  template class SampledFunctional<Int>;
  template class SampledFunctional<DComplex>;

  template class ScalarSampledFunctional<Int>;
  template class ScalarSampledFunctional<DComplex>;

  template void CholeskyDecomp(Matrix<Float> &, Vector<Float> &);
  template void CholeskySolve(Matrix<Float> &, Vector<Float> &, Vector<Float> &, Vector<Float> &);
  template Matrix<Float> invertSymPosDef(Matrix<Float> const &);
  template void invertSymPosDef(Matrix<Float> &, Float &, Matrix<Float> const &);

}
