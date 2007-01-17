
PROJECT (CASA_casa_tContainers)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  tBlock
  tDlink
  tDlist
  tHashMap
  tHashMapIO
  tHashMapIter
  tList
#  tListMap
#  tListMap2
  tObjectPool
#  tOrdMap2
#  tOrdMap
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

