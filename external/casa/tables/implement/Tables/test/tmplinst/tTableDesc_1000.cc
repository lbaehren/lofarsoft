// tTableDesc_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/test/tTableDesc.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIter.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/Block.h>
#include <tables/Tables/ArrColData.cc>
#include <tables/Tables/ArrColDesc.cc>
#include <tables/Tables/ScaColData.cc>
#include <tables/Tables/ScaColDesc.cc>
#include <casa/Utilities/Compare.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/ValTypeId.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<ExampleDesc>;
template class ArrayIterator<ExampleDesc>;
template class ReadOnlyArrayIterator<ExampleDesc>;
template class MaskedArray<ExampleDesc>;
template class Vector<ExampleDesc>;
template class ArrayColumnData<ExampleDesc>;
template class ArrayColumnDesc<ExampleDesc>;
template class ScalarColumnData<ExampleDesc>;
template class ScalarColumnDesc<ExampleDesc>;
template class ObjCompare<ExampleDesc>;
template class CountedConstPtr<Block<ExampleDesc> >;
template class CountedPtr<Block<ExampleDesc> >;
template class PtrRep<Block<ExampleDesc> >;
template class SimpleCountedConstPtr<Block<ExampleDesc> >;
template class SimpleCountedPtr<Block<ExampleDesc> >;
template String valDataTypeId(ExampleDesc const *);
} //# NAMESPACE - END
