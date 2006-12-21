
// include of header files 

#include <casa/BasicSL/Complex.h>
#include <casa/Containers/PoolStack.h>
#include <casa/Utilities/Fallible.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffIO.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <scimath/Mathematics/AutoDiffRep.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/ObjectPool.cc>
#include <casa/Containers/OrderedPair.cc>
#include <casa/Containers/PoolStack.cc>
#include <casa/Containers/SimOrdMap.cc>
#include <scimath/Mathematics/AutoDiff.cc>
#include <scimath/Mathematics/AutoDiffIO.cc>
#include <scimath/Mathematics/AutoDiffMath.cc>
#include <scimath/Mathematics/AutoDiffRep.cc>
#include <scimath/Mathematics/Convolver.cc>
#include <scimath/Mathematics/FFTServer.cc>
#include <scimath/Mathematics/HistAcc.cc>
#include <scimath/Mathematics/MathFunc.cc>
#include <scimath/Mathematics/MatrixMathLA.cc>
#include <scimath/Mathematics/StatAcc.cc>

namespace casa {

  template class Array<Double>;

  template class AutoDiff<Int>;
  template class AutoDiff<Float>;
  template class AutoDiff<Double>;
  template class AutoDiff<AutoDiff<Double> >;

//   template class Array<AutoDiff<AutoDiff<Double> > >:

  template ostream & operator<<(ostream &, Array<AutoDiff<Double> > const &);

  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);

  template void operator+=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator-=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator*=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator/=(Array<AutoDiff<Double> > &, AutoDiff<Double> const &);
  template void operator+=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator-=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator*=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template void operator/=(Array<AutoDiff<Double> > &, Array<AutoDiff<Double> > const &);
  template Array<AutoDiff<Double> > operator-(Array<AutoDiff<Double> > const &);

  template class MaskedArray<AutoDiff<AutoDiff<Double> > >;

  template class Vector<AutoDiff<AutoDiff<Double> > >;

  template class ObjectPool<AutoDiffRep<AutoDiff<Double> >, uInt>;

  template <class T> class AutoDiffRep;
  template class OrderedPair<uInt, PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt> *>;

  template class PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt>;

  template <class T> class AutoDiffRep;
  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<AutoDiff<Double> >, uInt>*>;

  template class Fallible<Float>;

  template ostream & operator<<(ostream &, AutoDiff<AutoDiff<Double> > const &);

  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<AutoDiff<Double> > const &, AutoDiff<AutoDiff<Double> > const &);
  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<Double> const &, AutoDiff<AutoDiff<Double> > const &);
  template AutoDiff<AutoDiff<Double> > operator*(AutoDiff<AutoDiff<Double> > const &, AutoDiff<Double> const &);

  template class AutoDiffRep<AutoDiff<Double> >;

  template class Convolver<Double>;

  template class FFTServer<Double, DComplex>;

  template class HistAcc<Float>;
  template class HistAcc<Int>;

  template class ExpSincConv<Float>;
  template class ExpSincConv<Double>;
  template class GaussianConv<Float>;
  template class GaussianConv<Double>;
  template class KB_Conv<Float>;
  template class KB_Conv<Double>;
  template class MathFunc<Float>;
//   template class MathFunc<Double>;
  template class Mod_KB_Conv<Float>;
  template class Mod_KB_Conv<Double>;
  template class Sinc_Conv<Float>;
  template class Sinc_Conv<Double>;
  template class Sph_Conv<Float>;
//   template class Sph_Conv<Double>;
  template class Unary<Float>;
  template class Unary<Double>;

  template Matrix<DComplex> invert(Matrix<DComplex> const &);
  template void invert(Matrix<DComplex> &, DComplex &, Matrix<DComplex> const &);

  template class StatAcc<Double>;
  template class StatAcc<Float>;
  template class StatAcc<Int>;

}
