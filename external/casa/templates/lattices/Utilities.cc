
// include of header files

#include <casa/BasicSL/Complex.h>
#include <lattices/Lattices/Lattice.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeIterInterface.h>
#include <lattices/Lattices/LCRegionSingle.h>
#include <lattices/Lattices/LELInterface.h>
#include <lattices/Lattices/LELLattCoordBase.h>
#include <lattices/Lattices/MaskedLattice.h>
#include <lattices/Lattices/TempLattice.h>

// include of implementation files

#include <casa/Utilities/CountedPtr.cc>
#include <casa/Utilities/PtrHolder.cc>
// #include <casa/Utilities/GenSort.cc>

namespace casa {

  // 1000 casa/Utilities/CountedPtr.cc 
  template class CountedConstPtr<Lattice<Complex> >;
  template class CountedPtr<Lattice<Complex> >;
  template class PtrRep<Lattice<Complex> >;
  template class SimpleCountedConstPtr<Lattice<Complex> >;
  template class SimpleCountedPtr<Lattice<Complex> >;

  // 1010 casa/Utilities/CountedPtr.cc 
  template class CountedConstPtr<MaskedLattice<Float> >;
  template class CountedPtr<MaskedLattice<Float> >;
  template class PtrRep<MaskedLattice<Float> >;
  template class SimpleCountedConstPtr<MaskedLattice<Float> >;
  template class SimpleCountedPtr<MaskedLattice<Float> >;
  template class CountedConstPtr<MaskedLattice<Double> >;
  template class CountedPtr<MaskedLattice<Double> >;
  template class PtrRep<MaskedLattice<Double> >;
  template class SimpleCountedConstPtr<MaskedLattice<Double> >;
  template class SimpleCountedPtr<MaskedLattice<Double> >;
  template class CountedConstPtr<MaskedLattice<Complex> >;
  template class CountedPtr<MaskedLattice<Complex> >;
  template class PtrRep<MaskedLattice<Complex> >;
  template class SimpleCountedConstPtr<MaskedLattice<Complex> >;
  template class SimpleCountedPtr<MaskedLattice<Complex> >;
  template class CountedConstPtr<MaskedLattice<DComplex> >;
  template class CountedPtr<MaskedLattice<DComplex> >;
  template class PtrRep<MaskedLattice<DComplex> >;
  template class SimpleCountedConstPtr<MaskedLattice<DComplex> >;
  template class SimpleCountedPtr<MaskedLattice<DComplex> >;
  template class CountedConstPtr<MaskedLattice<Bool> >;
  template class CountedPtr<MaskedLattice<Bool> >;
  template class PtrRep<MaskedLattice<Bool> >;
  template class SimpleCountedConstPtr<MaskedLattice<Bool> >;
  template class SimpleCountedPtr<MaskedLattice<Bool> >;

  // 1020 casa/Utilities/CountedPtr.cc
  template class CountedConstPtr<LELInterface<Float> >;
  template class CountedPtr<LELInterface<Float> >;
  template class PtrRep<LELInterface<Float> >;
  template class SimpleCountedConstPtr<LELInterface<Float> >;
  template class SimpleCountedPtr<LELInterface<Float> >;
  template class CountedConstPtr<LELInterface<Double> >;
  template class CountedPtr<LELInterface<Double> >;
  template class PtrRep<LELInterface<Double> >;
  template class SimpleCountedConstPtr<LELInterface<Double> >;
  template class SimpleCountedPtr<LELInterface<Double> >;
  template class CountedConstPtr<LELInterface<Complex> >;
  template class CountedPtr<LELInterface<Complex> >;
  template class PtrRep<LELInterface<Complex> >;
  template class SimpleCountedConstPtr<LELInterface<Complex> >;
  template class SimpleCountedPtr<LELInterface<Complex> >;
  template class CountedConstPtr<LELInterface<DComplex> >;
  template class CountedPtr<LELInterface<DComplex> >;
  template class PtrRep<LELInterface<DComplex> >;
  template class SimpleCountedConstPtr<LELInterface<DComplex> >;
  template class SimpleCountedPtr<LELInterface<DComplex> >;
  template class CountedConstPtr<LELInterface<Bool> >;
  template class CountedPtr<LELInterface<Bool> >;
  template class PtrRep<LELInterface<Bool> >;
  template class SimpleCountedConstPtr<LELInterface<Bool> >;
  template class SimpleCountedPtr<LELInterface<Bool> >;

