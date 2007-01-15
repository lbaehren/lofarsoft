
// include of header files 

#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffMath.h>

// include of implementation files

#include <casa/Arrays/Matrix.cc>
#include <casa/Arrays/Vector.cc>
#include <scimath/Fitting/FitGaussian.cc>
#include <scimath/Fitting/GenericL2Fit.cc>
#include <scimath/Fitting/LSQaips.cc>
#include <scimath/Fitting/LinearFit.cc>
#include <scimath/Fitting/LinearFitSVD.cc>
#include <scimath/Fitting/NonLinearFit.cc>
#include <scimath/Fitting/NonLinearFitLM.cc>

namespace casa {

  // casa/Arrays/Matrix.cc
  template class Matrix<Float>;
  template class Matrix<Double>;

  // casa/Arrays/Vector.cc
  template class Vector<Float>;
  template class Vector<Double>;

  // scimath/Fitting/FitGaussian.cc 
  template class FitGaussian<Float>;
  template class FitGaussian<Double>;

  // scimath/Fitting/GenericL2Fit.cc 
  template class GenericL2Fit<AutoDiff<Complex> >;
  template class GenericL2Fit<AutoDiffA<Complex> >;
  template class GenericL2Fit<AutoDiff<DComplex> >;
  template class GenericL2Fit<AutoDiffA<DComplex> >;
  template class GenericL2Fit<AutoDiff<Double> >;
  template class GenericL2Fit<AutoDiff<Float> >;
  template class GenericL2Fit<AutoDiffA<Float> >;
  template class GenericL2Fit<Complex>;
  template class GenericL2Fit<DComplex>;
  template class GenericL2Fit<Double>;
  template class GenericL2Fit<Float>;

// 1000 scimath/Fitting/LSQFit2.cc 
//      = algorithm 
//      #namespace 
//      #ifndef AIPS_INTELCC 
//        template casa::Double *swap_ranges<casa::Double *, casa::Double *>(casa::Double *, casa::Double *, casa::Double *) 
//      #endif 
// 1010 scimath/Fitting/LSQFit2.cc 
//      = casa/Arrays/VectorSTLIterator.h 
//      template void LSQFit::makeNorm<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Double, VectorSTLIterator<Double> >(VectorSTLIterator<Double> const &, Double const &) 
//      template Bool LSQFit::addConstraint<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, VectorSTLIterator<Double> >(uInt n, VectorSTLIterator<Double> const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, VectorSTLIterator<Double>, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Double> const &, Double const &) 
//      template Bool LSQFit::getConstraint<VectorSTLIterator<Double> >(uInt, VectorSTLIterator<Double> &) const 
//      template void LSQFit::solve<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &) 
//      template Bool LSQFit::solveLoop<VectorSTLIterator<Double> >(Double &, uInt &, VectorSTLIterator<Double> &, Bool) 
//      template Bool LSQFit::solveLoop<VectorSTLIterator<Double> >(uInt &, VectorSTLIterator<Double> &, Bool) 
//      template void LSQFit::copy<VectorSTLIterator<Double> >(Double const *, Double const *, VectorSTLIterator<Double> &, LSQReal) 
//      template void LSQFit::uncopy<VectorSTLIterator<Double> >(Double *, Double const *, VectorSTLIterator<Double> &, LSQReal) 
//      template void LSQFit::copyDiagonal<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &, LSQReal) 
//      template Bool LSQFit::getErrors<VectorSTLIterator<Double> >(VectorSTLIterator<Double> &) 
// 1020 scimath/Fitting/LSQFit2.cc 
//      = casa/Arrays/VectorSTLIterator.h 
//      template void LSQFit::makeNorm<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &, Float const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &, Float const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Float, VectorSTLIterator<Float> >(VectorSTLIterator<Float> const &, Float const &) 
//      template Bool LSQFit::addConstraint<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &) 
//      template Bool LSQFit::setConstraint<Float, VectorSTLIterator<Float> >(uInt n, VectorSTLIterator<Float> const &, Float const &) 
//      template Bool LSQFit::setConstraint<Float, VectorSTLIterator<Float>, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, VectorSTLIterator<Float> const &, Float const &) 
//      template Bool LSQFit::getConstraint<VectorSTLIterator<Float> >(uInt, VectorSTLIterator<Float> &) const 
//      template void LSQFit::solve<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &) 
//      template Bool LSQFit::solveLoop<VectorSTLIterator<Float> >(Double &, uInt &, VectorSTLIterator<Float> &, Bool) 
//      template Bool LSQFit::solveLoop<VectorSTLIterator<Float> >(uInt &, VectorSTLIterator<Float> &, Bool) 
//      template void LSQFit::copy<VectorSTLIterator<Float> >(Double const *, Double const *, VectorSTLIterator<Float> &, LSQReal) 
//      template void LSQFit::uncopy<VectorSTLIterator<Float> >(Double *, Double const *, VectorSTLIterator<Float> &, LSQReal) 
//      template void LSQFit::copyDiagonal<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &, LSQReal) 
//      template Bool LSQFit::getErrors<VectorSTLIterator<Float> >(VectorSTLIterator<Float> &) 
// 1030 scimath/Fitting/LSQFit2.cc 
//      = casa/Arrays/VectorSTLIterator.h 
//      typedef VectorSTLIterator<std::complex<Double> > It1 
//      typedef VectorSTLIterator<uInt> It1Int 
//      template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1>(It1 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Double, It1>(It1 const &, std::complex<Double> const &) 
//      template Bool LSQFit::addConstraint<Double, It1, It1Int>(uInt nIndex, It1Int const &, It1 const &, std::complex<Double> const &) 
//      template Bool LSQFit::setConstraint<Double, It1>(uInt n, It1 const &, std::complex<Double> const &) 
//      template Bool LSQFit::setConstraint<Double, It1, It1Int>(uInt n, uInt nIndex, It1Int const &, It1 const &, std::complex<Double> const &) 
//      template Bool LSQFit::getConstraint<It1>(uInt, It1 &) const 
//      template void LSQFit::solve<It1>(It1 &) 
//      template Bool LSQFit::solveLoop<It1>(Double &, uInt &, It1 &, Bool) 
//      template Bool LSQFit::solveLoop<It1>(uInt &, It1 &, Bool) 
//      template void LSQFit::copy<It1>(Double const *, Double const *, It1 &, LSQComplex) 
//      template void LSQFit::uncopy<It1>(Double *, Double const *, It1 &, LSQComplex) 
//      template void LSQFit::copyDiagonal<It1>(It1 &, LSQComplex) 
//      template Bool LSQFit::getErrors<It1>(It1 &) 
// 1040 scimath/Fitting/LSQFit2.cc 
//      = casa/Arrays/VectorSTLIterator.h 
//      typedef VectorSTLIterator<std::complex<Float> > It2 
//      typedef VectorSTLIterator<uInt> It2Int 
//      template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2>(It2 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Float, It2>(It2 const &, std::complex<Float> const &) 
//      template Bool LSQFit::addConstraint<Float, It2, It2Int>(uInt nIndex, It2Int const &, It2 const &, std::complex<Float> const &) 
//      template Bool LSQFit::setConstraint<Float, It2>(uInt n, It2 const &, std::complex<Float> const &) 
//      template Bool LSQFit::setConstraint<Float, It2, It2Int>(uInt n, uInt nIndex, It2Int const &, It2 const &, std::complex<Float> const &) 
//      template Bool LSQFit::getConstraint<It2>(uInt, It2 &) const 
//      template void LSQFit::solve<It2>(It2 &) 
//      template Bool LSQFit::solveLoop<It2>(Double &, uInt &, It2 &, Bool) 
//      template Bool LSQFit::solveLoop<It2>(uInt &, It2 &, Bool) 
//      template void LSQFit::copy<It2>(Double const *, Double const *, It2 &, LSQComplex) 
//      template void LSQFit::uncopy<It2>(Double *, Double const *, It2 &, LSQComplex) 
//      template void LSQFit::copyDiagonal<It2>(It2 &, LSQComplex) 
//      template Bool LSQFit::getErrors<It2>(It2 &) 
// 1050 scimath/Fitting/LSQFit2.cc 
//      = complex 
//      typedef std::complex<Double>* It3 
//      typedef uInt* It3Int 
//      template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Double, It3>(It3 const &, std::complex<Double> const &) 
//      template Bool LSQFit::addConstraint<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &) 
//      template Bool LSQFit::setConstraint<Double, It3>(uInt n, It3 const &, std::complex<Double> const &) 
//      template Bool LSQFit::setConstraint<Double, It3, It3Int>(uInt n, uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &) 
//      template Bool LSQFit::getConstraint<Double>(uInt, std::complex<Double> *) const 
//      template void LSQFit::solve<Double>(std::complex<Double> *) 
//      template Bool LSQFit::solveLoop<Double>(Double &, uInt &, std::complex<Double> *, Bool) 
//      template Bool LSQFit::solveLoop<Double>(uInt &, std::complex<Double> *, Bool) 
//      template void LSQFit::copy<std::complex<Double> >(Double const *, Double const *, std::complex<Double> *, LSQComplex) 
//      template void LSQFit::uncopy<std::complex<Double> >(Double *, Double const *, std::complex<Double> *, LSQComplex) 
//      template Bool LSQFit::getErrors<Double>(std::complex<Double> *) 
//      template Bool LSQFit::getCovariance<Double>(std::complex<Double> *) 
// 1060 scimath/Fitting/LSQFit2.cc 
//      = complex 
//      typedef std::complex<Float>* It4 
//      typedef uInt* It4Int 
//      template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Float, It4>(It4 const &, std::complex<Float> const &) 
//      template Bool LSQFit::addConstraint<Float, It4, It4Int>(uInt nIndex, It4Int const &, It4 const &, std::complex<Float> const &) 
//      template Bool LSQFit::setConstraint<Float, It4>(uInt n, It4 const &, std::complex<Float> const &) 
//      template Bool LSQFit::setConstraint<Float, It4, It4Int>(uInt n, uInt nIndex, It4Int const &, It4 const &, std::complex<Float> const &) 
//      template Bool LSQFit::getConstraint<Float>(uInt, std::complex<Float> *) const 
//      template void LSQFit::solve<Float>(std::complex<Float> *) 
//      template Bool LSQFit::solveLoop<Float>(Double &, uInt &, std::complex<Float> *, Bool) 
//      template Bool LSQFit::solveLoop<Float>(uInt &, std::complex<Float> *, Bool) 
//      template void LSQFit::copy<std::complex<Float> >(Double const *, Double const *, std::complex<Float> *, LSQComplex) 
//      template void LSQFit::uncopy<std::complex<Float> >(Double *, Double const *, std::complex<Float> *, LSQComplex) 
//      template Bool LSQFit::getErrors<Float>(std::complex<Float> *) 
//      template Bool LSQFit::getCovariance<Float>(std::complex<Float> *) 
// 1070 scimath/Fitting/LSQFit2.cc 
//      typedef Double* It5 
//      typedef uInt* It5Int 
//      template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Double, It5>(It5 const &, Double const &) 
//      template Bool LSQFit::addConstraint<Double, It5, It5Int>(uInt nIndex, It5Int const &, It5 const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, It5>(uInt n, It5 const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, It5, It5Int>(uInt n, uInt nIndex, It5Int const &, It5 const &, Double const &) 
//      template Bool LSQFit::getConstraint<Double>(uInt, Double *) const 
//      template void LSQFit::solve<Double>(Double *) 
//      template Bool LSQFit::solveLoop<Double>(Double &, uInt &, Double *, Bool) 
//      template Bool LSQFit::solveLoop<Double>(uInt &, Double *, Bool) 
//      template void LSQFit::copy<Double>(Double const *, Double const *, Double *, LSQReal) 
//      template void LSQFit::uncopy<Double>(Double *, Double const *, Double *, LSQReal) 
//      template Bool LSQFit::getErrors<Double>(Double *) 
//      template Bool LSQFit::getCovariance<Double>(Double *) 
// 1080 scimath/Fitting/LSQFit2.cc 
//      typedef Float* It6 
//      typedef uInt* It6Int 
//      template void LSQFit::makeNorm<Float, It6>(It6 const &, Float const &, Float const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It6>(It6 const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &, Float const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &, Float const &, LSQFit::Real, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Float, It6>(It6 const &, Float const &) 
//      template Bool LSQFit::addConstraint<Float, It6, It6Int>(uInt nIndex, It6Int const &, It6 const &, Float const &) 
//      template Bool LSQFit::setConstraint<Float, It6>(uInt n, It6 const &, Float const &) 
//      template Bool LSQFit::setConstraint<Float, It6, It6Int>(uInt n, uInt nIndex, It6Int const &, It6 const &, Float const &) 
//      template Bool LSQFit::getConstraint<Float>(uInt, Float *) const 
//      template void LSQFit::solve<Float>(Float *) 
//      template Bool LSQFit::solveLoop<Float>(Double &, uInt &, Float *, Bool) 
//      template Bool LSQFit::solveLoop<Float>(uInt &, Float *, Bool) 
//      template void LSQFit::copy<Float>(Double const *, Double const *, Float *, LSQReal) 
//      template void LSQFit::uncopy<Float>(Double *, Double const *, Float *, LSQReal) 
//      template Bool LSQFit::getErrors<Float>(Float *) 
//      template Bool LSQFit::getCovariance<Float>(Float *) 
// 1090 scimath/Fitting/LSQFit2.cc 
//      typedef Float* It7 
//      typedef uInt* It7Int 
//      template void LSQFit::makeNorm<Double, It7>(It7 const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It7>(It7 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &, Double const &, Bool, Bool) 
//      template void LSQFit::makeNorm<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool) 
//      template Bool LSQFit::addConstraint<Double, It7>(It7 const &, Double const &) 
//      template Bool LSQFit::addConstraint<Double, It7, It7Int>(uInt nIndex, It7Int const &, It7 const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, It7>(uInt n, It7 const &, Double const &) 
//      template Bool LSQFit::setConstraint<Double, It7, It7Int>(uInt n, uInt nIndex, It7Int const &, It7 const &, Double const &) 

