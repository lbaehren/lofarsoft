1000 casa/Arrays/Array.cc 
     = casa/Arrays/Slicer.h 
     template class Array<Slicer> 
1010 casa/Arrays/Array.cc 
     = casa/Arrays/Vector.h 
     = casa/BasicSL/Complex.h 
     template class Array<Vector<Complex> > 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Vector<Complex> >::ConstIteratorSTL 
     #endif 
1020 casa/Arrays/Array.cc 
     = casa/Arrays/Vector.h 
     template class Array<Vector<Double> > 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Vector<Double> >::ConstIteratorSTL 
     #endif 
1030 casa/Arrays/Array.cc 
     = casa/Arrays/Vector.h 
     template class Array<Vector<Float> > 
1040 casa/Arrays/Array.cc 
     = casa/BasicSL/Complex.h 
     template class Array<Complex> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Complex>::ConstIteratorSTL 
     #endif 
1050 casa/Arrays/Array.cc 
     = casa/BasicSL/Complex.h 
     template class Array<DComplex> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<DComplex>::ConstIteratorSTL 
     #endif 
1060 casa/Arrays/Array.cc 
     = casa/BasicSL/String.h 
     template class Array<String> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<String>::ConstIteratorSTL 
     #endif 
1070 casa/Arrays/Array.cc 
     = casa/Quanta/MVDirection.h 
     template class Array<MVDirection> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<MVDirection>::ConstIteratorSTL 
     #endif 
1080 casa/Arrays/Array.cc 
     = casa/Quanta/MVFrequency.h 
     template class Array<MVFrequency> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<MVFrequency>::ConstIteratorSTL 
     #endif 
1090 casa/Arrays/Array.cc 
     = casa/Quanta/MVPosition.h 
     template class Array<MVPosition> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<MVPosition>::ConstIteratorSTL 
     #endif 
1100 casa/Arrays/Array.cc 
     = casa/Quanta/MVRadialVelocity.h 
     template class Array<MVRadialVelocity> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<MVRadialVelocity>::ConstIteratorSTL 
     #endif 
1110 casa/Arrays/Array.cc 
     = casa/Quanta/MVTime.h 
     template class Array<MVTime> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<MVTime>::ConstIteratorSTL 
     #endif 
1120 casa/Arrays/Array.cc 
     = casa/Quanta/Quantum.h 
     template class Array<Quantum<Double> > 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Quantum<Double> >::ConstIteratorSTL 
     #endif 
1130 casa/Arrays/Array.cc 
     = casa/Quanta/Quantum.h 
     template class Array<Quantum<Float> > 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Quantum<Float> >::ConstIteratorSTL 
     #endif 
1140 casa/Arrays/Array.cc 
     = casa/Quanta/QuantumHolder.h 
     template class Array<QuantumHolder> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<QuantumHolder>::ConstIteratorSTL 
     #endif 
1150 casa/Arrays/Array.cc 
     = casa/Quanta/Unit.h 
     template class Array<Unit> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Unit>::ConstIteratorSTL 
     #endif 
1160 casa/Arrays/Array.cc casa/Arrays/Matrix.h 
     = casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Array<Matrix<CountedPtr<Random> > > 
1170 casa/Arrays/Array.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Array<CountedPtr<Random> > 
1180 casa/Arrays/Array.cc 
     template class Array<Array<Double> > 
1190 casa/Arrays/Array.cc 
     template class Array<Bool> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Bool>::ConstIteratorSTL 
     #endif 
1200 casa/Arrays/Array.cc 
     template class Array<Char> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Char>::ConstIteratorSTL 
     #endif 
1210 casa/Arrays/Array.cc 
     template class Array<Double> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Double>::ConstIteratorSTL 
     #endif 
1220 casa/Arrays/Array.cc 
     template class Array<Float *> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Float *>::ConstIteratorSTL 
     #endif 
1230 casa/Arrays/Array.cc 
     template class Array<Float> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Float>::ConstIteratorSTL 
     #endif 
1240 casa/Arrays/Array.cc 
     template class Array<Int> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Int>::ConstIteratorSTL 
     #endif 
1250 casa/Arrays/Array.cc 
     template class Array<Long> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Long>::ConstIteratorSTL 
     #endif 
1260 casa/Arrays/Array.cc 
     template class Array<Short> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<Short>::ConstIteratorSTL 
     #endif 
1270 casa/Arrays/Array.cc 
     template class Array<uChar> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<uChar>::ConstIteratorSTL 
     #endif 
1280 casa/Arrays/Array.cc 
     template class Array<uInt> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<uInt>::ConstIteratorSTL 
     #endif 
1290 casa/Arrays/Array.cc 
     template class Array<uLong> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<uLong>::ConstIteratorSTL 
     #endif 
1300 casa/Arrays/Array.cc 
     template class Array<uShort> 
     #ifdef AIPS_SUN_NATIVE 
       template class Array<uShort>::ConstIteratorSTL 
     #endif 
1000 casa/Arrays/ArrayIO.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template AipsIO & operator<<(AipsIO &, Array<Bool> const &) 
     template AipsIO & operator<<(AipsIO &, Array<uChar> const &) 
     template AipsIO & operator<<(AipsIO &, Array<Short> const &) 
     template AipsIO & operator<<(AipsIO &, Array<Int> const &) 
     template AipsIO & operator<<(AipsIO &, Array<uInt> const &) 
     template AipsIO & operator<<(AipsIO &, Array<Float> const &) 
     template AipsIO & operator<<(AipsIO &, Array<Double> const &) 
     template AipsIO & operator<<(AipsIO &, Array<Complex> const &) 
     template AipsIO & operator<<(AipsIO &, Array<DComplex> const &) 
     template AipsIO & operator<<(AipsIO &, Array<String> const &) 
     template void putArray(AipsIO &, Array<Bool> const &, Char const *) 
     template void putArray(AipsIO &, Array<uChar> const &, Char const *) 
     template void putArray(AipsIO &, Array<Short> const &, Char const *) 
     template void putArray(AipsIO &, Array<Int> const &, Char const *) 
     template void putArray(AipsIO &, Array<uInt> const &, Char const *) 
     template void putArray(AipsIO &, Array<Float> const &, Char const *) 
     template void putArray(AipsIO &, Array<Double> const &, Char const *) 
     template void putArray(AipsIO &, Array<Complex> const &, Char const *) 
     template void putArray(AipsIO &, Array<DComplex> const &, Char const *) 
     template void putArray(AipsIO &, Array<String> const &, Char const *) 
1010 casa/Arrays/ArrayIO.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template AipsIO & operator>>(AipsIO &, Array<Bool> &) 
     template AipsIO & operator>>(AipsIO &, Array<uChar> &) 
     template AipsIO & operator>>(AipsIO &, Array<Short> &) 
     template AipsIO & operator>>(AipsIO &, Array<Int> &) 
     template AipsIO & operator>>(AipsIO &, Array<uInt> &) 
     template AipsIO & operator>>(AipsIO &, Array<Float> &) 
     template AipsIO & operator>>(AipsIO &, Array<Double> &) 
     template AipsIO & operator>>(AipsIO &, Array<Complex> &) 
     template AipsIO & operator>>(AipsIO &, Array<DComplex> &) 
     template AipsIO & operator>>(AipsIO &, Array<String> &) 
1020 casa/Arrays/ArrayIO.cc casa/BasicSL/Complex.h 
     template ostream & operator<<(ostream &, Array<Complex> const &) 
1030 casa/Arrays/ArrayIO.cc casa/BasicSL/Complex.h 
     template ostream & operator<<(ostream &, Array<DComplex> const &) 
