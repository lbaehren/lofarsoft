
#include <casa/Utilities/CountedPtr.cc>

#include <tables/LogTables/LoggerHolder.h>

namespace casa {
  
  template class PtrRep<LoggerHolderRep>;
  template class CountedPtr<LoggerHolderRep>;
  template class CountedConstPtr<LoggerHolderRep>;
  template class SimpleCountedPtr<LoggerHolderRep>;
  template class SimpleCountedConstPtr<LoggerHolderRep>;
  
}
