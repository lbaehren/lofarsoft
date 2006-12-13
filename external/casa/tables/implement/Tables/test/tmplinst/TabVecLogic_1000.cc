// TabVecLogic_1000.cc -- Sun Sep 10 22:15:11 CEST 2006 -- root
#include <tables/Tables/TabVecLogic.cc>
#include <tables/Tables/TabVecMath.cc>
#include <tables/Tables/TVecLogic.cc>
#include <tables/Tables/TVecMath.cc>
namespace casa { //# NAMESPACE - BEGIN
template Bool allEQ(ROTableVector<Double> const &, ROTableVector<Double> const &);
template Bool allEQ(ROTableVector<Float> const &, ROTableVector<Float> const &);
template Bool allEQ(ROTableVector<Int> const &, ROTableVector<Int> const &);
template Bool allNE(ROTableVector<Int> const &, ROTableVector<Int> const &);
template Bool anyEQ(ROTableVector<Int> const &, ROTableVector<Int> const &);
template Bool anyNE(ROTableVector<Double> const &, ROTableVector<Double> const &);
template Bool anyNE(ROTableVector<Float> const &, ROTableVector<Float> const &);
template Bool anyNE(ROTableVector<Int> const &, ROTableVector<Int> const &);
template void operator*=(TableVector<Int> &, Int const &);
template TableVector<Int> operator+(ROTableVector<Int> const &, Int const &);
template TableVector<Int> operator+(ROTableVector<Int> const &, ROTableVector<Int> const &);
template void operator+=(TableVector<Int> &, ROTableVector<Int> const &);
template Bool tabVecReptvEQ(TabVecRep<Double> const &, TabVecRep<Double> const &);
template Bool tabVecReptvEQ(TabVecRep<Float> const &, TabVecRep<Float> const &);
template Bool tabVecReptvEQ(TabVecRep<Int> const &, TabVecRep<Int> const &);
template Bool tabVecReptvNE(TabVecRep<Int> const &, TabVecRep<Int> const &);
template TabVecRep<Int> & tabVecReptvadd(TabVecRep<Int> const &, TabVecRep<Int> const &);
template void tabVecReptvassadd(TabVecRep<Int> &, TabVecRep<Int> const &);
template void tabVecRepvalasstim(TabVecRep<Int> &, Int const &);
template TabVecRep<Int> & tabVecRepvalradd(TabVecRep<Int> const &, Int const &);
} //# NAMESPACE - END
