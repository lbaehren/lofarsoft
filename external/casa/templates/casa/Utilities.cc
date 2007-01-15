
// include of header files

#include <casa/fstream.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/BasicMath/Random.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Dlist.h>
#include <casa/Containers/IterError.h>
#include <casa/Containers/List.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Containers/RecordDescRep.h>
#include <casa/Containers/RecordInterface.h>
#include <casa/Containers/RecordRep.h>
#include <casa/Exceptions/Error.h>
#include <casa/Inputs/Param.h>
#include <casa/IO/ByteIO.h>
#include <casa/IO/TypeIO.h>
#include <casa/Logging/LogSinkInterface.h>
#include <casa/OS/DataConversion.h>
#include <casa/OS/SysEvent.h>
#include <casa/Quanta/MVBaseline.h> 
#include <casa/Quanta/MVDoppler.h> 
#include <casa/Quanta/MVDouble.h> 
#include <casa/Quanta/MVEarthMagnetic.h> 
#include <casa/Quanta/MVEpoch.h> 
#include <casa/Quanta/MVFrequency.h> 
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/MVPosition.h>
#include <casa/Quanta/MVRadialVelocity.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/MVuvw.h>
#include <casa/Quanta/QBase.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/QuantumHolder.h>
#include <casa/Quanta/Unit.h>
#include <casa/System/PGPlotterInterface.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/Utilities/DefaultValue.h>
#include <casa/Utilities/Fallible.h>
#include <casa/Utilities/Regex.h>
#include <casa/Utilities/Register.h>
#include <casa/Utilities/test/tCountedPtr.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Utilities/Assert.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/COWPtr.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/Compare.cc>
#include <casa/Utilities/Copy.cc>
#include <casa/Utilities/GenSort.cc>
#include <casa/Utilities/LinearSearch.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <casa/Utilities/Sequence.cc>

namespace casa {

  // ============================================================================
  //
  //  ReposFiller/templates
  //
  // ============================================================================

  // --------------------------------------------------------------------- assert

  template class assert_<ArrayError>;
  template class assert_<InvalidIterError>;
  template class assert_<AbortError>;
  template class assert_<AipsError>;

  // ------------------------------------------------------- binarySearchBrackets

  template Int binarySearchBrackets(Bool &, Int * const &, Int const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Complex> const &, Complex const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Double> const &, Double const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<Float> const &, Float const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Block<uInt> const &, uInt const &, uInt, Int);
  template Int binarySearchBrackets(Bool &, Vector<Float> const &, Float const &, uInt, Int);

  /*
    For better readability and easier upgrade, classes are collected into
    a block per variable type; if new classes are added the required adjustment
    can be done in the first block, which thereafter simply can be copied for
    the other supported data variable types.
   */

