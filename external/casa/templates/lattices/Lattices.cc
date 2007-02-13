
// include of header files

#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slicer.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Logging/LogIO.h>
#include <lattices/Lattices/Lattice.h>
#include <lattices/Lattices/MaskedLattice.h>

// include of implementation files

#include <casa/Arrays/ArrayMath.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <lattices/Lattices/ArrayLattice.cc>
#include <lattices/Lattices/ExtendLattice.cc>
#include <lattices/Lattices/LELArray.cc>
#include <lattices/Lattices/LELScalar.cc>
#include <lattices/Lattices/LELBinary.cc>
#include <lattices/Lattices/LELCondition.cc>
#include <lattices/Lattices/LELConvert.cc>
#include <lattices/Lattices/LELFunction.cc>
#include <lattices/Lattices/LELInterface.cc>
#include <lattices/Lattices/LELLattice.cc>
#include <lattices/Lattices/LELSpectralIndex.cc>
#include <lattices/Lattices/LELUnary.cc>
#include <lattices/Lattices/Lattice.cc>
#include <lattices/Lattices/LatticeApply.cc>
#include <lattices/Lattices/LineCollapser.cc>
#include <lattices/Lattices/TiledCollapser.cc>
#include <lattices/Lattices/LatticeCache.cc>
#include <lattices/Lattices/LatticeCleaner.cc>
#include <lattices/Lattices/LatticeConcat.cc>
#include <lattices/Lattices/LatticeConvolver.cc>
#include <lattices/Lattices/LatticeExpr.cc>
#include <lattices/Lattices/LatticeFractile.cc>
#include <lattices/Lattices/LatticeHistograms.cc>
#include <lattices/Lattices/LatticeIterator.cc>
#include <lattices/Lattices/LatticeIterInterface.cc>
#include <lattices/Lattices/LatticeSlice1D.cc>
#include <lattices/Lattices/LatticeStatistics.cc>
#include <lattices/Lattices/LatticeTwoPtCorr.cc>
#include <lattices/Lattices/LatticeUtilities.cc>
#include <lattices/Lattices/MaskedLattice.cc>
#include <lattices/Lattices/MaskedLatticeIterator.cc>
#include <lattices/Lattices/PagedArray.cc>
#include <lattices/Lattices/PagedArrIter.cc>
#include <lattices/Lattices/RebinLattice.cc>
#include <lattices/Lattices/SubLattice.cc>
#include <lattices/Lattices/TempLattice.cc>

#include <casa/Arrays/MaskedArray.cc>
#include <lattices/Lattices/CLInterpolator2D.cc>
#include <lattices/Lattices/CLIPNearest2D.cc>
#include <lattices/Lattices/CurvedLattice2D.cc>

namespace casa {

  // ============================================================================
  //
  //  Templates for library
  //
  // ============================================================================

  // 1000 lattices/Lattices/ArrayLattice.cc
  template class ArrayLattice<Complex>;
  template class ArrayLattice<DComplex>;

  // 1010 lattices/Lattices/ArrayLattice.cc 
  template class ArrayLattice<Bool>;

  // 1020 lattices/Lattices/ArrayLattice.cc 
  template class ArrayLattice<Float>;
  template class ArrayLattice<Double>;

  // 1030 lattices/Lattices/ArrayLattice.cc 
  template class ArrayLattice<Int>;

  // 1040 lattices/Lattices/ArrayLattice.cc 
  template class ArrayLattice<uInt>;

  // 1000 lattices/Lattices/ExtendLattice.cc 
  template class ExtendLattice<Float>;
  template class ExtendLattice<Complex>;
  template class ExtendLattice<Bool>;

  // 1000 lattices/Lattices/LELArray.cc lattices/Lattices/LELScalar.cc 
  template class LELArray<Bool>;
  template class LELArray<Float>;
  template class LELArray<Double>;
  template class LELArray<Complex>;
  template class LELArray<DComplex>;
  template class LELArrayRef<Bool>;
  template class LELArrayRef<Float>;
  template class LELArrayRef<Double>;
  template class LELArrayRef<Complex>;
  template class LELArrayRef<DComplex>;
  template class LELScalar<Bool>;
  template class LELScalar<Float>;
  template class LELScalar<Double>;
  template class LELScalar<Complex>;
  template class LELScalar<DComplex>;