1040 casa/Arrays/ArrayIO.cc casa/Logging/LogIO.h 
     = casa/BasicSL/String.h 
     template LogIO & operator<<(LogIO &, Array<Complex> const &) 
     template LogIO & operator<<(LogIO &, Array<DComplex> const &) 
     template LogIO & operator<<(LogIO &, Array<String> const &) 
     template LogIO & operator<<(LogIO &, Array<Double> const &) 
     template LogIO & operator<<(LogIO &, Array<Float> const &) 
     template LogIO & operator<<(LogIO &, Array<Int> const &) 
     template LogIO & operator<<(LogIO &, Array<uInt> const &) 
     template LogIO & operator<<(LogIO &, Array<Short> const &) 
     template LogIO & operator<<(LogIO &, Array<uChar> const &) 
     template LogIO & operator<<(LogIO &, Array<Bool> const &) 
1050 casa/Arrays/ArrayIO.cc casa/Quanta/Quantum.h 
     template ostream & operator<<(ostream &, Array<Quantum<Double> > const &) 
1060 casa/Arrays/ArrayIO.cc 
     template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<uInt> &, IPosition const *, Bool) 
1070 casa/Arrays/ArrayIO.cc 
     template istream & operator>>(istream &, Array<String> &) 
     template Bool read(istream &, Array<String> &, IPosition const *, Bool) 
     template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<String> &, IPosition const *, Bool) 
1080 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Bool> const &) 
1090 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Char> const &) 
1100 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Double> const &) 
1110 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Float> const &) 
1120 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Int> const &) 
1130 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Long> const &) 
1140 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<Short> const &) 
1150 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<String> const &) 
1160 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<uChar> const &) 
1170 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<uInt> const &) 
1180 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<uLong> const &) 
1190 casa/Arrays/ArrayIO.cc 
     template ostream & operator<<(ostream &, Array<uShort> const &) 
1000 casa/Arrays/ArrayIter.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template class ArrayIterator<Bool> 
     template class ArrayIterator<uChar> 
     template class ArrayIterator<Short> 
     template class ArrayIterator<uShort> 
     template class ArrayIterator<Int> 
     template class ArrayIterator<uInt> 
     template class ArrayIterator<Float> 
     template class ArrayIterator<Double> 
     template class ArrayIterator<Complex> 
     template class ArrayIterator<DComplex> 
     template class ArrayIterator<String> 
1010 casa/Arrays/ArrayIter.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template class ReadOnlyArrayIterator<Bool> 
     template class ReadOnlyArrayIterator<uChar> 
     template class ReadOnlyArrayIterator<Short> 
     template class ReadOnlyArrayIterator<uShort> 
     template class ReadOnlyArrayIterator<Int> 
     template class ReadOnlyArrayIterator<uInt> 
     template class ReadOnlyArrayIterator<Float> 
     template class ReadOnlyArrayIterator<Double> 
     template class ReadOnlyArrayIterator<Complex> 
     template class ReadOnlyArrayIterator<DComplex> 
     template class ReadOnlyArrayIterator<String> 
1020 casa/Arrays/ArrayIter.cc 
     template class ArrayIterator<Char> 
1030 casa/Arrays/ArrayIter.cc 
     template class ArrayIterator<uLong> 
1040 casa/Arrays/ArrayIter.cc 
     template class ReadOnlyArrayIterator<Char> 