  // Int
  template class CountedConstPtr<Array<Int> >;
  template class CountedConstPtr<Block<Int> >;
  template class CountedConstPtr<Block<Array<Int> > >;
  template class CountedPtr<Array<Int> >;
  template class CountedPtr<Block<Int> >;
  template class CountedPtr<Block<Array<Int> > >;
  template class COWPtr<Array<Int> >;
  template class Fallible<Int>;
  template class GenSort<Int>;
  template class GenSortIndirect<Int>;
  template class ObjCompare<Int>;
  template class PtrRep<Array<Int> >;
  template class PtrRep<Block<Int> >;
  template class PtrRep<Block<Array<Int> > >;
  template class SimpleCountedConstPtr<Array<Int> >;
  template class SimpleCountedConstPtr<Block<Int> >;
  template class SimpleCountedConstPtr<Block<Array<Int> > >;
  template class SimpleCountedPtr<Array<Int> >;
  template class SimpleCountedPtr<Block<Int> >;
  template class SimpleCountedPtr<Block<Array<Int> > >;
  template void defaultValue(Int &);
  // uInt
  template class CountedConstPtr<Array<uInt> >;
  template class CountedConstPtr<Block<uInt> >;
  template class CountedConstPtr<Block<Array<uInt> > >;
  template class CountedPtr<Array<uInt> >;
  template class CountedPtr<Block<uInt> >;
  template class CountedPtr<Block<Array<uInt> > >;
  template class COWPtr<Array<uInt> >;
  template class Fallible<uInt>;
  template class GenSort<uInt>;
  template class GenSortIndirect<uInt>;
  template class ObjCompare<uInt>;
  template class PtrRep<Array<uInt> >;
  template class PtrRep<Block<uInt> >;
  template class PtrRep<Block<Array<uInt> > >;
  template class SimpleCountedConstPtr<Array<uInt> >;
  template class SimpleCountedConstPtr<Block<uInt> >;
  template class SimpleCountedConstPtr<Block<Array<uInt> > >;
  template class SimpleCountedPtr<Array<uInt> >;
  template class SimpleCountedPtr<Block<uInt> >;
  template class SimpleCountedPtr<Block<Array<uInt> > >;
  template void defaultValue(uInt &);
  // Short
  template class CountedConstPtr<Array<Short> >;
  template class CountedConstPtr<Block<Short> >;
  template class CountedConstPtr<Block<Array<Short> > >;
  template class CountedPtr<Array<Short> >;
  template class CountedPtr<Block<Short> >;
  template class CountedPtr<Block<Array<Short> > >;
  template class COWPtr<Array<Short> >;
  template class Fallible<Short>;
  template class GenSort<Short>;
  template class GenSortIndirect<Short>;
  template class ObjCompare<Short>;
  template class PtrRep<Array<Short> >;
  template class PtrRep<Block<Short> >;
  template class PtrRep<Block<Array<Short> > >;
  template class SimpleCountedConstPtr<Array<Short> >;
  template class SimpleCountedConstPtr<Block<Short> >;
  template class SimpleCountedConstPtr<Block<Array<Short> > >;
  template class SimpleCountedPtr<Array<Short> >;
  template class SimpleCountedPtr<Block<Short> >;
  template class SimpleCountedPtr<Block<Array<Short> > >;
  template void defaultValue(Short &);
  // uShort
  template class CountedConstPtr<Array<uShort> >;
  template class CountedConstPtr<Block<uShort> >;
  template class CountedConstPtr<Block<Array<uShort> > >;
  template class CountedPtr<Array<uShort> >;
  template class CountedPtr<Block<uShort> >;
  template class CountedPtr<Block<Array<uShort> > >;
  template class COWPtr<Array<uShort> >;
  template class Fallible<uShort>;
  template class GenSort<uShort>;
  template class GenSortIndirect<uShort>;
  template class ObjCompare<uShort>;
  template class PtrRep<Array<uShort> >;
  template class PtrRep<Block<uShort> >;
  template class PtrRep<Block<Array<uShort> > >;
  template class SimpleCountedConstPtr<Array<uShort> >;
  template class SimpleCountedConstPtr<Block<uShort> >;
  template class SimpleCountedConstPtr<Block<Array<uShort> > >;
  template class SimpleCountedPtr<Array<uShort> >;
  template class SimpleCountedPtr<Block<uShort> >;
  template class SimpleCountedPtr<Block<Array<uShort> > >;
  template void defaultValue(uShort &);
  // Long
  template class CountedConstPtr<Array<Long> >;
  template class CountedConstPtr<Block<Long> >;
  template class CountedConstPtr<Block<Array<Long> > >;
  template class CountedPtr<Array<Long> >;
  template class CountedPtr<Block<Long> >;
  template class CountedPtr<Block<Array<Long> > >;
  template class COWPtr<Array<Long> >;
  template class Fallible<Long>;
  template class GenSort<Long>;
  template class GenSortIndirect<Long>;
  template class ObjCompare<Long>;
  template class PtrRep<Array<Long> >;
  template class PtrRep<Block<Long> >;
  template class PtrRep<Block<Array<Long> > >;
  template class SimpleCountedConstPtr<Array<Long> >;
  template class SimpleCountedConstPtr<Block<Long> >;
  template class SimpleCountedConstPtr<Block<Array<Long> > >;
  template class SimpleCountedPtr<Array<Long> >;
  template class SimpleCountedPtr<Block<Long> >;
  template class SimpleCountedPtr<Block<Array<Long> > >;
  template void defaultValue(Long &);
  // uLong
  template class CountedConstPtr<Array<uLong> >;
  template class CountedConstPtr<Block<uLong> >;
  template class CountedConstPtr<Block<Array<uLong> > >;
  template class CountedPtr<Array<uLong> >;
  template class CountedPtr<Block<uLong> >;
  template class CountedPtr<Block<Array<uLong> > >;
  template class COWPtr<Array<uLong> >;
  template class Fallible<uLong>;
  template class GenSort<uLong>;
  template class GenSortIndirect<uLong>;
  template class ObjCompare<uLong>;
  template class PtrRep<Array<uLong> >;
  template class PtrRep<Block<uLong> >;
  template class PtrRep<Block<Array<uLong> > >;
  template class SimpleCountedConstPtr<Array<uLong> >;
  template class SimpleCountedConstPtr<Block<uLong> >;
  template class SimpleCountedConstPtr<Block<Array<uLong> > >;
  template class SimpleCountedPtr<Array<uLong> >;
  template class SimpleCountedPtr<Block<uLong> >;
  template class SimpleCountedPtr<Block<Array<uLong> > >;
  template void defaultValue(uLong &);
  // Float
  template class CountedConstPtr<Array<Float> >;
  template class CountedConstPtr<Block<Float> >;
  template class CountedConstPtr<Block<Array<Float> > >;
  template class CountedPtr<Array<Float> >;
  template class CountedPtr<Block<Float> >;
  template class CountedPtr<Block<Array<Float> > >;
  template class COWPtr<Array<Float> >;
  template class Fallible<Float>;
  template class GenSort<Float>;
  template class GenSortIndirect<Float>;
  template class ObjCompare<Float>;
  template class PtrRep<Array<Float> >;
  template class PtrRep<Block<Float> >;
  template class PtrRep<Block<Array<Float> > >;
  template class SimpleCountedConstPtr<Array<Float> >;
  template class SimpleCountedConstPtr<Block<Float> >;
  template class SimpleCountedConstPtr<Block<Array<Float> > >;
  template class SimpleCountedPtr<Array<Float> >;
  template class SimpleCountedPtr<Block<Float> >;
  template class SimpleCountedPtr<Block<Array<Float> > >;
  template void defaultValue(Float &);
  // Double
  template class CountedConstPtr<Array<Double> >;
  template class CountedConstPtr<Block<Double> >;
  template class CountedConstPtr<Block<Array<Double> > >;
  template class CountedPtr<Array<Double> >;
  template class CountedPtr<Block<Double> >;
  template class CountedPtr<Block<Array<Double> > >;
  template class COWPtr<Array<Double> >;
  template class Fallible<Double>;
  template class GenSort<Double>;
  template class GenSortIndirect<Double>;
  template class ObjCompare<Double>;
  template class PtrRep<Array<Double> >;
  template class PtrRep<Block<Double> >;
  template class PtrRep<Block<Array<Double> > >;
  template class SimpleCountedConstPtr<Array<Double> >;
  template class SimpleCountedConstPtr<Block<Double> >;
  template class SimpleCountedConstPtr<Block<Array<Double> > >;
  template class SimpleCountedPtr<Array<Double> >;
  template class SimpleCountedPtr<Block<Double> >;
  template class SimpleCountedPtr<Block<Array<Double> > >;
  template void defaultValue(Double &);
  // Complex
  template class CountedConstPtr<Array<Complex> >;
  template class CountedConstPtr<Block<Complex> >;
  template class CountedConstPtr<Block<Array<Complex> > >;
  template class CountedPtr<Array<Complex> >;
  template class CountedPtr<Block<Complex> >;
  template class CountedPtr<Block<Array<Complex> > >;
  template class COWPtr<Array<Complex> >;
  template class Fallible<Complex>;
  template class GenSort<Complex>;
  template class GenSortIndirect<Complex>;
  template class ObjCompare<Complex>;
  template class PtrRep<Array<Complex> >;
  template class PtrRep<Block<Complex> >;
  template class PtrRep<Block<Array<Complex> > >;
  template class SimpleCountedConstPtr<Array<Complex> >;
  template class SimpleCountedConstPtr<Block<Complex> >;
  template class SimpleCountedConstPtr<Block<Array<Complex> > >;
  template class SimpleCountedPtr<Array<Complex> >;
  template class SimpleCountedPtr<Block<Complex> >;
  template class SimpleCountedPtr<Block<Array<Complex> > >;
  template void defaultValue(Complex &);
  // DComplex
  template class CountedConstPtr<Array<DComplex> >;
  template class CountedConstPtr<Block<DComplex> >;
  template class CountedConstPtr<Block<Array<DComplex> > >;
  template class CountedPtr<Array<DComplex> >;
  template class CountedPtr<Block<DComplex> >;
  template class CountedPtr<Block<Array<DComplex> > >;
  template class COWPtr<Array<DComplex> >;
  template class Fallible<DComplex>;
  template class GenSort<DComplex>;
  template class GenSortIndirect<DComplex>;
  template class ObjCompare<DComplex>;
  template class PtrRep<Array<DComplex> >;
  template class PtrRep<Block<DComplex> >;
  template class PtrRep<Block<Array<DComplex> > >;
  template class SimpleCountedConstPtr<Array<DComplex> >;
  template class SimpleCountedConstPtr<Block<DComplex> >;
  template class SimpleCountedConstPtr<Block<Array<DComplex> > >;
  template class SimpleCountedPtr<Array<DComplex> >;
  template class SimpleCountedPtr<Block<DComplex> >;
  template class SimpleCountedPtr<Block<Array<DComplex> > >;
  template void defaultValue(DComplex &);
  // Bool
  template class CountedConstPtr<Array<Bool> >;
  template class CountedConstPtr<Block<Bool> >;
  template class CountedConstPtr<Block<Array<Bool> > >;
  template class CountedPtr<Array<Bool> >;
  template class CountedPtr<Block<Bool> >;
  template class CountedPtr<Block<Array<Bool> > >;
  template class COWPtr<Array<Bool> >;
  template class Fallible<Bool>;
  template class GenSort<Bool>;
  template class GenSortIndirect<Bool>;
  template class ObjCompare<Bool>;
  template class PtrRep<Array<Bool> >;
  template class PtrRep<Block<Bool> >;
  template class PtrRep<Block<Array<Bool> > >;
  template class SimpleCountedConstPtr<Array<Bool> >;
  template class SimpleCountedConstPtr<Block<Bool> >;
  template class SimpleCountedConstPtr<Block<Array<Bool> > >;
  template class SimpleCountedPtr<Array<Bool> >;
  template class SimpleCountedPtr<Block<Bool> >;
  template class SimpleCountedPtr<Block<Array<Bool> > >;
  template void defaultValue(Bool &);
  // Char
  template class CountedConstPtr<Array<Char> >;
  template class CountedConstPtr<Block<Char> >;
  template class CountedConstPtr<Block<Array<Char> > >;
  template class CountedPtr<Array<Char> >;
  template class CountedPtr<Block<Char> >;
  template class CountedPtr<Block<Array<Char> > >;
  template class COWPtr<Array<Char> >;
  template class Fallible<Char>;
  template class GenSort<Char>;
  template class GenSortIndirect<Char>;
  template class ObjCompare<Char>;
  template class PtrRep<Array<Char> >;
  template class PtrRep<Block<Char> >;
  template class PtrRep<Block<Array<Char> > >;
  template class SimpleCountedConstPtr<Array<Char> >;
  template class SimpleCountedConstPtr<Block<Char> >;
  template class SimpleCountedConstPtr<Block<Array<Char> > >;
  template class SimpleCountedPtr<Array<Char> >;
  template class SimpleCountedPtr<Block<Char> >;
  template class SimpleCountedPtr<Block<Array<Char> > >;
  template void defaultValue(Char &);
  //

