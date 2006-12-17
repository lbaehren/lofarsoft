
#include <casa/BasicSL/String.h>

#include <casa/Exceptions/Error.cc>

namespace casa {
  template class indexError<String>;
  template class indexError<Double>;
  template class indexError<Float>;
  template class indexError<Int>;
  template class indexError<Short>;
  template class indexError<uInt>;
  template class indexError<void *>;
}
