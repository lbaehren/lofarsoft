
// include of header files

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/IterError.h>
#include <casa/Containers/RecordDescRep.h>
#include <casa/Containers/RecordRep.h>
#include <casa/Exceptions/Error.h>
#include <casa/Quanta/QuantumHolder.h>

// include of implementation files

#include <casa/Utilities/Assert.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/COWPtr.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/Compare.cc>

namespace casa {
  template class assert_<ArrayError>;
  template class assert_<InvalidIterError>;
  template class assert_<AbortError>;
  template class assert_<AipsError>;

  template Int binarySearchBrackets(Bool &, Vector<Float> const &, Float const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Complex> const &, Complex const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Double> const &, Double const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Float> const &, Float const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<uInt> const &, uInt const &, uInt, Int);

  template class COWPtr<Array<Complex> >;
  template class COWPtr<Array<Bool> >;
  template class COWPtr<Array<Double> >;
  template class COWPtr<Array<Float> >;
  template class COWPtr<Array<Int> >;
  template class COWPtr<Array<uInt> >;

  template class COWPtr<RecordDescRep>;
  template class COWPtr<RecordRep>;
  template class COWPtr<Array<DComplex> >;
  template class CountedPtr<Array<DComplex> >;
  template class PtrRep<Array<DComplex> >;
  template class SimpleCountedConstPtr<Array<DComplex> >;
  template class CountedConstPtr<Array<DComplex> >;
  template class SimpleCountedPtr<Array<DComplex> >;

  template class ObjCompare<Complex>;
  template class ObjCompare<DComplex>;
  template class ObjCompare<String>;
  template class ObjCompare<Bool>;
  template class ObjCompare<Char>;
  template class ObjCompare<Double>;
  template class ObjCompare<Float>;
  template class ObjCompare<Int>;
  template class ObjCompare<Short>;
  template class ObjCompare<uChar>;
  template class ObjCompare<uInt>;
  template class ObjCompare<uShort>;
  
  template class CountedConstPtr<Array<uInt> >;
  template class CountedPtr<Array<uInt> >;
  template class PtrRep<Array<uInt> >;
  template class SimpleCountedConstPtr<Array<uInt> >;
  template class SimpleCountedPtr<Array<uInt> >;
  template class CountedConstPtr<Block<Slicer> >;
  template class CountedPtr<Block<Slicer> >;
  template class PtrRep<Block<Slicer> >;
  template class SimpleCountedConstPtr<Block<Slicer> >;
  template class SimpleCountedPtr<Block<Slicer> >;
  template class CountedConstPtr<Block<QuantumHolder> >;
  template class CountedPtr<Block<QuantumHolder> >;
  template class PtrRep<Block<QuantumHolder> >;
  template class SimpleCountedConstPtr<Block<QuantumHolder> >;
  template class SimpleCountedPtr<Block<QuantumHolder> >;
}

