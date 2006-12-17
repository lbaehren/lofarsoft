
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


// include of implementation files

#include <casa/Utilities/Assert.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/COWPtr.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/Compare.cc>
#include <casa/Utilities/GenSort.cc>
#include <casa/Utilities/LinearSearch.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <casa/Utilities/Register.cc>
#include <casa/Utilities/Sequence.cc>

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
  template class CountedPtr<Block<Unit> >;
  template class CountedConstPtr<Block<Unit> >;
  template class PtrRep<Block<Unit> >;
  template class SimpleCountedPtr<Block<Unit> >;
  template class SimpleCountedConstPtr<Block<Unit> >;
  template class CountedConstPtr<Block<Long> >;
  template class CountedPtr<Block<Long> >;
  template class PtrRep<Block<Long> >;
  template class SimpleCountedConstPtr<Block<Long> >;
  template class SimpleCountedPtr<Block<Long> >;
  template class CountedConstPtr<Block<uLong> >;
  template class CountedPtr<Block<uLong> >;
  template class PtrRep<Block<uLong> >;
  template class SimpleCountedConstPtr<Block<uLong> >;
  template class SimpleCountedPtr<Block<uLong> >;
  template class CountedConstPtr<Block<void *> >;
  template class CountedPtr<Block<void *> >;
  template class PtrRep<Block<void *> >;
  template class SimpleCountedConstPtr<Block<void *> >;
  template class SimpleCountedPtr<Block<void *> >;
  template class CountedPtr<PGPlotterInterface>;
  template class CountedConstPtr<PGPlotterInterface>;
  template class SimpleCountedPtr<PGPlotterInterface>;
  template class SimpleCountedConstPtr<PGPlotterInterface>;
  template class PtrRep<PGPlotterInterface>;
  template class CountedPtr<Array<Complex> >;
  template class PtrRep<Array<Complex> >;
  template class SimpleCountedConstPtr<Array<Complex> >;
  template class CountedConstPtr<Array<Complex> >;
  template class SimpleCountedPtr<Array<Complex> >;
  template class CountedConstPtr<Array<Bool> >;
  template class CountedPtr<Array<Bool> >;
  template class PtrRep<Array<Bool> >;
  template class SimpleCountedConstPtr<Array<Bool> >;
  template class SimpleCountedPtr<Array<Bool> >;
  template class CountedConstPtr<Array<Double> >;
  template class CountedPtr<Array<Double> >;
  template class PtrRep<Array<Double> >;
  template class SimpleCountedConstPtr<Array<Double> >;
  template class SimpleCountedPtr<Array<Double> >;
  template class CountedConstPtr<Array<Float> >;
  template class CountedPtr<Array<Float> >;
  template class PtrRep<Array<Float> >;
  template class SimpleCountedConstPtr<Array<Float> >;
  template class SimpleCountedPtr<Array<Float> >;
  template class CountedConstPtr<Array<Int> >;
  template class CountedPtr<Array<Int> >;
  template class PtrRep<Array<Int> >;
  template class SimpleCountedConstPtr<Array<Int> >;
  template class SimpleCountedPtr<Array<Int> >;
  template class SimpleCountedConstPtr<Block<Vector<Float> > >;
  template class PtrRep<Block<Vector<Float> > >;
  template class CountedPtr<Block<Vector<Float> > >;
  template class CountedConstPtr<Block<Vector<Float> > >;
  template class SimpleCountedPtr<Block<Vector<Float> > >;
  template class SimpleCountedConstPtr<Random>;
  template class SimpleCountedPtr<Random>;
  template class CountedConstPtr<Random>;
  template class CountedPtr<Random>;
  template class PtrRep<Random>;
  template class CountedConstPtr<Block<Array<Double> > >;
  template class CountedPtr<Block<Array<Double> > >;
  template class PtrRep<Block<Array<Double> > >;
  template class SimpleCountedConstPtr<Block<Array<Double> > >;
  template class SimpleCountedPtr<Block<Array<Double> > >;
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
  template class CountedConstPtr<Block<Complex> >;
  template class CountedPtr<Block<Complex> >;
  template class PtrRep<Block<Complex> >;
  template class SimpleCountedConstPtr<Block<Complex> >;
  template class SimpleCountedPtr<Block<Complex> >;
  template class CountedConstPtr<Block<DComplex> >;
  template class CountedPtr<Block<DComplex> >;
  template class PtrRep<Block<DComplex> >;
  template class SimpleCountedConstPtr<Block<DComplex> >;
  template class SimpleCountedPtr<Block<DComplex> >;
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
  template class CountedConstPtr<Block<Bool> >;
  template class CountedPtr<Block<Bool> >;
  template class PtrRep<Block<Bool> >;
  template class SimpleCountedConstPtr<Block<Bool> >;
  template class SimpleCountedPtr<Block<Bool> >;
  template class CountedConstPtr<Block<Char> >;
  template class CountedPtr<Block<Char> >;
  template class PtrRep<Block<Char> >;
  template class SimpleCountedConstPtr<Block<Char> >;
  template class SimpleCountedPtr<Block<Char> >;
  template class CountedConstPtr<Block<Double> >;
  template class CountedPtr<Block<Double> >;
  template class PtrRep<Block<Double> >;
  template class SimpleCountedConstPtr<Block<Double> >;
  template class SimpleCountedPtr<Block<Double> >;
  template class CountedConstPtr<Block<Float> >;
  template class CountedPtr<Block<Float> >;
  template class PtrRep<Block<Float> >;
  template class SimpleCountedConstPtr<Block<Float> >;
  template class SimpleCountedPtr<Block<Float> >;
  template class CountedConstPtr<Block<Int> >;
  template class CountedPtr<Block<Int> >;
  template class PtrRep<Block<Int> >;
  template class SimpleCountedConstPtr<Block<Int> >;
  template class SimpleCountedPtr<Block<Int> >;
  template class CountedConstPtr<Block<Short> >;
  template class CountedPtr<Block<Short> >;
  template class PtrRep<Block<Short> >;
  template class SimpleCountedConstPtr<Block<Short> >;
  template class SimpleCountedPtr<Block<Short> >;
  template class CountedConstPtr<Block<uChar> >;
  template class CountedPtr<Block<uChar> >;
  template class PtrRep<Block<uChar> >;
  template class SimpleCountedConstPtr<Block<uChar> >;
  template class SimpleCountedPtr<Block<uChar> >;
  template class CountedConstPtr<Block<uInt> >;
  template class CountedPtr<Block<uInt> >;
  template class PtrRep<Block<uInt> >;
  template class SimpleCountedConstPtr<Block<uInt> >;
  template class SimpleCountedPtr<Block<uInt> >;
  template class CountedConstPtr<Block<uShort> >;
  template class CountedPtr<Block<uShort> >;
  template class PtrRep<Block<uShort> >;
  template class SimpleCountedConstPtr<Block<uShort> >;
  template class SimpleCountedPtr<Block<uShort> >;
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

  template void defaultValue(Complex &);
  template void defaultValue(DComplex &);
  template void defaultValue(Bool &);
  template void defaultValue(Double &);
  template void defaultValue(Float &);
  template void defaultValue(Int &);
  
  template class Fallible<Double>;
  template class Fallible<Int>;
  template class Fallible<uInt>;
  
  template class GenSort<DComplex>;
  template class GenSortIndirect<String>;
  template class GenSort<Double>;
  template class GenSort<Float>;
  template class GenSort<Int>;
  template class GenSort<String>;
  template class GenSort<uInt>;
  template class GenSortIndirect<Double>;
  template uInt genSort(Vector<uInt> &, Block<Double> const &);
  template uInt genSort(Vector<uInt> &, Block<Double> const &, Sort::Order);
  template class GenSortIndirect<Float>;
  template class GenSortIndirect<Int>;
  template class GenSortIndirect<uInt>;
  template uInt genSort(Array<uInt> &);
  template uInt genSort(Double *, uInt);
  template uInt genSort(Float *, uInt);
  
  template Int linearSearch(Bool &, Vector<Int> const &, Int const &, uInt, uInt);
  template Int linearSearch(Bool &, Vector<uInt> const &, uInt const &, uInt, uInt);
  
  template class PtrHolder<Random>;
  template class PtrHolder<QBase>;
  template class PtrHolder<Vector<String> >;
  template class PtrHolder<RecordInterface>;
  template class PtrHolder<LogSinkInterface>;
  
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template uInt Register(Quantum<Array<Complex> > const *);
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template uInt Register(Quantum<Array<DComplex> > const *);
  template <class Qtype> class Quantum;
  template <class T> class Vector;
  template uInt Register(Quantum<Vector<Complex> > const *);
  template <class Qtype> class Quantum;
  template <class T> class Vector;
  template uInt Register(Quantum<Vector<DComplex> > const *);
  template <class Qtype> class Quantum;
  template uInt Register(Quantum<Complex> const *);
  template <class Qtype> class Quantum;
  template uInt Register(Quantum<DComplex> const *);
  template uInt Register(Complex const *);
  template uInt Register(DComplex const *);
  template uInt Register(ListNotice<void *> const *);
  template uInt Register(OrderedMapNotice<String, Block<IPosition> > const *);
  template uInt Register(OrderedMapNotice<String, Int> const *);
  template uInt Register(String const *);
  template uInt Register(ListNotice<Param> const *);
  template uInt Register(ListNotice<CountedPtr<SysEventTargetInfo> > const *);
  template uInt Register(ListNotice<uInt> const *);
  template uInt Register(ListNotice<uLong> const *);
  template uInt Register(OrderedMapNotice<Int, Array<Float> > const *);
  template uInt Register(OrderedMapNotice<Int, Vector<Float> > const *);
  template uInt Register(OrderedMapNotice<Int, Int> const *);
  template uInt Register(RecordNotice const *);
  template uInt Register(SysEventTargetInfo const *);
  template uInt Register(MVBaseline const *);
  template uInt Register(MVDirection const *);
  template uInt Register(MVDoppler const *);
  template uInt Register(MVDouble const *);
  template uInt Register(MVEarthMagnetic const *);
  template uInt Register(MVEpoch const *);
  template uInt Register(MVFrequency const *);
  template uInt Register(MVPosition const *);
  template uInt Register(MVRadialVelocity const *);
  template uInt Register(MVuvw const *);
  template <class Qtype> class Quantum;
  template <class T> class Array;
  template uInt Register(Quantum<Array<Double> > const *);
  template uInt Register(Quantum<Array<Float> > const *);
  template uInt Register(Quantum<Array<Int> > const *);
  template <class T> class Matrix;
  template uInt Register(Quantum<Matrix<Double> > const *);
  template <class T> class Vector;
  template uInt Register(Quantum<Vector<Double> > const *);
  template uInt Register(Quantum<Vector<Float> > const *);
  template uInt Register(Quantum<Vector<Int> > const *);
  template uInt Register(Quantum<Double> const *);
  template uInt Register(Quantum<Float> const *);
  template uInt Register(Quantum<Int> const *);
  template uInt Register(Double const *);
  template uInt Register(Float const *);
  template uInt Register(Int const *);
  template uInt Register(Long const *);
  template uInt Register(lDouble const *);
  template uInt Register(uInt const *);

  template class Sequence<uInt>;

}
