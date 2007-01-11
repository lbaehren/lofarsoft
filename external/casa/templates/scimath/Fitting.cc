
// include of header files 

// include of implementation files

namespace casa {

1000 casa/stdmap.h 
     = casa/BasicSL/String.h 
     = scimath/Functionals/FuncExprData.h 
     #namespace 
     template class std::map<casa::String, casa::FuncExprData::ExprOperator> 
     AIPS_MAP_AUX_TEMPLATES(casa::String, casa::FuncExprData::ExprOperator) 
1010 casa/stdmap.h 
     = scimath/Functionals/FuncExprData.h 
     #namespace 
     template class std::map<casa::FuncExprData::opTypes, casa::FuncExprData::ExprOperator> 
     AIPS_MAP_AUX_TEMPLATES(casa::FuncExprData::opTypes, casa::FuncExprData::ExprOperator) 
1000 casa/stdvector.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     #namespace 
     template class std::vector<casa::AutoDiff<casa::Complex> > 
     AIPS_VECTOR_AUX_TEMPLATES(casa::AutoDiff<casa::Complex>) 
1010 casa/stdvector.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     #namespace 
     template class std::vector<casa::AutoDiff<casa::DComplex> > 
     AIPS_VECTOR_AUX_TEMPLATES(casa::AutoDiff<casa::DComplex>) 
1020 casa/stdvector.h 
     = scimath/Functionals/FuncExprData.h 
     #namespace 
     template class std::vector<casa::FuncExprData::ExprOperator> 
     AIPS_VECTOR_AUX_TEMPLATES(casa::FuncExprData::ExprOperator) 
1030 casa/stdvector.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     #namespace 
     template class std::vector<casa::AutoDiff<casa::Double> > 
     AIPS_VECTOR_AUX_TEMPLATES(casa::AutoDiff<casa::Double>) 
1040 casa/stdvector.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     #namespace 
     template class std::vector<casa::AutoDiff<casa::Float> > 
     AIPS_VECTOR_AUX_TEMPLATES(casa::AutoDiff<casa::Float>) 
1000 scimath/Fitting/FitGaussian.cc 
     template class FitGaussian<Float> 
1000 scimath/Fitting/GenericL2Fit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GenericL2Fit<AutoDiff<Complex> > 
     template class GenericL2Fit<AutoDiffA<Complex> > 
1010 scimath/Fitting/GenericL2Fit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GenericL2Fit<AutoDiff<DComplex> > 
     template class GenericL2Fit<AutoDiffA<DComplex> > 
1020 scimath/Fitting/GenericL2Fit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GenericL2Fit<AutoDiff<Double> > 
     template class GenericL2Fit<AutoDiffA<Double> > 
1030 scimath/Fitting/GenericL2Fit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GenericL2Fit<AutoDiff<Float> > 
     template class GenericL2Fit<AutoDiffA<Float> > 
1040 scimath/Fitting/GenericL2Fit.cc casa/BasicSL/Complex.h 
     template class GenericL2Fit<Complex> 
1050 scimath/Fitting/GenericL2Fit.cc casa/BasicSL/Complex.h 
     template class GenericL2Fit<DComplex> 
1060 scimath/Fitting/GenericL2Fit.cc 
     template class GenericL2Fit<Double> 
1070 scimath/Fitting/GenericL2Fit.cc 
     template class GenericL2Fit<Float> 
1000 scimath/Fitting/LSQFit2.cc 
     = algorithm 
     #namespace 
     #ifndef AIPS_INTELCC 
       template casa::Double *swap_ranges<casa::Double *, casa::Double *>(casa::Double *, casa::Double *, casa::Double *) 
     #endif 
1010 scimath/Fitting/LSQFit2.cc 
     = casa/Arrays/VectorSTLIterator.h 
     template void LSQFit::makeNorm<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template void LSQFit::makeNorm<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template Bool LSQFit::addConstraint<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &) 
     template Bool LSQFit::addConstraint<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, VectorSTLIterator<Double> >(uInt n, VectorSTLIterator<Double> const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &) 
     template Bool LSQFit::getConstraint<VectorSTLIterator<Double> >(uInt, VectorSTLIterator<Double> &) const 
     template void LSQFit::solve<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &) 
     template Bool LSQFit::solveLoop<VectorSTLIterator<Double> >(Double &, uInt &, VectorSTLIterator<Double> &, Bool) 
     template Bool LSQFit::solveLoop<VectorSTLIterator<Double> >(uInt &, VectorSTLIterator<Double> &, Bool) 
     template void LSQFit::copy<VectorSTLIterator<Double> >(Double const *, Double const *, VectorSTLIterator<Double> &, LSQReal) 
     template void LSQFit::uncopy<VectorSTLIterator<Double> >(Double *, Double const *, VectorSTLIterator<Double> &, LSQReal) 
     template void LSQFit::copyDiagonal<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &, LSQReal) 
     template Bool LSQFit::getErrors<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &) 
1020 scimath/Fitting/LSQFit2.cc 
     = casa/Arrays/VectorSTLIterator.h 
     template void LSQFit::makeNorm<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &, Float const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
     template void LSQFit::makeNorm<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &, Float const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
     template Bool LSQFit::addConstraint<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &) 
     template Bool LSQFit::addConstraint<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &) 
     template Bool LSQFit::setConstraint<Float, VectorSTLIterator<Float> >(uInt n, VectorSTLIterator<Float> const &, Float const &) 
     template Bool LSQFit::setConstraint<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &) 
     template Bool LSQFit::getConstraint<VectorSTLIterator<Float> >(uInt, VectorSTLIterator<Float> &) const 
     template void LSQFit::solve<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &) 
     template Bool LSQFit::solveLoop<VectorSTLIterator<Float> >(Double &, uInt &, VectorSTLIterator<Float> &, Bool) 
     template Bool LSQFit::solveLoop<VectorSTLIterator<Float> >(uInt &, VectorSTLIterator<Float> &, Bool) 
     template void LSQFit::copy<VectorSTLIterator<Float> >(Double const *, Double const *, VectorSTLIterator<Float> &, LSQReal) 
     template void LSQFit::uncopy<VectorSTLIterator<Float> >(Double *, Double const *, VectorSTLIterator<Float> &, LSQReal) 
     template void LSQFit::copyDiagonal<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &, LSQReal) 
     template Bool LSQFit::getErrors<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &) 
1030 scimath/Fitting/LSQFit2.cc 
     = casa/Arrays/VectorSTLIterator.h 
     typedef VectorSTLIterator<std::complex<Double> > It1 
     typedef VectorSTLIterator<uInt> It1Int 
     template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
     template Bool LSQFit::addConstraint<Double, It1>(It1 const &, std::complex<Double> const &) 
     template Bool LSQFit::addConstraint<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, std::complex<Double> const &) 
     template Bool LSQFit::setConstraint<Double, It1>(uInt n, It1 const &, std::complex<Double> const &) 
     template Bool LSQFit::setConstraint<Double, It1, It1Int>(uInt n, uInt nIndex, It1Int const &, It1 const &, std::complex<Double> const &) 
     template Bool LSQFit::getConstraint<It1>(uInt, It1 &) const 
     template void LSQFit::solve<It1>(It1 &) 
     template Bool LSQFit::solveLoop<It1>(Double &, uInt &, It1 &, Bool) 
     template Bool LSQFit::solveLoop<It1>(uInt &, It1 &, Bool) 
     template void LSQFit::copy<It1>(Double const *, Double const *, It1 &, LSQComplex) 
     template void LSQFit::uncopy<It1>(Double *, Double const *, It1 &, LSQComplex) 
     template void LSQFit::copyDiagonal<It1>(It1 &, LSQComplex) 
     template Bool LSQFit::getErrors<It1>(It1 &) 
1040 scimath/Fitting/LSQFit2.cc 
     = casa/Arrays/VectorSTLIterator.h 
     typedef VectorSTLIterator<std::complex<Float> > It2 
     typedef VectorSTLIterator<uInt> It2Int 
     template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
     template Bool LSQFit::addConstraint<Float, It2>(It2 const &, std::complex<Float> const &) 
     template Bool LSQFit::addConstraint<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, std::complex<Float> const &) 
     template Bool LSQFit::setConstraint<Float, It2>(uInt n, It2 const &, std::complex<Float> const &) 
     template Bool LSQFit::setConstraint<Float, It2, It2Int>(uInt n, uInt nIndex, It2Int const &, It2 const &, std::complex<Float> const &) 
     template Bool LSQFit::getConstraint<It2>(uInt, It2 &) const 
     template void LSQFit::solve<It2>(It2 &) 
     template Bool LSQFit::solveLoop<It2>(Double &, uInt &, It2 &, Bool) 
     template Bool LSQFit::solveLoop<It2>(uInt &, It2 &, Bool) 
     template void LSQFit::copy<It2>(Double const *, Double const *, It2 &, LSQComplex) 
     template void LSQFit::uncopy<It2>(Double *, Double const *, It2 &, LSQComplex) 
     template void LSQFit::copyDiagonal<It2>(It2 &, LSQComplex) 
     template Bool LSQFit::getErrors<It2>(It2 &) 
1050 scimath/Fitting/LSQFit2.cc 
     = complex 
     typedef std::complex<Double>* It3 
     typedef uInt* It3Int 
     template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
     template Bool LSQFit::addConstraint<Double, It3>(It3 const &, std::complex<Double> const &) 
     template Bool LSQFit::addConstraint<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &) 
     template Bool LSQFit::setConstraint<Double, It3>(uInt n, It3 const &, std::complex<Double> const &) 
     template Bool LSQFit::setConstraint<Double, It3, It3Int>(uInt n, uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &) 
     template Bool LSQFit::getConstraint<Double>(uInt, std::complex<Double> *) const 
     template void LSQFit::solve<Double>(std::complex<Double> *) 
     template Bool LSQFit::solveLoop<Double>(Double &, uInt &, std::complex<Double> *, Bool) 
     template Bool LSQFit::solveLoop<Double>(uInt &, std::complex<Double> *, Bool) 
     template void LSQFit::copy<std::complex<Double> >(Double const *, Double const *, std::complex<Double> *, LSQComplex) 
     template void LSQFit::uncopy<std::complex<Double> >(Double *, Double const *, std::complex<Double> *, LSQComplex) 
     template Bool LSQFit::getErrors<Double>(std::complex<Double> *) 
     template Bool LSQFit::getCovariance<Double>(std::complex<Double> *) 