// 1030 casa/Utilities/CountedPtr.cc 
//      = casa/Containers/Block.h 
//      = casa/Quanta/Unit.h 
//      template class CountedPtr<Block<Unit> > 
//      template class CountedConstPtr<Block<Unit> > 
//      template class PtrRep<Block<Unit> > 
//      template class SimpleCountedPtr<Block<Unit> > 
//      template class SimpleCountedConstPtr<Block<Unit> > 
// 1040 casa/Utilities/CountedPtr.cc 
//      = casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Long> > 
//      template class CountedPtr<Block<Long> > 
//      template class PtrRep<Block<Long> > 
//      template class SimpleCountedConstPtr<Block<Long> > 
//      template class SimpleCountedPtr<Block<Long> > 
// 1050 casa/Utilities/CountedPtr.cc 
//      = casa/Containers/Block.h 
//      template class CountedConstPtr<Block<uLong> > 
//      template class CountedPtr<Block<uLong> > 
//      template class PtrRep<Block<uLong> > 
//      template class SimpleCountedConstPtr<Block<uLong> > 
//      template class SimpleCountedPtr<Block<uLong> > 
// 1060 casa/Utilities/CountedPtr.cc 
//      = casa/Containers/Block.h 
//      template class CountedConstPtr<Block<void *> > 
//      template class CountedPtr<Block<void *> > 
//      template class PtrRep<Block<void *> > 
//      template class SimpleCountedConstPtr<Block<void *> > 
//      template class SimpleCountedPtr<Block<void *> > 
// 1070 casa/Utilities/CountedPtr.cc 
//      = casa/System/PGPlotterInterface.h 
//      template class CountedPtr<PGPlotterInterface> 
//      template class CountedConstPtr<PGPlotterInterface> 
//      template class SimpleCountedPtr<PGPlotterInterface> 
//      template class SimpleCountedConstPtr<PGPlotterInterface> 
//      template class PtrRep<PGPlotterInterface> 
// 1080 casa/Utilities/CountedPtr.cc casa/Arrays/Array.h 
//      = casa/BasicSL/Complex.h 
//      template class CountedPtr<Array<Complex> > 
//      template class PtrRep<Array<Complex> > 
//      template class SimpleCountedConstPtr<Array<Complex> > 
//      template class CountedConstPtr<Array<Complex> > 
//      template class SimpleCountedPtr<Array<Complex> > 
// 1090 casa/Utilities/CountedPtr.cc casa/Arrays/Array.h 
//      template class CountedConstPtr<Array<Bool> > 
//      template class CountedPtr<Array<Bool> > 
//      template class PtrRep<Array<Bool> > 
//      template class SimpleCountedConstPtr<Array<Bool> > 
//      template class SimpleCountedPtr<Array<Bool> > 
// 1100 casa/Utilities/CountedPtr.cc casa/Arrays/Array.h 
//      template class CountedConstPtr<Array<Double> > 
//      template class CountedPtr<Array<Double> > 
//      template class PtrRep<Array<Double> > 
//      template class SimpleCountedConstPtr<Array<Double> > 
//      template class SimpleCountedPtr<Array<Double> > 
// 1110 casa/Utilities/CountedPtr.cc casa/Arrays/Array.h 
//      template class CountedConstPtr<Array<Float> > 
//      template class CountedPtr<Array<Float> > 
//      template class PtrRep<Array<Float> > 
//      template class SimpleCountedConstPtr<Array<Float> > 
//      template class SimpleCountedPtr<Array<Float> > 
// 1120 casa/Utilities/CountedPtr.cc casa/Arrays/Array.h 
//      template class CountedConstPtr<Array<Int> > 
//      template class CountedPtr<Array<Int> > 
//      template class PtrRep<Array<Int> > 
//      template class SimpleCountedConstPtr<Array<Int> > 
//      template class SimpleCountedPtr<Array<Int> > 
// 1130 casa/Utilities/CountedPtr.cc casa/Arrays/Vector.h 
//      = casa/Containers/Block.h 
//      template class SimpleCountedConstPtr<Block<Vector<Float> > > 
//      template class PtrRep<Block<Vector<Float> > > 
//      template class CountedPtr<Block<Vector<Float> > > 
//      template class CountedConstPtr<Block<Vector<Float> > > 
//      template class SimpleCountedPtr<Block<Vector<Float> > > 
// 1140 casa/Utilities/CountedPtr.cc casa/BasicMath/Random.h 
//      = casa/Containers/Block.h 
//      template class SimpleCountedConstPtr<Random> 
//      template class SimpleCountedPtr<Random> 
//      template class CountedConstPtr<Random> 
//      template class CountedPtr<Random> 
//      template class PtrRep<Random> 
// 1150 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Arrays/Array.h 
//      template class CountedConstPtr<Block<Array<Double> > > 
//      template class CountedPtr<Block<Array<Double> > > 
//      template class PtrRep<Block<Array<Double> > > 
//      template class SimpleCountedConstPtr<Block<Array<Double> > > 
//      template class SimpleCountedPtr<Block<Array<Double> > > 
// 1160 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Arrays/Matrix.h casa/BasicMath/Random.h 
//      template class CountedConstPtr<Block<Matrix<CountedPtr<Random> > > > 
//      template class CountedPtr<Block<Matrix<CountedPtr<Random> > > > 
//      template class SimpleCountedConstPtr<Block<Matrix<CountedPtr<Random> > > > 
//      template class SimpleCountedPtr<Block<Matrix<CountedPtr<Random> > > > 
//      template class PtrRep<Block<Matrix<CountedPtr<Random> > > > 
// 1170 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Arrays/Vector.h casa/BasicSL/Complex.h 
//      template class CountedConstPtr<Block<Vector<Complex> > > 
//      template class CountedPtr<Block<Vector<Complex> > > 
//      template class PtrRep<Block<Vector<Complex> > > 
//      template class SimpleCountedConstPtr<Block<Vector<Complex> > > 
//      template class SimpleCountedPtr<Block<Vector<Complex> > > 
// 1180 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Arrays/Vector.h 
//      template class CountedConstPtr<Block<Vector<Double> > > 
//      template class CountedPtr<Block<Vector<Double> > > 
//      template class PtrRep<Block<Vector<Double> > > 
//      template class SimpleCountedConstPtr<Block<Vector<Double> > > 
//      template class SimpleCountedPtr<Block<Vector<Double> > > 
// 1190 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/BasicMath/Random.h 
//      template class CountedConstPtr<Block<CountedPtr<Random> > > 
//      template class CountedPtr<Block<CountedPtr<Random> > > 
//      template class SimpleCountedConstPtr<Block<CountedPtr<Random> > > 
//      template class SimpleCountedPtr<Block<CountedPtr<Random> > > 
//      template class PtrRep<Block<CountedPtr<Random> > > 
// 1200 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/BasicSL/Complex.h 
//      template class CountedConstPtr<Block<Complex> > 
//      template class CountedPtr<Block<Complex> > 
//      template class PtrRep<Block<Complex> > 
//      template class SimpleCountedConstPtr<Block<Complex> > 
//      template class SimpleCountedPtr<Block<Complex> > 
// 1210 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/BasicSL/Complex.h 
//      template class CountedConstPtr<Block<DComplex> > 
//      template class CountedPtr<Block<DComplex> > 
//      template class PtrRep<Block<DComplex> > 
//      template class SimpleCountedConstPtr<Block<DComplex> > 
//      template class SimpleCountedPtr<Block<DComplex> > 
// 1220 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/BasicSL/String.h casa/Utilities/Regex.h 
//      template class SimpleCountedConstPtr<Regex> 
//      template class CountedConstPtr<Regex> 
//      template class CountedPtr<Regex> 
//      template class PtrRep<Regex> 
//      template class SimpleCountedPtr<Regex> 
// 1230 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/BasicSL/String.h 
//      template class CountedConstPtr<Block<String> > 
//      template class CountedPtr<Block<String> > 
//      template class PtrRep<Block<String> > 
//      template class SimpleCountedConstPtr<Block<String> > 
//      template class SimpleCountedPtr<Block<String> > 
// 1240 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/MVDirection.h 
//      template class CountedConstPtr<Block<MVDirection> > 
//      template class CountedPtr<Block<MVDirection> > 
//      template class PtrRep<Block<MVDirection> > 
//      template class SimpleCountedConstPtr<Block<MVDirection> > 
//      template class SimpleCountedPtr<Block<MVDirection> > 
// 1250 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/MVFrequency.h 
//      template class CountedConstPtr<Block<MVFrequency> > 
//      template class CountedPtr<Block<MVFrequency> > 
//      template class PtrRep<Block<MVFrequency> > 
//      template class SimpleCountedConstPtr<Block<MVFrequency> > 
//      template class SimpleCountedPtr<Block<MVFrequency> > 
// 1260 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/MVPosition.h 
//      template class CountedConstPtr<Block<MVPosition> > 
//      template class CountedPtr<Block<MVPosition> > 
//      template class PtrRep<Block<MVPosition> > 
//      template class SimpleCountedConstPtr<Block<MVPosition> > 
//      template class SimpleCountedPtr<Block<MVPosition> > 
// 1270 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/MVRadialVelocity.h 
//      template class CountedConstPtr<Block<MVRadialVelocity> > 
//      template class CountedPtr<Block<MVRadialVelocity> > 
//      template class PtrRep<Block<MVRadialVelocity> > 
//      template class SimpleCountedConstPtr<Block<MVRadialVelocity> > 
//      template class SimpleCountedPtr<Block<MVRadialVelocity> > 
// 1280 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/MVTime.h 
//      template class CountedPtr<Block<MVTime> > 
//      template class SimpleCountedPtr<Block<MVTime> > 
//      template class CountedConstPtr<Block<MVTime> > 
//      template class SimpleCountedConstPtr<Block<MVTime> > 
//      template class PtrRep<Block<MVTime> > 
// 1290 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/Quantum.h 
//      template class CountedConstPtr<Block<Quantum<Double> > > 
//      template class CountedPtr<Block<Quantum<Double> > > 
//      template class PtrRep<Block<Quantum<Double> > > 
//      template class SimpleCountedConstPtr<Block<Quantum<Double> > > 
//      template class SimpleCountedPtr<Block<Quantum<Double> > > 
// 1300 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      = casa/Quanta/Quantum.h 
//      template class CountedPtr<Block<Quantum<Float> > > 
//      template class CountedConstPtr<Block<Quantum<Float> > > 
//      template class SimpleCountedPtr<Block<Quantum<Float> > > 
//      template class SimpleCountedConstPtr<Block<Quantum<Float> > > 
//      template class PtrRep<Block<Quantum<Float> > > 
// 1310 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Bool> > 
//      template class CountedPtr<Block<Bool> > 
//      template class PtrRep<Block<Bool> > 
//      template class SimpleCountedConstPtr<Block<Bool> > 
//      template class SimpleCountedPtr<Block<Bool> > 
// 1320 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Char> > 
//      template class CountedPtr<Block<Char> > 
//      template class PtrRep<Block<Char> > 
//      template class SimpleCountedConstPtr<Block<Char> > 
//      template class SimpleCountedPtr<Block<Char> > 
// 1330 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Double> > 
//      template class CountedPtr<Block<Double> > 
//      template class PtrRep<Block<Double> > 
//      template class SimpleCountedConstPtr<Block<Double> > 
//      template class SimpleCountedPtr<Block<Double> > 
// 1340 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Float> > 
//      template class CountedPtr<Block<Float> > 
//      template class PtrRep<Block<Float> > 
//      template class SimpleCountedConstPtr<Block<Float> > 
//      template class SimpleCountedPtr<Block<Float> > 
// 1350 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Int> > 
//      template class CountedPtr<Block<Int> > 
//      template class PtrRep<Block<Int> > 
//      template class SimpleCountedConstPtr<Block<Int> > 
//      template class SimpleCountedPtr<Block<Int> > 
// 1360 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<Short> > 
//      template class CountedPtr<Block<Short> > 
//      template class PtrRep<Block<Short> > 
//      template class SimpleCountedConstPtr<Block<Short> > 
//      template class SimpleCountedPtr<Block<Short> > 
// 1370 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<uChar> > 
//      template class CountedPtr<Block<uChar> > 
//      template class PtrRep<Block<uChar> > 
//      template class SimpleCountedConstPtr<Block<uChar> > 
//      template class SimpleCountedPtr<Block<uChar> > 
// 1380 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<uInt> > 
//      template class CountedPtr<Block<uInt> > 
//      template class PtrRep<Block<uInt> > 
//      template class SimpleCountedConstPtr<Block<uInt> > 
//      template class SimpleCountedPtr<Block<uInt> > 
// 1390 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedConstPtr<Block<uShort> > 
//      template class CountedPtr<Block<uShort> > 
//      template class PtrRep<Block<uShort> > 
//      template class SimpleCountedConstPtr<Block<uShort> > 
//      template class SimpleCountedPtr<Block<uShort> > 
// 1400 casa/Utilities/CountedPtr.cc casa/Containers/Block.h 
//      template class CountedPtr<Block<Float *> > 
//      template class SimpleCountedConstPtr<Block<Float *> > 
//      template class CountedConstPtr<Block<Float *> > 
//      template class PtrRep<Block<Float *> > 
//      template class SimpleCountedPtr<Block<Float *> > 
// 1410 casa/Utilities/CountedPtr.cc casa/Containers/RecordDescRep.h 
//      template class CountedConstPtr<RecordDescRep> 
//      template class CountedPtr<RecordDescRep> 
//      template class PtrRep<RecordDescRep> 
//      template class SimpleCountedConstPtr<RecordDescRep> 
//      template class SimpleCountedPtr<RecordDescRep> 
// 1420 casa/Utilities/CountedPtr.cc casa/Containers/RecordRep.h 
//      template class CountedConstPtr<RecordRep> 
//      template class CountedPtr<RecordRep> 
//      template class PtrRep<RecordRep> 
//      template class SimpleCountedConstPtr<RecordRep> 
//      template class SimpleCountedPtr<RecordRep> 
// 1430 casa/Utilities/CountedPtr.cc casa/IO/ByteIO.h 
//      template class CountedPtr<ByteIO> 
//      template class CountedConstPtr<ByteIO> 
//      template class SimpleCountedPtr<ByteIO> 
//      template class SimpleCountedConstPtr<ByteIO> 
//      template class PtrRep<ByteIO> 
// 1440 casa/Utilities/CountedPtr.cc casa/IO/TypeIO.h 
//      template class CountedPtr<TypeIO> 
//      template class CountedConstPtr<TypeIO> 
//      template class SimpleCountedPtr<TypeIO> 
//      template class SimpleCountedConstPtr<TypeIO> 
//      template class PtrRep<TypeIO> 
// 1450 casa/Utilities/CountedPtr.cc casa/Logging/LogSinkInterface.h 
//      template class CountedConstPtr<LogSinkInterface> 
//      template class CountedPtr<LogSinkInterface> 
//      template class PtrRep<LogSinkInterface> 
//      template class SimpleCountedConstPtr<LogSinkInterface> 
//      template class SimpleCountedPtr<LogSinkInterface> 
// 1460 casa/Utilities/CountedPtr.cc casa/OS/DataConversion.h 
//      template class CountedPtr<DataConversion> 
//      template class CountedConstPtr<DataConversion> 
//      template class SimpleCountedPtr<DataConversion> 
//      template class SimpleCountedConstPtr<DataConversion> 
//      template class PtrRep<DataConversion> 
// 1470 casa/Utilities/CountedPtr.cc casa/OS/SysEvent.h 
//      template class CountedConstPtr<SysEventTarget> 
//      template class CountedPtr<SysEventTarget> 
//      template class PtrRep<SysEventTarget> 
//      template class SimpleCountedConstPtr<SysEventTarget> 
//      template class SimpleCountedPtr<SysEventTarget> 
// 1480 casa/Utilities/CountedPtr.cc casa/OS/SysEvent.h 
//      template class CountedConstPtr<SysEventTargetInfo> 
//      template class CountedPtr<SysEventTargetInfo> 
//      template class PtrRep<SysEventTargetInfo> 
//      template class SimpleCountedConstPtr<SysEventTargetInfo> 
//      template class SimpleCountedPtr<SysEventTargetInfo> 
// 1490 casa/Utilities/CountedPtr.cc casa/fstream.h 
//      template class CountedConstPtr<fstream> 
//      template class CountedPtr<fstream> 
//      template class PtrRep<fstream> 
//      template class SimpleCountedConstPtr<fstream> 
//      template class SimpleCountedPtr<fstream> 
// 1000 casa/Utilities/DefaultValue.h casa/BasicSL/Complex.h 
//      template void defaultValue(Complex &) 
//      template void defaultValue(DComplex &) 
// 1010 casa/Utilities/DefaultValue.h 
//      template void defaultValue(Bool &) 
//      template void defaultValue(Double &) 
//      template void defaultValue(Float &) 
//      template void defaultValue(Int &) 
// 1000 casa/Utilities/Fallible.h 
//      template class Fallible<Double> 
// 1010 casa/Utilities/Fallible.h 
//      template class Fallible<Int> 
// 1020 casa/Utilities/Fallible.h 
//      template class Fallible<uInt> 
// 1000 casa/Utilities/GenSort.cc 
//      = casa/BasicSL/Complex.h 
//      template class GenSort<DComplex> 
// 1010 casa/Utilities/GenSort.cc casa/BasicSL/String.h 
//      template class GenSortIndirect<String> 
// 1020 casa/Utilities/GenSort.cc 
//      template class GenSort<Double> 
// 1030 casa/Utilities/GenSort.cc 
//      template class GenSort<Float> 
// 1040 casa/Utilities/GenSort.cc 
//      template class GenSort<Int> 
// 1050 casa/Utilities/GenSort.cc 
//      template class GenSort<String> 
// 1060 casa/Utilities/GenSort.cc 
//      template class GenSort<uInt> 
// 1070 casa/Utilities/GenSort.cc 
//      template class GenSortIndirect<Double> 
//      template uInt genSort(Vector<uInt> &, Block<Double> const &) 
//      template uInt genSort(Vector<uInt> &, Block<Double> const &, Sort::Order) 
// 1080 casa/Utilities/GenSort.cc 
//      template class GenSortIndirect<Float> 
// 1090 casa/Utilities/GenSort.cc 
//      template class GenSortIndirect<Int> 
// 1100 casa/Utilities/GenSort.cc 
//      template class GenSortIndirect<uInt> 
// 1110 casa/Utilities/GenSort.cc 
//      template uInt genSort(Array<uInt> &) 
// 1120 casa/Utilities/GenSort.cc 
//      template uInt genSort(Double *, uInt) 
// 1130 casa/Utilities/GenSort.cc 
//      template uInt genSort(Float *, uInt) 
// 1000 casa/Utilities/LinearSearch.cc casa/Arrays/Vector.h 
//      template Int linearSearch(Bool &, Vector<Int> const &, Int const &, uInt, uInt) 
//      template Int linearSearch(Bool &, Vector<uInt> const &, uInt const &, uInt, uInt) 
// 1000 casa/Utilities/PtrHolder.cc 
//      = casa/BasicMath/Random.h 
//      template class PtrHolder<Random> 
// 1010 casa/Utilities/PtrHolder.cc 
//      = casa/Quanta/QBase.h 
//      template class PtrHolder<QBase> 
// 1020 casa/Utilities/PtrHolder.cc casa/BasicSL/String.h 
//      = casa/Arrays/Vector.h 
//      template class PtrHolder<Vector<String> > 
// 1030 casa/Utilities/PtrHolder.cc casa/Containers/RecordInterface.h 
//      template class PtrHolder<RecordInterface> 
// 1040 casa/Utilities/PtrHolder.cc casa/Logging/LogSinkInterface.h 
//      template class PtrHolder<LogSinkInterface> 
// 1000 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template <class T> class Array 
//      template uInt Register(Quantum<Array<Complex> > const *) 
// 1010 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template <class T> class Array 
//      template uInt Register(Quantum<Array<DComplex> > const *) 
// 1020 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template <class T> class Vector 
//      template uInt Register(Quantum<Vector<Complex> > const *) 
// 1030 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template <class T> class Vector 
//      template uInt Register(Quantum<Vector<DComplex> > const *) 
// 1040 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template uInt Register(Quantum<Complex> const *) 
// 1050 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template <class Qtype> class Quantum 
//      template uInt Register(Quantum<DComplex> const *) 
// 1060 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template uInt Register(Complex const *) 
// 1070 casa/Utilities/Register.cc 
//      = casa/BasicSL/Complex.h 
//      template uInt Register(DComplex const *) 
// 1080 casa/Utilities/Register.cc 
//      = casa/Containers/List.h 
//      template uInt Register(ListNotice<void *> const *) 
// 1090 casa/Utilities/Register.cc 
//      = casa/Containers/OrderedMap.h 
//      = casa/BasicSL/String.h 
//      = casa/Arrays/IPosition.h 
//      = casa/Containers/Block.h 
//      template uInt Register(OrderedMapNotice<String, Block<IPosition> > const *) 
// 1100 casa/Utilities/Register.cc 
//      = casa/Containers/OrderedMap.h 
//      = casa/BasicSL/String.h 
//      template uInt Register(OrderedMapNotice<String, Int> const *) 
// 1110 casa/Utilities/Register.cc casa/BasicSL/String.h 
//      template uInt Register(String const *) 
// 1120 casa/Utilities/Register.cc casa/Containers/List.h 
//      = casa/Inputs/Param.h 
//      template uInt Register(ListNotice<Param> const *) 
// 1130 casa/Utilities/Register.cc casa/Containers/List.h 
//      = casa/Utilities/CountedPtr.h casa/OS/SysEvent.h 
//      template uInt Register(ListNotice<CountedPtr<SysEventTargetInfo> > const *) 
// 1140 casa/Utilities/Register.cc casa/Containers/List.h 
//      template uInt Register(ListNotice<uInt> const *) 
// 1150 casa/Utilities/Register.cc casa/Containers/List.h 
//      template uInt Register(ListNotice<uLong> const *) 
// 1160 casa/Utilities/Register.cc casa/Containers/OrderedMap.h 
//      = casa/Arrays/Array.h 
//      template uInt Register(OrderedMapNotice<Int, Array<Float> > const *) 
// 1170 casa/Utilities/Register.cc casa/Containers/OrderedMap.h 
//      = casa/Arrays/Vector.h 
//      template uInt Register(OrderedMapNotice<Int, Vector<Float> > const *) 
// 1180 casa/Utilities/Register.cc casa/Containers/OrderedMap.h 
//      template uInt Register(OrderedMapNotice<Int, Int> const *) 
// 1190 casa/Utilities/Register.cc casa/Containers/RecordInterface.h 
//      template uInt Register(RecordNotice const *) 
// 1200 casa/Utilities/Register.cc casa/OS/SysEvent.h 
//      template uInt Register(SysEventTargetInfo const *) 
// 1210 casa/Utilities/Register.cc casa/Quanta/MVBaseline.h 
//      template uInt Register(MVBaseline const *) 
// 1220 casa/Utilities/Register.cc casa/Quanta/MVDirection.h 
//      template uInt Register(MVDirection const *) 
// 1230 casa/Utilities/Register.cc casa/Quanta/MVDoppler.h 
//      template uInt Register(MVDoppler const *) 
// 1240 casa/Utilities/Register.cc casa/Quanta/MVDouble.h 
//      template uInt Register(MVDouble const *) 
// 1250 casa/Utilities/Register.cc casa/Quanta/MVEarthMagnetic.h 
//      template uInt Register(MVEarthMagnetic const *) 
// 1260 casa/Utilities/Register.cc casa/Quanta/MVEpoch.h 
//      template uInt Register(MVEpoch const *) 
// 1270 casa/Utilities/Register.cc casa/Quanta/MVFrequency.h 
//      template uInt Register(MVFrequency const *) 
// 1280 casa/Utilities/Register.cc casa/Quanta/MVPosition.h 
//      template uInt Register(MVPosition const *) 
// 1290 casa/Utilities/Register.cc casa/Quanta/MVRadialVelocity.h 
//      template uInt Register(MVRadialVelocity const *) 
// 1300 casa/Utilities/Register.cc casa/Quanta/MVuvw.h 
//      template uInt Register(MVuvw const *) 
// 1310 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Array 
//      template uInt Register(Quantum<Array<Double> > const *) 
// 1320 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Array 
//      template uInt Register(Quantum<Array<Float> > const *) 
// 1330 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Array 
//      template uInt Register(Quantum<Array<Int> > const *) 
// 1340 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Matrix 
//      template uInt Register(Quantum<Matrix<Double> > const *) 
// 1350 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Vector 
//      template uInt Register(Quantum<Vector<Double> > const *) 
// 1360 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Vector 
//      template uInt Register(Quantum<Vector<Float> > const *) 
// 1370 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template <class T> class Vector 
//      template uInt Register(Quantum<Vector<Int> > const *) 
// 1380 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template uInt Register(Quantum<Double> const *) 
// 1390 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template uInt Register(Quantum<Float> const *) 
// 1400 casa/Utilities/Register.cc 
//      template <class Qtype> class Quantum 
//      template uInt Register(Quantum<Int> const *) 
// 1410 casa/Utilities/Register.cc 
//      template uInt Register(Double const *) 
// 1420 casa/Utilities/Register.cc 
//      template uInt Register(Float const *) 
// 1430 casa/Utilities/Register.cc 
//      template uInt Register(Int const *) 
// 1440 casa/Utilities/Register.cc 
//      template uInt Register(Long const *) 
// 1450 casa/Utilities/Register.cc 
//      template uInt Register(lDouble const *) 
// 1460 casa/Utilities/Register.cc 
//      template uInt Register(uInt const *) 
// 1000 casa/Utilities/Sequence.cc 
//      template class Sequence<uInt> 
