
// include of header files 

#include <casa/Containers/OrderedPair.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <scimath/Functionals/AbstractFunctionFactory.h>
#include <scimath/Mathematics/AutoDiffRep.h>
#include <scimath/Mathematics/SquareMatrix.h>

// include of implementation files

#include <casa/Containers/Link.cc>
#include <casa/Containers/List.cc>
#include <casa/Containers/ListMap.cc>
#include <casa/Containers/Map.cc>
#include <casa/Containers/ObjectPool.cc>
#include <casa/Containers/OrderedMap.cc>
#include <casa/Containers/OrderedPair.cc>
#include <casa/Containers/PoolStack.cc>

namespace casa {

  template class Link<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > >;
  template class Link<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > >;

  template class List<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > >;
  template class ListIter<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > >;
  template class ListNotice<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > >;
  template class ConstListIter<OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> > >;
  template class List<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > >;
  template class ListIter<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > >;
  template class ListNotice<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > >;
  template class ConstListIter<OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> > >;

  template class ListMap<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class ListMapRep<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class ListMapIterRep<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class ListMap<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class ListMapRep<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class ListMapIterRep<String, OrderedPair<FunctionFactory<Float>*, Bool> >;

  template class Map<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class MapRep<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class ConstMapIter<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class MapIter<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class MapIterRep<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class Map<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class MapRep<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class ConstMapIter<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class MapIter<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class MapIterRep<String, OrderedPair<FunctionFactory<Float>*, Bool> >; 
  template class Map<Double, SquareMatrix<Complex, 2>*>;
  template class ConstMapIter<Double, SquareMatrix<Complex, 2>*>;
  template class MapIter<Double, SquareMatrix<Complex, 2>*>;
  template class MapIterRep<Double, SquareMatrix<Complex, 2>*>;
  template class MapRep<Double, SquareMatrix<Complex, 2>*>;

  template class ObjectPool<AutoDiffRep<Complex>, uInt>;
  template class ObjectPool<AutoDiffRep<DComplex>, uInt>;
  template class ObjectPool<AutoDiffRep<Double>, uInt>;
  template class ObjectPool<AutoDiffRep<Float>, uInt>;

  template class OrderedMap<Double, SquareMatrix<Complex, 2>*>;
  template class OrderedMapRep<Double, SquareMatrix<Complex, 2>*>;
  template class OrderedMapIterRep<Double, SquareMatrix<Complex, 2>*>;
  template class OrderedMapNotice<Double, SquareMatrix<Complex, 2>*>;

 
  template class OrderedPair<FunctionFactory<Double>*, Bool>;
  template class OrderedPair<String, OrderedPair<FunctionFactory<Double>*, Bool> >;
  template class OrderedPair<FunctionFactory<Float>*, Bool>;
  template class OrderedPair<String, OrderedPair<FunctionFactory<Float>*, Bool> >;
  template class OrderedPair<Double, SquareMatrix<Complex, 2>*>;

  template class PoolStack<AutoDiffRep<Complex>, uInt>;
  template class PoolStack<AutoDiffRep<DComplex>, uInt>;
  template class PoolStack<AutoDiffRep<Double>, uInt>;
  template class PoolStack<AutoDiffRep<Float>, uInt>;

}