1000 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Array<Bool> operator!=(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator!=(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator!=(Complex const &, Array<Complex> const &) 
     template Array<Bool> operator<(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator<(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator<(Complex const &, Array<Complex> const &) 
     template Array<Bool> operator<=(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator<=(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator<=(Complex const &, Array<Complex> const &) 
     template Array<Bool> operator==(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator==(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator==(Complex const &, Array<Complex> const &) 
     template Array<Bool> operator>(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator>(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator>(Complex const &, Array<Complex> const &) 
     template Array<Bool> operator>=(Array<Complex> const &, Array<Complex> const &) 
     template Array<Bool> operator>=(Array<Complex> const &, Complex const &) 
     template Array<Bool> operator>=(Complex const &, Array<Complex> const &) 
     template Bool allEQ(Array<Complex> const &, Array<Complex> const &) 
     template Bool allNear(Array<Complex> const &, Complex const &, Double) 
     template Bool allNearAbs(Array<Complex> const &, Complex const &, Double) 
1010 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Bool allEQ(Array<DComplex> const &, Array<DComplex> const &) 
     template Bool allNearAbs(Array<DComplex> const &, DComplex const &, Double) 
     template Bool anyEQ(DComplex const &, Array<DComplex> const &) 
     template LogicalArray operator!=(Array<DComplex> const &, Array<DComplex> const &) 
     template LogicalArray operator!=(Array<DComplex> const &, DComplex const &) 
     template LogicalArray operator!=(DComplex const &, Array<DComplex> const &) 
     template LogicalArray operator==(Array<DComplex> const &, Array<DComplex> const &) 
     template LogicalArray operator==(Array<DComplex> const &, DComplex const &) 
     template LogicalArray operator==(DComplex const &, Array<DComplex> const &) 
     template LogicalArray operator>(Array<DComplex> const &, Array<DComplex> const &) 
     template LogicalArray operator>(Array<DComplex> const &, DComplex const &) 
     template LogicalArray operator>(DComplex const &, Array<DComplex> const &) 
     template LogicalArray operator>=(Array<DComplex> const &, Array<DComplex> const &) 
     template LogicalArray operator>=(Array<DComplex> const &, DComplex const &) 
     template LogicalArray operator>=(DComplex const &, Array<DComplex> const &) 
     template LogicalArray near(Array<DComplex> const &, Array<DComplex> const &, Double) 
     template LogicalArray near(Array<DComplex> const &, DComplex const &, Double) 
     template LogicalArray near(DComplex const &, Array<DComplex> const &, Double) 
     template LogicalArray nearAbs(Array<DComplex> const &, Array<DComplex> const &, Double) 
     template LogicalArray nearAbs(Array<DComplex> const &, DComplex const &, Double) 
     template LogicalArray nearAbs(DComplex const &, Array<DComplex> const &, Double) 
1020 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     = casa/BasicSL/String.h 
     template Bool allEQ(Array<String> const &, Array<String> const &) 
     template Bool allEQ(Array<String> const &, String const &) 
     template Bool anyEQ(Array<String> const &, String const &) 
     template Bool anyEQ(String const &, Array<String> const &) 
     template Bool anyNE(Array<String> const &, Array<String> const &) 
     template LogicalArray operator!=(Array<String> const &, Array<String> const &) 
     template LogicalArray operator!=(Array<String> const &, String const &) 
     template LogicalArray operator!=(String const &, Array<String> const &) 
     template LogicalArray operator==(Array<String> const &, Array<String> const &) 
     template LogicalArray operator==(Array<String> const &, String const &) 
     template LogicalArray operator==(String const &, Array<String> const &) 
     template LogicalArray operator>(Array<String> const &, Array<String> const &) 
     template LogicalArray operator>(Array<String> const &, String const &) 
     template LogicalArray operator>(String const &, Array<String> const &) 
     template LogicalArray operator>=(Array<String> const &, Array<String> const &) 
     template LogicalArray operator>=(Array<String> const &, String const &) 
     template LogicalArray operator>=(String const &, Array<String> const &) 
1030 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     = casa/Quanta/MVTime.h 
     template Bool anyEQ(MVTime const &, Array<MVTime> const &) 
     template LogicalArray operator==(MVTime const &, Array<MVTime> const &) 
     template LogicalArray operator!=(Array<MVTime> const &, Array<MVTime> const &) 
     template LogicalArray operator!=(Array<MVTime> const &, MVTime const &) 
     template LogicalArray operator!=(MVTime const &, Array<MVTime> const &) 
     template LogicalArray operator>(Array<MVTime> const &, Array<MVTime> const &) 
     template LogicalArray operator>(Array<MVTime> const &, MVTime const &) 
     template LogicalArray operator>(MVTime const &, Array<MVTime> const &) 
     template LogicalArray operator>=(Array<MVTime> const &, Array<MVTime> const &) 
     template LogicalArray operator>=(Array<MVTime> const &, MVTime const &) 
     template LogicalArray operator>=(MVTime const &, Array<MVTime> const &) 
1040 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<Double> const &, Array<Double> const &) 
     template Bool allEQ(Array<Double> const &, Double const &) 
     template Bool allGE(Array<Double> const &, Double const &) 
     template Bool allLE(Array<Double> const &, Double const &) 
     template Bool allNear(Array<Double> const &, Array<Double> const &, Double) 
     template Bool allNear(Array<Double> const &, Double const &, Double) 
     template Bool allNearAbs(Array<Double> const &, Array<Double> const &, Double) 
     template Bool anyEQ(Double const &, Array<Double> const &) 
     template Bool anyGT(Array<Double> const &, Double const &) 
     template Bool anyLT(Array<Double> const &, Double const &) 
     template Bool anyNE(Array<Double> const &, Array<Double> const &) 
     template Bool anyNE(Array<Double> const &, Double const &) 
     template LogicalArray operator!=(Array<Double> const &, Array<Double> const &) 
     template LogicalArray operator!=(Array<Double> const &, Double const &) 
     template LogicalArray operator!=(Double const &, Array<Double> const &) 
     template LogicalArray operator==(Array<Double> const &, Array<Double> const &) 
     template LogicalArray operator==(Array<Double> const &, Double const &) 
     template LogicalArray operator==(Double const &, Array<Double> const &) 
     template LogicalArray operator>(Array<Double> const &, Array<Double> const &) 
     template LogicalArray operator>(Array<Double> const &, Double const &) 
     template LogicalArray operator>(Double const &, Array<Double> const &) 
     template LogicalArray operator>=(Array<Double> const &, Array<Double> const &) 
     template LogicalArray operator>=(Array<Double> const &, Double const &) 
     template LogicalArray operator>=(Double const &, Array<Double> const &) 
     template LogicalArray operator<=(Array<Double> const &, Double const &) 
     template LogicalArray near(Array<Double> const &, Array<Double> const &, Double) 
     template LogicalArray near(Array<Double> const &, Double const &, Double) 
     template LogicalArray near(Double const &, Array<Double> const &, Double) 
     template LogicalArray nearAbs(Array<Double> const &, Array<Double> const &, Double) 
     template LogicalArray nearAbs(Array<Double> const &, Double const &, Double) 
     template LogicalArray nearAbs(Double const &, Array<Double> const &, Double) 
1050 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<Float> const &, Array<Float> const &) 
     template Bool allEQ(Array<Float> const &, Float const &) 
     template Bool allGE(Array<Float> const &, Float const &) 
     template Bool allLT(Array<Float> const &, Float const &) 
     template Bool anyEQ(Array<Float> const &, Float const &) 
     template Bool anyGT(Array<Float> const &, Float const &) 
     template Bool anyLE(Array<Float> const &, Float const &) 
     template Bool anyLT(Array<Float> const &, Float const &) 
     template Bool anyNE(Array<Float> const &, Float const &) 
     template Bool anyNE(Array<Float> const &, Array<Float> const &) 
1060 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<Int> const &, Array<Int> const &) 
     template Bool allEQ(Array<Int> const &, Int const &) 
     template Bool allGE(Array<Int> const &, Int const &) 
     template Bool allLT(Array<Int> const &, Int const &) 
     template Bool anyEQ(Array<Int> const &, Int const &) 
     template Bool anyGE(Array<Int> const &, Int const &) 
     template Bool anyGT(Array<Int> const &, Int const &) 
     template Bool anyLE(Array<Int> const &, Int const &) 
     template Bool anyLT(Array<Int> const &, Int const &) 
     template Bool anyNE(Array<Int> const &, Int const &) 
     template Bool anyNE(Array<Int> const &, Array<Int> const &) 
     template Array<Bool> operator!=(Array<Int> const &, Int const &) 
     template Array<Bool> operator==(Array<Int> const &, Int const &) 
     template Array<Bool> operator>(Array<Int> const &, Int const &) 
     template Array<Bool> operator<(Array<Int> const &, Int const &) 
     template Array<Bool> operator>=(Array<Int> const &, Int const &) 
     template Array<Bool> operator<=(Array<Int> const &, Int const &) 
1070 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<Short> const &, Array<Short> const &) 
     template Array<Bool> operator==(Array<Short> const &, Short const &) 
     template Array<Bool> operator>(Array<Short> const &, Short const &) 
1080 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<uChar> const &, Array<uChar> const &) 
     template Bool allEQ(Array<uChar> const &, uChar const &) 
     template Array<Bool> operator==(Array<uChar> const &, uChar const &) 
     template Array<Bool> operator>(Array<uChar> const &, uChar const &) 
1090 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allEQ(Array<uInt> const &, Array<uInt> const &) 
     template Bool allEQ(Array<uInt> const &, uInt const &) 
     template Bool allGE(Array<uInt> const &, uInt const &) 
     template Bool allLT(Array<uInt> const &, uInt const &) 
     template Bool anyEQ(Array<uInt> const &, uInt const &) 
     template Bool anyGE(Array<uInt> const &, uInt const &) 
     template Bool anyGT(Array<uInt> const &, uInt const &) 
     template Bool anyLE(Array<uInt> const &, uInt const &) 
     template Bool anyLT(Array<uInt> const &, uInt const &) 
     template Bool anyNE(Array<uInt> const &, uInt const &) 
     template Bool anyNE(Array<uInt> const &, Array<uInt> const &) 
     template Array<Bool> operator!=(Array<uInt> const &, uInt const &) 
     template Array<Bool> operator==(Array<uInt> const &, uInt const &) 
     template Array<Bool> operator>(Array<uInt> const &, uInt const &) 
     template Array<Bool> operator<(Array<uInt> const &, uInt const &) 
     template Array<Bool> operator||(Array<uInt> const &, Array<uInt> const &) 
1100 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool allNear(Array<Float> const &, Array<Float> const &, Double) 
     template Bool allNear(Array<Float> const &, Float const &, Double) 
     template Bool allNearAbs(Array<Float> const &, Array<Float> const &, Double) 
     template Bool allNearAbs(Array<Float> const &, Float const &, Double) 
     template LogicalArray operator!=(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator!=(Array<Float> const &, Float const &) 
     template LogicalArray operator!=(Float const &, Array<Float> const &) 
     template LogicalArray operator<(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator<(Array<Float> const &, Float const &) 
     template LogicalArray operator<(Float const &, Array<Float> const &) 
     template LogicalArray operator<=(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator<=(Array<Float> const &, Float const &) 
     template LogicalArray operator<=(Float const &, Array<Float> const &) 
     template LogicalArray operator==(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator==(Array<Float> const &, Float const &) 
     template LogicalArray operator==(Float const &, Array<Float> const &) 
     template LogicalArray operator>(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator>(Array<Float> const &, Float const &) 
     template LogicalArray operator>(Float const &, Array<Float> const &) 
     template LogicalArray operator>=(Array<Float> const &, Array<Float> const &) 
     template LogicalArray operator>=(Array<Float> const &, Float const &) 
     template LogicalArray operator>=(Float const &, Array<Float> const &) 
1110 casa/Arrays/ArrayLogical.cc casa/Arrays/Array.h 
     template Bool anyNE(Array<Bool> const &, Array<Bool> const &) 
     template Bool anyNE(Array<Bool> const &, Bool const &) 
     template Bool allAND(Array<Bool> const &, Bool const &) 
     template Bool allEQ(Array<Bool> const &, Array<Bool> const &) 
     template Bool allEQ(Array<Bool> const &, Bool const &) 
     template Bool allOR(Array<Bool> const &, Array<Bool> const &) 
     template Bool anyEQ(Array<Bool> const &, Bool const &) 
     template Bool anyEQ(Bool const &, Array<Bool> const &) 
     template uInt ntrue(Array<Bool> const &) 
     template uInt nfalse(Array<Bool> const &) 
     template LogicalArray operator!(Array<Bool> const &) 
     template LogicalArray operator!=(Array<Bool> const &, Array<Bool> const &) 
     template LogicalArray operator!=(Array<Bool> const &, Bool const &) 
     template LogicalArray operator!=(Bool const &, Array<Bool> const &) 
     template LogicalArray operator&&(Array<Bool> const &, Array<Bool> const &) 
     template LogicalArray operator&&(Array<Bool> const &, Bool const &) 
     template LogicalArray operator&&(Bool const &, Array<Bool> const &) 
     template LogicalArray operator==(Array<Bool> const &, Array<Bool> const &) 
     template LogicalArray operator==(Array<Bool> const &, Bool const &) 
     template LogicalArray operator==(Bool const &, Array<Bool> const &) 
     template LogicalArray operator||(Array<Bool> const &, Array<Bool> const &) 
     template LogicalArray operator||(Array<Bool> const &, Bool const &) 
     template LogicalArray operator||(Bool const &, Array<Bool> const &) 
1120 casa/Arrays/ArrayLogical.cc 
     template Array<uInt> partialNTrue(Array<Bool> const &, IPosition const &) 
     template Array<uInt> partialNFalse(Array<Bool> const &, IPosition const &) 
1000 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     = casa/Quanta/MVTime.h 
     template void convertArray(Array<Double> &, Array<MVTime> const &) 
1010 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Array<Complex> abs(Array<Complex> const &) 
     template Array<Complex> cos(Array<Complex> const &) 
     template Array<Complex> cosh(Array<Complex> const &) 
     template Array<Complex> exp(Array<Complex> const &) 
     template Array<Complex> log(Array<Complex> const &) 
     template Array<Complex> log10(Array<Complex> const &) 
     template Array<Complex> operator*(Array<Complex> const &, Complex const &) 
     template Array<Complex> operator+(Array<Complex> const &, Array<Complex> const &) 
     template Array<Complex> operator-(Array<Complex> const &) 
     template Array<Complex> operator-(Complex const &, Array<Complex> const &) 
     template Array<Complex> operator-(Array<Complex> const &, Array<Complex> const &) 
     template Array<Complex> operator/(Array<Complex> const &, Complex const &) 
     template Array<Complex> operator/(Complex const &, Array<Complex> const &) 
     template Array<Complex> pow(Array<Complex> const &, Array<Complex> const &) 
     template Array<Complex> pow(Array<Complex> const &, Double const &) 
     template Array<Complex> sin(Array<Complex> const &) 
     template Array<Complex> sinh(Array<Complex> const &) 
     template Array<Complex> sqrt(Array<Complex> const &) 
     template Complex max(Array<Complex> const &) 
     template Complex min(Array<Complex> const &) 
     template void minMax(Complex &, Complex &, Array<Complex> const &) 
     template void operator*=(Array<Complex> &, Array<Complex> const &) 
     template void operator*=(Array<Complex> &, Complex const &) 
     template void operator+=(Array<Complex> &, Array<Complex> const &) 
     template void operator+=(Array<Complex> &, Complex const &) 
     template void operator-=(Array<Complex> &, Array<Complex> const &) 
     template void operator-=(Array<Complex> &, Complex const &) 
     template void operator/=(Array<Complex> &, Array<Complex> const &) 
     template void operator/=(Array<Complex> &, Complex const &) 
1020 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Array<DComplex> abs(Array<DComplex> const &) 
     template Array<DComplex> cos(Array<DComplex> const &) 
     template Array<DComplex> cosh(Array<DComplex> const &) 
     template Array<DComplex> exp(Array<DComplex> const &) 
     template Array<DComplex> log(Array<DComplex> const &) 
     template Array<DComplex> log10(Array<DComplex> const &) 
     template Array<DComplex> max(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> max(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> min(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> min(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> operator*(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> operator*(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> operator*(DComplex const &, Array<DComplex> const &) 
     template Array<DComplex> operator+(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> operator+(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> operator+(DComplex const &, Array<DComplex> const &) 
     template Array<DComplex> operator-(Array<DComplex> const &) 
     template Array<DComplex> operator-(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> operator-(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> operator-(DComplex const &, Array<DComplex> const &) 
     template Array<DComplex> operator/(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> operator/(Array<DComplex> const &, DComplex const &) 
     template Array<DComplex> operator/(DComplex const &, Array<DComplex> const &) 
     template Array<DComplex> pow(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<DComplex> pow(Array<DComplex> const &, Double const &) 
     template Array<DComplex> pow(DComplex const &, Array<DComplex> const &) 
     template Array<DComplex> sin(Array<DComplex> const &) 
     template Array<DComplex> sinh(Array<DComplex> const &) 
     template Array<DComplex> sqrt(Array<DComplex> const &) 
     template DComplex max(Array<DComplex> const &) 
     template DComplex min(Array<DComplex> const &) 
     template DComplex product(Array<DComplex> const &) 
     template DComplex sum(Array<DComplex> const &) 
     template Complex sum(Array<Complex> const &) 
     template void max(Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &) 
     template void max(Array<DComplex> &, Array<DComplex> const &, DComplex const &) 
     template void min(Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &) 
     template void min(Array<DComplex> &, Array<DComplex> const &, DComplex const &) 
     template void minMax(DComplex &, DComplex &, Array<DComplex> const &) 
     template void operator*=(Array<DComplex> &, Array<DComplex> const &) 
     template void operator*=(Array<DComplex> &, DComplex const &) 
     template void operator+=(Array<DComplex> &, Array<DComplex> const &) 
     template void operator+=(Array<DComplex> &, DComplex const &) 
     template void operator-=(Array<DComplex> &, Array<DComplex> const &) 
     template void operator-=(Array<DComplex> &, DComplex const &) 
     template void operator/=(Array<DComplex> &, Array<DComplex> const &) 
     template void operator/=(Array<DComplex> &, DComplex const &) 
1030 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template DComplex mean(Array<DComplex> const &) 
1040 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template DComplex median(Array<DComplex> const &, Bool) 
     template DComplex median(Array<DComplex> const &, Bool, Bool, Bool) 
1050 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Float product(Array<Float> const &); 
     template Array<Bool> operator*(Array<Bool> const &, Array<Bool> const &) 
1060 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template void convertArray(Array<Complex> &, Array<DComplex> const &) 
     template void convertArray(Array<Complex> &, Array<Double> const &) 
     template void convertArray(Array<Complex> &, Array<Float> const &) 
     template void convertArray(Array<Complex> &, Array<Int> const &) 
     template void convertArray(Array<Complex> &, Array<Short> const &) 
     template void convertArray(Array<Complex> &, Array<uChar> const &) 
     template void convertArray(Array<Complex> &, Array<uInt> const &) 
     template void convertArray(Array<DComplex> &, Array<Complex> const &) 
     template void convertArray(Array<DComplex> &, Array<DComplex> const &) 
     template void convertArray(Array<DComplex> &, Array<Double> const &) 
     template void convertArray(Array<DComplex> &, Array<Float> const &) 
     template void convertArray(Array<DComplex> &, Array<Int> const &) 
     template void convertArray(Array<DComplex> &, Array<Short> const &) 
     template void convertArray(Array<DComplex> &, Array<uChar> const &) 
     template void convertArray(Array<DComplex> &, Array<uInt> const &) 
     template void convertArray(Array<uInt> &, Array<Bool> const &) 
1070 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     = casa/BasicSL/String.h 
     template Array<String> operator+(String const &, Array<String> const &) 
     template void operator+=(Array<String> &, Array<String> const &) 
     template void operator+=(Array<String> &, String const &) 
1080 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Array<Double> abs(Array<Double> const &) 
     template Array<Double> acos(Array<Double> const &) 
     template Array<Double> asin(Array<Double> const &) 
     template Array<Double> atan(Array<Double> const &) 
     template Array<Double> atan2(Array<Double> const &, Array<Double> const &) 
     template Array<Double> ceil(Array<Double> const &) 
     template Array<Double> cos(Array<Double> const &) 
     template Array<Double> cosh(Array<Double> const &) 
     template Array<Double> exp(Array<Double> const &) 
     template Array<Double> fabs(Array<Double> const &) 
     template Array<Double> floor(Array<Double> const &) 
     template Array<Double> fmod(Array<Double> const &, Array<Double> const &) 
     template Array<Double> fmod(Array<Double> const &, Double const &) 
     template Array<Double> fmod(Double const &, Array<Double> const &) 
     template Array<Double> log(Array<Double> const &) 
     template Array<Double> log10(Array<Double> const &) 
     template Array<Double> max(Array<Double> const &, Array<Double> const &) 
     template Array<Double> max(Array<Double> const &, Double const &) 
     template Array<Double> min(Array<Double> const &, Array<Double> const &) 
     template Array<Double> min(Array<Double> const &, Double const &) 
     template Array<Double> operator*(Array<Double> const &, Array<Double> const &) 
     template Array<Double> operator*(Array<Double> const &, Double const &) 
     template Array<Double> operator*(Double const &, Array<Double> const &) 
     template Array<Double> operator+(Array<Double> const &, Array<Double> const &) 
     template Array<Double> operator+(Array<Double> const &, Double const &) 
     template Array<Double> operator+(Double const &, Array<Double> const &) 
     template Array<Double> operator-(Array<Double> const &) 
     template Array<Double> operator-(Array<Double> const &, Array<Double> const &) 
     template Array<Double> operator-(Array<Double> const &, Double const &) 
     template Array<Double> operator-(Double const &, Array<Double> const &) 
     template Array<Double> operator/(Array<Double> const &, Array<Double> const &) 
     template Array<Double> operator/(Array<Double> const &, Double const &) 
     template Array<Double> operator/(Double const &, Array<Double> const &) 
     template Array<Double> pow(Array<Double> const &, Array<Double> const &) 
     template Array<Double> pow(Array<Double> const &, Double const &) 
     template Array<Double> pow(Double const &, Array<Double> const &) 
     template Array<Double> sin(Array<Double> const &) 
     template Array<Double> sinh(Array<Double> const &) 
     template Array<Double> sqrt(Array<Double> const &) 
     template Array<Double> tan(Array<Double> const &) 
     template Array<Double> tanh(Array<Double> const &) 
     template Double avdev(Array<Double> const &) 
     template Double avdev(Array<Double> const &, Double) 
     template Double fractile(Array<Double> const &, Float, Bool, Bool) 
     template Double max(Array<Double> const &) 
     template Double mean(Array<Double> const &) 
     template Double median(Array<Double> const &, Bool) 
     template Double median(Array<Double> const &, Bool, Bool, Bool) 
     template Double min(Array<Double> const &) 
     template Double product(Array<Double> const &) 
     template Double stddev(Array<Double> const &) 
     template Double sum(Array<Double> const &) 
     template Double variance(Array<Double> const &) 
     template Double variance(Array<Double> const &, Double) 
     template void indgen(Array<Double> &) 
     template void indgen(Array<Double> &, Double) 
     template void indgen(Array<Double> &, Double, Double) 
     template void max(Array<Double> &, Array<Double> const &, Array<Double> const &) 
     template void max(Array<Double> &, Array<Double> const &, Double const &) 
     template void min(Array<Double> &, Array<Double> const &, Array<Double> const &) 
     template void min(Array<Double> &, Array<Double> const &, Double const &) 
     template void minMax(Double &, Double &, Array<Double> const &) 
     template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &) 
     template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &, Array<Bool> const &) 
     template void operator*=(Array<Double> &, Array<Double> const &) 
     template void operator*=(Array<Double> &, Double const &) 
     template void operator+=(Array<Double> &, Array<Double> const &) 
     template void operator+=(Array<Double> &, Double const &) 
     template void operator-=(Array<Double> &, Array<Double> const &) 
     template void operator-=(Array<Double> &, Double const &) 
     template void operator/=(Array<Double> &, Array<Double> const &) 
     template void operator/=(Array<Double> &, Double const &) 
1090 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Array<Float> abs(Array<Float> const &) 
     template Array<Float> acos(Array<Float> const &) 
     template Array<Float> asin(Array<Float> const &) 
     template Array<Float> atan(Array<Float> const &) 
     template Array<Float> atan2(Array<Float> const &, Array<Float> const &) 
     template Array<Float> ceil(Array<Float> const &) 
     template Array<Float> cos(Array<Float> const &) 
     template Array<Float> cosh(Array<Float> const &) 
     template Array<Float> exp(Array<Float> const &) 
     template Array<Float> floor(Array<Float> const &) 
     template Array<Float> fmod(Array<Float> const &, Array<Float> const &) 
     template Array<Float> log(Array<Float> const &) 
     template Array<Float> log10(Array<Float> const &) 
     template Array<Float> max(Array<Float> const &, Array<Float> const &) 
     template Array<Float> min(Array<Float> const &, Array<Float> const &) 
     template Array<Float> operator*(Array<Float> const &, Array<Float> const &) 
     template Array<Float> operator*(Array<Float> const &, Float const &) 
     template Array<Float> operator*(Float const &, Array<Float> const &) 
     template Array<Float> operator+(Array<Float> const &, Array<Float> const &) 
     template Array<Float> operator+(Array<Float> const &, Float const &) 
     template Array<Float> operator-(Array<Float> const &) 
     template Array<Float> operator-(Array<Float> const &, Array<Float> const &) 
     template Array<Float> operator-(Array<Float> const &, Float const &) 
     template Array<Float> operator-(Float const &, Array<Float> const &) 
     template Array<Float> operator/(Array<Float> const &, Array<Float> const &) 
     template Array<Float> operator/(Array<Float> const &, Float const &) 
     template Array<Float> operator/(Float const &, Array<Float> const &) 
     template Array<Float> pow(Array<Float> const &, Array<Float> const &) 
     template Array<Float> pow(Array<Float> const &, Double const &) 
     template Array<Float> sin(Array<Float> const &) 
     template Array<Float> sinh(Array<Float> const &) 
     template Array<Float> sqrt(Array<Float> const &) 
     template Array<Float> tan(Array<Float> const &) 
     template Array<Float> tanh(Array<Float> const &) 
     template Float fractile(Array<Float> const &, Float, Bool, Bool) 
     template Float max(Array<Float> const &) 
     template Float mean(Array<Float> const &) 
     template Float median(Array<Float> const &, Bool) 
     template Float median(Array<Float> const &, Bool, Bool, Bool) 
     template Float min(Array<Float> const &) 
     template Float stddev(Array<Float> const &) 
     template Float sum(Array<Float> const &) 
     template Float variance(Array<Float> const &) 
     template Float variance(Array<Float> const &, Float) 
     template void indgen(Array<Float> &) 
     template void indgen(Array<Float> &, Float) 
     template void indgen(Array<Float> &, Float, Float) 
     template void max(Array<Float> &, Array<Float> const &, Array<Float> const &) 
     template void max(Array<Float> &, Array<Float> const &, Float const &) 
     template void min(Array<Float> &, Array<Float> const &, Array<Float> const &) 
     template void min(Array<Float> &, Array<Float> const &, Float const &) 
     template void minMax(Float &, Float &, Array<Float> const &) 
     template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &) 
     template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Bool> const &) 
     template void minMaxMasked(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Float> const &) 
     template void operator*=(Array<Float> &, Array<Float> const &) 
     template void operator*=(Array<Float> &, Float const &) 
     template void operator+=(Array<Float> &, Array<Float> const &) 
     template void operator+=(Array<Float> &, Float const &) 
     template void operator-=(Array<Float> &, Array<Float> const &) 
     template void operator-=(Array<Float> &, Float const &) 
     template void operator/=(Array<Float> &, Array<Float> const &) 
     template void operator/=(Array<Float> &, Float const &) 
1100 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Array<Int> abs(Array<Int> const &) 
     template Array<Int> max(Array<Int> const &, Array<Int> const &) 
     template Array<Int> min(Array<Int> const &, Array<Int> const &) 
     template Array<Int> operator*(Array<Int> const &, Array<Int> const &) 
     template Array<Int> operator*(Array<Int> const &, Int const &) 
     template Array<Int> operator*(Int const &, Array<Int> const &) 
     template Array<Int> operator+(Array<Int> const &, Int const &) 
     template Array<Int> operator+(Array<Int> const &, Array<Int> const &) 
     template Array<Int> operator-(Array<Int> const &) 
     template Array<Int> operator/(Array<Int> const &, Int const &) 
     template Int max(Array<Int> const &) 
     template Int min(Array<Int> const &) 
     template Int product(Array<Int> const &) 
     template Int sum(Array<Int> const &) 
     template void indgen(Array<Int> &) 
     template void indgen(Array<Int> &, Int) 
     template void indgen(Array<Int> &, Int, Int) 
     template void max(Array<Int> &, Array<Int> const &, Array<Int> const &) 
     template void min(Array<Int> &, Array<Int> const &, Array<Int> const &) 
     template void minMax(Int &, Int &, Array<Int> const &) 
     template void operator*=(Array<Int> &, Array<Int> const &) 
     template void operator*=(Array<Int> &, Int const &) 
     template void operator+=(Array<Int> &, Array<Int> const &) 
     template void operator+=(Array<Int> &, Int const &) 
     template void operator-=(Array<Int> &, Array<Int> const &) 
     template void operator-=(Array<Int> &, Int const &) 
     template void operator/=(Array<Int> &, Array<Int> const &) 
     template void operator/=(Array<Int> &, Int const &) 
1110 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Array<uChar> min<uChar>(Array<uChar> const &, Array<uChar> const &) 
     template void min<uChar>(Array<uChar> &, Array<uChar> const &, Array<uChar> const &) 
     template uChar max(Array<uChar> const &) 
     template uChar min(Array<uChar> const &) 
     template void indgen(Array<uChar> &) 
     template void indgen(Array<uChar> &, uChar, uChar) 
     template void minMax(uChar &, uChar &, Array<uChar> const &) 
1120 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Array<uInt> operator*(Array<uInt> const &, uInt const &) 
     template Array<uInt> operator-(Array<uInt> const &, uInt const &) 
     template Array<uInt> operator/(Array<uInt> const &, uInt const &) 
     template uInt max(Array<uInt> const &) 
     template uInt min(Array<uInt> const &) 
     template uInt sum(Array<uInt> const &) 
     template void indgen(Array<uInt> &) 
     template void indgen(Array<uInt> &, uInt) 
     template void indgen(Array<uInt> &, uInt, uInt) 
     template void minMax(uInt &, uInt &, Array<uInt> const &) 
     template void operator*=(Array<uInt> &, uInt const &) 
     template void operator+=(Array<uInt> &, uInt const &) 
     template void operator-=(Array<uInt> &, uInt const &) 
     template void operator/=(Array<uInt> &, uInt const &) 
1130 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Char max(Array<Char> const &) 
     template Char min(Array<Char> const &) 
     template void indgen(Array<Char> &) 
     template void indgen(Array<Char> &, Char, Char) 
     template void minMax(Char &, Char &, Array<Char> const &) 
1140 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Long max(Array<Long> const &) 
     template Long min(Array<Long> const &) 
     template void indgen(Array<Long> &) 
     template void indgen(Array<Long> &, Long, Long) 
     template void minMax(Long &, Long &, Array<Long> const &) 
1150 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template Short max(Array<Short> const &) 
     template Short min(Array<Short> const &) 
     template void indgen(Array<Short> &) 
     template void indgen(Array<Short> &, Short, Short) 
     template void minMax(Short &, Short &, Array<Short> const &) 
     template void operator+=(Array<Short> &, Short const &) 
     template void operator-=(Array<Short> &, Short const &) 
1160 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template uLong max(Array<uLong> const &) 
     template uLong min(Array<uLong> const &) 
     template void indgen(Array<uLong> &) 
     template void indgen(Array<uLong> &, uLong, uLong) 
     template void minMax(uLong &, uLong &, Array<uLong> const &) 
1170 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template uShort max(Array<uShort> const &) 
     template uShort min(Array<uShort> const &) 
     template void indgen(Array<uShort> &) 
     template void indgen(Array<uShort> &, uShort, uShort) 
     template void minMax(uShort &, uShort &, Array<uShort> const &) 
1180 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template void convertArray(Array<Double> &, Array<Double> const &) 
     template void convertArray(Array<Double> &, Array<Float> const &) 
     template void convertArray(Array<Double> &, Array<Int> const &) 
     template void convertArray(Array<Double> &, Array<Short> const &) 
     template void convertArray(Array<Double> &, Array<uChar> const &) 
     template void convertArray(Array<Double> &, Array<uInt> const &) 
     template void convertArray(Array<Double> &, Array<uShort> const &) 
     template void convertArray(Array<Float> &, Array<Double> const &) 
     template void convertArray(Array<Float> &, Array<Int> const &) 
     template void convertArray(Array<Float> &, Array<Short> const &) 
     template void convertArray(Array<Float> &, Array<uShort> const & ) 
     template void convertArray(Array<Float> &, Array<uChar> const &) 
     template void convertArray(Array<Float> &, Array<uInt> const &) 
     template void convertArray(Array<Int> &, Array<Short> const &) 
     template void convertArray(Array<Int> &, Array<uChar> const &) 
     template void convertArray(Array<Int> &, Array<uInt> const &) 
     template void convertArray(Array<Int> &, Array<uShort> const &) 
     template void convertArray(Array<Int> &, Array<Bool> const &) 
     template void convertArray(Array<Int> &, Array<Float> const &) 
     template void convertArray(Array<Int> &, Array<Double> const &) 
     template void convertArray(Array<Short> &, Array<Char> const &) 
     template void convertArray(Array<Short> &, Array<Double> const &) 
     template void convertArray(Array<Short> &, Array<Float> const &) 
     template void convertArray(Array<Short> &, Array<Int> const &) 
     template void convertArray(Array<Short> &, Array<Long> const &) 
     template void convertArray(Array<Short> &, Array<Short> const &) 
     template void convertArray(Array<Short> &, Array<uChar> const &) 
     template void convertArray(Array<Short> &, Array<uInt> const &) 
     template void convertArray(Array<Short> &, Array<uLong> const &) 
     template void convertArray(Array<Short> &, Array<uShort> const &) 
     template void convertArray(Array<uChar> &, Array<Int> const &) 
     template void convertArray(Array<uChar> &, Array<Short> const &) 
     template void convertArray(Array<uChar> &, Array<Float> const &) 
     template void convertArray(Array<uChar> &, Array<Double> const &) 
     template void convertArray(Array<uInt> &, Array<Int> const &) 
     template void convertArray(Array<uInt> &, Array<Short> const &) 
     template void convertArray(Array<uInt> &, Array<uChar> const &) 
     template void convertArray(Array<uInt> &, Array<Float> const &) 
     template void convertArray(Array<uInt> &, Array<Double> const &) 
     template void convertArray(Array<uShort> &, Array<uChar> const &) 
     template void convertArray(Array<uShort> &, Array<Short> const &) 
     template void convertArray(Array<uShort> &, Array<Int> const &) 
     template void convertArray(Array<uShort> &, Array<Float> const &) 
     template void convertArray(Array<uShort> &, Array<Double> const &) 
1190 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template void operator*=(Array<Bool> &, Array<Bool> const &) 
     template Bool sum(Array<Bool> const &) 
1200 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
     template void operator*=(Array<uInt> &, Array<uInt> const &) 
     template void operator+=(Array<uInt> &, Array<uInt> const &) 
     template void operator-=(Array<uInt> &, Array<uInt> const &) 
     template void operator/=(Array<uInt> &, Array<uInt> const &) 
1210 casa/Arrays/ArrayMath.cc casa/BasicSL/Complex.h 
     template Array<Double> partialMins(Array<Double> const &, IPosition const &) 
     template Array<Double> partialMaxs(Array<Double> const &, IPosition const &) 
     template Array<Double> partialSums(Array<Double> const &, IPosition const &) 
     template Array<Double> partialProducts(Array<Double> const &, IPosition const &) 
     template Array<Double> partialMeans(Array<Double> const &, IPosition const &) 
     template Array<Double> partialVariances(Array<Double> const &, IPosition const &, Array<Double> const &) 
     template Array<Double> partialAvdevs(Array<Double> const &, IPosition const &, Array<Double> const &) 
     template Array<Double> partialMedians(Array<Double> const &, IPosition const &, Bool, Bool) 
     template Array<Double> partialFractiles(Array<Double> const &, IPosition const &, Float, Bool) 
     template Array<DComplex> partialSums(Array<DComplex> const &, IPosition const &) 
     template Array<DComplex> partialProducts(Array<DComplex> const &, IPosition const &) 
1000 casa/Arrays/ArrayUtil.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template Array<Bool> concatenateArray(Array<Bool> const &, Array<Bool> const &) 
     template Array<Int> concatenateArray(Array<Int> const &, Array<Int> const &) 
     template Array<uInt> concatenateArray(Array<uInt> const &, Array<uInt> const &) 
     template Array<Float> concatenateArray(Array<Float> const &, Array<Float> const &) 
     template Array<Double> concatenateArray(Array<Double> const &, Array<Double> const &) 
     template Array<Complex> concatenateArray(Array<Complex> const &, Array<Complex> const &) 
     template Array<DComplex> concatenateArray(Array<DComplex> const &, Array<DComplex> const &) 
     template Array<String> concatenateArray(Array<String> const &, Array<String> const &) 
1010 casa/Arrays/ArrayUtil.cc 
     template Array<Double> reorderArray(Array<Double> const &, IPosition const &, Bool) 
1000 casa/Arrays/Cube.cc casa/Arrays/Matrix.h 
     = casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Cube<Matrix<CountedPtr<Random> > > 
1010 casa/Arrays/Cube.cc casa/BasicSL/Complex.h 
     template class Cube<Complex> 
     template class Cube<DComplex> 
1020 casa/Arrays/Cube.cc 
     template class Cube<Bool> 
1030 casa/Arrays/Cube.cc 
     template class Cube<Double> 
1040 casa/Arrays/Cube.cc 
     template class Cube<Float> 
1050 casa/Arrays/Cube.cc 
     template class Cube<Int> 
1060 casa/Arrays/Cube.cc 
     template class Cube<Short> 
1070 casa/Arrays/Cube.cc 
     template class Cube<uInt> 
1000 casa/Arrays/MaskArrMath.cc 
     = casa/Arrays/MaskedArray.h 
     = casa/Arrays/Array.h 
     = casa/BasicSL/Complex.h 
     template Array<Complex> & operator/=(Array<Complex> &, MaskedArray<Complex> const &) 
1010 casa/Arrays/MaskArrMath.cc 
     = casa/Arrays/MaskedArray.h 
     template Float min(MaskedArray<Float> const &) 
     template Float max(MaskedArray<Float> const &) 
     template Double min(MaskedArray<Double> const &) 
     template Double max(MaskedArray<Double> const &) 
     template MaskedArray<Float> abs(MaskedArray<Float> const &) 
1020 casa/Arrays/MaskArrMath.cc 
     = casa/Arrays/MaskedArray.h 
     template Float sum(MaskedArray<Float> const &) 
     template Float mean(MaskedArray<Float> const &) 
1030 casa/Arrays/MaskArrMath.cc casa/BasicSL/Complex.h 
     template Array<Complex> & operator+=(Array<Complex> &, MaskedArray<Complex> const &) 
1040 casa/Arrays/MaskArrMath.cc casa/BasicSL/Complex.h 
     template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, Complex const &) 
1050 casa/Arrays/MaskArrMath.cc casa/BasicSL/Complex.h 
     template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, MaskedArray<Complex> const &) 
1060 casa/Arrays/MaskArrMath.cc casa/BasicSL/Complex.h 
     template const MaskedArray<Complex> & operator/=(MaskedArray<Complex> const &, MaskedArray<Float> const &) 
1070 casa/Arrays/MaskArrMath.cc casa/BasicSL/Complex.h 
     template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, Array<Float> const &) 
     template Array<Float> & operator+=(Array<Float> &, MaskedArray<Float> const &) 
     template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, Array<Complex> const &) 
1080 casa/Arrays/MaskArrMath.cc 
     template Float variance(MaskedArray<Float> const &) 
     template Float variance(MaskedArray<Float> const &, Float) 
     template MaskedArray<Float> operator-(MaskedArray<Float> const &, Float const &) 
     template MaskedArray<Float> operator-(Float const &, MaskedArray<Float> const &) 
     template const MaskedArray<Float> & operator*=(MaskedArray<Float> const &, MaskedArray<Float> const &) 
     template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, Float const &) 
     template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, MaskedArray<Float> const &) 
1090 casa/Arrays/MaskArrMath.cc 
     template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, Float const &) 
1100 casa/Arrays/MaskArrMath.cc 
     template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, MaskedArray<Float> const &) 
     template const MaskedArray<Float> & operator/=(MaskedArray<Float> const &, MaskedArray<Float> const &) 
1110 casa/Arrays/MaskArrMath.cc 
     template const MaskedArray<Int> & operator+=(MaskedArray<Int> const &, Int const &) 
     template MaskedArray<Int> operator-(MaskedArray<Int> const &) 
     template MaskedArray<uInt> operator-(MaskedArray<uInt> const &) 
1120 casa/Arrays/MaskArrMath.cc 
     template void minMax(Float &, Float &, IPosition &, IPosition &, MaskedArray<Float> const &) 
     template void minMax(Float &, Float &, MaskedArray<Float> const &) 
1000 casa/Arrays/MaskedArray.cc 
     = casa/Arrays/Array.h 
     template class MaskedArray<Array<Double> > 
1010 casa/Arrays/MaskedArray.cc 
     = casa/Arrays/Slicer.h 
     template class MaskedArray<Slicer> 
1020 casa/Arrays/MaskedArray.cc 
     = casa/Arrays/Vector.h 
     = casa/BasicSL/Complex.h 
     template class MaskedArray<Vector<Complex> > 
1030 casa/Arrays/MaskedArray.cc 
     = casa/Arrays/Vector.h 
     template class MaskedArray<Vector<Double> > 
1040 casa/Arrays/MaskedArray.cc 
     = casa/Arrays/Vector.h 
     template class MaskedArray<Vector<Float> > 
1050 casa/Arrays/MaskedArray.cc 
     = casa/BasicSL/Complex.h casa/BasicSL/String.h 
     template class MaskedArray<Bool> 
     template class MaskedArray<uChar> 
     template class MaskedArray<Short> 
     template class MaskedArray<uShort> 
     template class MaskedArray<Int> 
     template class MaskedArray<uInt> 
     template class MaskedArray<Float> 
     template class MaskedArray<Double> 
     template class MaskedArray<Complex> 
     template class MaskedArray<DComplex> 
     template class MaskedArray<String> 
1060 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/MVDirection.h 
     template class MaskedArray<MVDirection> 
1070 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/MVFrequency.h 
     template class MaskedArray<MVFrequency> 
1080 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/MVPosition.h 
     template class MaskedArray<MVPosition> 
1090 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/MVRadialVelocity.h 
     template class MaskedArray<MVRadialVelocity> 
1100 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/MVTime.h 
     template class MaskedArray<MVTime> 
1110 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/Quantum.h 
     template class MaskedArray<Quantum<Double> > 
1120 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/Quantum.h 
     template class MaskedArray<Quantum<Float> > 
1130 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/QuantumHolder.h 
     template class MaskedArray<QuantumHolder> 
1140 casa/Arrays/MaskedArray.cc 
     = casa/Quanta/Unit.h 
     template class MaskedArray<Unit> 
1150 casa/Arrays/MaskedArray.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h casa/Arrays/Matrix.h 
     template class MaskedArray<Matrix<CountedPtr<Random> > > 
1160 casa/Arrays/MaskedArray.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class MaskedArray<CountedPtr<Random> > 
1170 casa/Arrays/MaskedArray.cc 
     template class MaskedArray<Char> 
1180 casa/Arrays/MaskedArray.cc 
     template class MaskedArray<Float *> 
1190 casa/Arrays/MaskedArray.cc 
     template class MaskedArray<Long> 
1200 casa/Arrays/MaskedArray.cc 
     template class MaskedArray<uLong> 
1000 casa/Arrays/Matrix.cc 
     = casa/BasicSL/Complex.h 
     template class Matrix<Complex> 
1010 casa/Arrays/Matrix.cc 
     = casa/BasicSL/Complex.h 
     template class Matrix<DComplex> 
1020 casa/Arrays/Matrix.cc 
     = casa/BasicSL/String.h 
     template class Matrix<String> 
1030 casa/Arrays/Matrix.cc 
     = casa/Quanta/MVPosition.h 
     template class Matrix<MVPosition> 
1040 casa/Arrays/Matrix.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Matrix<CountedPtr<Random> > 
1050 casa/Arrays/Matrix.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Matrix<Matrix<CountedPtr<Random> > > 
1060 casa/Arrays/Matrix.cc 
     template class Matrix<Bool> 
1070 casa/Arrays/Matrix.cc 
     template class Matrix<Char> 
1080 casa/Arrays/Matrix.cc 
     template class Matrix<Double> 
1090 casa/Arrays/Matrix.cc 
     template class Matrix<Float> 
1100 casa/Arrays/Matrix.cc 
     template class Matrix<Int> 
1110 casa/Arrays/Matrix.cc 
     template class Matrix<Long> 
1120 casa/Arrays/Matrix.cc 
     template class Matrix<Short> 
1130 casa/Arrays/Matrix.cc 
     template class Matrix<uChar> 
1140 casa/Arrays/Matrix.cc 
     template class Matrix<uInt> 
1150 casa/Arrays/Matrix.cc 
     template class Matrix<uLong> 
1160 casa/Arrays/Matrix.cc 
     template class Matrix<uShort> 
1000 casa/Arrays/MatrixMath.cc casa/BasicSL/Complex.h 
     template Matrix<Complex> product(Matrix<Complex> const &, Matrix<Complex> const &) 
1010 casa/Arrays/MatrixMath.cc casa/BasicSL/Complex.h 
     template Matrix<Complex> transpose(Matrix<Complex> const &) 
1020 casa/Arrays/MatrixMath.cc casa/BasicSL/Complex.h 
     template Matrix<DComplex> transpose(Matrix<DComplex> const &) 
1030 casa/Arrays/MatrixMath.cc 
     template Double innerProduct(Vector<Double> const &, Vector<Double> const &) 
1040 casa/Arrays/MatrixMath.cc 
     template Float innerProduct(Vector<Float> const &, Vector<Float> const &) 
1050 casa/Arrays/MatrixMath.cc 
     template Int innerProduct(Vector<Int> const &, Vector<Int> const &) 
1060 casa/Arrays/MatrixMath.cc 
     template Matrix<Bool> transpose(Matrix<Bool> const &) 
1070 casa/Arrays/MatrixMath.cc 
     template Matrix<Double> transpose(Matrix<Double> const &) 
1080 casa/Arrays/MatrixMath.cc 
     template Matrix<Float> transpose(Matrix<Float> const &) 
1090 casa/Arrays/MatrixMath.cc 
     template Matrix<Int> transpose(Matrix<Int> const &) 
1100 casa/Arrays/MatrixMath.cc 
     template Vector<Double> product(Matrix<Double> const &, Vector<Double> const &) 
1110 casa/Arrays/MatrixMath.cc 
     template Vector<Float> product(Matrix<Float> const &, Vector<Float> const &) 
1000 casa/Arrays/Vector.cc 
     = casa/Arrays/Slicer.h 
     template class Vector<Slicer> 
1010 casa/Arrays/Vector.cc 
     = casa/BasicSL/Complex.h 
     template class Vector<Vector<Complex> > 
1020 casa/Arrays/Vector.cc 
     = casa/Quanta/MVDirection.h 
     template class Vector<MVDirection> 
1030 casa/Arrays/Vector.cc 
     = casa/Quanta/MVFrequency.h 
     template class Vector<MVFrequency> 
1040 casa/Arrays/Vector.cc 
     = casa/Quanta/MVPosition.h 
     template class Vector<MVPosition> 
1050 casa/Arrays/Vector.cc 
     = casa/Quanta/MVRadialVelocity.h 
     template class Vector<MVRadialVelocity> 
1060 casa/Arrays/Vector.cc 
     = casa/Quanta/MVTime.h 
     template class Vector<MVTime> 
1070 casa/Arrays/Vector.cc 
     = casa/Quanta/QuantumHolder.h 
     template class Vector<QuantumHolder> 
1080 casa/Arrays/Vector.cc 
     = casa/Quanta/Unit.h 
     template class Vector<Unit> 
1090 casa/Arrays/Vector.cc casa/Arrays/Array.h 
     template class Vector<Array<Double> > 
1100 casa/Arrays/Vector.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     template class Vector<Bool> 
     template class Vector<uChar> 
     template class Vector<Short> 
     template class Vector<uShort> 
     template class Vector<Int> 
     template class Vector<uInt> 
     template class Vector<Float> 
     template class Vector<Double> 
     template class Vector<Complex> 
     template class Vector<DComplex> 
     template class Vector<String> 
1110 casa/Arrays/Vector.cc casa/Quanta/Quantum.h 
     template class Vector<Quantum<Double> > 
1120 casa/Arrays/Vector.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     = casa/Arrays/Matrix.h 
     template class Vector<Matrix<CountedPtr<Random> > > 
1130 casa/Arrays/Vector.cc casa/Utilities/CountedPtr.h 
     = casa/BasicMath/Random.h 
     template class Vector<CountedPtr<Random> > 
1140 casa/Arrays/Vector.cc 
     template class Vector<Char> 
1150 casa/Arrays/Vector.cc 
     template class Vector<Float *> 
1160 casa/Arrays/Vector.cc 
     template class Vector<Long> 
1170 casa/Arrays/Vector.cc 
     template class Vector<Vector<Double> > 
1180 casa/Arrays/Vector.cc 
     template class Vector<Vector<Float> > 
1190 casa/Arrays/Vector.cc 
     template class Vector<uLong> 
1000 casa/Arrays/Vector2.cc casa/BasicSL/Complex.h 
     = casa/BasicSL/String.h 
     AIPS_VECTOR2_AUX_TEMPLATES(Int) 
     AIPS_VECTOR2_AUX_TEMPLATES(Bool) 
     AIPS_VECTOR2_AUX_TEMPLATES(uChar) 
     AIPS_VECTOR2_AUX_TEMPLATES(Short) 
     AIPS_VECTOR2_AUX_TEMPLATES(uShort) 
     AIPS_VECTOR2_AUX_TEMPLATES(uInt) 
     AIPS_VECTOR2_AUX_TEMPLATES(Float) 
     AIPS_VECTOR2_AUX_TEMPLATES(Double) 
     AIPS_VECTOR2_AUX_TEMPLATES(Complex) 
     AIPS_VECTOR2_AUX_TEMPLATES(DComplex) 
     AIPS_VECTOR2_AUX_TEMPLATES(String) 
1000 casa/Arrays/VectorIter.cc casa/BasicSL/Complex.h 
     template class ReadOnlyVectorIterator<Complex> 
     template class VectorIterator<Complex> 
1010 casa/Arrays/VectorIter.cc casa/BasicSL/Complex.h 
     template class ReadOnlyVectorIterator<DComplex> 
     template class VectorIterator<DComplex> 
1020 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<Bool> 
     template class VectorIterator<Bool> 
1030 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<Double> 
     template class VectorIterator<Double> 
1040 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<Float> 
     template class VectorIterator<Float> 
1050 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<uChar> 
     template class VectorIterator<uChar> 
1060 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<uInt> 
     template class VectorIterator<uInt> 
1070 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<uLong> 
     template class VectorIterator<uLong> 
1080 casa/Arrays/VectorIter.cc 
     template class ReadOnlyVectorIterator<uShort> 
     template class VectorIterator<uShort> 
1090 casa/Arrays/VectorIter.cc 
     template class VectorIterator<Int> 