1060 scimath/Fitting/LSQFit2.cc 
     = complex 
     typedef std::complex<Float>* It4 
     typedef uInt* It4Int 
     template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
     template Bool LSQFit::addConstraint<Float, It4>(It4 const &, std::complex<Float> const &) 
     template Bool LSQFit::addConstraint<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, std::complex<Float> const &) 
     template Bool LSQFit::setConstraint<Float, It4>(uInt n, It4 const &, std::complex<Float> const &) 
     template Bool LSQFit::setConstraint<Float, It4, It4Int>(uInt n, uInt nIndex, It4Int const &, It4 const &, std::complex<Float> const &) 
     template Bool LSQFit::getConstraint<Float>(uInt, std::complex<Float> *) const 
     template void LSQFit::solve<Float>(std::complex<Float> *) 
     template Bool LSQFit::solveLoop<Float>(Double &, uInt &, std::complex<Float> *, Bool) 
     template Bool LSQFit::solveLoop<Float>(uInt &, std::complex<Float> *, Bool) 
     template void LSQFit::copy<std::complex<Float> >(Double const *, Double const *, std::complex<Float> *, LSQComplex) 
     template void LSQFit::uncopy<std::complex<Float> >(Double *, Double const *, std::complex<Float> *, LSQComplex) 
     template Bool LSQFit::getErrors<Float>(std::complex<Float> *) 
     template Bool LSQFit::getCovariance<Float>(std::complex<Float> *) 