  template class CountedConstPtr<Block<Slicer> >;

  template class SimpleCountedPtr<Block<QuantumHolder> >;
  template class SimpleCountedPtr<Block<Slicer> >;
  template class SimpleCountedPtr<Block<Unit> >;

  template class CountedPtr<Block<QuantumHolder> >;
  template class CountedPtr<Block<Slicer> >;
  template class CountedPtr<Block<Unit> >;
  template class CountedPtr<Block<void *> >;
  template class CountedPtr<Block<Vector<Float> > >;
  template class CountedPtr<PGPlotterInterface>;
  template class CountedPtr<Random>;

  template class COWPtr<RecordDescRep>;
  template class COWPtr<RecordRep>;

  template class ObjCompare<String>;
  template class ObjCompare<uChar>;
  
  template class PtrRep<Block<Slicer> >;

  template class SimpleCountedConstPtr<Block<Slicer> >;
  template class CountedConstPtr<Block<QuantumHolder> >;
  template class PtrRep<Block<QuantumHolder> >;
  template class SimpleCountedConstPtr<Block<QuantumHolder> >;
  template class CountedConstPtr<Block<Unit> >;
  template class PtrRep<Block<Unit> >;
  template class SimpleCountedConstPtr<Block<Unit> >;
  template class CountedConstPtr<Block<void *> >;
  template class PtrRep<Block<void *> >;
  template class SimpleCountedConstPtr<Block<void *> >;
  template class SimpleCountedPtr<Block<void *> >;
  template class CountedConstPtr<PGPlotterInterface>;
  template class SimpleCountedPtr<PGPlotterInterface>;
  template class SimpleCountedConstPtr<PGPlotterInterface>;
  template class PtrRep<PGPlotterInterface>;
  template class SimpleCountedConstPtr<Block<Vector<Float> > >;
  template class PtrRep<Block<Vector<Float> > >;
  template class CountedConstPtr<Block<Vector<Float> > >;
  template class SimpleCountedPtr<Block<Vector<Float> > >;
  template class SimpleCountedConstPtr<Random>;
  template class SimpleCountedPtr<Random>;
  template class CountedConstPtr<Random>;
  template class PtrRep<Random>;
  template class CountedConstPtr<Block<Matrix<CountedPtr<Random> > > >;
  template class CountedPtr<Block<Matrix<CountedPtr<Random> > > >;
  template class SimpleCountedConstPtr<Block<Matrix<CountedPtr<Random> > > >;
  template class SimpleCountedPtr<Block<Matrix<CountedPtr<Random> > > >;
  template class PtrRep<Block<Matrix<CountedPtr<Random> > > >;
  template class CountedConstPtr<Block<Vector<Complex> > >;
  template class CountedPtr<Block<Vector<Complex> > >;
  template class PtrRep<Block<Vector<Complex> > >;
  template class SimpleCountedConstPtr<Block<Vector<Complex> > >;
  template class SimpleCountedPtr<Block<Vector<Complex> > >;
  template class CountedConstPtr<Block<Vector<Double> > >;
  template class CountedPtr<Block<Vector<Double> > >;
  template class PtrRep<Block<Vector<Double> > >;
  template class SimpleCountedConstPtr<Block<Vector<Double> > >;
  template class SimpleCountedPtr<Block<Vector<Double> > >;
  template class CountedConstPtr<Block<CountedPtr<Random> > >;
  template class CountedPtr<Block<CountedPtr<Random> > >;
  template class SimpleCountedConstPtr<Block<CountedPtr<Random> > >;
  template class SimpleCountedPtr<Block<CountedPtr<Random> > >;
  template class PtrRep<Block<CountedPtr<Random> > >;
  template class SimpleCountedConstPtr<Regex>;
  template class CountedConstPtr<Regex>;
  template class CountedPtr<Regex>;
  template class PtrRep<Regex>;
  template class SimpleCountedPtr<Regex>;
  template class CountedConstPtr<Block<String> >;
  template class CountedPtr<Block<String> >;
  template class PtrRep<Block<String> >;
  template class SimpleCountedConstPtr<Block<String> >;
  template class SimpleCountedPtr<Block<String> >;
  template class CountedConstPtr<Block<MVDirection> >;
  template class CountedPtr<Block<MVDirection> >;
  template class PtrRep<Block<MVDirection> >;
  template class SimpleCountedConstPtr<Block<MVDirection> >;
  template class SimpleCountedPtr<Block<MVDirection> >;
  template class CountedConstPtr<Block<MVFrequency> >;
  template class CountedPtr<Block<MVFrequency> >;
  template class PtrRep<Block<MVFrequency> >;
  template class SimpleCountedConstPtr<Block<MVFrequency> >;
  template class SimpleCountedPtr<Block<MVFrequency> >;
  template class CountedConstPtr<Block<MVPosition> >;
  template class CountedPtr<Block<MVPosition> >;
  template class PtrRep<Block<MVPosition> >;
  template class SimpleCountedConstPtr<Block<MVPosition> >;
  template class SimpleCountedPtr<Block<MVPosition> >;
  template class CountedConstPtr<Block<MVRadialVelocity> >;
  template class CountedPtr<Block<MVRadialVelocity> >;
  template class PtrRep<Block<MVRadialVelocity> >;
  template class SimpleCountedConstPtr<Block<MVRadialVelocity> >;
  template class SimpleCountedPtr<Block<MVRadialVelocity> >;
  template class CountedPtr<Block<MVTime> >;
  template class SimpleCountedPtr<Block<MVTime> >;
  template class CountedConstPtr<Block<MVTime> >;
  template class SimpleCountedConstPtr<Block<MVTime> >;
  template class PtrRep<Block<MVTime> >;
  template class CountedConstPtr<Block<Quantum<Double> > >;
  template class CountedPtr<Block<Quantum<Double> > >;
  template class PtrRep<Block<Quantum<Double> > >;
  template class SimpleCountedConstPtr<Block<Quantum<Double> > >;
  template class SimpleCountedPtr<Block<Quantum<Double> > >;
  template class CountedPtr<Block<Quantum<Float> > >;
  template class CountedConstPtr<Block<Quantum<Float> > >;
  template class SimpleCountedPtr<Block<Quantum<Float> > >;
  template class SimpleCountedConstPtr<Block<Quantum<Float> > >;
  template class PtrRep<Block<Quantum<Float> > >;
  template class CountedConstPtr<Block<uChar> >;
  template class CountedPtr<Block<uChar> >;
  template class PtrRep<Block<uChar> >;
  template class SimpleCountedConstPtr<Block<uChar> >;
  template class SimpleCountedPtr<Block<uChar> >;
  template class CountedPtr<Block<Float *> >;
  template class SimpleCountedConstPtr<Block<Float *> >;
  template class CountedConstPtr<Block<Float *> >;
  template class PtrRep<Block<Float *> >;
  template class SimpleCountedPtr<Block<Float *> >;
  template class CountedConstPtr<RecordDescRep>;
  template class CountedPtr<RecordDescRep>;
  template class PtrRep<RecordDescRep>;
  template class SimpleCountedConstPtr<RecordDescRep>;
  template class SimpleCountedPtr<RecordDescRep>;
  template class CountedConstPtr<RecordRep>;
  template class CountedPtr<RecordRep>;
  template class PtrRep<RecordRep>;
  template class SimpleCountedConstPtr<RecordRep>;
  template class SimpleCountedPtr<RecordRep>;
  template class CountedPtr<ByteIO>;
  template class CountedConstPtr<ByteIO>;
  template class SimpleCountedPtr<ByteIO>;
  template class SimpleCountedConstPtr<ByteIO>;
  template class PtrRep<ByteIO>;
  template class CountedPtr<TypeIO>;
  template class CountedConstPtr<TypeIO>;
  template class SimpleCountedPtr<TypeIO>;
  template class SimpleCountedConstPtr<TypeIO>;
  template class PtrRep<TypeIO>;
  template class CountedConstPtr<LogSinkInterface>;
  template class CountedPtr<LogSinkInterface>;
  template class PtrRep<LogSinkInterface>;
  template class SimpleCountedConstPtr<LogSinkInterface>;
  template class SimpleCountedPtr<LogSinkInterface>;
  template class CountedPtr<DataConversion>;
  template class CountedConstPtr<DataConversion>;
  template class SimpleCountedPtr<DataConversion>;
  template class SimpleCountedConstPtr<DataConversion>;
  template class PtrRep<DataConversion>;
  template class CountedConstPtr<SysEventTarget>;
  template class CountedPtr<SysEventTarget>;
  template class PtrRep<SysEventTarget>;
  template class SimpleCountedConstPtr<SysEventTarget>;
  template class SimpleCountedPtr<SysEventTarget>;
  template class CountedConstPtr<SysEventTargetInfo>;
  template class CountedPtr<SysEventTargetInfo>;
  template class PtrRep<SysEventTargetInfo>;
  template class SimpleCountedConstPtr<SysEventTargetInfo>;
  template class SimpleCountedPtr<SysEventTargetInfo>;
  template class CountedConstPtr<fstream>;
  template class CountedPtr<fstream>;
  template class PtrRep<fstream>;
  template class SimpleCountedConstPtr<fstream>;
  template class SimpleCountedPtr<fstream>;