  // scimath/Fitting/LSQaips.cc 
  template Bool LSQaips::getCovariance<Double>(Array<Double> &);
  template Bool LSQaips::solveLoop<Double>(Double &, uInt &, Vector<Double> &, Bool doSVD);
  template Bool LSQaips::solveLoop<Double>(uInt &, Vector<Double> &, Bool doSVD);
  template Bool LSQaips::getCovariance<Float>(Array<Float> &);
  template Bool LSQaips::solveLoop<Float>(Double &, uInt &, Vector<Float> &, Bool doSVD);
  template Bool LSQaips::solveLoop<Float>(uInt &, Vector<Float> &, Bool doSVD);
  template Bool LSQaips::getCovariance<std::complex<Double> >(Array<std::complex<Double> > &);
  template Bool LSQaips::solveLoop<std::complex<Double> >(Double &, uInt &, Vector<std::complex<Double> > &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Double> >(uInt &, Vector<std::complex<Double> > &, Bool doSVD);
  template Bool LSQaips::getCovariance<std::complex<Float> >(Array<std::complex<Float> > &);
  template Bool LSQaips::solveLoop<std::complex<Float> >(Double &, uInt &, Vector<std::complex<Float> > &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Float> >(uInt &, Vector<std::complex<Float> > &, Bool doSVD);

  // scimath/Fitting/LinearFit.cc 
  template class LinearFit<AutoDiff<Complex> >;
  template class LinearFit<AutoDiffA<Complex> >;
  template class LinearFit<AutoDiff<DComplex> >;
  template class LinearFit<AutoDiffA<DComplex> >;
  template class LinearFit<Complex>;
  template class LinearFit<DComplex>;
  template class LinearFit<AutoDiff<Double> >;
  template class LinearFit<AutoDiffA<Double> >;
  template class LinearFit<AutoDiff<Float> >;
  template class LinearFit<AutoDiffA<Float> >;
  template class LinearFit<Double>;
  template class LinearFit<Float>;
  
  // scimath/Fitting/LinearFitSVD.cc 
  template class LinearFitSVD<Complex>;
  template class LinearFitSVD<DComplex>;
  template class LinearFitSVD<AutoDiff<Complex> >;
  template class LinearFitSVD<AutoDiffA<Complex> >;
  template class LinearFitSVD<AutoDiff<DComplex> >;
  template class LinearFitSVD<AutoDiffA<DComplex> >;
  template class LinearFitSVD<AutoDiff<Double> >;
  template class LinearFitSVD<AutoDiffA<Double> >;
  template class LinearFitSVD<AutoDiff<Float> >;
  template class LinearFitSVD<AutoDiffA<Float> >;
  template class LinearFitSVD<Double>;
  template class LinearFitSVD<Float>;

  // scimath/Fitting/NonLinearFit.cc 
  template class NonLinearFit<AutoDiff<Complex> >;
  template class NonLinearFit<AutoDiffA<Complex> >;
  template class NonLinearFit<AutoDiff<DComplex> >;
  template class NonLinearFit<AutoDiffA<DComplex> >;
  template class NonLinearFit<Complex>;
  template class NonLinearFit<DComplex>;
  template class NonLinearFit<AutoDiff<Double> >;
  template class NonLinearFit<AutoDiffA<Double> >;
  template class NonLinearFit<AutoDiff<Float> >;
  template class NonLinearFit<AutoDiffA<Float> >;
  template class NonLinearFit<Double>;
  template class NonLinearFit<Float>;
  
  // scimath/Fitting/NonLinearFitLM.cc 
  template class NonLinearFitLM<Complex>;
  template class NonLinearFitLM<DComplex>;
  template class NonLinearFitLM<AutoDiff<Complex> >;
  template class NonLinearFitLM<AutoDiffA<Complex> >;
  template class NonLinearFitLM<AutoDiff<DComplex> >;
  template class NonLinearFitLM<AutoDiffA<DComplex> >;
  template class NonLinearFitLM<AutoDiff<Double> >;
  template class NonLinearFitLM<AutoDiffA<Double> >;
  template class NonLinearFitLM<AutoDiff<Float> >;
  template class NonLinearFitLM<AutoDiffA<Float> >;
  template class NonLinearFitLM<Double>;
  template class NonLinearFitLM<Float>;

}