1070 scimath/Fitting/LSQFit2.cc 
     typedef Double* It5 
     typedef uInt* It5Int 
     template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template Bool LSQFit::addConstraint<Double, It5>(It5 const &, Double const &) 
     template Bool LSQFit::addConstraint<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, It5>(uInt n, It5 const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, It5, It5Int>(uInt n, uInt nIndex, It5Int const &, It5 const &, Double const &) 
     template Bool LSQFit::getConstraint<Double>(uInt, Double *) const 
     template void LSQFit::solve<Double>(Double *) 
     template Bool LSQFit::solveLoop<Double>(Double &, uInt &, Double *, Bool) 
     template Bool LSQFit::solveLoop<Double>(uInt &, Double *, Bool) 
     template void LSQFit::copy<Double>(Double const *, Double const *, Double *, LSQReal) 
     template void LSQFit::uncopy<Double>(Double *, Double const *, Double *, LSQReal) 
     template Bool LSQFit::getErrors<Double>(Double *) 
     template Bool LSQFit::getCovariance<Double>(Double *) 
1080 scimath/Fitting/LSQFit2.cc 
     typedef Float* It6 
     typedef uInt* It6Int 
     template void LSQFit::makeNorm<Float, It6>(It6 const &, Float const &, Float const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It6>(It6 const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &, Float const &, Bool, Bool) 
     template void LSQFit::makeNorm<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
     template Bool LSQFit::addConstraint<Float, It6>(It6 const &, Float const &) 
     template Bool LSQFit::addConstraint<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &) 
     template Bool LSQFit::setConstraint<Float, It6>(uInt n, It6 const &, Float const &) 
     template Bool LSQFit::setConstraint<Float, It6, It6Int>(uInt n, uInt nIndex, It6Int const &, It6 const &, Float const &) 
     template Bool LSQFit::getConstraint<Float>(uInt, Float *) const 
     template void LSQFit::solve<Float>(Float *) 
     template Bool LSQFit::solveLoop<Float>(Double &, uInt &, Float *, Bool) 
     template Bool LSQFit::solveLoop<Float>(uInt &, Float *, Bool) 
     template void LSQFit::copy<Float>(Double const *, Double const *, Float *, LSQReal) 
     template void LSQFit::uncopy<Float>(Double *, Double const *, Float *, LSQReal) 
     template Bool LSQFit::getErrors<Float>(Float *) 
     template Bool LSQFit::getCovariance<Float>(Float *) 
1090 scimath/Fitting/LSQFit2.cc 
     typedef Float* It7 
     typedef uInt* It7Int 
     template void LSQFit::makeNorm<Double, It7>(It7 const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It7>(It7 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &, Double const &, Bool, Bool) 
     template void LSQFit::makeNorm<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
     template Bool LSQFit::addConstraint<Double, It7>(It7 const &, Double const &) 
     template Bool LSQFit::addConstraint<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, It7>(uInt n, It7 const &, Double const &) 
     template Bool LSQFit::setConstraint<Double, It7, It7Int>(uInt n, uInt nIndex, It7Int const &, It7 const &, Double const &) 
1000 scimath/Fitting/LSQaips.cc 
     = casa/Arrays/Array.h 
     = casa/Arrays/Vector.h 
     template Bool LSQaips::getCovariance<Double>(Array<Double> &) 
     template Bool LSQaips::solveLoop<Double>(Double &, uInt &, Vector<Double> &, Bool doSVD) 
     template Bool LSQaips::solveLoop<Double>(uInt &, Vector<Double> &, Bool doSVD) 
1010 scimath/Fitting/LSQaips.cc 
     = casa/Arrays/Array.h 
     = casa/Arrays/Vector.h 
     template Bool LSQaips::getCovariance<Float>(Array<Float> &) 
     template Bool LSQaips::solveLoop<Float>(Double &, uInt &, Vector<Float> &, Bool doSVD) 
     template Bool LSQaips::solveLoop<Float>(uInt &, Vector<Float> &, Bool doSVD) 
1020 scimath/Fitting/LSQaips.cc 
     = casa/Arrays/Array.h 
     = casa/Arrays/Vector.h 
     template Bool LSQaips::getCovariance<std::complex<Double> >(Array<std::complex<Double> > &) 
     template Bool LSQaips::solveLoop<std::complex<Double> >(Double &, uInt &, Vector<std::complex<Double> > &, Bool doSVD) 
     template Bool LSQaips::solveLoop<std::complex<Double> >(uInt &, Vector<std::complex<Double> > &, Bool doSVD) 
1030 scimath/Fitting/LSQaips.cc 
     = casa/Arrays/Array.h 
     = casa/Arrays/Vector.h 
     template Bool LSQaips::getCovariance<std::complex<Float> >(Array<std::complex<Float> > &) 
     template Bool LSQaips::solveLoop<std::complex<Float> >(Double &, uInt &, Vector<std::complex<Float> > &, Bool doSVD) 
     template Bool LSQaips::solveLoop<std::complex<Float> >(uInt &, Vector<std::complex<Float> > &, Bool doSVD) 
1000 scimath/Fitting/LinearFit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFit<AutoDiff<Complex> > 
     template class LinearFit<AutoDiffA<Complex> > 
1010 scimath/Fitting/LinearFit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFit<AutoDiff<DComplex> > 
     template class LinearFit<AutoDiffA<DComplex> > 
1020 scimath/Fitting/LinearFit.cc 
     = casa/BasicSL/Complex.h 
     template class LinearFit<Complex> 
     template class LinearFit<DComplex> 
1030 scimath/Fitting/LinearFit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFit<AutoDiff<Double> > 
     template class LinearFit<AutoDiffA<Double> > 
1040 scimath/Fitting/LinearFit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFit<AutoDiff<Float> > 
     template class LinearFit<AutoDiffA<Float> > 
1050 scimath/Fitting/LinearFit.cc 
     template class LinearFit<Double> 
     template class LinearFit<Float> 
1000 scimath/Fitting/LinearFitSVD.cc 
     = casa/BasicSL/Complex.h 
     template class LinearFitSVD<Complex> 
     template class LinearFitSVD<DComplex> 
1010 scimath/Fitting/LinearFitSVD.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFitSVD<AutoDiff<Complex> > 
     template class LinearFitSVD<AutoDiffA<Complex> > 
1020 scimath/Fitting/LinearFitSVD.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFitSVD<AutoDiff<DComplex> > 
     template class LinearFitSVD<AutoDiffA<DComplex> > 
1030 scimath/Fitting/LinearFitSVD.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFitSVD<AutoDiff<Double> > 
     template class LinearFitSVD<AutoDiffA<Double> > 
1040 scimath/Fitting/LinearFitSVD.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class LinearFitSVD<AutoDiff<Float> > 
     template class LinearFitSVD<AutoDiffA<Float> > 
1050 scimath/Fitting/LinearFitSVD.cc 
     template class LinearFitSVD<Double> 
     template class LinearFitSVD<Float> 
1000 scimath/Fitting/NonLinearFit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFit<AutoDiff<Complex> > 
     template class NonLinearFit<AutoDiffA<Complex> > 
1010 scimath/Fitting/NonLinearFit.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFit<AutoDiff<DComplex> > 
     template class NonLinearFit<AutoDiffA<DComplex> > 
1020 scimath/Fitting/NonLinearFit.cc 
     = casa/BasicSL/Complex.h 
     template class NonLinearFit<Complex> 
     template class NonLinearFit<DComplex> 
1030 scimath/Fitting/NonLinearFit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFit<AutoDiff<Double> > 
     template class NonLinearFit<AutoDiffA<Double> > 
1040 scimath/Fitting/NonLinearFit.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFit<AutoDiff<Float> > 
     template class NonLinearFit<AutoDiffA<Float> > 
1050 scimath/Fitting/NonLinearFit.cc 
     template class NonLinearFit<Double> 
     template class NonLinearFit<Float> 
1000 scimath/Fitting/NonLinearFitLM.cc 
     = casa/BasicSL/Complex.h 
     template class NonLinearFitLM<Complex> 
     template class NonLinearFitLM<DComplex> 
1010 scimath/Fitting/NonLinearFitLM.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFitLM<AutoDiff<Complex> > 
     template class NonLinearFitLM<AutoDiffA<Complex> > 
1020 scimath/Fitting/NonLinearFitLM.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFitLM<AutoDiff<DComplex> > 
     template class NonLinearFitLM<AutoDiffA<DComplex> > 
1030 scimath/Fitting/NonLinearFitLM.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFitLM<AutoDiff<Double> > 
     template class NonLinearFitLM<AutoDiffA<Double> > 
1040 scimath/Fitting/NonLinearFitLM.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class NonLinearFitLM<AutoDiff<Float> > 
     template class NonLinearFitLM<AutoDiffA<Float> > 
1050 scimath/Fitting/NonLinearFitLM.cc 
     template class NonLinearFitLM<Double> 
     template class NonLinearFitLM<Float> 
1000 scimath/Functionals/AbstractFunctionFactory.h 
     template class FunctionFactory<Double> 
1010 scimath/Functionals/AbstractFunctionFactory.h 
     template class FunctionFactory<Float> 
1000 scimath/Functionals/Chebyshev.cc 
     = casa/BasicSL/Complex.h 
     template class Chebyshev<DComplex> 
     template class Chebyshev<Complex> 
1010 scimath/Functionals/Chebyshev.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Chebyshev<AutoDiff<DComplex> > 
     template class Chebyshev<AutoDiff<Complex> > 
1020 scimath/Functionals/Chebyshev.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Chebyshev<AutoDiff<Double> > 
     template class Chebyshev<AutoDiff<Float> > 
1030 scimath/Functionals/Chebyshev.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Chebyshev<AutoDiffA<Double> > 
     template class Chebyshev<AutoDiffA<Float> > 
1040 scimath/Functionals/Chebyshev.cc 
     template class Chebyshev<Double> 
     template class Chebyshev<Float> 
1000 scimath/Functionals/ChebyshevParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class ChebyshevParam<AutoDiff<DComplex> > 
     template class ChebyshevParam<AutoDiff<Complex> > 
     template class ChebyshevParamModeImpl<AutoDiff<DComplex> > 
     template class ChebyshevParamModeImpl<AutoDiff<Complex> > 
1010 scimath/Functionals/ChebyshevParam.cc 
     = casa/BasicSL/Complex.h 
     template class ChebyshevParam<DComplex> 
     template class ChebyshevParam<Complex> 
     template class ChebyshevParamModeImpl<DComplex> 
     template class ChebyshevParamModeImpl<Complex> 
1020 scimath/Functionals/ChebyshevParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class ChebyshevParam<AutoDiff<Double> > 
     template class ChebyshevParam<AutoDiff<Float> > 
     template class ChebyshevParamModeImpl<AutoDiff<Double> > 
     template class ChebyshevParamModeImpl<AutoDiff<Float> > 
1030 scimath/Functionals/ChebyshevParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class ChebyshevParam<AutoDiffA<Double> > 
     template class ChebyshevParam<AutoDiffA<Float> > 
     template class ChebyshevParamModeImpl<AutoDiffA<Double> > 
     template class ChebyshevParamModeImpl<AutoDiffA<Float> > 
1040 scimath/Functionals/ChebyshevParam.cc 
     template class ChebyshevParam<Double> 
     template class ChebyshevParam<Float> 
     template class ChebyshevParamModeImpl<Double> 
     template class ChebyshevParamModeImpl<Float> 
1000 scimath/Functionals/Combi2Function.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CombiFunction<AutoDiff<DComplex> > 
     template class CombiFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/Combi2Function.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CombiFunction<AutoDiff<Double> > 
     template class CombiFunction<AutoDiff<Float> > 
1000 scimath/Functionals/CombiFunction.cc 
     = casa/BasicSL/Complex.h 
     template class CombiFunction<DComplex> 
     template class CombiFunction<Complex> 
1010 scimath/Functionals/CombiFunction.cc 
     template class CombiFunction<Double> 
     template class CombiFunction<Float> 
1000 scimath/Functionals/CombiParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CombiParam<AutoDiff<DComplex> > 
     template class CombiParam<AutoDiff<Complex> > 
1010 scimath/Functionals/CombiParam.cc 
     = casa/BasicSL/Complex.h 
     template class CombiParam<DComplex> 
     template class CombiParam<Complex> 
1020 scimath/Functionals/CombiParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CombiParam<AutoDiff<Double> > 
     template class CombiParam<AutoDiff<Float> > 
1030 scimath/Functionals/CombiParam.cc 
     template class CombiParam<Double> 
     template class CombiParam<Float> 
1000 scimath/Functionals/CompiledFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledFunction<AutoDiff<DComplex> > 
     template class CompiledFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/CompiledFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledFunction<DComplex> 
     template class CompiledFunction<Complex> 
1020 scimath/Functionals/CompiledFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledFunction<AutoDiff<Double> > 
     template class CompiledFunction<AutoDiff<Float> > 
1030 scimath/Functionals/CompiledFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledFunction<Double> 
     template class CompiledFunction<Float> 
1040 scimath/Functionals/CompiledFunction.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledFunction<AutoDiffA<Double> > 
     template class CompiledFunction<AutoDiffA<Float> > 
1000 scimath/Functionals/CompiledParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledParam<AutoDiff<DComplex> > 
     template class CompiledParam<AutoDiff<Complex> > 
1010 scimath/Functionals/CompiledParam.cc 
     = casa/BasicSL/Complex.h 
     template class CompiledParam<DComplex> 
     template class CompiledParam<Complex> 
1020 scimath/Functionals/CompiledParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledParam<AutoDiff<Double> > 
     template class CompiledParam<AutoDiff<Float> > 
1030 scimath/Functionals/CompiledParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompiledParam<AutoDiffA<Double> > 
     template class CompiledParam<AutoDiffA<Float> > 
1040 scimath/Functionals/CompiledParam.cc 
     template class CompiledParam<Double> 
     template class CompiledParam<Float> 
1000 scimath/Functionals/Compound2Function.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompoundFunction<AutoDiff<DComplex> > 
     template class CompoundFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/Compound2Function.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompoundFunction<AutoDiff<Double> > 
     template class CompoundFunction<AutoDiff<Float> > 
1000 scimath/Functionals/CompoundFunction.cc 
     = casa/BasicSL/Complex.h 
     template class CompoundFunction<DComplex> 
     template class CompoundFunction<Complex> 
1010 scimath/Functionals/CompoundFunction.cc 
     template class CompoundFunction<Double> 
     template class CompoundFunction<Float> 
1000 scimath/Functionals/CompoundParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompoundParam<AutoDiff<DComplex> > 
     template class CompoundParam<AutoDiff<Complex> > 
1010 scimath/Functionals/CompoundParam.cc 
     = casa/BasicSL/Complex.h 
     template class CompoundParam<DComplex> 
     template class CompoundParam<Complex> 
1020 scimath/Functionals/CompoundParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class CompoundParam<AutoDiff<Double> > 
     template class CompoundParam<AutoDiff<Float> > 
1030 scimath/Functionals/CompoundParam.cc 
     template class CompoundParam<Double> 
     template class CompoundParam<Float> 
1000 scimath/Functionals/DiracDFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDFunction<AutoDiff<DComplex> > 
     template class DiracDFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/DiracDFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDFunction<DComplex> 
     template class DiracDFunction<Complex> 
1020 scimath/Functionals/DiracDFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDFunction<AutoDiff<Double> > 
     template class DiracDFunction<AutoDiff<Float> > 
1030 scimath/Functionals/DiracDFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDFunction<Double> 
     template class DiracDFunction<Float> 
1000 scimath/Functionals/DiracDParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDParam<AutoDiff<DComplex> > 
     template class DiracDParam<AutoDiff<Complex> > 
1010 scimath/Functionals/DiracDParam.cc 
     = casa/BasicSL/Complex.h 
     template class DiracDParam<DComplex> 
     template class DiracDParam<Complex> 
1020 scimath/Functionals/DiracDParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class DiracDParam<AutoDiff<Double> > 
     template class DiracDParam<AutoDiff<Float> > 
1030 scimath/Functionals/DiracDParam.cc 
     template class DiracDParam<Double> 
     template class DiracDParam<Float> 
1000 scimath/Functionals/EclecticFunctionFactory.cc 
     template class EclecticFunctionFactory<Double> 
1010 scimath/Functionals/EclecticFunctionFactory.cc 
     template class EclecticFunctionFactory<Float> 
1000 scimath/Functionals/EvenPolynomial.cc 
     = casa/BasicSL/Complex.h 
     template class EvenPolynomial<DComplex> 
     template class EvenPolynomial<Complex> 
1010 scimath/Functionals/EvenPolynomial.cc 
     template class EvenPolynomial<Double> 
1000 scimath/Functionals/EvenPolynomial2.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class EvenPolynomial<AutoDiff<DComplex> > 
     template class EvenPolynomial<AutoDiff<Complex> > 
1010 scimath/Functionals/EvenPolynomial2.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class EvenPolynomial<AutoDiff<Double> > 
1000 scimath/Functionals/EvenPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class EvenPolynomialParam<AutoDiff<DComplex> > 
     template class EvenPolynomialParam<AutoDiff<Complex> > 
1010 scimath/Functionals/EvenPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     template class EvenPolynomialParam<DComplex> 
     template class EvenPolynomialParam<Complex> 
1020 scimath/Functionals/EvenPolynomialParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class EvenPolynomialParam<AutoDiff<Double> > 
1030 scimath/Functionals/EvenPolynomialParam.cc 
     template class EvenPolynomialParam<Double> 
1000 scimath/Functionals/Function.cc 
     = scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     template class Function<AutoDiff<Complex> > 
     template class Function<AutoDiff<DComplex> > 
1010 scimath/Functionals/Function.cc 
     = scimath/Mathematics/AutoDiff.h 
     template class Function<AutoDiff<Double> > 
     template class Function<AutoDiff<Float> > 
1020 scimath/Functionals/Function.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = casa/BasicSL/Complex.h 
     template class Function<AutoDiffA<Complex> > 
     template class Function<AutoDiffA<DComplex> > 
1030 scimath/Functionals/Function.cc 
     = scimath/Mathematics/AutoDiffA.h 
     template class Function<AutoDiffA<Double> > 
     template class Function<AutoDiffA<Float> > 
1040 scimath/Functionals/Function.cc casa/Arrays/Array.h 
     template class Function<Double, Array<Double> > 
1050 scimath/Functionals/Function.cc casa/BasicSL/Complex.h 
     template class Function<Complex> 
     template class Function<DComplex> 
1060 scimath/Functionals/Function.cc 
     template class Function<Double, Float> 
1070 scimath/Functionals/Function.cc 
     template class Function<Double> 
     template class Function<Float> 
1000 scimath/Functionals/Function1D.h 
     = scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     template class Function1D<AutoDiff<Complex> > 
     template class Function1D<AutoDiff<DComplex> > 
1010 scimath/Functionals/Function1D.h 
     = scimath/Mathematics/AutoDiff.h 
     template class Function1D<AutoDiff<Double> > 
     template class Function1D<AutoDiff<Float> > 
1020 scimath/Functionals/Function1D.h 
     = scimath/Mathematics/AutoDiffA.h 
     = casa/BasicSL/Complex.h 
     template class Function1D<AutoDiffA<Complex> > 
1030 scimath/Functionals/Function1D.h 
     = scimath/Mathematics/AutoDiffA.h 
     = casa/BasicSL/Complex.h 
     template class Function1D<AutoDiffA<DComplex> > 
1040 scimath/Functionals/Function1D.h 
     = scimath/Mathematics/AutoDiffA.h 
     template class Function1D<AutoDiffA<Double> > 
     template class Function1D<AutoDiffA<Float> > 
1050 scimath/Functionals/Function1D.h casa/Arrays/Array.h 
     template class Function1D<Double, Array<Double> > 
1060 scimath/Functionals/Function1D.h casa/BasicSL/Complex.h 
     template class Function1D<Complex> 
     template class Function1D<DComplex> 
1070 scimath/Functionals/Function1D.h 
     template class Function1D<Double> 
     template class Function1D<Float> 
1000 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Bool FunctionHolder<DComplex>::getRecord<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &, RecordInterface const &) 
1010 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Bool FunctionHolder<DComplex>::getType<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &, RecordInterface const &) 
1020 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     template Bool FunctionHolder<DComplex>::getRecord<DComplex>(String &, Function<DComplex> * &, RecordInterface const &) 
1030 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     template Bool FunctionHolder<DComplex>::getType<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &) 
1040 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     template Bool FunctionHolder<DComplex>::getType<DComplex>(String &, Function<DComplex> * &) 
1050 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     template Bool FunctionHolder<DComplex>::getType<DComplex>(String &, Function<DComplex> * &, RecordInterface const &) 
1060 scimath/Functionals/FunctionHolder.cc 
     = casa/BasicSL/Complex.h 
     template class FunctionHolder<DComplex> 