  template class GenSortIndirect<String>;
  template class GenSort<String>;
  template uInt genSort(Vector<uInt> &, Block<Double> const &);
  template uInt genSort(Vector<uInt> &, Block<Double> const &, Sort::Order);
  template uInt genSort(Array<uInt> &);
  template uInt genSort(Double *, uInt);
  template uInt genSort(Float *, uInt);
  template uInt genSort(Int *, uInt, Sort::Order, Int);
  template uInt genSort(Vector<uInt> &, Int const *, uInt, Sort::Order, Int);

  template Int linearSearch(Bool &, Vector<Int> const &, Int const &, uInt, uInt);
  template Int linearSearch(Bool &, Vector<uInt> const &, uInt const &, uInt, uInt);
  
  template class PtrHolder<Int>;
  template class PtrHolder<Random>;
  template class PtrHolder<QBase>;
  template class PtrHolder<Vector<String> >;
  template class PtrHolder<RecordInterface>;
  template class PtrHolder<LogSinkInterface>;
  
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template <class Qtype> class Quantum;
  template <class T> class Vector;
  template <class Qtype> class Quantum;
  template <class T> class Vector;
  template <class Qtype> class Quantum;
  template <class Qtype> class Quantum;
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template <class T> class Matrix;
  template <class T> class Vector;

