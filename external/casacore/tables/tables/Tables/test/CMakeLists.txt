
## ------------------------------------------------------------------------------
## Test programs for tables/Tables

SET (tests
  ascii2Table
  dRetypedArrayEngine
  dVSCEngine
  dVirtColEng
  nISMBucket
  tColumnsIndex
  tColumnsIndexArray
  tCompressComplex
  tCompressFloat
  tExprNode
  tForwardCol
  tForwardColRow
  tIncrementalStMan
  tMappedArrayEngine
  tMemoryStMan
  tMemoryTable
  tReadAsciiTable
  tReadAsciiTable2
  tRecordExpr
  tRecordGram
  tRecordGramTable
  tRefRows
  tRowCopier
  tSSMStringHandler
  tScalarRecordColumn
  tScaledArrayEngine
  tScaledComplexData
  tStArrayFile
  tStMan
  tStMan1
  tStandardStMan
  tTSMShape
  tTaQLNode
  tTable
  tTableCopy
  tTableDesc
  tTableDescHyper
  tTableExprData
  tTableGram
  tTableInfo
  tTableIter
  tTableKeywords
  tTableLock
  tTableLockSync
  tTableLockSync_2
  tTableRecord
  tTableRow
  tTableVector
  tTable_1
  tTable_2
  tTable_3
  tTable_4
  tTiledCellStM_1
  tTiledCellStMan
  tTiledColumnStMan
  tTiledDataStM_1
  tTiledDataStMan
  tTiledFileAccess
  tTiledShapeStM_1
  tTiledShapeStMan
  tTiledStMan
  tVSCEngine
  tVirtColEng
  tVirtualTaQLColumn
  )

## ------------------------------------------------------------------------------
## Build the test programs

include_directories (.)

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa_tables)
  ## testing 
  add_test (${src} ${src})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
