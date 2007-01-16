
// include of header files 

#include <casa/Arrays.h>
#include <casa/Arrays/VectorSTLIterator.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffMath.h>

#include <algorithm>

// include of implementation files

#include <casa/Arrays/Matrix.cc>
#include <casa/Arrays/Vector.cc>
#include <scimath/Fitting/FitGaussian.cc>
#include <scimath/Fitting/GenericL2Fit.cc>
#include <scimath/Fitting/LSQaips.cc>
#include <scimath/Fitting/LinearFit.cc>
#include <scimath/Fitting/LinearFitSVD.cc>
#include <scimath/Fitting/LSQFit2.cc>
#include <scimath/Fitting/NonLinearFit.cc>
#include <scimath/Fitting/NonLinearFitLM.cc>

namespace casa {

  // ============================================================================
  //
  //  ReposFiller/templates
  //
  // ============================================================================

  // casa/Arrays/Matrix.cc
  template class Matrix<Float>;
  template class Matrix<Double>;

  // casa/Arrays/Vector.cc
  template class Vector<Float>;
  template class Vector<Double>;

  // scimath/Fitting/FitGaussian.cc 
  template class FitGaussian<Float>;
  template class FitGaussian<Double>;
//   template Matrix<Float> FitGaussian<Float>::fit (const Matrix<Float>&, const Vector<Float>&, const Vector<Float>&, Float, uInt, Float);

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

  // 1000 scimath/Fitting/LSQFit2.cc ---------------------------------
  
#if !defined(AIPS_INTELCC) && !defined(AIPS_CRAY_PGI)
  template Double *swap_ranges<Double *, Double *>(Double *, Double *, Double *);
#endif

  typedef uInt* It3Int;
  typedef Double* It5;
  
  typedef std::complex<Float>* It4;
  typedef std::complex<Double>* It3;

  typedef VectorSTLIterator<uInt> ItuInt;
  typedef VectorSTLIterator<Float> ItFloat;
  typedef VectorSTLIterator<Double> ItDouble;
  typedef VectorSTLIterator<std::complex<Float> > ItComplex;
  typedef VectorSTLIterator<std::complex<Double> > ItDComplex;
  
  // 1010 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Double, ItDouble >(ItDouble const &, Double const &, Double const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDouble >(ItDouble const &, Double const &, Double const &, LSQFit::Real, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDouble, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItDouble const &, Double const &, Double const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDouble, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItDouble const &, Double const &, Double const &, LSQFit::Real, Bool, Bool);
  template Bool LSQFit::addConstraint<Double, ItDouble >(ItDouble const &, Double const &);
  template Bool LSQFit::addConstraint<Double, ItDouble, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItDouble const &, Double const &);
  template Bool LSQFit::setConstraint<Double, ItDouble >(uInt n, ItDouble const &, Double const &);
  template Bool LSQFit::setConstraint<Double, ItDouble, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, ItDouble const &, Double const &);
  template Bool LSQFit::getConstraint<ItDouble >(uInt, ItDouble &) const;
  template void LSQFit::solve<ItDouble >(ItDouble &);
  template Bool LSQFit::solveLoop<ItDouble >(Double &, uInt &, ItDouble &, Bool);
  template Bool LSQFit::solveLoop<ItDouble >(uInt &, ItDouble &, Bool);
  template void LSQFit::copy<ItDouble >(Double const *, Double const *, ItDouble &, LSQReal);
  template void LSQFit::uncopy<ItDouble >(Double *, Double const *, ItDouble &, LSQReal);
  template void LSQFit::copyDiagonal<ItDouble >(ItDouble &, LSQReal);
  template Bool LSQFit::getErrors<ItDouble >(ItDouble &);
  
  // 1020 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Float, ItFloat >(ItFloat const &, Float const &, Float const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItFloat >(ItFloat const &, Float const &, Float const &, LSQFit::Real, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItFloat, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItFloat const &, Float const &, Float const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItFloat, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItFloat const &, Float const &, Float const &, LSQFit::Real, Bool, Bool);
  template Bool LSQFit::addConstraint<Float, ItFloat >(ItFloat const &, Float const &);
  template Bool LSQFit::addConstraint<Float, ItFloat, VectorSTLIterator<uInt> >(uInt nIndex, VectorSTLIterator<uInt> const &, ItFloat const &, Float const &);
  template Bool LSQFit::setConstraint<Float, ItFloat >(uInt n, ItFloat const &, Float const &);
  template Bool LSQFit::setConstraint<Float, ItFloat, VectorSTLIterator<uInt> >(uInt n, uInt nIndex, VectorSTLIterator<uInt> const &, ItFloat const &, Float const &);
  template Bool LSQFit::getConstraint<ItFloat >(uInt, ItFloat &) const;
  template void LSQFit::solve<ItFloat >(ItFloat &);
  template Bool LSQFit::solveLoop<ItFloat >(Double &, uInt &, ItFloat &, Bool);
  template Bool LSQFit::solveLoop<ItFloat >(uInt &, ItFloat &, Bool);
  template void LSQFit::copy<ItFloat >(Double const *, Double const *, ItFloat &, LSQReal);
  template void LSQFit::uncopy<ItFloat >(Double *, Double const *, ItFloat &, LSQReal);
  template void LSQFit::copyDiagonal<ItFloat >(ItFloat &, LSQReal);
  template Bool LSQFit::getErrors<ItFloat >(ItFloat &);
  
  // 1030 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Double, ItDComplex>(ItDComplex const &, Double const &, std::complex<Double> const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex>(ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex>(ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex>(ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex>(ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, Double const &, std::complex<Double> const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool);
  template Bool LSQFit::addConstraint<Double, ItDComplex>(ItDComplex const &, std::complex<Double> const &);
  template Bool LSQFit::addConstraint<Double, ItDComplex, ItuInt>(uInt nIndex, ItuInt const &, ItDComplex const &, std::complex<Double> const &);
  template Bool LSQFit::setConstraint<Double, ItDComplex>(uInt n, ItDComplex const &, std::complex<Double> const &);
  template Bool LSQFit::setConstraint<Double, ItDComplex, ItuInt>(uInt n, uInt nIndex, ItuInt const &, ItDComplex const &, std::complex<Double> const &);
  template Bool LSQFit::getConstraint<ItDComplex>(uInt, ItDComplex &) const;
  template void LSQFit::solve<ItDComplex>(ItDComplex &);
  template Bool LSQFit::solveLoop<ItDComplex>(Double &, uInt &, ItDComplex &, Bool);
  template Bool LSQFit::solveLoop<ItDComplex>(uInt &, ItDComplex &, Bool);
  template void LSQFit::copy<ItDComplex>(Double const *, Double const *, ItDComplex &, LSQComplex);
  template void LSQFit::uncopy<ItDComplex>(Double *, Double const *, ItDComplex &, LSQComplex);
  template void LSQFit::copyDiagonal<ItDComplex>(ItDComplex &, LSQComplex);
  template Bool LSQFit::getErrors<ItDComplex>(ItDComplex &);
  
  // 1040 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Float, ItComplex>(ItComplex const &, Float const &, std::complex<Float> const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex>(ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex>(ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex>(ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex>(ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, Float const &, std::complex<Float> const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool);
  template Bool LSQFit::addConstraint<Float, ItComplex>(ItComplex const &, std::complex<Float> const &);
