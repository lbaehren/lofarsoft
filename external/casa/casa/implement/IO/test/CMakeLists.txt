
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
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

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