1070 scimath/Functionals/FunctionHolder.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Bool FunctionHolder<Double>::getRecord<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &, RecordInterface const &) 
1080 scimath/Functionals/FunctionHolder.cc 
     template Bool FunctionHolder<Double>::getRecord<Double>(String &, Function<Double> * &, RecordInterface const &) 
1090 scimath/Functionals/FunctionHolder.cc 
     template Bool FunctionHolder<Double>::getType<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &) 
1100 scimath/Functionals/FunctionHolder.cc 
     template Bool FunctionHolder<Double>::getType<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &, RecordInterface const &) 
1110 scimath/Functionals/FunctionHolder.cc 
     template Bool FunctionHolder<Double>::getType<Double>(String &, Function<Double> * &) 
1120 scimath/Functionals/FunctionHolder.cc 
     template Bool FunctionHolder<Double>::getType<Double>(String &, Function<Double> * &, RecordInterface const &) 
1130 scimath/Functionals/FunctionHolder.cc 
     template class FunctionHolder<Double> 
1000 scimath/Functionals/FunctionParam.cc 
     = casa/BasicSL/Complex.h 
     template class FunctionParam<Complex> 
     template class FunctionParam<DComplex> 
1010 scimath/Functionals/FunctionParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     = casa/BasicSL/Complex.h 
     template class FunctionParam<AutoDiff<Complex> > 
     template class FunctionParam<AutoDiff<DComplex> > 
1020 scimath/Functionals/FunctionParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = scimath/Mathematics/AutoDiffIO.h 
     template class FunctionParam<AutoDiff<Double> > 
     template class FunctionParam<AutoDiff<Float> > 
1030 scimath/Functionals/FunctionParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffIO.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/BasicSL/Complex.h 
     template class FunctionParam<AutoDiffA<Complex> > 
     template class FunctionParam<AutoDiffA<DComplex> > 
1040 scimath/Functionals/FunctionParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffIO.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class FunctionParam<AutoDiffA<Double> > 
     template class FunctionParam<AutoDiffA<Float> > 
1050 scimath/Functionals/FunctionParam.cc 
     template class FunctionParam<Double> 
     template class FunctionParam<Float> 
1000 scimath/Functionals/FunctionWrapper.cc 
     template class FunctionWrapper<AutoDiff<Double> > 
     template class FunctionWrapper<AutoDiff<Float> > 
1010 scimath/Functionals/FunctionWrapper.cc 
     template class FunctionWrapper<Double> 
     template class FunctionWrapper<Float> 
1000 scimath/Functionals/GNoiseFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GNoiseFunction<AutoDiff<DComplex> > 
     template class GNoiseFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/GNoiseFunction.cc 
     = casa/BasicSL/Complex.h 
     template class GNoiseFunction<DComplex> 
     template class GNoiseFunction<Complex> 
1020 scimath/Functionals/GNoiseFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GNoiseFunction<AutoDiff<Double> > 
     template class GNoiseFunction<AutoDiff<Float> > 
1030 scimath/Functionals/GNoiseFunction.cc 
     template class GNoiseFunction<Double> 
     template class GNoiseFunction<Float> 
1000 scimath/Functionals/GNoiseParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GNoiseParam<AutoDiff<DComplex> > 
     template class GNoiseParam<AutoDiff<Complex> > 
1010 scimath/Functionals/GNoiseParam.cc 
     = casa/BasicSL/Complex.h 
     template class GNoiseParam<DComplex> 
     template class GNoiseParam<Complex> 
1020 scimath/Functionals/GNoiseParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GNoiseParam<AutoDiff<Double> > 
     template class GNoiseParam<AutoDiff<Float> > 
1030 scimath/Functionals/GNoiseParam.cc 
     template class GNoiseParam<Double> 
     template class GNoiseParam<Float> 
1000 scimath/Functionals/Gaussian1D.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian1D<DComplex> 
     template class Gaussian1D<Complex> 
1010 scimath/Functionals/Gaussian1D.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1D<AutoDiffA<Double> > 
     template class Gaussian1D<AutoDiffA<Float> > 
1020 scimath/Functionals/Gaussian1D.cc 
     template class Gaussian1D<Double> 
     template class Gaussian1D<Float> 
1000 scimath/Functionals/Gaussian1D2.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1D<AutoDiff<DComplex> > 
     template class Gaussian1D<AutoDiff<Complex> > 
1010 scimath/Functionals/Gaussian1D2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1D<AutoDiff<Double> > 
     template class Gaussian1D<AutoDiff<Float> > 
1000 scimath/Functionals/Gaussian1DParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1DParam<AutoDiff<DComplex> > 
     template class Gaussian1DParam<AutoDiff<Complex> > 
1010 scimath/Functionals/Gaussian1DParam.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian1DParam<DComplex> 
     template class Gaussian1DParam<Complex> 
1020 scimath/Functionals/Gaussian1DParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1DParam<AutoDiff<Double> > 
     template class Gaussian1DParam<AutoDiff<Float> > 
1030 scimath/Functionals/Gaussian1DParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian1DParam<AutoDiffA<Double> > 
     template class Gaussian1DParam<AutoDiffA<Float> > 
1040 scimath/Functionals/Gaussian1DParam.cc 
     template class Gaussian1DParam<Double> 
     template class Gaussian1DParam<Float> 
1000 scimath/Functionals/Gaussian2D.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian2D<DComplex> 
     template class Gaussian2D<Complex> 
1010 scimath/Functionals/Gaussian2D.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2D<AutoDiffA<Double> > 
     template class Gaussian2D<AutoDiffA<Float> > 
1020 scimath/Functionals/Gaussian2D.cc 
     template class Gaussian2D<Double> 
     template class Gaussian2D<Float> 
1000 scimath/Functionals/Gaussian2D2.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2D<AutoDiff<DComplex> > 
     template class Gaussian2D<AutoDiff<Complex> > 
1010 scimath/Functionals/Gaussian2D2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2D<AutoDiff<Double> > 
     template class Gaussian2D<AutoDiff<Float> > 
1000 scimath/Functionals/Gaussian2DParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2DParam<AutoDiff<DComplex> > 
     template class Gaussian2DParam<AutoDiff<Complex> > 
1010 scimath/Functionals/Gaussian2DParam.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian2DParam<DComplex> 
     template class Gaussian2DParam<Complex> 
1020 scimath/Functionals/Gaussian2DParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2DParam<AutoDiff<Double> > 
     template class Gaussian2DParam<AutoDiff<Float> > 
1030 scimath/Functionals/Gaussian2DParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian2DParam<AutoDiffA<Double> > 
     template class Gaussian2DParam<AutoDiffA<Float> > 
1040 scimath/Functionals/Gaussian2DParam.cc 
     template class Gaussian2DParam<Double> 
     template class Gaussian2DParam<Float> 
1000 scimath/Functionals/Gaussian3D.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian3D<DComplex> 
     template class Gaussian3D<Complex> 
1010 scimath/Functionals/Gaussian3D.cc 
     template class Gaussian3D<Float> 
     template class Gaussian3D<Double> 
1000 scimath/Functionals/Gaussian3D2.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3D<AutoDiff<Float> > 
1010 scimath/Functionals/Gaussian3D2.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3D<AutoDiff<DComplex> > 
     template class Gaussian3D<AutoDiff<Complex> > 
1020 scimath/Functionals/Gaussian3D2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3D<AutoDiff<Double> > 
1000 scimath/Functionals/Gaussian3DParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3DParam<AutoDiff<DComplex> > 
     template class Gaussian3DParam<AutoDiff<Complex> > 
1010 scimath/Functionals/Gaussian3DParam.cc 
     = casa/BasicSL/Complex.h 
     template class Gaussian3DParam<DComplex> 
     template class Gaussian3DParam<Complex> 
1020 scimath/Functionals/Gaussian3DParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3DParam<AutoDiff<Double> > 
1030 scimath/Functionals/Gaussian3DParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Gaussian3DParam<AutoDiff<Float> > 
1040 scimath/Functionals/Gaussian3DParam.cc 
     template class Gaussian3DParam<Double> 
1050 scimath/Functionals/Gaussian3DParam.cc 
     template class Gaussian3DParam<Float> 
1000 scimath/Functionals/GaussianND.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GaussianND<AutoDiff<DComplex> > 
     template class GaussianND<AutoDiff<Complex> > 
1010 scimath/Functionals/GaussianND.cc 
     = casa/BasicSL/Complex.h 
     template class GaussianND<DComplex> 
     template class GaussianND<Complex> 
1020 scimath/Functionals/GaussianND.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GaussianND<AutoDiff<Double> > 
1030 scimath/Functionals/GaussianND.cc 
     template class GaussianND<Double> 
1000 scimath/Functionals/GaussianNDParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GaussianNDParam<AutoDiff<DComplex> > 
     template class GaussianNDParam<AutoDiff<Complex> > 
1010 scimath/Functionals/GaussianNDParam.cc 
     = casa/BasicSL/Complex.h 
     template class GaussianNDParam<DComplex> 
     template class GaussianNDParam<Complex> 
1020 scimath/Functionals/GaussianNDParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class GaussianNDParam<AutoDiff<Double> > 
1030 scimath/Functionals/GaussianNDParam.cc 
     template class GaussianNDParam<Double> 
