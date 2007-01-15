
// include of header files

#include <casa/IO/test/tAipsIOCarray.h>

// include of implementation files

#include <casa/IO/AipsIOCarray.cc>

namespace casa {

#if !defined(AIPS_STDLIB) 
  template void putAipsIO(AipsIO &, uInt, uInt const *);
  template void getAipsIO(AipsIO &, uInt, uInt *);
  template void getnewAipsIO(AipsIO &, uInt &, uInt **);
#endif 

  template void putAipsIO(AipsIO &, uInt, AipsIOCarrayEx1 const *);
  template void getAipsIO(AipsIO &, uInt, AipsIOCarrayEx1 *);
  template void getnewAipsIO(AipsIO &, uInt &, AipsIOCarrayEx1 **);
  
}
