
// include of header files

#include <casa/BasicSL/Complex.h>
#include <fits/FITS/fits.h>
#include <fits/FITS/hdu.h>
#include <fits/FITS/FITSFieldCopier.h>

// include of implementation files

#include <fits/FITS/FITS2.cc>
#include <fits/FITS/hdu.cc>

namespace casa {
  
  // fits/FITS/FITS2.cc fits/FITS/hdu.h
  template void ReadFITSin(PrimaryArray<Double> &, Array<Float> &, Bool &, String &, String *, Vector<String> *, Vector<Float> *, Vector<Float> *, Vector<Float> *, Map<String, Double> *, String *);
  template void ReadFITSin(PrimaryArray<FitsLong> &, Array<Float> &, Bool &, String &, String *, Vector<String> *, Vector<Float> *, Vector<Float> *, Vector<Float> *, Map<String, Double> *, String *);
  template void ReadFITSin(PrimaryArray<Float> &, Array<Float> &, Bool &, String &, String *, Vector<String> *, Vector<Float> *, Vector<Float> *, Vector<Float> *, Map<String, Double> *, String *);
  template void ReadFITSin(PrimaryArray<Short> &, Array<Float> &, Bool &, String &, String *, Vector<String> *, Vector<Float> *, Vector<Float> *, Vector<Float> *, Map<String, Double> *, String *);
  template void ReadFITSin(PrimaryArray<uChar> &, Array<Float> &, Bool &, String &, String *, Vector<String> *, Vector<Float> *, Vector<Float> *, Vector<Float> *, Map<String, Double> *, String *);
  
  // fits/FITS/fits.h casa/BasicSL/Complex.h 
  template class NoConvert<Complex>;
  template class NoConvert<DComplex>;
  template class NoConvert<IComplex>;
  template class NoConvert<Char>;
  template class NoConvert<Double>;
  template class NoConvert<FitsLogical>;
  template class NoConvert<FitsLong>;
  template class NoConvert<FitsVADesc>;
  template class NoConvert<Float>;
  template class NoConvert<Short>;
  template class NoConvert<uChar>;
  
  // fits/FITS/hdu.cc casa/BasicSL/Complex.h 
  template class FitsArray<Complex>;
  template class FitsArray<DComplex>;
  template class FitsArray<IComplex>;
  template class FitsField<Complex>;
  template class FitsField<DComplex>;
  template class FitsField<IComplex>;
  template class FitsArray<Char>;
  template class FitsArray<Double>;
  template class FitsArray<FitsLogical>;
  template class FitsArray<FitsLong>;
  template class FitsArray<FitsVADesc>;
  template class FitsArray<Float>;
  template class FitsArray<Short>;
  template class FitsArray<uChar>;
  template class FitsField<Char>;
  template class FitsField<Double>;
  template class FitsField<FitsLogical>;
  template class FitsField<FitsLong>;
  template class FitsField<FitsVADesc>;
  template class FitsField<Float>;
  template class FitsField<Short>;
  template class FitsField<uChar>;
  template class ImageExtension<Double>;
  template class ImageExtension<FitsLong>;
  template class ImageExtension<Float>;
  template class ImageExtension<Short>;
  template class ImageExtension<uChar>;
  template class PrimaryArray<Double>;
  template class PrimaryArray<FitsLong>;
  template class PrimaryArray<Float>;
  template class PrimaryArray<Int>;
  template class PrimaryArray<Short>;
  template class PrimaryArray<uChar>;
  template class PrimaryGroup<FitsLong>;
  template class PrimaryGroup<Float>;
  template class PrimaryGroup<Short>;
  
  // fits/FITS/FITSFieldCopier.h 
  template class ArrayFITSFieldCopier<Bool, FitsLogical>;
  template class ArrayFITSFieldCopier<Complex, Complex>;
  template class ArrayFITSFieldCopier<DComplex, DComplex>;
  template class ArrayFITSFieldCopier<Double, Double>;
  template class ArrayFITSFieldCopier<Float, Float>;
  template class ArrayFITSFieldCopier<Int, FitsLong>;
  template class ArrayFITSFieldCopier<Short, Short>;
  template class ArrayFITSFieldCopier<uChar, uChar>;
  template class ScalarFITSFieldCopier<Bool, FitsLogical>;
  template class ScalarFITSFieldCopier<Complex, Complex>;
  template class ScalarFITSFieldCopier<DComplex, DComplex>;
  template class ScalarFITSFieldCopier<Double, Double>;
  template class ScalarFITSFieldCopier<Float, Float>;
  template class ScalarFITSFieldCopier<Int, FitsLong>;
  template class ScalarFITSFieldCopier<Short, Short>;
  template class ScalarFITSFieldCopier<uChar, uChar>;
  template class VariableArrayFITSFieldCopier<Bool, FitsLogical>;
  template class VariableArrayFITSFieldCopier<Complex, Complex>;
  template class VariableArrayFITSFieldCopier<DComplex, DComplex>;
  template class VariableArrayFITSFieldCopier<Double, Double>;
  template class VariableArrayFITSFieldCopier<Float, Float>;
  template class VariableArrayFITSFieldCopier<Int, FitsLong>;
  template class VariableArrayFITSFieldCopier<Short, Short>;
  template class VariableArrayFITSFieldCopier<uChar, uChar>;

  // ============================================================================
  //
  //  Template instantiations required for test programs
  //
  // ============================================================================

  template class PrimaryGroup<Double>;
  template class PrimaryGroup<uChar>;
  
}
