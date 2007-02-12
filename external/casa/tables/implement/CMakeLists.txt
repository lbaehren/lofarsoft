
PROJECT (casa_tables_libtables)

## -----------------------------------------------------------------------------
## Collect the source files for the library and the template instantiation

SET (tables_modules
  LogTables
  TablePlot
  Tables
  )

## -----------------------------------------------------------------------------
## Run flex and bison
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }

if (flex_bin AND bison_bin)
  foreach (tables_module ${tables_modules})
    message (STATUS "Parsing files in ${casa_tables_libtables_SOURCE_DIR}/${tables_module} ...")
    ## [1] get input files for Flex
    FILE (GLOB flex_inputs ${tables_module}/*.l)
    foreach (flex_input ${flex_inputs})
      ## Make processing conditional, since files also could have been generated
      ## manually; later option should be kept for providing an override machanism
      if (NOT ${flex_input})
	string (REGEX REPLACE "\\.l" ".lcc" flex_output ${flex_input})
	execute_process (
	  WORKING_DIRECTORY "${casa_tables_libtables_SOURCE_DIR}/${tables_module}"
	  COMMAND "${flex_bin} --stdout ${flex_input} > ${flex_output}"
	  TIMEOUT 30
	  ERROR_VARIABLE flex_error
	  OUTPUT_QUIET
	  )
	message ("${flex_input} -> ${flex_output}")
	if (flex_error)
	  message (FATAL_ERROR "flex failed!\n${flex_error}")
	endif (flex_error)
      endif (NOT ${flex_input})
    endforeach (flex_input)
    ## [2] get input files for Bison
    FILE (GLOB bison_inputs ${tables_module}/*.y)
    foreach (bison_input ${bison_inputs})
      string (REGEX REPLACE "\\.y" ".ycc" bison_output ${bison_input})
      message ("${bison_input} -> ${bison_output}")
    endforeach (bison_input)
  endforeach (tables_module)
endif (flex_bin AND bison_bin)

## -----------------------------------------------------------------------------
## build the library (libtables)

## collect the source files for the library

foreach (tables_module ${tables_modules})
  FILE (GLOB tables_${tables_module} ${tables_module}/*.cc  ${tables_module}/*.lcc)
  LIST(APPEND tables_sources ${tables_${tables_module}})
endforeach (tables_module)

## collect the files responsible for template instantiation

FILE (GLOB tables_templates ../../templates/tables/*.cc)

## start building the library

add_library (tables
  ${tables_LogTables}
  ${tables_TablePlot}
  ## -- Tables --
  Tables/ArrColData.cc
  Tables/ArrColDesc.cc
  Tables/ArrayColumn.cc
  Tables/BaseColDesc.cc
  Tables/BaseColumn.cc
  Tables/BaseMappedArrayEngine.cc
  Tables/BaseTabIter.cc
  Tables/BaseTable.cc
  Tables/ColDescSet.cc
  Tables/ColumnCache.cc
  Tables/ColumnDesc.cc
  Tables/ColumnSet.cc
  Tables/ColumnsIndex.cc
  Tables/ColumnsIndexArray.cc
  Tables/CompressComplex.cc
  Tables/CompressFloat.cc
  Tables/DataManAccessor.cc
  Tables/DataManError.cc
  Tables/DataManager.cc
  Tables/ExprConeNode.cc
  Tables/ExprDerNode.cc
  Tables/ExprDerNodeArray.cc
  #	Tables/ExprFuncNode.cc
  #	Tables/ExprFuncNodeArray.cc
  Tables/ExprLogicNode.cc
  Tables/ExprLogicNodeArray.cc
  Tables/ExprMathNode.cc
  Tables/ExprMathNodeArray.cc
  Tables/ExprNode.cc
  #	Tables/ExprNodeArray.cc
  Tables/ExprNodeRecord.cc
  Tables/ExprNodeRep.cc
  Tables/ExprNodeSet.cc
  Tables/ExprRange.cc
  Tables/ExternalLockSync.cc
  Tables/ForwardCol.cc
  Tables/ForwardColRow.cc
  Tables/ISMBase.cc
  Tables/ISMBucket.cc
  Tables/ISMColumn.cc
  Tables/ISMIndColumn.cc
  Tables/ISMIndex.cc
  Tables/IncrStManAccessor.cc
  Tables/IncrementalStMan.cc
  Tables/MSMBase.cc
  Tables/MSMColumn.cc
  Tables/MSMDirColumn.cc
  Tables/MSMIndColumn.cc
  Tables/MappedArrayEngine.cc
  Tables/MemoryStMan.cc
  Tables/MemoryTable.cc
  Tables/NullTable.cc
  Tables/PlainColumn.cc
  Tables/PlainTable.cc
  Tables/ReadAsciiTable.cc
  Tables/RecordExpr.cc
  Tables/RecordGram.cc
  Tables/RefColumn.cc
  Tables/RefRows.cc
  Tables/RefTable.cc
  Tables/RetypedArrayEngine.cc
  Tables/RetypedArraySetGet.cc
  Tables/RowCopier.cc
  Tables/SSMBase.cc
  Tables/SSMColumn.cc
  Tables/SSMDirColumn.cc
  Tables/SSMIndColumn.cc
  Tables/SSMIndStringColumn.cc
  Tables/SSMIndex.cc
  Tables/SSMStringHandler.cc
  Tables/ScaColData.cc
  Tables/ScaColDesc.cc
  Tables/ScaRecordColData.cc
  Tables/ScaRecordColDesc.cc
  Tables/ScalarColumn.cc
  Tables/ScaledArrayEngine.cc
  Tables/ScaledComplexData.cc
  Tables/SetupNewTab.cc
  Tables/StArrAipsIO.cc
  Tables/StArrayFile.cc
  Tables/StIndArrAIO.cc
  Tables/StIndArray.cc
  Tables/StManAipsIO.cc
  Tables/StManColumn.cc
  Tables/StandardStMan.cc
  Tables/StandardStManAccessor.cc
  Tables/SubTabDesc.cc
  Tables/TSMColumn.cc
  Tables/TSMCoordColumn.cc
  Tables/TSMCube.cc
  Tables/TSMDataColumn.cc
  Tables/TSMFile.cc
  Tables/TSMIdColumn.cc
  Tables/TSMShape.cc
  Tables/TVec.cc
  Tables/TVecLogic.cc
  Tables/TVecMath.cc
  Tables/TVecScaCol.cc
  Tables/TVecTemp.cc
  Tables/TaQLNode.cc
  Tables/TaQLNodeDer.cc
  Tables/TaQLNodeHandler.cc
  Tables/TaQLNodeRep.cc
  Tables/TaQLNodeResult.cc
  Tables/TaQLNodeVisitor.cc
  Tables/TaQLResult.cc
  Tables/TaQLStyle.cc
  Tables/TabPath.cc
  Tables/TabVecLogic.cc
  Tables/TabVecMath.cc
  Tables/Table.cc
  Tables/TableAttr.cc
  Tables/TableCache.cc
  Tables/TableColumn.cc
  Tables/TableCopy.cc
  Tables/TableDesc.cc
  Tables/TableError.cc
  Tables/TableExprData.cc
  Tables/TableExprId.cc
  Tables/TableGram.cc
  Tables/TableIndexProxy.cc
  Tables/TableInfo.cc
  Tables/TableIter.cc
  Tables/TableIterProxy.cc
  Tables/TableKeyword.cc
  Tables/TableLock.cc
  Tables/TableLockData.cc
  Tables/TableLocker.cc
  Tables/TableParse.cc
  Tables/TableProxy.cc
  Tables/TableRecord.cc
  Tables/TableRecordRep.cc
  Tables/TableRow.cc
  Tables/TableRowProxy.cc
  Tables/TableSyncData.cc
  Tables/TableVector.cc
  Tables/TiledCellStMan.cc
  Tables/TiledColumnStMan.cc
  Tables/TiledDataStMan.cc
  Tables/TiledDataStManAccessor.cc
  Tables/TiledFileAccess.cc
  Tables/TiledFileHelper.cc
  Tables/TiledShapeStMan.cc
  Tables/TiledStMan.cc
  Tables/TiledStManAccessor.cc
  Tables/VSCEngine.cc
  Tables/VirtArrCol.cc
  Tables/VirtColEng.cc
  Tables/VirtScaCol.cc
  Tables/VirtualTaQLColumn.cc
  ## -- Template instantiation
  ${tables_templates}
  )

## library dependencies

target_link_libraries (tables casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

#foreach (tables_module ${tables_modules})
#  LIST (APPEND tests "${tables_module}/test")
#endforeach (tables_module)

set (tests
  LogTables/test
  Tables/test
)

SUBDIRS (${tests})
