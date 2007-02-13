
// include of header files

#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <tables/TablePlot/BasePlot.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableIter.h>
#include <tables/Tables/TableRecord.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/Cube.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>

namespace casa {

  template class Array<IPosition>;
  template class Array<BasePlot<Float> >;
  template class Array<TableExprNode>;
  template class Array<Table>;
  template class Array<TableIterator>; 
  template class Array<TableRecord>;

#ifdef AIPS_SUN_NATIVE 
  template class Array<TableRecord>::ConstIteratorSTL;
#endif 

  template class MaskedArray<IPosition>;
  template class MaskedArray<BasePlot<Float> >;
  template class MaskedArray<TableExprNode>;
  template class MaskedArray<Table>;
  template class MaskedArray<TableIterator>;
  template class MaskedArray<TableRecord>;

  template class Vector<IPosition>;
  template class Vector<BasePlot<Float> >;
  template class Vector<TableExprNode>;
  template class Vector<Table>;
  template class Vector<TableIterator>;
  template class Vector<TableRecord>;

  // -- casa/Arrays/ArrayLogical.cc
//   template LogicalArray operator<(Array<Double> const &, Array<Double> const &);
//   template LogicalArray operator<(Double const &, Array<Double> const &);
//   template LogicalArray operator<=(Array<Double> const &, Array<Double> const &);
//   template LogicalArray operator<=(Double const &, Array<Double> const &);
//   template LogicalArray operator<(Array<DComplex> const &, Array<DComplex> const &);
//   template LogicalArray operator<(Array<DComplex> const &, DComplex const &);
//   template LogicalArray operator<(DComplex const &, Array<DComplex> const &);
//   template LogicalArray operator<=(Array<DComplex> const &, Array<DComplex> const &);
  template LogicalArray operator<=(Array<DComplex> const &, DComplex const &);
//   template LogicalArray operator<=(DComplex const &, Array<DComplex> const &);
  template LogicalArray operator<(Array<String> const &, Array<String> const &);
  template LogicalArray operator<(Array<String> const &, String const &);
  template LogicalArray operator<(String const &, Array<String> const &);
  template LogicalArray operator<=(Array<String> const &, Array<String> const &);
  template LogicalArray operator<=(Array<String> const &, String const &);
  template LogicalArray operator<=(String const &, Array<String> const &);
//   template Bool allNear(Array<DComplex> const &, Array<DComplex> const &, Double);

  // -- casa/Arrays/ArrayLogical.cc
//   template Bool allEQ(Array<Complex> const &, Complex const &);
//   template Bool allEQ(Array<DComplex> const &, DComplex const &);
//   template Bool allEQ(Array<Short> const &, Short const &);
//   template Bool allEQ(Array<uShort> const &, Array<uShort> const &);
//   template Bool allNear(Array<Complex> const &, Array<Complex> const &, Double);
//   template LogicalArray operator<(Array<Double> const &, Double const &);

  // -- casa/Arrays/ArrayMath.cc
  template void indgen(Array<Complex> &);
  template void indgen(Array<Complex> &, Complex, Complex);
  template void operator+=(Array<uChar> &, uChar const &);
  template void operator+=(Array<uShort> &, uShort const &);
  template Array<Complex> operator+(Array<Complex> const &, Complex const &);
  template Array<String> operator+(Array<String> const &, String const &);
  template Double stddev(Array<Double> const &, Double);
  template Array<Float> fmod(Array<Float> const &, Float const &);
  template void indgen(Array<DComplex> &);
  template void indgen(Array<DComplex> &, DComplex, DComplex);
//   template void convertArray(Array<uShort> &, Array<Float> const &);

  // -- casa/Arrays/Cube.cc 
//   template class Cube<uChar>;
//   template class Cube<uShort>;

}
