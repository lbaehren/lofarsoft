
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/List.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/TableRecordRep.h>
#include <tables/TablePlot/BasePlot.h>
#include <tables/Tables/ExprNode.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableIter.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TiledFileAccess.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableRecordRep.h>
#include <tables/Tables/TableParse.h>
#include <tables/Tables/DataManError.h>

#include <casa/Utilities/Assert.cc>
#include <casa/Utilities/BinarySearch.cc>
#include <casa/Utilities/COWPtr.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/Register.cc>
#include <casa/Utilities/GenSort.cc>

namespace casa {

  template Int binarySearch(Bool &, ROScalarColumn<Double> const &, Double const &, uInt, Int);
  template Int binarySearch(Bool &, ROScalarColumn<Int> const &, Int const &, uInt, Int);

  template class COWPtr<TableRecordRep>;
  
  template class CountedConstPtr<Block<IPosition> >;
  template class CountedPtr<Block<IPosition> >;
  template class PtrRep<Block<IPosition> >;
  template class SimpleCountedConstPtr<Block<IPosition> >;
  template class SimpleCountedPtr<Block<IPosition> >;
  template class CountedConstPtr<Block<BasePlot<Float> > >;
  template class CountedPtr<Block<BasePlot<Float> > >;
  template class PtrRep<Block<BasePlot<Float> > >;
  template class SimpleCountedConstPtr<Block<BasePlot<Float> > >;
  template class SimpleCountedPtr<Block<BasePlot<Float> > >;
  template class CountedConstPtr<Block<TableExprNode> >;
  template class CountedPtr<Block<TableExprNode> >;
  template class PtrRep<Block<TableExprNode> >;
  template class SimpleCountedConstPtr<Block<TableExprNode> >;
  template class SimpleCountedPtr<Block<TableExprNode> >;
  template class CountedConstPtr<Block<Table> >;
  template class CountedPtr<Block<Table> >;
  template class PtrRep<Block<Table> >;
  template class SimpleCountedConstPtr<Block<Table> >;
  template class SimpleCountedPtr<Block<Table> >;
  template class CountedConstPtr<Block<TableIterator> >;
  template class CountedPtr<Block<TableIterator> >;
  template class PtrRep<Block<TableIterator> >;
  template class SimpleCountedConstPtr<Block<TableIterator> >;
  template class SimpleCountedPtr<Block<TableIterator> >;
  template class CountedConstPtr<Block<TableRecord> >;
  template class CountedPtr<Block<TableRecord> >;
  template class PtrRep<Block<TableRecord> >;
  template class SimpleCountedConstPtr<Block<TableRecord> >;
  template class SimpleCountedPtr<Block<TableRecord> >;
  template class CountedConstPtr<TiledFileAccess>;
  template class CountedPtr<TiledFileAccess>;
  template class PtrRep<TiledFileAccess>;
  template class SimpleCountedConstPtr<TiledFileAccess>;
  template class SimpleCountedPtr<TiledFileAccess>;
  template class PtrRep<TableDesc>;
  template class SimpleCountedConstPtr<TableDesc>;
  template class CountedConstPtr<TableRecordRep>;
  template class CountedPtr<TableRecordRep>;
  template class PtrRep<TableRecordRep>;
  template class SimpleCountedConstPtr<TableRecordRep>;
  template class SimpleCountedPtr<TableRecordRep>;

  template uInt Register(ListNotice<TableParse> const *);

  // -- casa/Utilities/Assert.cc
  template class assert_<DataManError>;

  // -- casa/Utilities/GenSort.cc  
  template uInt genSort(Vector<uInt> &, Block<uInt> const &);

  }