  template class Sequence<Int>;
  template class Sequence<uInt>;
  template class Sequence<Short>;
  template class Sequence<uShort>;
  template class Sequence<Float>;
  template class Sequence<Double>;
  
  template void objcopy(String*, const String*, uInt);
  template void objcopy(uInt*, const uInt*, uInt);
  template void objcopy(Int*, const Int*, uInt);
  template void objcopy(Float*, const Float*, uInt);
  template void objcopy(Double*, const Double*, uInt);
  template void objcopy(Complex*, const Complex*, uInt);
  template void objcopy(DComplex*, const DComplex*, uInt);
  template void objcopy(char*, const char*, uInt);
  template void objcopy(bool*, const bool*, uInt);
  template void objcopy(short*, const short*, uInt);
  template void objcopy(long*, const long*, uInt);
  template void objcopy(long long*, const long long*, uInt);
  template void objcopy(IPosition*, const IPosition*, uInt);
  template void objcopy(MVPosition*, const MVPosition*, uInt);
  template void objcopy(ArraySlicer*, const ArraySlicer*, uInt);
  template void objcopy(MVDirection*, const MVDirection*, uInt);
  template void objcopy(MVFrequency*, const MVFrequency*, uInt);

  template void objcopy<void*>(void**, void* const*, uInt);
  template void objcopy<unsigned char>(unsigned char*, unsigned char const*, uInt);
  template void objcopy<uLong>(uLong*, uLong const*, uInt);
  template void objcopy<uShort>(uShort*, uShort const*, uInt);
  template void objcopy<float*>(float**, float* const*, uInt);
  template void objcopy<Array<double> >(Array<double>*, Array<double> const*, uInt);
  template void objcopy<Vector<float> >(Vector<float>*, Vector<float> const*, uInt);
  template void objcopy<Vector<double> >(Vector<double>*, Vector<double> const*, uInt);
  template void objcopy<Vector<Complex> >(Vector<Complex>*, Vector<Complex> const*, uInt);
  template void objcopy<Vector<DComplex> >(Vector<DComplex>*, Vector<DComplex> const*, uInt);
  template void objcopy<Vector<String> >(Vector<String>*, Vector<String> const*, uInt);
  template void objcopy<Unit>(Unit*, Unit const*, uInt);
  template void objcopy<QuantumHolder>(QuantumHolder*, QuantumHolder const*, uInt);
  template void objcopy<MVTime>(MVTime*, MVTime const*, uInt);
  template void objcopy<CountedPtr<Random> >(CountedPtr<Random> *, CountedPtr<Random>  const*, uInt);

