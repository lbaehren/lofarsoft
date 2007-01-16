
## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  tAipsIOCarray
  tAipsIO
  tBucketCache
  tBucketFile
  tByteIO
  tByteSink
  tByteSinkSource
  tFilebufIO
  tFileIO
  tLargeFilebufIO
  tLargeFileIO
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

