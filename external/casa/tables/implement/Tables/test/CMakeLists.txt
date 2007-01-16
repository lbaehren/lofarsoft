
## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  ascii2Table
  dRetypedArrayEngine
  dVirtColEng
  dVSCEngine
  nISMBucket
  tColumnsIndexArray
  tColumnsIndex
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
  tRecordExpr
  tRecordGram
  tRecordGramTable
  tRefRows
  tRowCopier
  tScalarRecordColumn
  tScaledArrayEngine
  tScaledComplexData
  tSSMStringHandler
  tStandardStMan
  tStArrayFile
  tStMan1
  tStMan
  tTable_1
  tTable_2
  tTable_3
  tTable_4
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
  tTableLockSync_2
  tTableLockSync
  tTableRecord
  tTableRow
  tTableVector
  tTaQLNode
  tTiledCellStM_1
  tTiledCellStMan
  tTiledColumnStMan
  tTiledDataStM_1
  tTiledDataStMan
  tTiledFileAccess
  tTiledShapeStM_1
  tTiledShapeStMan
  tTiledStMan
  tTSMShape
  tVirtColEng
  tVirtualTaQLColumn
  tVSCEngine
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} tables casa)
ENDFOREACH (src)
