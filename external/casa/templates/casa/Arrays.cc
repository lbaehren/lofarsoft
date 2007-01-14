
// include of header files 

#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Random.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Logging/LogIO.h>
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

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayIter.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/ArrayUtil.cc>
#include <casa/Arrays/Cube.cc>
#include <casa/Arrays/MaskArrMath.cc>
#include <casa/Arrays/Matrix.cc>
#include <casa/Arrays/MatrixIter.cc>
#include <casa/Arrays/MatrixMath.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Arrays/Vector2.cc>
#include <casa/Arrays/VectorIter.cc>

namespace casa {
  
  template class Array<Slicer>;
  template class Array<Vector<Complex> >;
  template class Array<Vector<Double> >;
  template class Array<Vector<Float> >;
  template class Array<Complex>;
  template class Array<DComplex>;
  template class Array<String>;
  template class Array<MVDirection>;
  template class Array<MVFrequency>;
  template class Array<MVPosition>;
  template class Array<MVRadialVelocity>;
  template class Array<MVTime>;
  template class Array<Quantum<Double> >;
  template class Array<Quantum<Float> >;
  template class Array<QuantumHolder>;
  template class Array<Unit>;
  template class Array<Matrix<CountedPtr<Random> > >;
  template class Array<CountedPtr<Random> >;
  template class Array<Array<Double> >;
  template class Array<Double>;
  template class Array<Float *>;
  template class Array<Float>;
  template class Array<uLong>;
  template class Array<Long>;
  template class Array<uInt>;
  template class Array<Int>;
  template class Array<uShort>;
  template class Array<Short>;
  template class Array<uChar>;
  template class Array<Char>;
  template class Array<Bool>;

  template class Cube<Matrix<CountedPtr<Random> > >;
  template class Cube<Bool>;
  template class Cube<Int>;
  template class Cube<uInt>;
  template class Cube<Short>;
  template class Cube<uShort>;
  template class Cube<Long>;
  template class Cube<uLong>;
  template class Cube<Float>;
  template class Cube<Double>;
  template class Cube<Complex>;
  template class Cube<DComplex>;

  
#ifdef AIPS_SUN_NATIVE 
  template class Array<Vector<Complex> >::ConstIteratorSTL;
  template class Array<Vector<Double> >::ConstIteratorSTL;
  template class Array<Complex>::ConstIteratorSTL;
  template class Array<DComplex>::ConstIteratorSTL;
  template class Array<String>::ConstIteratorSTL;
  template class Array<MVDirection>::ConstIteratorSTL;
  template class Array<MVFrequency>::ConstIteratorSTL;
  template class Array<MVPosition>::ConstIteratorSTL;
  template class Array<MVRadialVelocity>::ConstIteratorSTL;
  template class Array<MVTime>::ConstIteratorSTL;
  template class Array<Quantum<Double> >::ConstIteratorSTL;
  template class Array<Quantum<Float> >::ConstIteratorSTL;
  template class Array<QuantumHolder>::ConstIteratorSTL 
  template class Array<Unit>::ConstIteratorSTL;
  template class Array<Bool>::ConstIteratorSTL;
  template class Array<Char>::ConstIteratorSTL;
  template class Array<Double>::ConstIteratorSTL;
  template class Array<Float *>::ConstIteratorSTL;
  template class Array<Float>::ConstIteratorSTL;
  template class Array<Int>::ConstIteratorSTL;
  template class Array<Long>::ConstIteratorSTL;
  template class Array<Short>::ConstIteratorSTL;
  template class Array<uChar>::ConstIteratorSTL;
  template class Array<uInt>::ConstIteratorSTL;
  template class Array<uLong>::ConstIteratorSTL;
  template class Array<uShort>::ConstIteratorSTL;
#endif 
  
