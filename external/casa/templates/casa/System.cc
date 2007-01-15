
#include <casa/Arrays/Vector.h>

#include <casa/System/AipsrcValue.cc>
#include <casa/System/AipsrcVector.cc>

namespace casa {

  template class AipsrcValue<Int>;
  template class AipsrcValue<Float>;
  template class AipsrcValue<Double>;
  template class AipsrcValue<String>;
  
  template class AipsrcVector<Int>;
  template class AipsrcVector<Float>;
  template class AipsrcVector<Double>;
  template class AipsrcVector<String>;
}
