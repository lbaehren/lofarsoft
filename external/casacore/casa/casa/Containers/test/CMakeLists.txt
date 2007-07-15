
## ------------------------------------------------------------------------------
## Test programs for casa/Containers

SET (tests
  tBlock
  tHashMap
  tHashMapIO
  tHashMapIter
  tList
  tObjectPool
  tOrdMap
  tOrdMap2
  tPoolStack
  tQueue
  tRecord
  tRecordDesc
  tSimOrdMap
  tStack
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)
