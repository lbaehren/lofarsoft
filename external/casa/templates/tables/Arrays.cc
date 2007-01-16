
// include of header files

#include <casa/Arrays/IPosition.h>
#include <tables/TablePlot/BasePlot.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableIter.h>
#include <tables/Tables/TableRecord.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>

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

}