  template void objcopy<bool>(bool*, bool const*, uInt, uInt, uInt);
  template void objcopy<char>(char*, char const*, uInt, uInt, uInt);
  template void objcopy<unsigned char>(unsigned char*, unsigned char const*, uInt, uInt, uInt);
  template void objcopy<int>(int*, int const*, uInt, uInt, uInt);
  template void objcopy<uInt>(uInt*, uInt const*, uInt, uInt, uInt);
  template void objcopy<short>(short*, short const*, uInt, uInt, uInt);
  template void objcopy<uShort>(uShort*, uShort const*, uInt, uInt, uInt);
  template void objcopy<long>(long*, long const*, uInt, uInt, uInt);
  template void objcopy<uLong>(uLong*, uLong const*, uInt, uInt, uInt);
  template void objcopy<float>(float*, float const*, uInt, uInt, uInt);
  template void objcopy<float*>(float**, float* const*, uInt, uInt, uInt);
  template void objcopy<Double>(Double*, Double const*, uInt, uInt, uInt);
  template void objcopy<Complex>(Complex*, Complex const*, uInt, uInt, uInt);
  template void objcopy<DComplex>(DComplex*, DComplex const*, uInt, uInt, uInt);
  template void objcopy<String>(String*, String const*, uInt, uInt, uInt);
  template void objcopy<MVPosition>(MVPosition*, MVPosition const*, uInt, uInt, uInt);
  template void objcopy<ArraySlicer>(ArraySlicer*, ArraySlicer const*, uInt, uInt, uInt);
  template void objcopy<MVDirection>(MVDirection*, MVDirection const*, uInt, uInt, uInt);
  template void objcopy<MVFrequency>(MVFrequency*, MVFrequency const*, uInt, uInt, uInt);
  template void objcopy<QuantumHolder>(QuantumHolder*, QuantumHolder const*, uInt, uInt, uInt);
  template void objcopy<MVRadialVelocity>(MVRadialVelocity*, MVRadialVelocity const*, uInt);
  template void objcopy<MVRadialVelocity>(MVRadialVelocity*, MVRadialVelocity const*, uInt, uInt, uInt);
  template void objcopy<Unit>(Unit*, Unit const*, uInt, uInt, uInt);
  template void objcopy<Array<double> >(Array<double>*, Array<double> const*, uInt, uInt, uInt);
  template void objcopy<Vector<float> >(Vector<float>*, Vector<float> const*, uInt, uInt, uInt);
  template void objcopy<Vector<double> >(Vector<double>*, Vector<double> const*, uInt, uInt, uInt);
  template void objcopy<Vector<Complex> >(Vector<Complex>*, Vector<Complex> const*, uInt, uInt, uInt);
  template void objcopy<MVTime>(MVTime*, MVTime const*, uInt, uInt, uInt);
  template void objcopy<Quantum<float> >(Quantum<float>*, Quantum<float> const*, uInt);
  template void objcopy<Quantum<float> >(Quantum<float>*, Quantum<float> const*, uInt, uInt, uInt);
  template void objcopy<Quantum<double> >(Quantum<double>*, Quantum<double> const*, uInt);
  template void objcopy<Quantum<double> >(Quantum<double>*, Quantum<double> const*, uInt, uInt, uInt);
  template void objcopy<CountedPtr<Random> >(CountedPtr<Random> *, CountedPtr<Random>  const*, uInt, uInt, uInt);
  template void objcopy<Matrix<CountedPtr<Random> > >(Matrix<CountedPtr<Random> >*, Matrix<CountedPtr<Random> > const*, uInt);
  template void objcopy<Matrix<CountedPtr<Random> > >(Matrix<CountedPtr<Random> >*, Matrix<CountedPtr<Random> > const*, uInt, uInt, uInt);


