
// include of header files 

#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/List.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Containers/OrderedPair.h>
#include <scimath/Functionals/AbstractFunctionFactory.h>
#include <scimath/Functionals/Function.h>
#include <scimath/Functionals/Function1D.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/RigidVector.h>
#include <scimath/Mathematics/SquareMatrix.h>

// include of implementation files

#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <casa/Utilities/Register.cc> 

namespace casa {

  // casa/Utilities/CountedPtr

  template class SimpleCountedConstPtr<Block<AutoDiffA<Complex> > >;
  template class SimpleCountedPtr<Block<AutoDiffA<Complex> > >;
  template class CountedConstPtr<Block<AutoDiffA<Complex> > >;
  template class CountedPtr<Block<AutoDiffA<Complex> > >;
  template class PtrRep<Block<AutoDiffA<Complex> > >;
  template class SimpleCountedConstPtr<Block<AutoDiffA<DComplex> > >;
  template class SimpleCountedPtr<Block<AutoDiffA<DComplex> > >;
  template class CountedConstPtr<Block<AutoDiffA<DComplex> > >;
  template class CountedPtr<Block<AutoDiffA<DComplex> > >;
  template class PtrRep<Block<AutoDiffA<DComplex> > >;
  template class CountedPtr<Block<Vector<SquareMatrix<Complex, 2> > > >;
  template class CountedConstPtr<Block<Vector<SquareMatrix<Complex, 2> > > >;
  template class SimpleCountedPtr<Block<Vector<SquareMatrix<Complex, 2> > > >;
  template class SimpleCountedConstPtr<Block<Vector<SquareMatrix<Complex, 2> > > >;
  template class PtrRep<Block<Vector<SquareMatrix<Complex, 2> > > >;
  template class CountedPtr<Block<Vector<SquareMatrix<Complex, 4> > > >;
  template class CountedConstPtr<Block<Vector<SquareMatrix<Complex, 4> > > >;
  template class SimpleCountedPtr<Block<Vector<SquareMatrix<Complex, 4> > > >;
  template class SimpleCountedConstPtr<Block<Vector<SquareMatrix<Complex, 4> > > >;
  template class PtrRep<Block<Vector<SquareMatrix<Complex, 4> > > >;
  template class CountedConstPtr<Block<Vector<SquareMatrix<Float, 2> > > >;
  template class CountedPtr<Block<Vector<SquareMatrix<Float, 2> > > >;
  template class PtrRep<Block<Vector<SquareMatrix<Float, 2> > > >;
  template class SimpleCountedConstPtr<Block<Vector<SquareMatrix<Float, 2> > > >;
  template class SimpleCountedPtr<Block<Vector<SquareMatrix<Float, 2> > > >;
  template class CountedConstPtr<Block<OrderedMap<Double, SquareMatrix<Complex, 2>*>*> >;
  template class CountedPtr<Block<OrderedMap<Double, SquareMatrix<Complex, 2>*>*> >;
  template class SimpleCountedConstPtr<Block<OrderedMap<Double, SquareMatrix<Complex, 2>*>*> >;
  template class SimpleCountedPtr<Block<OrderedMap<Double, SquareMatrix<Complex, 2>*>*> >;
  template class PtrRep<Block<OrderedMap<Double, SquareMatrix<Complex, 2>*>*> >;
  template class CountedConstPtr<Block<CountedPtr<Function1D<Float> > > >;
  template class CountedPtr<Block<CountedPtr<Function1D<Float> > > >;
  template class SimpleCountedConstPtr<Block<CountedPtr<Function1D<Float> > > >;
  template class SimpleCountedPtr<Block<CountedPtr<Function1D<Float> > > >;
  template class PtrRep<Block<CountedPtr<Function1D<Float> > > >;
  template class CountedPtr<Block<RigidVector<Double, 3> > >;
  template class CountedConstPtr<Block<RigidVector<Double, 3> > >;
  template class SimpleCountedPtr<Block<RigidVector<Double, 3> > >;
  template class SimpleCountedConstPtr<Block<RigidVector<Double, 3> > >;
  template class PtrRep<Block<RigidVector<Double, 3> > >;
  template class CountedPtr<Block<RigidVector<Double, 2> > >;
  template class CountedConstPtr<Block<RigidVector<Double, 2> > >;
  template class SimpleCountedPtr<Block<RigidVector<Double, 2> > >;
  template class SimpleCountedConstPtr<Block<RigidVector<Double, 2> > >;
  template class PtrRep<Block<RigidVector<Double, 2> > >;
  template class CountedPtr<Block<SquareMatrix<Complex, 2> > >;
  template class CountedConstPtr<Block<SquareMatrix<Complex, 2> > >;
  template class SimpleCountedPtr<Block<SquareMatrix<Complex, 2> > >;
  template class SimpleCountedConstPtr<Block<SquareMatrix<Complex, 2> > >;
  template class PtrRep<Block<SquareMatrix<Complex, 2> > >;
  template class CountedPtr<Block<SquareMatrix<Complex, 4> > >;
  template class CountedConstPtr<Block<SquareMatrix<Complex, 4> > >;
  template class SimpleCountedPtr<Block<SquareMatrix<Complex, 4> > >;
  template class SimpleCountedConstPtr<Block<SquareMatrix<Complex, 4> > >;
  template class PtrRep<Block<SquareMatrix<Complex, 4> > >;
  template class CountedPtr<Block<SquareMatrix<Float, 2> > >;
  template class CountedConstPtr<Block<SquareMatrix<Float, 2> > >;
  template class SimpleCountedPtr<Block<SquareMatrix<Float, 2> > >;
  template class SimpleCountedConstPtr<Block<SquareMatrix<Float, 2> > >;
  template class PtrRep<Block<SquareMatrix<Float, 2> > >;
  template class CountedPtr<Block<SquareMatrix<Float, 4> > >;
  template class CountedConstPtr<Block<SquareMatrix<Float, 4> > >;
  template class SimpleCountedPtr<Block<SquareMatrix<Float, 4> > >;
  template class SimpleCountedConstPtr<Block<SquareMatrix<Float, 4> > >;
  template class PtrRep<Block<SquareMatrix<Float, 4> > >;
  template class SimpleCountedConstPtr<Function1D<Float> >;
  template class SimpleCountedPtr<Function1D<Float> >;
  template class CountedConstPtr<Function1D<Float> >;
  template class CountedPtr<Function1D<Float> >;
  template class PtrRep<Function1D<Float> >;
  template class CountedPtr<Block<AutoDiff<Complex> > >;
  template class CountedConstPtr<Block<AutoDiff<Complex> > >;
  template class SimpleCountedPtr<Block<AutoDiff<Complex> > >;
  template class SimpleCountedConstPtr<Block<AutoDiff<Complex> > >;
  template class PtrRep<Block<AutoDiff<Complex> > >;
  template class CountedPtr<Block<AutoDiff<DComplex> > >;
  template class CountedConstPtr<Block<AutoDiff<DComplex> > >;
  template class SimpleCountedPtr<Block<AutoDiff<DComplex> > >;
  template class SimpleCountedConstPtr<Block<AutoDiff<DComplex> > >;
  template class PtrRep<Block<AutoDiff<DComplex> > >;
  template class CountedPtr<Block<AutoDiff<Double> > >;
  template class CountedConstPtr<Block<AutoDiff<Double> > >;
  template class SimpleCountedPtr<Block<AutoDiff<Double> > >;
  template class SimpleCountedConstPtr<Block<AutoDiff<Double> > >;
  template class PtrRep<Block<AutoDiff<Double> > >;
  template class CountedPtr<Block<AutoDiff<Float> > >;
  template class CountedConstPtr<Block<AutoDiff<Float> > >;
  template class SimpleCountedPtr<Block<AutoDiff<Float> > >;
  template class SimpleCountedConstPtr<Block<AutoDiff<Float> > >;
  template class PtrRep<Block<AutoDiff<Float> > >;
  template class CountedPtr<Block<AutoDiffA<Double> > >;
  template class CountedConstPtr<Block<AutoDiffA<Double> > >;
  template class SimpleCountedPtr<Block<AutoDiffA<Double> > >;
  template class SimpleCountedConstPtr<Block<AutoDiffA<Double> > >;
  template class PtrRep<Block<AutoDiffA<Double> > >;
  template class CountedPtr<Block<AutoDiffA<Float> > >;
  template class CountedConstPtr<Block<AutoDiffA<Float> > >;
  template class SimpleCountedPtr<Block<AutoDiffA<Float> > >;
  template class SimpleCountedConstPtr<Block<AutoDiffA<Float> > >;
  template class PtrRep<Block<AutoDiffA<Float> > >;
  template class SimpleCountedConstPtr<SquareMatrix<Complex, 4> >;
  template class SimpleCountedPtr<SquareMatrix<Complex, 4> >;
  template class CountedConstPtr<SquareMatrix<Complex, 4> >;
  template class CountedPtr<SquareMatrix<Complex, 4> >;
  template class PtrRep<SquareMatrix<Complex, 4> >;
  template class SimpleCountedConstPtr<SquareMatrix<Complex, 2> >;
  template class SimpleCountedPtr<SquareMatrix<Complex, 2> >;
  template class CountedConstPtr<SquareMatrix<Complex, 2> >;
  template class CountedPtr<SquareMatrix<Complex, 2> >;
  template class PtrRep<SquareMatrix<Complex, 2> >;
  template class SimpleCountedConstPtr<Block<CountedPtr<SquareMatrix<Complex, 4> > > >;
  template class SimpleCountedPtr<Block<CountedPtr<SquareMatrix<Complex, 4> > > >;
  template class CountedConstPtr<Block<CountedPtr<SquareMatrix<Complex, 4> > > >;
  template class CountedPtr<Block<CountedPtr<SquareMatrix<Complex, 4> > > >;
  template class PtrRep<Block<CountedPtr<SquareMatrix<Complex, 4> > > >;
  template class SimpleCountedConstPtr<Block<CountedPtr<SquareMatrix<Complex, 2> > > >;
  template class SimpleCountedPtr<Block<CountedPtr<SquareMatrix<Complex, 2> > > >;
  template class CountedConstPtr<Block<CountedPtr<SquareMatrix<Complex, 2> > > >;
  template class CountedPtr<Block<CountedPtr<SquareMatrix<Complex, 2> > > >;
  template class PtrRep<Block<CountedPtr<SquareMatrix<Complex, 2> > > >;

  // casa/Utilities/PtrHolder

  template class PtrHolder<Function<DComplex> >;
  template class PtrHolder<Function<Double> >;

  // casa/Utilities/Register

  template uInt Register(OrderedMapNotice<Double, SquareMatrix<Complex, 2>*> const *);
  template uInt Register(ListNotice<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > > const *);
  template uInt Register(ListNotice<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > > const *);

}