1000 scimath/Functionals/HyperPlane.cc 
     = casa/BasicSL/Complex.h 
     template class HyperPlane<DComplex> 
     template class HyperPlane<Complex> 
1010 scimath/Functionals/HyperPlane.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlane<AutoDiffA<Double> > 
     template class HyperPlane<AutoDiffA<Float> > 
     = casa/BasicSL/Complex.h 
     template class HyperPlane<AutoDiffA<DComplex> > 
     template class HyperPlane<AutoDiffA<Complex> > 
1020 scimath/Functionals/HyperPlane.cc 
     template class HyperPlane<Double> 
     template class HyperPlane<Float> 
1000 scimath/Functionals/HyperPlane2.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlane<AutoDiff<DComplex> > 
     template class HyperPlane<AutoDiff<Complex> > 
1010 scimath/Functionals/HyperPlane2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlane<AutoDiff<Double> > 
     template class HyperPlane<AutoDiff<Float> > 
1000 scimath/Functionals/HyperPlaneParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlaneParam<AutoDiff<DComplex> > 
     template class HyperPlaneParam<AutoDiff<Complex> > 
1010 scimath/Functionals/HyperPlaneParam.cc 
     = casa/BasicSL/Complex.h 
     template class HyperPlaneParam<DComplex> 
     template class HyperPlaneParam<Complex> 
1020 scimath/Functionals/HyperPlaneParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlaneParam<AutoDiff<Double> > 
     template class HyperPlaneParam<AutoDiff<Float> > 
1030 scimath/Functionals/HyperPlaneParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class HyperPlaneParam<AutoDiffA<Double> > 
     template class HyperPlaneParam<AutoDiffA<Float> > 
     = casa/BasicSL/Complex.h 
     template class HyperPlaneParam<AutoDiffA<Complex> > 
     template class HyperPlaneParam<AutoDiffA<DComplex> > 
1040 scimath/Functionals/HyperPlaneParam.cc 
     template class HyperPlaneParam<Double> 
     template class HyperPlaneParam<Float> 
1000 scimath/Functionals/Interpolate1D.cc 
     = casa/Arrays/Array.h 
     template class Interpolate1D<Double, Array<Double> > 
1010 scimath/Functionals/Interpolate1D.cc 
     = scimath/Functionals/Function1D.h 
     template class Interpolate1D<Double, Float> 
     template class Function1D<Double, Float> 
1020 scimath/Functionals/Interpolate1D.cc 
     template class Interpolate1D<Double, Double> 
1000 scimath/Functionals/KaiserBFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBFunction<AutoDiff<DComplex> > 
     template class KaiserBFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/KaiserBFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBFunction<DComplex> 
     template class KaiserBFunction<Complex> 
1020 scimath/Functionals/KaiserBFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBFunction<AutoDiff<Double> > 
     template class KaiserBFunction<AutoDiff<Float> > 
1030 scimath/Functionals/KaiserBFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBFunction<Double> 
     template class KaiserBFunction<Float> 
1000 scimath/Functionals/KaiserBParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBParam<AutoDiff<DComplex> > 
     template class KaiserBParam<AutoDiff<Complex> > 
1010 scimath/Functionals/KaiserBParam.cc 
     = casa/BasicSL/Complex.h 
     template class KaiserBParam<DComplex> 
     template class KaiserBParam<Complex> 
1020 scimath/Functionals/KaiserBParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class KaiserBParam<AutoDiff<Double> > 
     template class KaiserBParam<AutoDiff<Float> > 
1030 scimath/Functionals/KaiserBParam.cc 
     template class KaiserBParam<Double> 
     template class KaiserBParam<Float> 
1000 scimath/Functionals/MarshButterworthBandpass.cc 
     template class MarshButterworthBandpass<Double> 
1010 scimath/Functionals/MarshButterworthBandpass.cc 
     template class MarshButterworthBandpass<Float> 
1000 scimath/Functionals/MarshallableChebyshev.cc 
     template class MarshallableChebyshev<Double> 
1010 scimath/Functionals/MarshallableChebyshev.cc 
     template class MarshallableChebyshev<Float> 
1000 scimath/Functionals/OddPolynomial.cc 
     = casa/BasicSL/Complex.h 
     template class OddPolynomial<DComplex> 
     template class OddPolynomial<Complex> 
1010 scimath/Functionals/OddPolynomial.cc 
     template class OddPolynomial<Double> 
1000 scimath/Functionals/OddPolynomial2.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class OddPolynomial<AutoDiff<DComplex> > 
     template class OddPolynomial<AutoDiff<Complex> > 
1010 scimath/Functionals/OddPolynomial2.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class OddPolynomial<AutoDiff<Double> > 
1000 scimath/Functionals/OddPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class OddPolynomialParam<AutoDiff<DComplex> > 
     template class OddPolynomialParam<AutoDiff<Complex> > 
1010 scimath/Functionals/OddPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     template class OddPolynomialParam<DComplex> 
     template class OddPolynomialParam<Complex> 
1020 scimath/Functionals/OddPolynomialParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class OddPolynomialParam<AutoDiff<Double> > 
1030 scimath/Functionals/OddPolynomialParam.cc 
     template class OddPolynomialParam<Double> 
1000 scimath/Functionals/Polynomial.cc 
     = casa/BasicSL/Complex.h 
     template class Polynomial<Complex> 
     template class Polynomial<DComplex> 
1010 scimath/Functionals/Polynomial.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Polynomial<AutoDiffA<Double> > 
     template class Polynomial<AutoDiffA<Float> > 
1020 scimath/Functionals/Polynomial.cc 
     template class Polynomial<Double> 
     template class Polynomial<Float> 
1000 scimath/Functionals/Polynomial2.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Polynomial<AutoDiff<Complex> > 
     template class Polynomial<AutoDiff<DComplex> > 
1010 scimath/Functionals/Polynomial2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Polynomial<AutoDiff<Double> > 
     template class Polynomial<AutoDiff<Float> > 
1000 scimath/Functionals/PolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class PolynomialParam<AutoDiff<Complex> > 
     template class PolynomialParam<AutoDiff<DComplex> > 
1010 scimath/Functionals/PolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     template class PolynomialParam<Complex> 
     template class PolynomialParam<DComplex> 
1020 scimath/Functionals/PolynomialParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class PolynomialParam<AutoDiff<Double> > 
     template class PolynomialParam<AutoDiff<Float> > 
1030 scimath/Functionals/PolynomialParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class PolynomialParam<AutoDiffA<Double> > 
     template class PolynomialParam<AutoDiffA<Float> > 
1040 scimath/Functionals/PolynomialParam.cc 
     template class PolynomialParam<Double> 
     template class PolynomialParam<Float> 
1000 scimath/Functionals/SPolynomial.cc 
     = casa/BasicSL/Complex.h 
     template class SPolynomial<Complex> 
     template class SPolynomial<DComplex> 
1010 scimath/Functionals/SPolynomial.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomial<AutoDiff<Complex> > 
     template class SPolynomial<AutoDiff<DComplex> > 
1020 scimath/Functionals/SPolynomial.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomial<AutoDiff<Double> > 
     template class SPolynomial<AutoDiff<Float> > 
1030 scimath/Functionals/SPolynomial.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomial<AutoDiffA<Double> > 
     template class SPolynomial<AutoDiffA<Float> > 
1040 scimath/Functionals/SPolynomial.cc 
     template class SPolynomial<Double> 
     template class SPolynomial<Float> 
1000 scimath/Functionals/SPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomialParam<AutoDiff<Complex> > 
     template class SPolynomialParam<AutoDiff<DComplex> > 
1010 scimath/Functionals/SPolynomialParam.cc 
     = casa/BasicSL/Complex.h 
     template class SPolynomialParam<Complex> 
     template class SPolynomialParam<DComplex> 
1020 scimath/Functionals/SPolynomialParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomialParam<AutoDiff<Double> > 
     template class SPolynomialParam<AutoDiff<Float> > 
1030 scimath/Functionals/SPolynomialParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SPolynomialParam<AutoDiffA<Double> > 
     template class SPolynomialParam<AutoDiffA<Float> > 
1040 scimath/Functionals/SPolynomialParam.cc 
     template class SPolynomialParam<Double> 
     template class SPolynomialParam<Float> 
1000 scimath/Functionals/SampledFunctional.h 
     = casa/Arrays/Array.h 
     template class SampledFunctional<Array<Double> > 
1010 scimath/Functionals/SampledFunctional.h 
     template class SampledFunctional<Double> 
1020 scimath/Functionals/SampledFunctional.h 
     template class SampledFunctional<Float> 
1000 scimath/Functionals/ScalarSampledFunctional.cc 
     = casa/Arrays/Array.h 
     template class ScalarSampledFunctional<Array<Double> > 
1010 scimath/Functionals/ScalarSampledFunctional.cc 
     template class ScalarSampledFunctional<Double> 
1020 scimath/Functionals/ScalarSampledFunctional.cc 
     template class ScalarSampledFunctional<Float> 
1000 scimath/Functionals/SimButterworthBandpass.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SimButterworthBandpass<AutoDiff<Complex> > 
     template class SimButterworthBandpass<AutoDiff<DComplex> > 
1010 scimath/Functionals/SimButterworthBandpass.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SimButterworthBandpass<AutoDiff<Double> > 
     template class SimButterworthBandpass<AutoDiff<Float> > 
1020 scimath/Functionals/SimButterworthBandpass.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SimButterworthBandpass<AutoDiffA<Complex> > 
     template class SimButterworthBandpass<AutoDiffA<DComplex> > 
1030 scimath/Functionals/SimButterworthBandpass.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SimButterworthBandpass<AutoDiffA<Double> > 
     template class SimButterworthBandpass<AutoDiffA<Float> > 
1040 scimath/Functionals/SimButterworthBandpass.cc 
     template class SimButterworthBandpass<Complex> 
     template class SimButterworthBandpass<DComplex> 
1050 scimath/Functionals/SimButterworthBandpass.cc 
     template class SimButterworthBandpass<Double> 
     template class SimButterworthBandpass<Float> 
1000 scimath/Functionals/SincFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincFunction<AutoDiff<DComplex> > 
     template class SincFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/SincFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincFunction<DComplex> 
     template class SincFunction<Complex> 
1020 scimath/Functionals/SincFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincFunction<AutoDiff<Double> > 
     template class SincFunction<AutoDiff<Float> > 
1030 scimath/Functionals/SincFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincFunction<Double> 
     template class SincFunction<Float> 
1000 scimath/Functionals/SincParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincParam<AutoDiff<DComplex> > 
     template class SincParam<AutoDiff<Complex> > 