  template void objset<void*>(void**, void*, uInt);
  template void objset<bool*>(bool**, bool*, uInt);
  template void objset<bool*>(bool**, bool*, uInt, uInt);
  template void objset<Float*>(Float**, Float*, uInt);
  template void objset<Float*>(Float**, Float*, uInt, uInt);
  template void objset<bool>(bool*, bool, uInt);
  template void objset<bool>(bool*, bool, uInt, uInt);
  template void objset<char>(char*, char, uInt);
  template void objset<char>(char*, char, uInt, uInt);
  template void objset<double>(double*, double, uInt);
  template void objset<double>(double*, double, uInt, uInt);
  template void objset<float>(float*, float, uInt);
  template void objset<float>(float*, float, uInt, uInt);
  template void objset<Complex*>(Complex**, Complex*, uInt);
  template void objset<Complex*>(Complex**, Complex*, uInt, uInt);
  template void objset<DComplex*>(DComplex**, DComplex*, uInt);
  template void objset<DComplex*>(DComplex**, DComplex*, uInt, uInt);
  template void objset<unsigned char>(unsigned char*, unsigned char, uInt);
  template void objset<unsigned char>(unsigned char*, unsigned char, uInt, uInt);
  template void objset<int>(int*, int, uInt);
  template void objset<int>(int*, int, uInt, uInt);
  template void objset<uInt>(uInt*, uInt, uInt);
  template void objset<uInt>(uInt*, uInt, uInt, uInt);
  template void objset<long>(long*, long, uInt);
  template void objset<long>(long*, long, uInt, uInt);
  template void objset<uLong>(uLong*, uLong, uInt);
  template void objset<uLong>(uLong*, uLong, uInt, uInt);
  template void objset<short>(short*, short, uInt);
  template void objset<short>(short*, short, uInt, uInt);
  template void objset<uShort>(uShort*, uShort, uInt);
  template void objset<uShort>(uShort*, uShort, uInt, uInt);
  template void objset<Complex>(Complex*, Complex, uInt);
  template void objset<Complex>(Complex*, Complex, uInt, uInt);
  template void objset<DComplex>(DComplex*, DComplex, uInt);
  template void objset<DComplex>(DComplex*, DComplex, uInt, uInt);
  template void objset<String>(String*, String, uInt);
  template void objset<String>(String*, String, uInt, uInt);
  template void objset<Unit>(Unit*, Unit, uInt);
  template void objset<Unit>(Unit*, Unit, uInt, uInt);
  template void objset<MVTime>(MVTime*, MVTime, uInt);
  template void objset<MVTime>(MVTime*, MVTime, uInt, uInt);
  template void objset<Array<double> >(Array<double>*, Array<double>, uInt);
  template void objset<Array<double> >(Array<double>*, Array<double>, uInt, uInt);
  template void objset<Vector<float> >(Vector<float>*, Vector<float>, uInt);
  template void objset<Vector<float> >(Vector<float>*, Vector<float>, uInt, uInt);
  template void objset<Vector<double> >(Vector<double>*, Vector<double>, uInt);
  template void objset<Vector<double> >(Vector<double>*, Vector<double>, uInt, uInt);
  template void objset<Vector<Complex> >(Vector<Complex>*, Vector<Complex>, uInt);
  template void objset<Vector<Complex> >(Vector<Complex>*, Vector<Complex>, uInt, uInt);
  template void objset<MVPosition>(MVPosition*, MVPosition, uInt);
  template void objset<MVPosition>(MVPosition*, MVPosition, uInt, uInt);
  template void objset<ArraySlicer>(ArraySlicer*, ArraySlicer, uInt);
  template void objset<ArraySlicer>(ArraySlicer*, ArraySlicer, uInt, uInt);
  template void objset<MVDirection>(MVDirection*, MVDirection, uInt);
  template void objset<MVDirection>(MVDirection*, MVDirection, uInt, uInt);
  template void objset<MVFrequency>(MVFrequency*, MVFrequency, uInt);
  template void objset<MVFrequency>(MVFrequency*, MVFrequency, uInt, uInt);
  template void objset<QuantumHolder>(QuantumHolder*, QuantumHolder, uInt);
  template void objset<QuantumHolder>(QuantumHolder*, QuantumHolder, uInt, uInt);
  template void objset<MVRadialVelocity>(MVRadialVelocity*, MVRadialVelocity, uInt);
  template void objset<MVRadialVelocity>(MVRadialVelocity*, MVRadialVelocity, uInt, uInt);
  template void objset<Quantum<double> >(Quantum<double>*, Quantum<double>, uInt);
  template void objset<Quantum<double> >(Quantum<double>*, Quantum<double>, uInt, uInt);
  template void objset<Quantum<float> >(Quantum<float>*, Quantum<float>, uInt);
  template void objset<Quantum<float> >(Quantum<float>*, Quantum<float>, uInt, uInt);
  template void objset<CountedPtr<Random> >(CountedPtr<Random>*, CountedPtr<Random>, uInt);
  template void objset<CountedPtr<Random> >(CountedPtr<Random>*, CountedPtr<Random>, uInt, uInt);
  template void objset<Matrix<CountedPtr<Random> > >(Matrix<CountedPtr<Random> >*, Matrix<CountedPtr<Random> >, uInt);
  template void objset<Matrix<CountedPtr<Random> > >(Matrix<CountedPtr<Random> >*, Matrix<CountedPtr<Random> >, uInt, uInt);
  