  // 1000 lattices/Lattices/LELBinary.cc
  template class LELBinary<Float>;
  template class LELBinary<Double>;
  template class LELBinary<Complex>;
  template class LELBinary<DComplex>;
  template class LELBinaryCmp<Float>;
  template class LELBinaryCmp<Double>;
  template class LELBinaryCmp<Complex>;
  template class LELBinaryCmp<DComplex>;

  // 1000 lattices/Lattices/LELCondition.cc
  template class LELCondition<Float>;
  template class LELCondition<Double>;
  template class LELCondition<Complex>;
  template class LELCondition<DComplex>;
  template class LELCondition<Bool>;

  // 1000 lattices/Lattices/LELConvert.cc casa/Arrays/ArrayMath.cc 
  template class LELConvert<Double, Float>;
  template class LELConvert<Complex, Float>;
  template class LELConvert<DComplex, Float>;
  template class LELConvert<Float, Double>;
  template class LELConvert<Complex, Double>;
  template class LELConvert<DComplex, Double>;
  template class LELConvert<DComplex, Complex>;
  template class LELConvert<Complex, DComplex>;

  // 1000 lattices/Lattices/LELFunction.cc
  template class LELFunction1D<Float>;
  template class LELFunction1D<Double>;
  template class LELFunction1D<Complex>;
  template class LELFunction1D<DComplex>;
  template class LELFunctionReal1D<Float>;
  template class LELFunctionReal1D<Double>;
  template class LELFunctionND<Float>;
  template class LELFunctionND<Double>;
  template class LELFunctionND<Complex>;
  template class LELFunctionND<DComplex>;
  template class LELFunctionND<Bool>;

  // 1000 lattices/Lattices/LELInterface.cc
  template class LELInterface<Float>;
  template class LELInterface<Double>;
  template class LELInterface<Complex>;
  template class LELInterface<DComplex>;
  template class LELInterface<Bool>;

  // 1000 lattices/Lattices/LELLattice.cc
  template class LELLattice<Float>;
  template class LELLattice<Double>;
  template class LELLattice<Complex>;
  template class LELLattice<DComplex>;
  template class LELLattice<Bool>;

  // 1000 lattices/Lattices/LELSpectralIndex.cc
  template class LELSpectralIndex<Float>;
  template class LELSpectralIndex<Double>;

  // 1000 lattices/Lattices/LELUnary.cc casa/BasicSL/Complex.h 
  template class LELUnaryConst<Float>;
  template class LELUnaryConst<Double>;
  template class LELUnaryConst<Complex>;
  template class LELUnaryConst<DComplex>;
  template class LELUnaryConst<Bool>;
  template class LELUnary<Float>;
  template class LELUnary<Double>;
  template class LELUnary<Complex>;
  template class LELUnary<DComplex>;

  // 1000 lattices/Lattices/Lattice.cc
  template class Lattice<Complex>;
  template class Lattice<DComplex>;

  // 1010 lattices/Lattices/Lattice.cc 
  template class Lattice<Bool>;
  
  // 1020 lattices/Lattices/Lattice.cc 
  template class Lattice<Double>;
  
  // 1030 lattices/Lattices/Lattice.cc 
  template class Lattice<Float>;

  // 1040 lattices/Lattices/Lattice.cc 
  template class Lattice<Int>;

  // 1050 lattices/Lattices/Lattice.cc 
  template class Lattice<uInt>;

  // 1000 lattices/Lattices/LatticeApply.cc 
  template class LatticeApply<Int>;
  template class LineCollapser<Int>;
  template class TiledCollapser<Int>;
  template class LatticeApply<Float>;
  template class LineCollapser<Float>;
  template class TiledCollapser<Float>;

  // 1000 lattices/Lattices/LatticeCache.cc
  template class LatticeCache<Complex>;

  //1010 lattices/Lattices/LatticeCache.cc 
  template class LatticeCache<Float>;

