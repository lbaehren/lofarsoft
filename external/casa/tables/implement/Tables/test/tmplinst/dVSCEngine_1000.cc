// dVSCEngine_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/test/dVSCEngine.h>
#include <casa/Arrays/Array.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Containers/Block.h>
#include <tables/Tables/ScaColData.cc>
#include <tables/Tables/ScaColDesc.cc>
#include <tables/Tables/ScalarColumn.cc>
#include <tables/Tables/VSCEngine.cc>
#include <tables/Tables/VirtScaCol.cc>
#include <casa/Utilities/Compare.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/ValTypeId.h>
namespace casa { //# NAMESPACE - BEGIN
template class Array<VSCExample>;
template class MaskedArray<VSCExample>;
template class Vector<VSCExample>;
template class ScalarColumnData<VSCExample>;
template class ScalarColumnDesc<VSCExample>;
template class ROScalarColumn<VSCExample>;
template class ScalarColumn<VSCExample>;
template class VSCEngine<VSCExample>;
template class VirtualScalarColumn<VSCExample>;
template class ObjCompare<VSCExample>;
template class CountedConstPtr<Block<VSCExample> >;
template class CountedPtr<Block<VSCExample> >;
template class PtrRep<Block<VSCExample> >;
template class SimpleCountedConstPtr<Block<VSCExample> >;
template class SimpleCountedPtr<Block<VSCExample> >;
template String valDataTypeId(VSCExample const *);
} //# NAMESPACE - END
