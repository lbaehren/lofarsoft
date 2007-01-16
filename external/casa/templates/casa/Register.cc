
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
#include <casa/Utilities/test/tCountedPtr.h>
#include <casa/Utilities/test/tRegister.h>

// include of implementation files

// #include <casa/Arrays/Array.cc>
// #include <casa/Arrays/ArrayIO.cc>
// #include <casa/Arrays/MaskedArray.cc>
// #include <casa/Arrays/Vector.cc>
// #include <casa/Utilities/Assert.cc>
// #include <casa/Utilities/BinarySearch.cc>
// #include <casa/Utilities/COWPtr.cc>
// #include <casa/Utilities/CountedPtr.cc>
// #include <casa/Utilities/Compare.cc>
// #include <casa/Utilities/Copy.cc>
// #include <casa/Utilities/GenSort.cc>
// #include <casa/Utilities/LinearSearch.cc>
// #include <casa/Utilities/PtrHolder.cc>
#include <casa/Utilities/Register.cc>
#include <casa/Utilities/Sequence.cc>

namespace casa {

  // ============================================================================
  //
  //  ReposFiller/templates
  //
  // ============================================================================

  // ------------------------------------------------------------------- Register
  
  template uInt Register(Int const *);
  template uInt Register(uInt const *);
  template uInt Register(Short const *);
  template uInt Register(uShort const *);
  template uInt Register(Long const *);
  template uInt Register(uLong const *);
  template uInt Register(Float const *);
  template uInt Register(Double const *);
  template uInt Register(lDouble const *);
  template uInt Register(Complex const *);
  template uInt Register(DComplex const *);
  template uInt Register(String const *);
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

  template uInt Register (Quantum<Int> const *);
  template uInt Register (Quantum<uInt> const *);
  template uInt Register (Quantum<Short> const *);
  template uInt Register (Quantum<uShort> const *);
  template uInt Register (Quantum<Float> const *);
  template uInt Register (Quantum<Double> const *);
  template uInt Register (Quantum<Complex> const *);
  template uInt Register (Quantum<DComplex> const *);

  template uInt Register (Quantum<Array<Int> > const *);
  template uInt Register (Quantum<Array<uInt> > const *);
  template uInt Register (Quantum<Array<Short> > const *);
  template uInt Register (Quantum<Array<uShort> > const *);
  template uInt Register (Quantum<Array<Float> > const *);
  template uInt Register (Quantum<Array<Double> > const *);
  template uInt Register (Quantum<Array<Complex> > const *);
  template uInt Register (Quantum<Array<DComplex> > const *);

  template uInt Register (Quantum<Matrix<Int> > const *);
  template uInt Register (Quantum<Matrix<uInt> > const *);
  template uInt Register (Quantum<Matrix<Short> > const *);
  template uInt Register (Quantum<Matrix<uShort> > const *);
  template uInt Register (Quantum<Matrix<Float> > const *);
  template uInt Register (Quantum<Matrix<Double> > const *);
  template uInt Register (Quantum<Matrix<Complex> > const *);
  template uInt Register (Quantum<Matrix<DComplex> > const *);

  template uInt Register (Quantum<Vector<Int> > const *);
  template uInt Register (Quantum<Vector<uInt> > const *);
  template uInt Register (Quantum<Vector<Short> > const *);
  template uInt Register (Quantum<Vector<uShort> > const *);
  template uInt Register (Quantum<Vector<Float> > const *);
  template uInt Register (Quantum<Vector<Double> > const *);
  template uInt Register (Quantum<Vector<Complex> > const *);
  template uInt Register (Quantum<Vector<DComplex> > const *);
  
  template uInt Register (ListNotice<void *> const *);
  template uInt Register (ListNotice<Int> const *);
  template uInt Register (ListNotice<uInt> const *);
  template uInt Register (ListNotice<Short> const *);
  template uInt Register (ListNotice<uShort> const *);
  template uInt Register (ListNotice<Long> const *);
  template uInt Register (ListNotice<uLong> const *);
  template uInt Register (ListNotice<Float> const *);
  template uInt Register (ListNotice<Double> const *);
  template uInt Register (ListNotice<Param> const *);
  template uInt Register (ListNotice<Block<IPosition> > const *);
  template uInt Register (ListNotice<CountedPtr<SysEventTargetInfo> > const *);
  template uInt Register (ListNotice<Dlist<int> > const*);
  template uInt Register (ListNotice<List<int> > const*);
  template uInt Register (ListNotice<OrderedPair<int, int> > const*);
  template uInt Register (ListNotice<OrderedPair<int, Array<float> > > const*);
  template uInt Register (ListNotice<OrderedPair<int, Vector<float> > > const*);
  template uInt Register (ListNotice<OrderedPair<String, int> > const*);
  template uInt Register (ListNotice<OrderedPair<String, uInt> > const *);
  template uInt Register (ListNotice<OrderedPair<String, Double> > const *);
  template uInt Register (ListNotice<OrderedPair<String, Block<IPosition> > > const*);
  template uInt Register (ListNotice<OrderedPair<String, OrderedPair<String, uInt> > > const*);

  template uInt Register (OrderedMapNotice<Int, Int> const*);
  template uInt Register (OrderedMapNotice<Int, uInt> const*);
  template uInt Register (OrderedMapNotice<Int, Array<Float> > const *);
  template uInt Register (OrderedMapNotice<Int, Vector<Float> > const *);
  template uInt Register (OrderedMapNotice<String, Int> const *);
  template uInt Register (OrderedMapNotice<String, uInt> const*);
  template uInt Register (OrderedMapNotice<String, Block<IPosition> > const *);
  template uInt Register (OrderedMapNotice<String, OrderedPair<String, uInt> > const*);

  // casa/Utilities/Register.cc
  template uInt Register(bar2foo const *);
  template uInt Register(foo2bar2 const *);
  template uInt Register(bar const *);
  template uInt Register(foobar const *);
  template uInt Register(mytmp<Float> const *);
  template uInt Register(mytmp<Int> const *);
  template uInt Register(mytmp<Long> const *);
  template uInt Register(mytmp<Short> const *);
  template uInt Register(barfoo2 const *);
  template uInt Register(foo const *);
  template uInt Register(bar2 const *);
  template uInt Register(mytmp2<Short> const *);
  template uInt Register(mytmp2<Float> const *);
  template uInt Register(mytmp2<Int> const *);
  template uInt Register(mytmp2<Long> const *);
  template uInt Register(foo2 const *);
  template uInt Register(bar2foo2 const *);
  template uInt Register(foobar2 const *);
  template uInt Register(foo2bar const *);

}