  // 1000 lattices/Lattices/LatticeCleaner.cc 
  template class LatticeCleaner<Float>;

  // 1000 lattices/Lattices/LatticeConcat.cc
  template class LatticeConcat<Float>;
  template class LatticeConcat<Bool>;

  // 1000 lattices/Lattices/LatticeConvolver.cc
  template class LatticeConvolver<Float>;

  // 1000 lattices/Lattices/LatticeExpr.cc
  template class LatticeExpr<Float>;
  template class LatticeExpr<Double>;
  template class LatticeExpr<Complex>;
  template class LatticeExpr<DComplex>;
  template class LatticeExpr<Bool>;

  // 1000 lattices/Lattices/LatticeFractile.cc 
  template class LatticeFractile<Float>;
  template class LatticeFractile<Double>;

  // 1000 lattices/Lattices/LatticeHistograms.cc
  template class LatticeHistograms<Float>;
  template class HistTiledCollapser<Float>;

  // 1000 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<Complex>;
  template class RO_LatticeIterator<Complex>;
  template class LatticeIterInterface<Complex>;
  template class LatticeIterator<DComplex>;
  template class RO_LatticeIterator<DComplex>;
  template class LatticeIterInterface<DComplex>;

  // 1010 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<Bool>;
  template class RO_LatticeIterator<Bool>;
  template class LatticeIterInterface<Bool>;

  // 1020 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<Double>;
  template class RO_LatticeIterator<Double>;
  template class LatticeIterInterface<Double>;

  // 1030 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<Float>;
  template class RO_LatticeIterator<Float>;
  template class LatticeIterInterface<Float>;

  // 1040 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<Int>;
  template class RO_LatticeIterator<Int>;
  template class LatticeIterInterface<Int>;

  // 1050 lattices/Lattices/LatticeIterator.cc 
  template class LatticeIterator<uInt>;
  template class RO_LatticeIterator<uInt>;
  template class LatticeIterInterface<uInt>;

  // 1000 lattices/Lattices/LatticeSlice1D.cc 
  template class LatticeSlice1D<Float>;

  // 1000 lattices/Lattices/LatticeStatistics.cc 
  template class LatticeStatistics<Float>;
  template class LatticeApply<Float, Double>;
  template class StatsTiledCollapser<Float, Double>;
  template class TiledCollapser<Float, Double>;

  // 1000 lattices/Lattices/LatticeTwoPtCorr.cc 
  template class LatticeTwoPtCorr<Float>;

  // 1000 lattices/Lattices/LatticeUtilities.cc 
  template void minMax(Float &, Float &, IPosition &, IPosition &, Lattice<Float> const &);
  template void LatticeUtilities::copyDataAndMask<Float>(LogIO &, MaskedLattice<Float> &, MaskedLattice<Float> const &, Bool);
  template void LatticeUtilities::copyDataAndMask<Complex>(LogIO &, MaskedLattice<Complex> &, MaskedLattice<Complex> const &, Bool);
  template void LatticeUtilities::collapse<Float>(Array<Float> &, IPosition const &, MaskedLattice<Float> const &, Bool);
  template void LatticeUtilities::collapse<Float>(Array<Float> &, Array<Bool> &, IPosition const &, MaskedLattice<Float> const &, Bool, Bool, Bool);
  template void LatticeUtilities::replicate<Float>(Lattice<Float> &, Slicer const &, Array<Float> const &);
  template void LatticeUtilities::replicate<Bool>(Lattice<Bool> &, Slicer const &, Array<Bool> const &);
  template void LatticeUtilities::addDegenerateAxes<Float>(Lattice<Float>* &, Lattice<Float> const &, uInt);

  // 1000 lattices/Lattices/MaskedLattice.cc
  template class MaskedLattice<Int>;
  template class MaskedLattice<Float>;
  template class MaskedLattice<Double>;
  template class MaskedLattice<Complex>;
  template class MaskedLattice<DComplex>;
  template class MaskedLattice<Bool>;

  // 1000 lattices/Lattices/MaskedLatticeIterator.cc 
  template class RO_MaskedLatticeIterator<Float>;
  template class RO_MaskedLatticeIterator<Double>;
  template class RO_MaskedLatticeIterator<Complex>;
  template class RO_MaskedLatticeIterator<DComplex>;
  template class RO_MaskedLatticeIterator<Bool>;