template Bool LSQFit::addConstraint<Float, ItComplex, ItuInt>(uInt nIndex, ItuInt const &, ItComplex const &, std::complex<Float> const &);
  template Bool LSQFit::setConstraint<Float, ItComplex>(uInt n, ItComplex const &, std::complex<Float> const &);
  template Bool LSQFit::setConstraint<Float, ItComplex, ItuInt>(uInt n, uInt nIndex, ItuInt const &, ItComplex const &, std::complex<Float> const &);
  template Bool LSQFit::getConstraint<ItComplex>(uInt, ItComplex &) const;
  template void LSQFit::solve<ItComplex>(ItComplex &);
  template Bool LSQFit::solveLoop<ItComplex>(Double &, uInt &, ItComplex &, Bool);
  template Bool LSQFit::solveLoop<ItComplex>(uInt &, ItComplex &, Bool);
  template void LSQFit::copy<ItComplex>(Double const *, Double const *, ItComplex &, LSQComplex);
  template void LSQFit::uncopy<ItComplex>(Double *, Double const *, ItComplex &, LSQComplex);
  template void LSQFit::copyDiagonal<ItComplex>(ItComplex &, LSQComplex);
  template Bool LSQFit::getErrors<ItComplex>(ItComplex &);
  
  // 1050 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3>(It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, Double const &, std::complex<Double> const &, LSQFit::Separable, Bool, Bool);
  template Bool LSQFit::addConstraint<Double, It3>(It3 const &, std::complex<Double> const &);
  template Bool LSQFit::addConstraint<Double, It3, It3Int>(uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &);
  template Bool LSQFit::setConstraint<Double, It3>(uInt n, It3 const &, std::complex<Double> const &);
  template Bool LSQFit::setConstraint<Double, It3, It3Int>(uInt n, uInt nIndex, It3Int const &, It3 const &, std::complex<Double> const &);
  template Bool LSQFit::getConstraint<Double>(uInt, std::complex<Double> *) const;
  template void LSQFit::solve<Double>(std::complex<Double> *);
  template Bool LSQFit::solveLoop<Double>(Double &, uInt &, std::complex<Double> *, Bool);
  template Bool LSQFit::solveLoop<Double>(uInt &, std::complex<Double> *, Bool);
  template void LSQFit::copy<std::complex<Double> >(Double const *, Double const *, std::complex<Double> *, LSQComplex);
  template void LSQFit::uncopy<std::complex<Double> >(Double *, Double const *, std::complex<Double> *, LSQComplex);
  template Bool LSQFit::getErrors<Double>(std::complex<Double> *);
  template Bool LSQFit::getCovariance<Double>(std::complex<Double> *);
  
  // 1060 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4>(It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, Float const &, std::complex<Float> const &, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::AsReal, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Complex, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Conjugate, Bool, Bool);
  template void LSQFit::makeNorm<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, Float const &, std::complex<Float> const &, LSQFit::Separable, Bool, Bool);
  template Bool LSQFit::addConstraint<Float, It4>(It4 const &, std::complex<Float> const &);
  template Bool LSQFit::addConstraint<Float, It4, It3Int>(uInt nIndex, It3Int const &, It4 const &, std::complex<Float> const &);
  template Bool LSQFit::setConstraint<Float, It4>(uInt n, It4 const &, std::complex<Float> const &);
  template Bool LSQFit::setConstraint<Float, It4, It3Int>(uInt n, uInt nIndex, It3Int const &, It4 const &, std::complex<Float> const &);
  template Bool LSQFit::getConstraint<Float>(uInt, std::complex<Float> *) const;
  template void LSQFit::solve<Float>(std::complex<Float> *);
  template Bool LSQFit::solveLoop<Float>(Double &, uInt &, std::complex<Float> *, Bool);
  template Bool LSQFit::solveLoop<Float>(uInt &, std::complex<Float> *, Bool);
  template void LSQFit::copy<std::complex<Float> >(Double const *, Double const *, std::complex<Float> *, LSQComplex);
  template void LSQFit::uncopy<std::complex<Float> >(Double *, Double const *, std::complex<Float> *, LSQComplex);
  template Bool LSQFit::getErrors<Float>(std::complex<Float> *);
  template Bool LSQFit::getCovariance<Float>(std::complex<Float> *);
  
  // 1070 scimath/Fitting/LSQFit2.cc ---------------------------------
  
  template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, It5>(It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool);
  template void LSQFit::makeNorm<Double, It5, It3Int>(uInt nIndex, It3Int const &, It5 const &, Double const &, Double const &, Bool, Bool);
  template void LSQFit::makeNorm<Double, It5, It3Int>(uInt nIndex, It3Int const &, It5 const &, Double const &, Double const &, LSQFit::Real, Bool, Bool);
  template Bool LSQFit::addConstraint<Double, It5>(It5 const &, Double const &);
  template Bool LSQFit::addConstraint<Double, It5, It3Int>(uInt nIndex, It3Int const &, It5 const &, Double const &);
  template Bool LSQFit::setConstraint<Double, It5>(uInt n, It5 const &, Double const &);
  template Bool LSQFit::setConstraint<Double, It5, It3Int>(uInt n, uInt nIndex, It3Int const &, It5 const &, Double const &);
  template Bool LSQFit::getConstraint<Double>(uInt, Double *) const;
  template void LSQFit::solve<Double>(Double *);
  template Bool LSQFit::solveLoop<Double>(Double &, uInt &, Double *, Bool);
  template Bool LSQFit::solveLoop<Double>(uInt &, Double *, Bool);
  template void LSQFit::copy<Double>(Double const *, Double const *, Double *, LSQReal);
  template void LSQFit::uncopy<Double>(Double *, Double const *, Double *, LSQReal);
  template Bool LSQFit::getErrors<Double>(Double *);
  template Bool LSQFit::getCovariance<Double>(Double *);
  
  // 1080 scimath/Fitting/LSQFit2.cc ---------------------------------


  // 1090 scimath/Fitting/LSQFit2.cc ---------------------------------


  // scimath/Fitting/LSQaips.cc 
  template Bool LSQaips::getCovariance<Float>(Array<Float> &);
  template Bool LSQaips::getCovariance<Double>(Array<Double> &);
  template Bool LSQaips::getCovariance<std::complex<Float> >(Array<std::complex<Float> > &);
  template Bool LSQaips::getCovariance<std::complex<Double> >(Array<std::complex<Double> > &);

  template Bool LSQaips::solveLoop<Float>(uInt &, Vector<Float> &, Bool doSVD);
  template Bool LSQaips::solveLoop<Double>(uInt &, Vector<Double> &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Float> >(uInt &, Vector<std::complex<Float> > &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Double> >(uInt &, Vector<std::complex<Double> > &, Bool doSVD);
  
  template Bool LSQaips::solveLoop<Float>(Double &, uInt &, Vector<Float> &, Bool doSVD);  
  template Bool LSQaips::solveLoop<Double>(Double &, uInt &, Vector<Double> &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Float> >(Double &, uInt &, Vector<std::complex<Float> > &, Bool doSVD);
  template Bool LSQaips::solveLoop<std::complex<Double> >(Double &, uInt &, Vector<std::complex<Double> > &, Bool doSVD);
  
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

  // ============================================================================
  //
  //  test/templates
  //
  // ============================================================================

}
