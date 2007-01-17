
#include <tables/Tables/TableParse.h>
#include <casa/BasicSL/String.h>
#include <tables/Tables/ColumnDesc.h>
#include <tables/Tables/RefColumn.h>
#include <tables/Tables/BaseColDesc.h>
#include <tables/Tables/DataManager.h>
#include <tables/Tables/TableRecord.h>

#include <casa/Containers/BlockIO.cc>
#include <casa/Containers/Link.cc>
#include <casa/Containers/List.cc>
#include <casa/Containers/OrderedPair.cc>
#include <casa/Containers/RecordField.cc>
#include <casa/Containers/SimOrdMap.cc>

namespace casa {

  template class Link<TableParse>;

  template class ConstListIter<TableParse>;
  template class List<TableParse>;
  template class ListIter<TableParse>;
  template class ListNotice<TableParse>;

  template class OrderedPair<String, ColumnDesc>;
  template class OrderedPair<String, RefColumn *>;
  template class OrderedPair<String, BaseColumnDesc *(*)(String const &)>;
  template class OrderedPair<String, DataManager *(*)(String const &, Record const &)>;

  template class RORecordFieldPtr<TableRecord>;
  template class RecordFieldPtr<TableRecord>;

  template class SimpleOrderedMap<String, BaseColumnDesc *(*)(String const &)>;
  template class SimpleOrderedMap<String, ColumnDesc>;
  template class SimpleOrderedMap<String, DataManager *(*)(String const &, Record const &)>;
  template class SimpleOrderedMap<String, RefColumn *>;

  // -- casa/Containers/BlockIO.cc
  template ostream & operator<<(ostream &, Block<uInt> const &);

  // -- casa/Containers/RecordField.cc
  template class RORecordFieldPtr<Table>;
  template class RecordFieldPtr<Table>;


}