1010 scimath/Functionals/SincParam.cc 
     = casa/BasicSL/Complex.h 
     template class SincParam<DComplex> 
     template class SincParam<Complex> 
1020 scimath/Functionals/SincParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class SincParam<AutoDiff<Double> > 
     template class SincParam<AutoDiff<Float> > 
1030 scimath/Functionals/SincParam.cc 
     template class SincParam<Double> 
     template class SincParam<Float> 
1000 scimath/Functionals/Sinusoid1D.cc 
     = casa/BasicSL/Complex.h 
     template class Sinusoid1D<DComplex> 
     template class Sinusoid1D<Complex> 
1010 scimath/Functionals/Sinusoid1D.cc scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1D<AutoDiffA<Double> > 
     template class Sinusoid1D<AutoDiffA<Float> > 
1020 scimath/Functionals/Sinusoid1D.cc 
     template class Sinusoid1D<Double> 
     template class Sinusoid1D<Float> 
1000 scimath/Functionals/Sinusoid1D2.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1D<AutoDiff<DComplex> > 
     template class Sinusoid1D<AutoDiff<Complex> > 
1010 scimath/Functionals/Sinusoid1D2.cc scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1D<AutoDiff<Double> > 
     template class Sinusoid1D<AutoDiff<Float> > 
1000 scimath/Functionals/Sinusoid1DParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1DParam<AutoDiff<DComplex> > 
     template class Sinusoid1DParam<AutoDiff<Complex> > 
1010 scimath/Functionals/Sinusoid1DParam.cc 
     = casa/BasicSL/Complex.h 
     template class Sinusoid1DParam<DComplex> 
     template class Sinusoid1DParam<Complex> 
1020 scimath/Functionals/Sinusoid1DParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1DParam<AutoDiff<Double> > 
     template class Sinusoid1DParam<AutoDiff<Float> > 
1030 scimath/Functionals/Sinusoid1DParam.cc 
     = scimath/Mathematics/AutoDiffA.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class Sinusoid1DParam<AutoDiffA<Double> > 
     template class Sinusoid1DParam<AutoDiffA<Float> > 
1040 scimath/Functionals/Sinusoid1DParam.cc 
     template class Sinusoid1DParam<Double> 
     template class Sinusoid1DParam<Float> 
1000 scimath/Functionals/SpecificFunctionFactory.h 
     = scimath/Functionals/MarshButterworthBandpass.h 
     template class SpecificFunctionFactory<Double, MarshButterworthBandpass<Double> > 
1010 scimath/Functionals/SpecificFunctionFactory.h 
     = scimath/Functionals/MarshButterworthBandpass.h 
     template class SpecificFunctionFactory<Float, MarshButterworthBandpass<Float> > 
1020 scimath/Functionals/SpecificFunctionFactory.h 
     = scimath/Functionals/MarshallableChebyshev.h 
     template class SpecificFunctionFactory<Double, MarshallableChebyshev<Double> > 
1030 scimath/Functionals/SpecificFunctionFactory.h 
     = scimath/Functionals/MarshallableChebyshev.h 
     template class SpecificFunctionFactory<Float, MarshallableChebyshev<Float> > 
1000 scimath/Functionals/UnaryFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryFunction<AutoDiff<DComplex> > 
     template class UnaryFunction<AutoDiff<Complex> > 
1010 scimath/Functionals/UnaryFunction.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryFunction<DComplex> 
     template class UnaryFunction<Complex> 
1020 scimath/Functionals/UnaryFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryFunction<AutoDiff<Double> > 
     template class UnaryFunction<AutoDiff<Float> > 
1030 scimath/Functionals/UnaryFunction.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryFunction<Double> 
     template class UnaryFunction<Float> 
1000 scimath/Functionals/UnaryParam.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryParam<AutoDiff<DComplex> > 
     template class UnaryParam<AutoDiff<Complex> > 
1010 scimath/Functionals/UnaryParam.cc 
     = casa/BasicSL/Complex.h 
     template class UnaryParam<DComplex> 
     template class UnaryParam<Complex> 
1020 scimath/Functionals/UnaryParam.cc 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template class UnaryParam<AutoDiff<Double> > 
     template class UnaryParam<AutoDiff<Float> > 
1030 scimath/Functionals/UnaryParam.cc 
     template class UnaryParam<Double> 
     template class UnaryParam<Float> 
1000 scimath/Functionals/WrapperBase.h 
     template class WrapperBase<AutoDiff<Double> > 
     template class WrapperBase<AutoDiff<Float> > 
1010 scimath/Functionals/WrapperBase.h 
     template class WrapperBase<Double> 
     template class WrapperBase<Float> 
1000 scimath/Functionals/WrapperData.h 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, True, True> 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, True, False> 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, False, True> 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, False, False> 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, Vector<AutoDiff<Double> >, True, True> 
     template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, Vector<AutoDiff<Double> >, False, True> 
     template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, AutoDiff<Double>, True, True> 
     template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, AutoDiff<Double>, True, False> 
     template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, Vector<AutoDiff<Double> >, True, True> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, True, True> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, True, False> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, False, True> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, False, False> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, Vector<AutoDiff<Float> >, True, True> 
     template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, Vector<AutoDiff<Float> >, False, True> 
     template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, AutoDiff<Float>, True, True> 
     template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, AutoDiff<Float>, True, False> 
     template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, Vector<AutoDiff<Float> >, True, True> 
1010 scimath/Functionals/WrapperData.h 
     template class WrapperData<Double, Double, Double, True, True> 
     template class WrapperData<Double, Double, Double, True, False> 
     template class WrapperData<Double, Double, Double, False, True> 
     template class WrapperData<Double, Double, Double, False, False> 
     template class WrapperData<Double, Double, Vector<Double>, True, True> 
     template class WrapperData<Double, Double, Vector<Double>, False, True> 
     template class WrapperData<Double, Vector<Double>, Double, True, True> 
     template class WrapperData<Double, Vector<Double>, Double, True, False> 
     template class WrapperData<Double, Vector<Double>, Vector<Double>, True, True> 
     template class WrapperData<Float, Float, Float, True, True> 
     template class WrapperData<Float, Float, Float, True, False> 
     template class WrapperData<Float, Float, Float, False, True> 
     template class WrapperData<Float, Float, Float, False, False> 
     template class WrapperData<Float, Float, Vector<Float>, True, True> 
     template class WrapperData<Float, Float, Vector<Float>, False, True> 
     template class WrapperData<Float, Vector<Float>, Float, True, True> 
     template class WrapperData<Float, Vector<Float>, Float, True, False> 
     template class WrapperData<Float, Vector<Float>, Vector<Float>, True, True> 
1000 scimath/Functionals/WrapperParam.cc 
     template class WrapperParam<AutoDiff<Double> > 
     template class WrapperParam<AutoDiff<Float> > 
1010 scimath/Functionals/WrapperParam.cc 
     template class WrapperParam<Double> 
     template class WrapperParam<Float> 
1000 scimath/Mathematics/AutoDiff.cc casa/BasicSL/Complex.h 
     template class AutoDiff<Complex> 
1010 scimath/Mathematics/AutoDiff.cc casa/BasicSL/Complex.h 
     template class AutoDiff<DComplex> 
1020 scimath/Mathematics/AutoDiff.cc 
     template class AutoDiff<Double> 
1030 scimath/Mathematics/AutoDiff.cc 
     template class AutoDiff<Float> 
1000 scimath/Mathematics/AutoDiffIO.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     template ostream & operator<<(ostream &, AutoDiff<Complex> const &) 
1010 scimath/Mathematics/AutoDiffIO.cc scimath/Mathematics/AutoDiff.h 
     = casa/BasicSL/Complex.h 
     template ostream & operator<<(ostream &, AutoDiff<DComplex> const &) 
1020 scimath/Mathematics/AutoDiffIO.cc scimath/Mathematics/AutoDiff.h 
     template ostream & operator<<(ostream &, AutoDiff<Double> const &) 
1030 scimath/Mathematics/AutoDiffIO.cc scimath/Mathematics/AutoDiff.h 
     template ostream & operator<<(ostream &, AutoDiff<Float> const &) 