  // 1000 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<Complex>;
  template class PagedArrIter<Complex>;

  // 1010 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<DComplex>;
  template class PagedArrIter<DComplex>;

  // 1020 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<Bool>;
  template class PagedArrIter<Bool>;

  // 1030 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<Double>;
  template class PagedArrIter<Double>;

  // 1040 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<Float>;
  template class PagedArrIter<Float>;

  // 1050 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<Int>;
  template class PagedArrIter<Int>;

  /// 1060 lattices/Lattices/PagedArray.cc lattices/Lattices/PagedArrIter.cc 
  template class PagedArray<uInt>;
  template class PagedArrIter<uInt>;

  // 1000 lattices/Lattices/RebinLattice.cc 
  template class RebinLattice<Float>;
  template class RebinLattice<Double>;
  template class RebinLattice<Complex>;
  template class RebinLattice<DComplex>;
  template class RebinLattice<Bool>;

  // 1000 lattices/Lattices/SubLattice.cc
  template class SubLattice<Int>;
  template class SubLattice<Float>;
  template class SubLattice<Double>;
  template class SubLattice<Complex>;
  template class SubLattice<DComplex>;
  template class SubLattice<Bool>;

  // 1000 lattices/Lattices/TempLattice.cc
  template class TempLattice<Complex>;

  // 1010 lattices/Lattices/TempLattice.cc 
  template class TempLattice<Bool>;
  template class CountedConstPtr<Lattice<Bool> >;
  template class CountedPtr<Lattice<Bool> >;
  template class PtrRep<Lattice<Bool> >;
  template class SimpleCountedConstPtr<Lattice<Bool> >;
  template class SimpleCountedPtr<Lattice<Bool> >;

  // 1020 lattices/Lattices/TempLattice.cc 
  template class TempLattice<Double>;
  template class CountedConstPtr<Lattice<Double> >;
  template class CountedPtr<Lattice<Double> >;
  template class PtrRep<Lattice<Double> >;
  template class SimpleCountedConstPtr<Lattice<Double> >;
  template class SimpleCountedPtr<Lattice<Double> >;

  // 1030 lattices/Lattices/TempLattice.cc 
  template class TempLattice<Float>;

  // 1040 lattices/Lattices/TempLattice.cc 
  template class TempLattice<Int>;

  // 1050 lattices/Lattices/TempLattice.cc 
  template class TempLattice<uInt>;
  
  // ============================================================================
  //
  //  Additional templates for test programs
  //
  // ============================================================================

  //1000 lattices/Lattices/CLInterpolator2D.cc 
  template class CLInterpolator2D<Int>;
  template class CLIPNearest2D<Int>;
  template class CurvedLattice2D<Int>;
  
  // 1000 lattices/Lattices/ExtendLattice.cc 
  template class ExtendLattice<Int>;
  
  // 1000 lattices/Lattices/LatticeApply.cc 
  template class LatticeApply<Complex>;
  template class LineCollapser<Complex>;
  template class TiledCollapser<Complex>;
  
  // 1000 lattices/Lattices/LatticeStatistics.cc 
  template class LatticeStatistics<Complex>;
  template class LatticeApply<Complex, DComplex>;
  template class StatsTiledCollapser<Complex, DComplex>;
  template class TiledCollapser<Complex, DComplex>;

  // 1000 lattices/Lattices/LatticeUtilities.cc  
  template void LatticeUtilities::bin<Float>(MaskedArray<Float> &, MaskedArray<Float> const &, uInt, uInt);

  // 1000 lattices/Lattices/TempLattice.cc casa/BasicSL/Complex.h 
  template class TempLattice<DComplex>;
  template class CountedConstPtr<Lattice<DComplex> >;
  template class CountedPtr<Lattice<DComplex> >;
  template class PtrRep<Lattice<DComplex> >;
  template class SimpleCountedConstPtr<Lattice<DComplex> >;
  template class SimpleCountedPtr<Lattice<DComplex> >;
}
