// BaseMappedArrayEngine_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/BaseMappedArrayEngine.cc>
#include <tables/Tables/ScaledArrayEngine.cc>
namespace casa { //# NAMESPACE - BEGIN
template class BaseMappedArrayEngine<Double, Int>;
template class BaseMappedArrayEngine<Float, uChar>;
template class ScaledArrayEngine<Double, Int>;
template class ScaledArrayEngine<Float, uChar>;
} //# NAMESPACE - END
