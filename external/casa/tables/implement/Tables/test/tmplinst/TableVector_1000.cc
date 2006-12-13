// TableVector_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/TableVector.cc>
#include <tables/Tables/TVec.cc>
#include <tables/Tables/TVecScaCol.cc>
#include <tables/Tables/TVecTemp.cc>
namespace casa { //# NAMESPACE - BEGIN
template class ROTableVector<Float>;
template class TableVector<Float>;
template class TabVecRep<Float>;
template class TabVecScaCol<Float>;
template class TabVecTemp<Float>;
} //# NAMESPACE - END