  template AipsIO & operator<<(AipsIO &, Array<Bool> const &);
  template AipsIO & operator<<(AipsIO &, Array<uChar> const &);
  template AipsIO & operator<<(AipsIO &, Array<Short> const &);
  template AipsIO & operator<<(AipsIO &, Array<Int> const &);
  template AipsIO & operator<<(AipsIO &, Array<uInt> const &);
  template AipsIO & operator<<(AipsIO &, Array<Float> const &);
  template AipsIO & operator<<(AipsIO &, Array<Double> const &);
  template AipsIO & operator<<(AipsIO &, Array<Complex> const &);
  template AipsIO & operator<<(AipsIO &, Array<DComplex> const &);
  template AipsIO & operator<<(AipsIO &, Array<String> const &);
  template void putArray(AipsIO &, Array<Bool> const &, Char const *);
  template void putArray(AipsIO &, Array<uChar> const &, Char const *);
  template void putArray(AipsIO &, Array<Short> const &, Char const *);
  template void putArray(AipsIO &, Array<Int> const &, Char const *);
  template void putArray(AipsIO &, Array<uInt> const &, Char const *);
  template void putArray(AipsIO &, Array<Float> const &, Char const *);
  template void putArray(AipsIO &, Array<Double> const &, Char const *);
  template void putArray(AipsIO &, Array<Complex> const &, Char const *);
  template void putArray(AipsIO &, Array<DComplex> const &, Char const *);
  template void putArray(AipsIO &, Array<String> const &, Char const *);
  template AipsIO & operator>>(AipsIO &, Array<Bool> &);
  template AipsIO & operator>>(AipsIO &, Array<uChar> &);
  template AipsIO & operator>>(AipsIO &, Array<Short> &);
  template AipsIO & operator>>(AipsIO &, Array<Int> &);
  template AipsIO & operator>>(AipsIO &, Array<uInt> &);
  template AipsIO & operator>>(AipsIO &, Array<Float> &);
  template AipsIO & operator>>(AipsIO &, Array<Double> &);
  template AipsIO & operator>>(AipsIO &, Array<Complex> &);
  template AipsIO & operator>>(AipsIO &, Array<DComplex> &);
  template AipsIO & operator>>(AipsIO &, Array<String> &);
  template ostream & operator<<(ostream &, Array<Complex> const &);
  template ostream & operator<<(ostream &, Array<DComplex> const &);
  template LogIO & operator<<(LogIO &, Array<Complex> const &);
  template LogIO & operator<<(LogIO &, Array<DComplex> const &);
  template LogIO & operator<<(LogIO &, Array<String> const &);
  template LogIO & operator<<(LogIO &, Array<Double> const &);
  template LogIO & operator<<(LogIO &, Array<Float> const &);
  template LogIO & operator<<(LogIO &, Array<Int> const &);
  template LogIO & operator<<(LogIO &, Array<uInt> const &);
  template LogIO & operator<<(LogIO &, Array<Short> const &);
  template LogIO & operator<<(LogIO &, Array<uChar> const &);
  template LogIO & operator<<(LogIO &, Array<Bool> const &);
  template ostream & operator<<(ostream &, Array<Quantum<Double> > const &);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<uInt> &, IPosition const *, Bool);
  template istream & operator>>(istream &, Array<String> &);
  template Bool read(istream &, Array<String> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<String> &, IPosition const *, Bool);
  template ostream & operator<<(ostream &, Array<Bool> const &);
  template ostream & operator<<(ostream &, Array<Char> const &);
  template ostream & operator<<(ostream &, Array<Double> const &);
  template ostream & operator<<(ostream &, Array<Float> const &);
  template ostream & operator<<(ostream &, Array<Int> const &);
  template ostream & operator<<(ostream &, Array<Long> const &);
  template ostream & operator<<(ostream &, Array<Short> const &);
  template ostream & operator<<(ostream &, Array<String> const &);
  template ostream & operator<<(ostream &, Array<uChar> const &);
  template ostream & operator<<(ostream &, Array<uInt> const &);
  template ostream & operator<<(ostream &, Array<uLong> const &);
  template ostream & operator<<(ostream &, Array<uShort> const &);
  // test/templates
  template Bool read(istream &, Array<Complex> &, IPosition const *, Bool);
  template Bool read(istream &, Array<Double> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Complex> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Double> &, IPosition const *, Bool);
  template istream & operator>>(istream &, Array<Double> &);
  template void readAsciiMatrix(Matrix<Int> &, Char const *);
  template void readAsciiVector(Vector<Double> &, Char const *);
  template void read_array(Array<Int> &, Char const *);
  template void read_array(Array<Int> &, String const &);
  template void writeAsciiMatrix(Matrix<Int> const &, Char const *);
  template void writeAsciiVector(Vector<Double> const &, Char const *);
  template void write_array(Array<Int> const &, Char const *);
  template void write_array(Array<Int> const &, String const &);
  template istream & operator>>(istream &, Array<Quantum<Double> > &);
  template Bool read(istream &, Array<Quantum<Double> > &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Quantum<Double> > &, IPosition const *, Bool);
  
  template class ArrayIterator<Bool>;
  template class ArrayIterator<uChar>;
  template class ArrayIterator<Short>;
  template class ArrayIterator<uShort>;
  template class ArrayIterator<Int>;
  template class ArrayIterator<uInt>;
  template class ArrayIterator<Float>;
  template class ArrayIterator<Double>;
  template class ArrayIterator<Complex>;
  template class ArrayIterator<DComplex>;
  template class ArrayIterator<String>;
  template class ReadOnlyArrayIterator<Bool>;
  template class ReadOnlyArrayIterator<uChar>;
  template class ReadOnlyArrayIterator<Short>;
  template class ReadOnlyArrayIterator<uShort>;
  template class ReadOnlyArrayIterator<Int>;
  template class ReadOnlyArrayIterator<uInt>;
  template class ReadOnlyArrayIterator<Float>;
  template class ReadOnlyArrayIterator<Double>;
  template class ReadOnlyArrayIterator<Complex>;
  template class ReadOnlyArrayIterator<DComplex>;
  template class ReadOnlyArrayIterator<String>;
  template class ArrayIterator<Char>;
  template class ArrayIterator<uLong>;
  template class ReadOnlyArrayIterator<Char>;

  template class MatrixIterator<Bool>;
  template class MatrixIterator<uChar>;
  template class MatrixIterator<Short>;
  template class MatrixIterator<uShort>;
  template class MatrixIterator<Int>;
  template class MatrixIterator<uInt>;
  template class MatrixIterator<Float>;
  template class MatrixIterator<Double>;
  template class MatrixIterator<Complex>;
  template class MatrixIterator<DComplex>;
  template class MatrixIterator<String>;

  // ------------------------------------------------------------------ operator<

  template Array<Bool> operator<(Int const &, Array<Int> const &);
  template Array<Bool> operator<(uInt const &, Array<uInt> const &);
  template Array<Bool> operator<(Short const &, Array<Short> const &);
  template Array<Bool> operator<(Long const &, Array<Long> const &);
  template Array<Bool> operator<(Float const &, Array<Float> const &);
  template Array<Bool> operator<(Double const &, Array<Double> const &);
  template Array<Bool> operator<(Complex const &, Array<Complex> const &);
  template Array<Bool> operator<(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator<(Array<Int> const &, Int const &);
  template Array<Bool> operator<(Array<uInt> const &, uInt const &);
  template Array<Bool> operator<(Array<Short> const &, Short const &);
  template Array<Bool> operator<(Array<Long> const &, Long const &);
  template Array<Bool> operator<(Array<Float> const &, Float const &);
  template Array<Bool> operator<(Array<Double> const &, Double const &);
  template Array<Bool> operator<(Array<Complex> const &, Complex const &);
  template Array<Bool> operator<(Array<DComplex> const &, DComplex const &);

  template Array<Bool> operator<(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator<(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator<(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator<(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator<(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator<(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator<(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator<(Array<DComplex> const &, Array<DComplex> const &);
  
  // ----------------------------------------------------------------- operator<=

  template Array<Bool> operator<=(Int const &, Array<Int> const &);
  template Array<Bool> operator<=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator<=(Short const &, Array<Short> const &);
  template Array<Bool> operator<=(Long const &, Array<Long> const &);
  template Array<Bool> operator<=(Float const &, Array<Float> const &);
  template Array<Bool> operator<=(Double const &, Array<Double> const &);
  template Array<Bool> operator<=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator<=(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator<=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator<=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator<=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator<=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator<=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator<=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator<=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator<=(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator==(Int const &, Array<Int> const &);
  template Array<Bool> operator==(uInt const &, Array<uInt> const &);
  template Array<Bool> operator==(Short const &, Array<Short> const &);
  template Array<Bool> operator==(Long const &, Array<Long> const &);
  template Array<Bool> operator==(Float const &, Array<Float> const &);
  template Array<Bool> operator==(Double const &, Array<Double> const &);
  template Array<Bool> operator==(Complex const &, Array<Complex> const &);
  template Array<Bool> operator==(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator==(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator==(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator==(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator==(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator==(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator==(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator==(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator==(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator>(Int const &, Array<Int> const &);
  template Array<Bool> operator>(uInt const &, Array<uInt> const &);
  template Array<Bool> operator>(Short const &, Array<Short> const &);
  template Array<Bool> operator>(Long const &, Array<Long> const &);
  template Array<Bool> operator>(Float const &, Array<Float> const &);
  template Array<Bool> operator>(Double const &, Array<Double> const &);
  template Array<Bool> operator>(Complex const &, Array<Complex> const &);
  template Array<Bool> operator>(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator>(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator>(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator>(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator>(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator>(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator>(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator>(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator>(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator>=(Int const &, Array<Int> const &);
  template Array<Bool> operator>=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator>=(Short const &, Array<Short> const &);
  template Array<Bool> operator>=(Long const &, Array<Long> const &);
  template Array<Bool> operator>=(Float const &, Array<Float> const &);
  template Array<Bool> operator>=(Double const &, Array<Double> const &);
  template Array<Bool> operator>=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator>=(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator>=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator>=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator>=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator>=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator>=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator>=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator>=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator>=(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator!=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator!=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator!=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator!=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator!=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator!=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator!=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator!=(Array<DComplex> const &, Array<DComplex> const &);

  template Array<Bool> operator!=(Int const &, Array<Int> const &);
  template Array<Bool> operator!=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator!=(Short const &, Array<Short> const &);
  template Array<Bool> operator!=(Long const &, Array<Long> const &);
  template Array<Bool> operator!=(Float const &, Array<Float> const &);
  template Array<Bool> operator!=(Double const &, Array<Double> const &);
  template Array<Bool> operator!=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator!=(DComplex const &, Array<DComplex> const &);
  
  template Array<Bool> operator!=(Array<Complex> const &, Complex const &);
  template Array<Bool> operator<=(Array<Complex> const &, Complex const &);
  template Array<Bool> operator==(Array<Complex> const &, Complex const &);
  template Array<Bool> operator>(Array<Complex> const &, Complex const &);
  template Array<Bool> operator>=(Array<Complex> const &, Complex const &);
  template Bool allNear(Array<Complex> const &, Complex const &, Double);
  template LogicalArray operator!=(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator==(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator>(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator>=(Array<DComplex> const &, DComplex const &);
  template LogicalArray near(Array<DComplex> const &, Array<DComplex> const &, Double);
  template LogicalArray near(Array<DComplex> const &, DComplex const &, Double);
  template LogicalArray near(DComplex const &, Array<DComplex> const &, Double);
  template LogicalArray nearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);
  template LogicalArray nearAbs(Array<DComplex> const &, DComplex const &, Double);
  template LogicalArray nearAbs(DComplex const &, Array<DComplex> const &, Double);
  template LogicalArray operator!=(Array<String> const &, Array<String> const &);
  template LogicalArray operator!=(Array<String> const &, String const &);
  template LogicalArray operator!=(String const &, Array<String> const &);
  template LogicalArray operator==(Array<String> const &, Array<String> const &);
  template LogicalArray operator==(Array<String> const &, String const &);
  template LogicalArray operator==(String const &, Array<String> const &);
  template LogicalArray operator>(Array<String> const &, Array<String> const &);
  template LogicalArray operator>(Array<String> const &, String const &);
  template LogicalArray operator>(String const &, Array<String> const &);
  template LogicalArray operator>=(Array<String> const &, Array<String> const &);
  template LogicalArray operator>=(Array<String> const &, String const &);
  template LogicalArray operator>=(String const &, Array<String> const &);
  template Bool anyEQ(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator==(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator!=(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator!=(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator!=(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator>(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator>(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator>(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator>=(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator>=(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator>=(MVTime const &, Array<MVTime> const &);
  template Bool allNear(Array<Double> const &, Double const &, Double);
  template LogicalArray operator!=(Array<Double> const &, Double const &);
  template LogicalArray operator==(Array<Double> const &, Double const &);
  template LogicalArray operator>(Array<Double> const &, Double const &);
  template LogicalArray operator>=(Array<Double> const &, Double const &);
  template LogicalArray operator<=(Array<Double> const &, Double const &);
  template LogicalArray near(Array<Double> const &, Array<Double> const &, Double);
  template LogicalArray near(Array<Double> const &, Double const &, Double);
  template LogicalArray near(Double const &, Array<Double> const &, Double);
  template LogicalArray nearAbs(Array<Double> const &, Array<Double> const &, Double);
  template LogicalArray nearAbs(Array<Double> const &, Double const &, Double);
  template LogicalArray nearAbs(Double const &, Array<Double> const &, Double);
  template Array<Bool> operator!=(Array<Int> const &, Int const &);
  template Array<Bool> operator==(Array<Int> const &, Int const &);
  template Array<Bool> operator>(Array<Int> const &, Int const &);
  template Array<Bool> operator>=(Array<Int> const &, Int const &);
  template Array<Bool> operator<=(Array<Int> const &, Int const &);
  template Array<Bool> operator==(Array<Short> const &, Short const &);
  template Array<Bool> operator>(Array<Short> const &, Short const &);
  template Bool allEQ(Array<uChar> const &, Array<uChar> const &);
  template Bool allEQ(Array<uChar> const &, uChar const &);
  template Array<Bool> operator==(Array<uChar> const &, uChar const &);
  template Array<Bool> operator>(Array<uChar> const &, uChar const &);
  template Array<Bool> operator!=(Array<uInt> const &, uInt const &);
  template Array<Bool> operator==(Array<uInt> const &, uInt const &);
  template Array<Bool> operator>(Array<uInt> const &, uInt const &);
  template Array<Bool> operator||(Array<uInt> const &, Array<uInt> const &);
  template Bool allNear(Array<Float> const &, Float const &, Double);
  template LogicalArray operator!=(Array<Float> const &, Float const &);
  template LogicalArray operator<=(Array<Float> const &, Float const &);
  template LogicalArray operator==(Array<Float> const &, Float const &);
  template LogicalArray operator>(Array<Float> const &, Float const &);
  template LogicalArray operator>=(Array<Float> const &, Float const &);
  template Bool allAND(Array<Bool> const &, Bool const &);
  template Bool allOR(Array<Bool> const &, Array<Bool> const &);
  template uInt ntrue(Array<Bool> const &);
  template uInt nfalse(Array<Bool> const &);
  template LogicalArray operator!(Array<Bool> const &);
  template LogicalArray operator!=(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator!=(Array<Bool> const &, Bool const &);
  template LogicalArray operator!=(Bool const &, Array<Bool> const &);
  template LogicalArray operator&&(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator&&(Array<Bool> const &, Bool const &);
  template LogicalArray operator&&(Bool const &, Array<Bool> const &);
  template LogicalArray operator==(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator==(Array<Bool> const &, Bool const &);
  template LogicalArray operator==(Bool const &, Array<Bool> const &);
  template LogicalArray operator||(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator||(Array<Bool> const &, Bool const &);
  template LogicalArray operator||(Bool const &, Array<Bool> const &);
  template Array<uInt> partialNTrue(Array<Bool> const &, IPosition const &);
  template Array<uInt> partialNFalse(Array<Bool> const &, IPosition const &);
  
  template Array<Float> pow(Float const &, Array<Float> const &);
  template Array<Double> pow(Double const &, Array<Double> const &);
  template Array<Complex> pow(Complex const &, Array<Complex> const &);
  template Array<DComplex> pow(DComplex const &, Array<DComplex> const &);

  template Array<Float> pow(Array<Float> const &, Array<Float> const &);
  template Array<Double> pow(Array<Double> const &, Array<Double> const &);
  template Array<Complex> pow(Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> pow(Array<DComplex> const &, Array<DComplex> const &);

  template Array<Float> pow(Array<Float> const &, Double const &);
  template Array<Double> pow(Array<Double> const &, Double const &);
  template Array<Complex> pow(Array<Complex> const &, Double const &);
  template Array<DComplex> pow(Array<DComplex> const &, Double const &);

  template void convertArray(Array<Double> &, Array<MVTime> const &);
  template Array<Complex> abs(Array<Complex> const &);
  template Array<Complex> cos(Array<Complex> const &);
  template Array<Complex> cosh(Array<Complex> const &);
  template Array<Complex> exp(Array<Complex> const &);
  template Array<Complex> log(Array<Complex> const &);
  template Array<Complex> log10(Array<Complex> const &);
  template Array<Complex> operator*(Array<Complex> const &, Complex const &);
  template Array<Complex> operator+(Array<Complex> const &, Array<Complex> const &);
  template Array<Complex> operator-(Array<Complex> const &);
  template Array<Complex> operator-(Complex const &, Array<Complex> const &);
  template Array<Complex> operator-(Array<Complex> const &, Array<Complex> const &);
  template Array<Complex> operator/(Array<Complex> const &, Complex const &);
  template Array<Complex> operator/(Complex const &, Array<Complex> const &);
  template Array<Complex> sin(Array<Complex> const &);
  template Array<Complex> sinh(Array<Complex> const &);
  template Array<Complex> sqrt(Array<Complex> const &);
  template void minMax(Complex &, Complex &, Array<Complex> const &);
  template void operator*=(Array<Complex> &, Array<Complex> const &);
  template void operator*=(Array<Complex> &, Complex const &);
  template void operator+=(Array<Complex> &, Array<Complex> const &);
  template void operator+=(Array<Complex> &, Complex const &);
  template void operator-=(Array<Complex> &, Array<Complex> const &);
  template void operator-=(Array<Complex> &, Complex const &);
  template void operator/=(Array<Complex> &, Array<Complex> const &);
  template void operator/=(Array<Complex> &, Complex const &);
  template Array<DComplex> abs(Array<DComplex> const &);
  template Array<DComplex> cos(Array<DComplex> const &);
  template Array<DComplex> cosh(Array<DComplex> const &);
  template Array<DComplex> exp(Array<DComplex> const &);
  template Array<DComplex> log(Array<DComplex> const &);
  template Array<DComplex> log10(Array<DComplex> const &);
  template Array<DComplex> operator*(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator*(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator*(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator+(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator+(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator+(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator-(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator/(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator/(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator/(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> sin(Array<DComplex> const &);
  template Array<DComplex> sinh(Array<DComplex> const &);
  template Array<DComplex> sqrt(Array<DComplex> const &);
  template DComplex sum(Array<DComplex> const &);
  template Complex sum(Array<Complex> const &);
  template void minMax(DComplex &, DComplex &, Array<DComplex> const &);
  template void operator*=(Array<DComplex> &, Array<DComplex> const &);
  template void operator*=(Array<DComplex> &, DComplex const &);
  template void operator+=(Array<DComplex> &, Array<DComplex> const &);
  template void operator+=(Array<DComplex> &, DComplex const &);
  template void operator-=(Array<DComplex> &, Array<DComplex> const &);
  template void operator-=(Array<DComplex> &, DComplex const &);
  template void operator/=(Array<DComplex> &, Array<DComplex> const &);
  template void operator/=(Array<DComplex> &, DComplex const &);
  template DComplex mean(Array<DComplex> const &);
  template DComplex median(Array<DComplex> const &, Bool);
  template DComplex median(Array<DComplex> const &, Bool, Bool, Bool);
  template Array<Bool> operator*(Array<Bool> const &, Array<Bool> const &);
  template void convertArray(Array<uInt> &, Array<Bool> const &);
  template Array<String> operator+(String const &, Array<String> const &);
  template void operator+=(Array<String> &, Array<String> const &);
  template void operator+=(Array<String> &, String const &);
  template Array<Double> abs(Array<Double> const &);
  template Array<Double> acos(Array<Double> const &);
  template Array<Double> asin(Array<Double> const &);
  template Array<Double> atan(Array<Double> const &);
  template Array<Double> atan2(Array<Double> const &, Array<Double> const &);
  template Array<Double> ceil(Array<Double> const &);
  template Array<Double> cos(Array<Double> const &);
  template Array<Double> cosh(Array<Double> const &);
  template Array<Double> exp(Array<Double> const &);
  template Array<Double> fabs(Array<Double> const &);
  template Array<Double> floor(Array<Double> const &);
  template Array<Double> fmod(Array<Double> const &, Array<Double> const &);
  template Array<Double> fmod(Array<Double> const &, Double const &);
  template Array<Double> fmod(Double const &, Array<Double> const &);
  template Array<Double> log(Array<Double> const &);
  template Array<Double> log10(Array<Double> const &);
  template Array<Double> operator*(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator*(Array<Double> const &, Double const &);
  template Array<Double> operator*(Double const &, Array<Double> const &);
  template Array<Double> operator+(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator+(Array<Double> const &, Double const &);
  template Array<Double> operator+(Double const &, Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &, Double const &);
  template Array<Double> operator-(Double const &, Array<Double> const &);
  template Array<Double> operator/(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator/(Array<Double> const &, Double const &);
  template Array<Double> operator/(Double const &, Array<Double> const &);
  template Array<Double> sin(Array<Double> const &);
  template Array<Double> sinh(Array<Double> const &);
  template Array<Double> sqrt(Array<Double> const &);
  template Array<Double> tan(Array<Double> const &);
  template Array<Double> tanh(Array<Double> const &);
  template Double avdev(Array<Double> const &);
  template Double avdev(Array<Double> const &, Double);
  template Double fractile(Array<Double> const &, Float, Bool, Bool);
  template Double mean(Array<Double> const &);
  template Double median(Array<Double> const &, Bool);
  template Double median(Array<Double> const &, Bool, Bool, Bool);
  template Double stddev(Array<Double> const &);
  template Double sum(Array<Double> const &);
  template Double variance(Array<Double> const &);
  template Double variance(Array<Double> const &, Double);
  template void indgen(Array<Double> &);
  template void indgen(Array<Double> &, Double);
  template void indgen(Array<Double> &, Double, Double);
  template void minMax(Double &, Double &, Array<Double> const &);
  template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &);
  template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &, Array<Bool> const &);
  template void operator*=(Array<Double> &, Array<Double> const &);
  template void operator*=(Array<Double> &, Double const &);
  template void operator+=(Array<Double> &, Array<Double> const &);
  template void operator+=(Array<Double> &, Double const &);
  template void operator-=(Array<Double> &, Array<Double> const &);
  template void operator-=(Array<Double> &, Double const &);
  template void operator/=(Array<Double> &, Array<Double> const &);
  template void operator/=(Array<Double> &, Double const &);
  template Array<Float> abs(Array<Float> const &);
  template Array<Float> acos(Array<Float> const &);
  template Array<Float> asin(Array<Float> const &);
  template Array<Float> atan(Array<Float> const &);
  template Array<Float> atan2(Array<Float> const &, Array<Float> const &);
  template Array<Float> ceil(Array<Float> const &);
  template Array<Float> cos(Array<Float> const &);
  template Array<Float> cosh(Array<Float> const &);
  template Array<Float> exp(Array<Float> const &);
  template Array<Float> floor(Array<Float> const &);
  template Array<Float> fmod(Array<Float> const &, Array<Float> const &);
  template Array<Float> log(Array<Float> const &);
  template Array<Float> log10(Array<Float> const &);
  template Array<Float> operator*(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator*(Array<Float> const &, Float const &);
  template Array<Float> operator*(Float const &, Array<Float> const &);
  template Array<Float> operator+(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator+(Array<Float> const &, Float const &);
  template Array<Float> operator-(Array<Float> const &);
  template Array<Float> operator-(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator-(Array<Float> const &, Float const &);
  template Array<Float> operator-(Float const &, Array<Float> const &);
  template Array<Float> operator/(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator/(Array<Float> const &, Float const &);
  template Array<Float> operator/(Float const &, Array<Float> const &);
  template Array<Float> sin(Array<Float> const &);
  template Array<Float> sinh(Array<Float> const &);
  template Array<Float> sqrt(Array<Float> const &);
  template Array<Float> tan(Array<Float> const &);
  template Array<Float> tanh(Array<Float> const &);
  template Float fractile(Array<Float> const &, Float, Bool, Bool);
  template Float mean(Array<Float> const &);
  template Float median(Array<Float> const &, Bool);
  template Float median(Array<Float> const &, Bool, Bool, Bool);
  template Float stddev(Array<Float> const &);
  template Float sum(Array<Float> const &);
  template Float variance(Array<Float> const &);
  template Float variance(Array<Float> const &, Float);
  template void indgen(Array<Float> &);
  template void indgen(Array<Float> &, Float);
  template void indgen(Array<Float> &, Float, Float);
  template void minMax(Float &, Float &, Array<Float> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Bool> const &);
  template void minMaxMasked(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Float> const &);
  template void operator*=(Array<Float> &, Array<Float> const &);
  template void operator*=(Array<Float> &, Float const &);
  template void operator+=(Array<Float> &, Array<Float> const &);
  template void operator+=(Array<Float> &, Float const &);
  template void operator-=(Array<Float> &, Array<Float> const &);
  template void operator-=(Array<Float> &, Float const &);
  template void operator/=(Array<Float> &, Array<Float> const &);
  template void operator/=(Array<Float> &, Float const &);
  template Array<Int> abs(Array<Int> const &);
  template Array<Int> operator*(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator*(Array<Int> const &, Int const &);
  template Array<Int> operator*(Int const &, Array<Int> const &);
  template Array<Int> operator+(Array<Int> const &, Int const &);
  template Array<Int> operator+(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &);
  template Array<Int> operator/(Array<Int> const &, Int const &);
  template Int sum(Array<Int> const &);
  template void indgen(Array<Int> &);
  template void indgen(Array<Int> &, Int);
  template void indgen(Array<Int> &, Int, Int);
  template void minMax(Int &, Int &, Array<Int> const &);
  template void operator*=(Array<Int> &, Array<Int> const &);
  template void operator*=(Array<Int> &, Int const &);
  template void operator+=(Array<Int> &, Array<Int> const &);
  template void operator+=(Array<Int> &, Int const &);
  template void operator-=(Array<Int> &, Array<Int> const &);
  template void operator-=(Array<Int> &, Int const &);
  template void operator/=(Array<Int> &, Array<Int> const &);
  template void operator/=(Array<Int> &, Int const &);
  template Array<uChar> min<uChar>(Array<uChar> const &, Array<uChar> const &);
  template void min<uChar>(Array<uChar> &, Array<uChar> const &, Array<uChar> const &);
  template void indgen(Array<uChar> &);
  template void indgen(Array<uChar> &, uChar, uChar);
  template void minMax(uChar &, uChar &, Array<uChar> const &);
  template Array<uInt> operator*(Array<uInt> const &, uInt const &);
  template Array<uInt> operator-(Array<uInt> const &, uInt const &);
  template Array<uInt> operator/(Array<uInt> const &, uInt const &);
  template uInt sum(Array<uInt> const &);
  template void indgen(Array<uInt> &);
  template void indgen(Array<uInt> &, uInt);
  template void indgen(Array<uInt> &, uInt, uInt);
  template void minMax(uInt &, uInt &, Array<uInt> const &);
  template void operator*=(Array<uInt> &, uInt const &);
  template void operator+=(Array<uInt> &, uInt const &);
  template void operator-=(Array<uInt> &, uInt const &);
  template void operator/=(Array<uInt> &, uInt const &);
// 1130 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void indgen(Array<Char> &);
  template void indgen(Array<Char> &, Char, Char);
  template void minMax(Char &, Char &, Array<Char> const &);
// 1140 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void indgen(Array<Long> &);
  template void indgen(Array<Long> &, Long, Long);
  template void minMax(Long &, Long &, Array<Long> const &);
// 1150 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void indgen(Array<Short> &);
  template void indgen(Array<Short> &, Short, Short);
  template void minMax(Short &, Short &, Array<Short> const &);
  template void operator+=(Array<Short> &, Short const &);
  template void operator-=(Array<Short> &, Short const &);
// 1160 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void indgen(Array<uLong> &);
  template void indgen(Array<uLong> &, uLong, uLong);
  template void minMax(uLong &, uLong &, Array<uLong> const &);
// 1170 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void indgen(Array<uShort> &);
  template void indgen(Array<uShort> &, uShort, uShort);
  template void minMax(uShort &, uShort &, Array<uShort> const &);
// 1190 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void operator*=(Array<Bool> &, Array<Bool> const &);
  template Bool sum(Array<Bool> const &);
// 1200 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void operator*=(Array<uInt> &, Array<uInt> const &);
  template void operator+=(Array<uInt> &, Array<uInt> const &);
  template void operator-=(Array<uInt> &, Array<uInt> const &);
  template void operator/=(Array<uInt> &, Array<uInt> const &);
// 1210 casa/Arrays/ArrayMath.cc casa/BasicSL/Complex.h 
  template Array<Double> partialMins(Array<Double> const &, IPosition const &);
  template Array<Double> partialMaxs(Array<Double> const &, IPosition const&);
  template Array<Double> partialSums(Array<Double> const &, IPosition const &);
  template Array<Double> partialProducts(Array<Double> const &, IPosition const &);
  template Array<Double> partialMeans(Array<Double> const &, IPosition const &);
  template Array<Double> partialVariances(Array<Double> const &, IPosition const &, Array<Double> const &);
  template Array<Double> partialAvdevs(Array<Double> const &, IPosition const &, Array<Double> const &);
  template Array<Double> partialMedians(Array<Double> const &, IPosition const &, Bool, Bool);
  template Array<Double> partialFractiles(Array<Double> const &, IPosition const &, Float, Bool);
  template Array<DComplex> partialSums(Array<DComplex> const &, IPosition const &);
  template Array<DComplex> partialProducts(Array<DComplex> const &, IPosition const &);

  template Array<Bool> concatenateArray(Array<Bool> const &, Array<Bool> const &);
  template Array<Int> concatenateArray(Array<Int> const &, Array<Int> const &);
  template Array<uInt> concatenateArray(Array<uInt> const &, Array<uInt> const &);
  template Array<Float> concatenateArray(Array<Float> const &, Array<Float> const &);
  template Array<Double> concatenateArray(Array<Double> const &, Array<Double> const &);
  template Array<Complex> concatenateArray(Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> concatenateArray(Array<DComplex> const &, Array<DComplex> const &);
  template Array<String> concatenateArray(Array<String> const &, Array<String> const &);

  template Array<uInt> reorderArray(Array<uInt> const &, IPosition const &, Bool);
  template Array<Int> reorderArray(Array<Int> const &, IPosition const &, Bool);
  template Array<Float> reorderArray(Array<Float> const &, IPosition const &, Bool);
  template Array<Double> reorderArray(Array<Double> const &, IPosition const &, Bool);
  template Array<Complex> reorderArray(Array<Complex> const &, IPosition const &, Bool);
  template Array<DComplex> reorderArray(Array<DComplex> const &, IPosition const &, Bool);

  // test/templates
  template Array<Int> cube(Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &, Int const &);
  template Array<Int> square(Array<Int> const &);
  template Int mean(Array<Int> const &);
  template Int median(Array<Int> const &, Bool);
  template Int median(Array<Int> const &, Bool, Bool, Bool);

  template class Matrix<Complex>;
  template class Matrix<DComplex>;
  template class Matrix<String>;
  template class Matrix<MVPosition>;
  template class Matrix<CountedPtr<Random> >;
  template class Matrix<Matrix<CountedPtr<Random> > >;
  template class Matrix<Bool>;
  template class Matrix<Char>;
  template class Matrix<Double>;
  template class Matrix<Float>;
  template class Matrix<Int>;
  template class Matrix<Long>;
  template class Matrix<Short>;
  template class Matrix<uChar>;
  template class Matrix<uInt>;
  template class Matrix<uLong>;
  template class Matrix<uShort>;

  template class Vector<Slicer>;
  template class Vector<Vector<Complex> >;
  template class Vector<MVDirection>;
  template class Vector<MVFrequency>;
  template class Vector<MVPosition>;
  template class Vector<MVRadialVelocity>;
  template class Vector<MVTime>;
  template class Vector<QuantumHolder>;
  template class Vector<Unit>;
  template class Vector<Array<Double> >;
  template class Vector<Bool>;
  template class Vector<uChar>;
  template class Vector<Short>;
  template class Vector<uShort>;
  template class Vector<Int>;
  template class Vector<uInt>;
  template class Vector<Float>;
  template class Vector<Double>;
  template class Vector<Complex>;
  template class Vector<DComplex>;
  template class Vector<String>;
  template class Vector<Quantum<Double> >;
  template class Vector<Matrix<CountedPtr<Random> > >;
  template class Vector<CountedPtr<Random> >;
  template class Vector<Char>;
  template class Vector<Float *>;
  template class Vector<Long>;
  template class Vector<Vector<Double> >;
  template class Vector<Vector<Float> >;
  template class Vector<uLong>;

  AIPS_VECTOR2_AUX_TEMPLATES(Int);
  AIPS_VECTOR2_AUX_TEMPLATES(Bool);
  AIPS_VECTOR2_AUX_TEMPLATES(uChar);
  AIPS_VECTOR2_AUX_TEMPLATES(Short);
  AIPS_VECTOR2_AUX_TEMPLATES(uShort);
  AIPS_VECTOR2_AUX_TEMPLATES(uInt);
  AIPS_VECTOR2_AUX_TEMPLATES(Float);
  AIPS_VECTOR2_AUX_TEMPLATES(Double);
  AIPS_VECTOR2_AUX_TEMPLATES(Complex);
  AIPS_VECTOR2_AUX_TEMPLATES(DComplex);
  AIPS_VECTOR2_AUX_TEMPLATES(String);

  template class ReadOnlyVectorIterator<Complex>;
  template class VectorIterator<Complex>;
  template class ReadOnlyVectorIterator<DComplex>;
  template class VectorIterator<DComplex>;
  template class ReadOnlyVectorIterator<Bool>;
  template class VectorIterator<Bool>;
  template class ReadOnlyVectorIterator<Double>;
  template class VectorIterator<Double>;
  template class ReadOnlyVectorIterator<Float>;
  template class VectorIterator<Float>;
  template class ReadOnlyVectorIterator<uChar>;
  template class VectorIterator<uChar>;
  template class ReadOnlyVectorIterator<uInt>;
  template class VectorIterator<uInt>;
  template class ReadOnlyVectorIterator<uLong>;
  template class VectorIterator<uLong>;
  template class ReadOnlyVectorIterator<uShort>;
  template class VectorIterator<uShort>;
  template class VectorIterator<Int>;

}
