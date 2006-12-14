
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tBlock
  tDlink
  tDlist
  tHashMap
  tHashMapIO
  tHashMapIter
  tList
  tListMap2
  tListMap
  tObjectPool
  tOrdMap2
  tOrdMap
  tPoolStack
  tQueue
  tRecord
  tRecordDesc
  tSimOrdMap
  tStack
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

