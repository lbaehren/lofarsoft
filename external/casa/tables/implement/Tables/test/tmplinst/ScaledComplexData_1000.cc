// ScaledComplexData_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/ScaledComplexData.cc>
#include <tables/Tables/BaseMappedArrayEngine.cc>
#include <tables/Tables/VirtArrCol.cc>
#include <casa/BasicSL/Complex.h>
namespace casa { //# NAMESPACE - BEGIN
template class BaseMappedArrayEngine<DComplex, Int>;
template class BaseMappedArrayEngine<Complex, Short>;
template class ScaledComplexData<DComplex, Int>;
template class ScaledComplexData<Complex, Short>;
template class VirtualArrayColumn<DComplex>;
} //# NAMESPACE - END
