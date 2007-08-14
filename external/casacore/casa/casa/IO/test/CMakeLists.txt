
## ------------------------------------------------------------------------------
## Test programs for casa/IO

SET (tests
  tAipsIO
  tAipsIOCarray
  tBucketCache
  tBucketFile
  tByteIO
  tByteSink
  tByteSinkSource
  tFileIO
  tFilebufIO
  tLargeFileIO
  tLargeFilebufIO
  tLockFile
  tMappedIO
  tTapeIO
  tTypeIO
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