  template void objmove<String>(String*, String const*, uInt);
  
  // ============================================================================
  //
  //  Utilities/test/templates
  //
  // ============================================================================

  // casa/Arrays/Array.cc
  template class Array<Regex>;
  template class MaskedArray<Regex>;
  template class Vector<Regex>;
  template ostream & operator<<(ostream &, Array<Regex> const &);

  // casa/Utilities/BinarySearch.cc
  template Int binarySearch(Bool &, IPosition const &, Int const &, uInt, Int);

  // casa/Utilities/CountedPtr.cc
  template class CountedConstPtr<Block<Regex> >;
  template class CountedPtr<Block<Regex> >;
  template class PtrRep<Block<Regex> >;
  template class SimpleCountedConstPtr<Block<Regex> >;
  template class SimpleCountedPtr<Block<Regex> >;
  template class CountedConstPtr<myobj>;
  template class CountedPtr<myobj>;
  template class PtrRep<myobj>;
  template class SimpleCountedConstPtr<myobj>;
  template class SimpleCountedPtr<myobj>;

  // casa/Utilities/LinearSearch.cc
  template Int linearSearch(Bool &, IPosition const &, Int const &, uInt, uInt);
  template Int linearSearch1(IPosition const &, Int const &, uInt);
  template Int linearSearchBrackets(Bool &, Int * const &, Int const &, uInt, uInt);

}