1000 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Complex> acos(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> asin(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> atan(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> atan2(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> cos(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> cosh(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> exp(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> log(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> log10(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> erf(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> erfc(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> pow(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> pow(AutoDiff<Complex> const &, Complex const &) 
     template AutoDiff<Complex> sin(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> sinh(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> sqrt(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> abs(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> ceil(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> floor(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> fmod(AutoDiff<Complex> const &, Complex const &) 
     template AutoDiff<Complex> fmod(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> max(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> min(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
#     template AutoDiff<Complex> square(AutoDiff<Complex> const &) 
#     template AutoDiff<Complex> cube(AutoDiff<Complex> const &) 
#     template AutoDiff<Complex> tan(AutoDiff<Complex> const &) 
#     template AutoDiff<Complex> tanh(AutoDiff<Complex> const &) 
1010 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Complex> operator+(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator-(AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator*(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator+(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator-(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator/(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator*(Complex const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator+(Complex const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator-(Complex const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator/(Complex const &, AutoDiff<Complex> const &) 
     template AutoDiff<Complex> operator*(AutoDiff<Complex> const &, Complex const &) 
     template AutoDiff<Complex> operator+(AutoDiff<Complex> const &, Complex const &) 
     template AutoDiff<Complex> operator-(AutoDiff<Complex> const &, Complex const &) 
     template AutoDiff<Complex> operator/(AutoDiff<Complex> const &, Complex const &) 
1020 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<DComplex> acos(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> asin(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> atan(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> atan2(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> cos(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> cosh(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> exp(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> log(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> log10(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> erf(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> erfc(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> pow(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> pow(AutoDiff<DComplex> const &, DComplex const &) 
     template AutoDiff<DComplex> sin(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> sinh(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> sqrt(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> abs(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> ceil(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> floor(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> fmod(AutoDiff<DComplex> const &, DComplex const &) 
     template AutoDiff<DComplex> fmod(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> max(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> min(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
#     template AutoDiff<DComplex> square(AutoDiff<DComplex> const &) 
#     template AutoDiff<DComplex> cube(AutoDiff<DComplex> const &) 
#     template AutoDiff<DComplex> tan(AutoDiff<DComplex> const &) 
#     template AutoDiff<DComplex> tanh(AutoDiff<DComplex> const &) 
1030 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<DComplex> operator+(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator-(AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator*(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator+(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator-(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator/(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator*(DComplex const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator+(DComplex const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator-(DComplex const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator/(DComplex const &, AutoDiff<DComplex> const &) 
     template AutoDiff<DComplex> operator*(AutoDiff<DComplex> const &, DComplex const &) 
     template AutoDiff<DComplex> operator+(AutoDiff<DComplex> const &, DComplex const &) 
     template AutoDiff<DComplex> operator-(AutoDiff<DComplex> const &, DComplex const &) 
     template AutoDiff<DComplex> operator/(AutoDiff<DComplex> const &, DComplex const &) 
1040 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template Bool operator!=(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator<=(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator<(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator==(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator>(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator>=(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool near(AutoDiff<Complex> const &, AutoDiff<Complex> const &) 
     template Bool operator!=(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator<=(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator<(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator==(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator>(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator>=(AutoDiff<Complex> const &, Complex const &) 
     template Bool near(AutoDiff<Complex> const &, Complex const &) 
     template Bool operator!=(Complex const &, AutoDiff<Complex> const &) 
     template Bool operator<=(Complex const &, AutoDiff<Complex> const &) 
     template Bool operator<(Complex const &, AutoDiff<Complex> const &) 
     template Bool operator==(Complex const &, AutoDiff<Complex> const &) 
     template Bool operator>(Complex const &, AutoDiff<Complex> const &) 
     template Bool operator>=(Complex const &, AutoDiff<Complex> const &) 
     template Bool near(Complex const &, AutoDiff<Complex> const &) 
1050 scimath/Mathematics/AutoDiffMath.cc 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     template Bool operator!=(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator<=(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator<(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator==(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator>(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator>=(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool near(AutoDiff<DComplex> const &, AutoDiff<DComplex> const &) 
     template Bool operator!=(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator<=(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator<(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator==(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator>(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator>=(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool near(AutoDiff<DComplex> const &, DComplex const &) 
     template Bool operator!=(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool operator<=(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool operator<(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool operator==(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool operator>(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool operator>=(DComplex const &, AutoDiff<DComplex> const &) 
     template Bool near(DComplex const &, AutoDiff<DComplex> const &) 
1060 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Double> acos(AutoDiff<Double> const &) 
     template AutoDiff<Double> asin(AutoDiff<Double> const &) 
     template AutoDiff<Double> atan(AutoDiff<Double> const &) 
     template AutoDiff<Double> atan2(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> cos(AutoDiff<Double> const &) 
     template AutoDiff<Double> cosh(AutoDiff<Double> const &) 
     template AutoDiff<Double> exp(AutoDiff<Double> const &) 
     template AutoDiff<Double> log(AutoDiff<Double> const &) 
     template AutoDiff<Double> log10(AutoDiff<Double> const &) 
     template AutoDiff<Double> erf(AutoDiff<Double> const &) 
     template AutoDiff<Double> erfc(AutoDiff<Double> const &) 
     template AutoDiff<Double> pow(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> pow(AutoDiff<Double> const &, Double const &) 
     template AutoDiff<Double> square(AutoDiff<Double> const &) 
     template AutoDiff<Double> cube(AutoDiff<Double> const &) 
     template AutoDiff<Double> sin(AutoDiff<Double> const &) 
     template AutoDiff<Double> sinh(AutoDiff<Double> const &) 
     template AutoDiff<Double> sqrt(AutoDiff<Double> const &) 
     template AutoDiff<Double> tan(AutoDiff<Double> const &) 
     template AutoDiff<Double> tanh(AutoDiff<Double> const &) 
     template AutoDiff<Double> abs(AutoDiff<Double> const &) 
     template AutoDiff<Double> ceil(AutoDiff<Double> const &) 
     template AutoDiff<Double> floor(AutoDiff<Double> const &) 
     template AutoDiff<Double> fmod(AutoDiff<Double> const &, Double const &) 
     template AutoDiff<Double> fmod(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> max(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> min(AutoDiff<Double> const &, AutoDiff<Double> const &) 
1070 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Double> operator+(AutoDiff<Double> const &) 
     template AutoDiff<Double> operator-(AutoDiff<Double> const &) 
     template AutoDiff<Double> operator*(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator+(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator-(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator/(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator*(Double const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator+(Double const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator-(Double const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator/(Double const &, AutoDiff<Double> const &) 
     template AutoDiff<Double> operator*(AutoDiff<Double> const &, Double const &) 
     template AutoDiff<Double> operator+(AutoDiff<Double> const &, Double const &) 
     template AutoDiff<Double> operator-(AutoDiff<Double> const &, Double const &) 
     template AutoDiff<Double> operator/(AutoDiff<Double> const &, Double const &) 
1080 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Float> acos(AutoDiff<Float> const &) 
     template AutoDiff<Float> asin(AutoDiff<Float> const &) 
     template AutoDiff<Float> atan(AutoDiff<Float> const &) 
     template AutoDiff<Float> atan2(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> cos(AutoDiff<Float> const &) 
     template AutoDiff<Float> cosh(AutoDiff<Float> const &) 
     template AutoDiff<Float> exp(AutoDiff<Float> const &) 
     template AutoDiff<Float> log(AutoDiff<Float> const &) 
     template AutoDiff<Float> log10(AutoDiff<Float> const &) 
     template AutoDiff<Float> erf(AutoDiff<Float> const &) 
     template AutoDiff<Float> erfc(AutoDiff<Float> const &) 
     template AutoDiff<Float> pow(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> pow(AutoDiff<Float> const &, Float const &) 
     template AutoDiff<Float> square(AutoDiff<Float> const &) 
     template AutoDiff<Float> cube(AutoDiff<Float> const &) 
     template AutoDiff<Float> sin(AutoDiff<Float> const &) 
     template AutoDiff<Float> sinh(AutoDiff<Float> const &) 
     template AutoDiff<Float> sqrt(AutoDiff<Float> const &) 
     template AutoDiff<Float> tan(AutoDiff<Float> const &) 
     template AutoDiff<Float> tanh(AutoDiff<Float> const &) 
     template AutoDiff<Float> abs(AutoDiff<Float> const &) 
     template AutoDiff<Float> ceil(AutoDiff<Float> const &) 
     template AutoDiff<Float> floor(AutoDiff<Float> const &) 
     template AutoDiff<Float> fmod(AutoDiff<Float> const &, Float const &) 
     template AutoDiff<Float> fmod(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> max(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> min(AutoDiff<Float> const &, AutoDiff<Float> const &) 
1090 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template AutoDiff<Float> operator+(AutoDiff<Float> const &) 
     template AutoDiff<Float> operator-(AutoDiff<Float> const &) 
     template AutoDiff<Float> operator*(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator+(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator-(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator/(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator*(Float const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator+(Float const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator-(Float const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator/(Float const &, AutoDiff<Float> const &) 
     template AutoDiff<Float> operator*(AutoDiff<Float> const &, Float const &) 
     template AutoDiff<Float> operator+(AutoDiff<Float> const &, Float const &) 
     template AutoDiff<Float> operator-(AutoDiff<Float> const &, Float const &) 
     template AutoDiff<Float> operator/(AutoDiff<Float> const &, Float const &) 
1100 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template Bool near(AutoDiff<Double> const &, AutoDiff<Double> const &, Double const) 
     template Bool near(AutoDiff<Double> const &, Double const &, Double const) 
     template Bool near(Double const &, AutoDiff<Double> const &, Double const) 
     template Bool allnear(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool allnear(AutoDiff<Double> const &, Double const &) 
     template Bool allnear(Double const &, AutoDiff<Double> const &) 
     template Bool allnear(AutoDiff<Double> const &, AutoDiff<Double> const &, Double const) 
     template Bool allnear(AutoDiff<Double> const &, Double const &, Double const) 
     template Bool allnear(Double const &, AutoDiff<Double> const &, Double const) 
     template Bool nearAbs(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool nearAbs(AutoDiff<Double> const &, Double const &) 
     template Bool nearAbs(Double const &, AutoDiff<Double> const &) 
     template Bool nearAbs(AutoDiff<Double> const &, AutoDiff<Double> const &, Double const) 
     template Bool nearAbs(AutoDiff<Double> const &, Double const &, Double const) 
     template Bool nearAbs(Double const &, AutoDiff<Double> const &, Double const) 
     template Bool allnearAbs(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool allnearAbs(AutoDiff<Double> const &, Double const &) 
     template Bool allnearAbs(Double const &, AutoDiff<Double> const &) 
     template Bool allnearAbs(AutoDiff<Double> const &, AutoDiff<Double> const &, Double const) 
     template Bool allnearAbs(AutoDiff<Double> const &, Double const &, Double const) 
     template Bool allnearAbs(Double const &, AutoDiff<Double> const &, Double const) 
1110 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template Bool near(AutoDiff<Float> const &, AutoDiff<Float> const &, Double const) 
     template Bool near(AutoDiff<Float> const &, Float const &, Double const) 
     template Bool near(Float const &, AutoDiff<Float> const &, Double const) 
     template Bool allnear(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool allnear(AutoDiff<Float> const &, Float const &) 
     template Bool allnear(Float const &, AutoDiff<Float> const &) 
     template Bool allnear(AutoDiff<Float> const &, AutoDiff<Float> const &, Double const) 
     template Bool allnear(AutoDiff<Float> const &, Float const &, Double const) 
     template Bool allnear(Float const &, AutoDiff<Float> const &, Double const) 
     template Bool nearAbs(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool nearAbs(AutoDiff<Float> const &, Float const &) 
     template Bool nearAbs(Float const &, AutoDiff<Float> const &) 
     template Bool nearAbs(AutoDiff<Float> const &, AutoDiff<Float> const &, Double const) 
     template Bool nearAbs(AutoDiff<Float> const &, Float const &, Double const) 
     template Bool nearAbs(Float const &, AutoDiff<Float> const &, Double const) 
     template Bool allnearAbs(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool allnearAbs(AutoDiff<Float> const &, Float const &) 
     template Bool allnearAbs(Float const &, AutoDiff<Float> const &) 
     template Bool allnearAbs(AutoDiff<Float> const &, AutoDiff<Float> const &, Double const) 
     template Bool allnearAbs(AutoDiff<Float> const &, Float const &, Double const) 
     template Bool allnearAbs(Float const &, AutoDiff<Float> const &, Double const) 
1120 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template Bool operator!=(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator<=(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator<(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator==(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator>(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator>=(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool near(AutoDiff<Double> const &, AutoDiff<Double> const &) 
     template Bool operator!=(AutoDiff<Double> const &, Double const &) 
     template Bool operator<=(AutoDiff<Double> const &, Double const &) 
     template Bool operator<(AutoDiff<Double> const &, Double const &) 
     template Bool operator==(AutoDiff<Double> const &, Double const &) 
     template Bool operator>(AutoDiff<Double> const &, Double const &) 
     template Bool operator>=(AutoDiff<Double> const &, Double const &) 
     template Bool near(AutoDiff<Double> const &, Double const &) 
     template Bool operator!=(Double const &, AutoDiff<Double> const &) 
     template Bool operator<=(Double const &, AutoDiff<Double> const &) 
     template Bool operator<(Double const &, AutoDiff<Double> const &) 
     template Bool operator==(Double const &, AutoDiff<Double> const &) 
     template Bool operator>(Double const &, AutoDiff<Double> const &) 
     template Bool operator>=(Double const &, AutoDiff<Double> const &) 
     template Bool near(Double const &, AutoDiff<Double> const &) 
1130 scimath/Mathematics/AutoDiffMath.cc 
     = scimath/Mathematics/AutoDiff.h 
     template Bool operator!=(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator<=(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator<(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator==(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator>(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator>=(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool near(AutoDiff<Float> const &, AutoDiff<Float> const &) 
     template Bool operator!=(AutoDiff<Float> const &, Float const &) 
     template Bool operator<=(AutoDiff<Float> const &, Float const &) 
     template Bool operator<(AutoDiff<Float> const &, Float const &) 
     template Bool operator==(AutoDiff<Float> const &, Float const &) 
     template Bool operator>(AutoDiff<Float> const &, Float const &) 
     template Bool operator>=(AutoDiff<Float> const &, Float const &) 
     template Bool near(AutoDiff<Float> const &, Float const &) 
     template Bool operator!=(Float const &, AutoDiff<Float> const &) 
     template Bool operator<=(Float const &, AutoDiff<Float> const &) 
     template Bool operator<(Float const &, AutoDiff<Float> const &) 
     template Bool operator==(Float const &, AutoDiff<Float> const &) 
     template Bool operator>(Float const &, AutoDiff<Float> const &) 
     template Bool operator>=(Float const &, AutoDiff<Float> const &) 
     template Bool near(Float const &, AutoDiff<Float> const &) 
1000 scimath/Mathematics/AutoDiffRep.cc 
     = casa/BasicSL/Complex.h 
     template class AutoDiffRep<Complex> 
1010 scimath/Mathematics/AutoDiffRep.cc 
     = casa/BasicSL/Complex.h 
     template class AutoDiffRep<DComplex> 
1020 scimath/Mathematics/AutoDiffRep.cc 
     template class AutoDiffRep<Double> 
1030 scimath/Mathematics/AutoDiffRep.cc 
     template class AutoDiffRep<Float> 
1000 scimath/Mathematics/ConvolveGridder.cc 
     = casa/BasicSL/Complex.h 
     template class ConvolveGridder<Double, Complex> 
1000 scimath/Mathematics/Convolver.cc 
     template class Convolver<Float> 
1000 scimath/Mathematics/FFTServer.cc casa/BasicSL/Complex.h 
     template class FFTServer<Float, Complex> 
     template class FFTServer<Double, DComplex>
1000 scimath/Mathematics/Gridder.cc casa/BasicSL/Complex.h 
     template class Gridder<Double, Complex> 
1000 scimath/Mathematics/Interpolate2D2.cc 
     template Bool Interpolate2D::interpCubic<Double>(Double &result, Vector<Double> const &where, Matrix<Double> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpLinear<Double>(Double &result, Vector<Double> const &where, Matrix<Double> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpNearest<Double>(Double &result, Vector<Double> const &where, Matrix<Double> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpLinear2<Double>(Double &resultI, Double &resultJ, Vector<Double> const &where, Matrix<Double> const &dataI, Matrix<Double> const &dataJ, Matrix<Bool> const &mask) const 
1010 scimath/Mathematics/Interpolate2D2.cc 
     template Bool Interpolate2D::interpCubic<Float>(Float &result, Vector<Double> const &where, Matrix<Float> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpLinear<Float>(Float &result, Vector<Double> const &where, Matrix<Float> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpNearest<Float>(Float &result, Vector<Double> const &where, Matrix<Float> const &data, Matrix<Bool> const* &maskPtr) const 
     template Bool Interpolate2D::interpLinear2<Float>(Float &resultI, Float &resultJ, Vector<Double> const &where, Matrix<Float> const &dataI, Matrix<Float> const &dataJ, Matrix<Bool> const &mask) const 
1000 scimath/Mathematics/InterpolateArray1D.cc 
     = casa/BasicSL/Complex.h 
     template class InterpolateArray1D<Float, Complex> 
1010 scimath/Mathematics/InterpolateArray1D.cc 
     template class InterpolateArray1D<Float, Float> 
1000 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<Double> &, Vector<Double> &) 
     template void CholeskySolve(Matrix<Double> &, Vector<Double> &, Vector<Double> &, Vector<Double> &) 
1010 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<AutoDiff<Complex> > &, Vector<AutoDiff<Complex> > &) 
     template void CholeskySolve(Matrix<AutoDiff<Complex> > &, Vector<AutoDiff<Complex> > &, Vector<AutoDiff<Complex> > &, Vector<AutoDiff<Complex> > &) 
1020 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<AutoDiff<DComplex> > &, Vector<AutoDiff<DComplex> > &) 
     template void CholeskySolve(Matrix<AutoDiff<DComplex> > &, Vector<AutoDiff<DComplex> > &, Vector<AutoDiff<DComplex> > &, Vector<AutoDiff<DComplex> > &) 
1030 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<Complex> &, Vector<Complex> &) 
     template void CholeskySolve(Matrix<Complex> &, Vector<Complex> &, Vector<Complex> &, Vector<Complex> &) 
1040 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<DComplex> &, Vector<DComplex> &) 
     template void CholeskySolve(Matrix<DComplex> &, Vector<DComplex> &, Vector<DComplex> &, Vector<DComplex> &) 
1050 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<AutoDiff<Complex> > invertSymPosDef(Matrix<AutoDiff<Complex> > const &) 
     template void invertSymPosDef(Matrix<AutoDiff<Complex> > &, AutoDiff<Complex> &, Matrix<AutoDiff<Complex> > const &) 
1060 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<AutoDiff<DComplex> > invertSymPosDef(Matrix<AutoDiff<DComplex> > const &) 
     template void invertSymPosDef(Matrix<AutoDiff<DComplex> > &, AutoDiff<DComplex> &, Matrix<AutoDiff<DComplex> > const &) 
1070 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<Complex> invertSymPosDef(Matrix<Complex> const &) 
     template void invertSymPosDef(Matrix<Complex> &, Complex &, Matrix<Complex> const &) 
1080 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = casa/BasicSL/Complex.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<DComplex> invertSymPosDef(Matrix<DComplex> const &) 
     template void invertSymPosDef(Matrix<DComplex> &, DComplex &, Matrix<DComplex> const &) 
1090 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<AutoDiff<Double> > &, Vector<AutoDiff<Double> > &) 
     template void CholeskySolve(Matrix<AutoDiff<Double> > &, Vector<AutoDiff<Double> > &, Vector<AutoDiff<Double> > &, Vector<AutoDiff<Double> > &) 
1100 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     = casa/Arrays/Vector.h 
     template void CholeskyDecomp(Matrix<AutoDiff<Float> > &, Vector<AutoDiff<Float> > &) 
     template void CholeskySolve(Matrix<AutoDiff<Float> > &, Vector<AutoDiff<Float> > &, Vector<AutoDiff<Float> > &, Vector<AutoDiff<Float> > &) 
1110 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<AutoDiff<Double> > invertSymPosDef(Matrix<AutoDiff<Double> > const &) 
     template void invertSymPosDef(Matrix<AutoDiff<Double> > &, AutoDiff<Double> &, Matrix<AutoDiff<Double> > const &) 
1120 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     = scimath/Mathematics/AutoDiff.h 
     = scimath/Mathematics/AutoDiffMath.h 
     template Matrix<AutoDiff<Float> > invertSymPosDef(Matrix<AutoDiff<Float> > const &) 
     template void invertSymPosDef(Matrix<AutoDiff<Float> > &, AutoDiff<Float> &, Matrix<AutoDiff<Float> > const &) 
1130 scimath/Mathematics/MatrixMathLA.cc 
     = casa/Arrays/Matrix.h 
     template Matrix<Double> invertSymPosDef(Matrix<Double> const &) 
     template void invertSymPosDef(Matrix<Double> &, Double &, Matrix<Double> const &) 
1140 scimath/Mathematics/MatrixMathLA.cc 
     = casa/BasicSL/Complex.h 
     template Matrix<Complex> invert(Matrix<Complex> const &) 
1150 scimath/Mathematics/MatrixMathLA.cc 
     = casa/BasicSL/Complex.h 
     template void invert(Matrix<Complex> &, Complex &, Matrix<Complex> const &) 
1160 scimath/Mathematics/MatrixMathLA.cc 
     template Matrix<Double> invert(Matrix<Double> const &) 
1170 scimath/Mathematics/MatrixMathLA.cc 
     template Matrix<Float> invert(Matrix<Float> const &) 
1180 scimath/Mathematics/MatrixMathLA.cc 
     template void invert(Matrix<Double> &, Double &, Matrix<Double> const &) 
1190 scimath/Mathematics/MatrixMathLA.cc 
     template void invert(Matrix<Float> &, Float &, Matrix<Float> const &) 
1000 scimath/Mathematics/RigidVector.cc casa/BasicSL/Complex.h 
     template class RigidVector<Complex, 2> 
1010 scimath/Mathematics/RigidVector.cc casa/BasicSL/Complex.h 
     template class RigidVector<Complex, 4> 
1020 scimath/Mathematics/RigidVector.cc 
     template class RigidVector<Double, 3> 
     template class RigidVector<Double, 2> 
1030 scimath/Mathematics/RigidVector.cc 
     template class RigidVector<Float, 4> 
1000 scimath/Mathematics/SquareMatrix.cc 
     = casa/BasicSL/Complex.h 
     template class SquareMatrix<Complex, 2> 
1010 scimath/Mathematics/SquareMatrix.cc 
     = casa/BasicSL/Complex.h 
     template class SquareMatrix<Complex, 4> 
1020 scimath/Mathematics/SquareMatrix.cc 
     template class SquareMatrix<Float, 2> 
     template class SquareMatrix<Double, 2> 
1030 scimath/Mathematics/SquareMatrix.cc 
     template class SquareMatrix<Float, 4> 
1000 scimath/Mathematics/StatAcc.cc 
     template class StatAcc<Double> 

}