  // 1030 casa/Utilities/CountedPtr.cc
  template class CountedConstPtr<LELLattCoordBase>;
  template class CountedPtr<LELLattCoordBase>;
  template class PtrRep<LELLattCoordBase>;
  template class SimpleCountedConstPtr<LELLattCoordBase>;
  template class SimpleCountedPtr<LELLattCoordBase>;

  // 1040 casa/Utilities/CountedPtr.cc
  template class CountedPtr<Lattice<Float> >;
  template class CountedConstPtr<Lattice<Float> >;
  template class SimpleCountedPtr<Lattice<Float> >;
  template class SimpleCountedConstPtr<Lattice<Float> >;
  template class PtrRep<Lattice<Float> >;

  // 1050 casa/Utilities/CountedPtr.cc
  template class CountedPtr<Lattice<Int> >;
  template class CountedConstPtr<Lattice<Int> >;
  template class SimpleCountedPtr<Lattice<Int> >;
  template class SimpleCountedConstPtr<Lattice<Int> >;
  template class PtrRep<Lattice<Int> >;

  // 1060 casa/Utilities/CountedPtr.cc
  template class CountedPtr<Lattice<uInt> >;
  template class CountedConstPtr<Lattice<uInt> >;
  template class SimpleCountedPtr<Lattice<uInt> >;
  template class SimpleCountedConstPtr<Lattice<uInt> >;
  template class PtrRep<Lattice<uInt> >;

  // 1070 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<Complex> >;
  template class CountedConstPtr<LatticeIterInterface<Complex> >;
  template class SimpleCountedPtr<LatticeIterInterface<Complex> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<Complex> >;
  template class PtrRep<LatticeIterInterface<Complex> >;

  // 1080 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<DComplex> >;
  template class CountedConstPtr<LatticeIterInterface<DComplex> >;
  template class SimpleCountedPtr<LatticeIterInterface<DComplex> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<DComplex> >;
  template class PtrRep<LatticeIterInterface<DComplex> >;

  // 1090 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<Bool> >;
  template class CountedConstPtr<LatticeIterInterface<Bool> >;
  template class SimpleCountedPtr<LatticeIterInterface<Bool> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<Bool> >;
  template class PtrRep<LatticeIterInterface<Bool> >;

  // 1100 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<Double> >;
  template class CountedConstPtr<LatticeIterInterface<Double> >;
  template class SimpleCountedPtr<LatticeIterInterface<Double> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<Double> >;
  template class PtrRep<LatticeIterInterface<Double> >;

  // 1110 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<Float> >;
  template class CountedConstPtr<LatticeIterInterface<Float> >;
  template class SimpleCountedPtr<LatticeIterInterface<Float> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<Float> >;
  template class PtrRep<LatticeIterInterface<Float> >;

  // 1120 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<Int> >;
  template class CountedConstPtr<LatticeIterInterface<Int> >;
  template class SimpleCountedPtr<LatticeIterInterface<Int> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<Int> >;
  template class PtrRep<LatticeIterInterface<Int> >;

  // 1130 casa/Utilities/CountedPtr.cc
  template class CountedPtr<LatticeIterInterface<uInt> >;
  template class CountedConstPtr<LatticeIterInterface<uInt> >;
  template class SimpleCountedPtr<LatticeIterInterface<uInt> >;
  template class SimpleCountedConstPtr<LatticeIterInterface<uInt> >;
  template class PtrRep<LatticeIterInterface<uInt> >;

  // 1140 casa/Utilities/CountedPtr.cc
  template class CountedPtr<TempLattice<Float> >;
  template class CountedConstPtr<TempLattice<Float> >;
  template class SimpleCountedPtr<TempLattice<Float> >;
  template class SimpleCountedConstPtr<TempLattice<Float> >;
  template class PtrRep<TempLattice<Float> >;

  // 1000 casa/Utilities/PtrHolder.cc 
  template class PtrHolder<LCRegionSingle>;

  // 1010 casa/Utilities/PtrHolder.cc 
  template class PtrHolder<LatticeIterator<Bool> >;

  // 1020 casa/Utilities/PtrHolder.cc 
  template class PtrHolder<LatticeIterator<Float> >;

  // ============================================================================
  //
  //  Additional templates for test programs
  //
  // ============================================================================

  // 1000 casa/Utilities/GenSort.cc
//   template class GenSort<Float>;
//   template class GenSort<Double>;
//   template class GenSort<Complex>;

}
