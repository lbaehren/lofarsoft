
#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Utilities/Copy.h>
#include <images/Images/ImageConcat.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/MomentCalculator.h>
#include <tables/Tables/ExprRange.h>

#include <casa/Utilities/Copy2.cc>
#include <casa/Utilities/PtrHolder.cc>
#include <images/Images/PagedImage.cc>

namespace casa {
  
  template void objset<int>(int*, int, unsigned int);
  template void objset<short>(short*, short, unsigned int);
  template void objset<long>(long*, long, unsigned int);
  template void objset<float>(float*, float, unsigned int);
  template void objset<double>(double*, double, unsigned int);
  template void objset<Complex>(Complex*, Complex, unsigned int);
  template void objset<DComplex>(DComplex*, DComplex, unsigned int);
  template void objset<String>(String*, String, unsigned int);

  template void objset<double>(double*, double, unsigned int, unsigned int);
  template void objset<Complex>(Complex*, Complex, unsigned int, unsigned int);
  template void objset<DComplex>(DComplex*, DComplex, unsigned int, unsigned int);

  template void objcopy<double>(double*, double const*, unsigned int);
  template void objcopy<double>(double*, double const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<String>(String*, String const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<DComplex>(DComplex*, DComplex const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<Complex>(Complex*, Complex const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<bool>(bool*, bool const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<float>(float*, float const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<unsigned char>(unsigned char*, unsigned char const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<int>(int*, int const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<unsigned int>(unsigned int*, unsigned int const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<short>(short*, short const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<unsigned short>(unsigned short*, unsigned short const*, unsigned int, unsigned int, unsigned int);
  template void objcopy<Complex>(Complex*, Complex const*, unsigned int);
  template void objcopy<DComplex>(DComplex*, DComplex const*, unsigned int);
  
  template void objset<String>(String*, String, unsigned int, unsigned int);
  template void objcopy<String>(String*, String const*, unsigned int);
  template void objset<bool>(bool*, bool, unsigned int);
  template void objset<bool>(bool*, bool, unsigned int, unsigned int);
  template void objcopy<bool>(bool*, bool const*, unsigned int);
  template void objset<float>(float*, float, unsigned int, unsigned int);
  template void objcopy<float>(float*, float const*, unsigned int);
  template void objset<int>(int*, int, unsigned int, unsigned int);
  template void objcopy<int>(int*, int const*, unsigned int);
  template void objset<short>(short*, short, unsigned int, unsigned int);
  template void objcopy<short>(short*, short const*, unsigned int);
  template void objset<unsigned char>(unsigned char*, unsigned char, unsigned int);
  template void objset<unsigned char>(unsigned char*, unsigned char, unsigned int, unsigned int);
  template void objcopy<unsigned char>(unsigned char*, unsigned char const*, unsigned int);
  template void objset<unsigned int>(unsigned int*, unsigned int, unsigned int);
  template void objset<unsigned int>(unsigned int*, unsigned int, unsigned int, unsigned int);
  template void objcopy<unsigned int>(unsigned int*, unsigned int const*, unsigned int);
  template void objset<unsigned short>(unsigned short*, unsigned short, unsigned int);
  template void objset<unsigned short>(unsigned short*, unsigned short, unsigned int, unsigned int);
  template void objcopy<unsigned short>(unsigned short*, unsigned short const*, unsigned int);

  template void objset<void*>(void**, void*, unsigned int);

  // objcopy
  
  template void objcopy<void*>(void**, void* const*, unsigned int);
  template void objcopy<TableExprRange>(TableExprRange*, TableExprRange const*, unsigned int);
  template void objmove<unsigned int>(unsigned int*, unsigned int const*, unsigned int);
  template void objmove<String>(String*, String const*, unsigned int);
  template void objmove<void*>(void**, void* const*, unsigned int);
  template void objmove<std::complex<double> >(std::complex<double>*, std::complex<double> const*, unsigned int);
  template void objmove<std::complex<float> >(std::complex<float>*, std::complex<float> const*, unsigned int);
  template void objmove<bool>(bool*, bool const*, unsigned int);
  template void objmove<double>(double*, double const*, unsigned int);
  template void objmove<float>(float*, float const*, unsigned int);
  template void objmove<unsigned char>(unsigned char*, unsigned char const*, unsigned int);
  template void objmove<int>(int*, int const*, unsigned int);
  template void objmove<short>(short*, short const*, unsigned int);
  template void objmove<unsigned short>(unsigned short*, unsigned short const*, unsigned int);
  
  // ============================================================================
  //
  //  libimages
  //
  // ============================================================================

  //1000 casa/Utilities/PtrHolder.cc 
  template class PtrHolder<ImageConcat<Float> >;
  template class PtrHolder<ImageInterface<Float> >;
  template class PtrHolder<MomentCalcBase<Float> >;
  template class PtrHolder<PagedImage<Float> >;

  template class PtrHolder<ImageConcat<Double> >;
  template class PtrHolder<ImageInterface<Double> >;
  template class PtrHolder<MomentCalcBase<Double> >;
  template class PtrHolder<PagedImage<Double> >;

  // 1000 images/Images/PagedImage.cc casa/BasicSL/Complex.h 
  template class PagedImage<Float>;
  template class PagedImage<Complex>;

}
