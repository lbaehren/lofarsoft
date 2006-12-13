// RecordField_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <casa/Containers/RecordField.cc>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>
namespace casa { //# NAMESPACE - BEGIN
template class RORecordFieldPtr<Table>;
template class RecordFieldPtr<Table>;
} //# NAMESPACE - END
