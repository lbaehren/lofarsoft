
## -----------------------------------------------------------------------------
## Files processed by Flex
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

find_program (flex_bin flex
  PATHS /usr/bin /usr/local/bin
  )

if (flex_bin)
  # Tables/RecordGram.l
  execute_process (
    WORKING_DIRECTORY ${CASA_SOURCE_DIR}/tables/implement/Tables
    COMMAND flex -t -PRecordGram RecordGram.l > RecordGram.lcc
    TIMEOUT 20
    ERROR_VARIABLE flex_error
    OUTPUT_QUIET
    )
  # Tables/TableGram.l
  execute_process (
    WORKING_DIRECTORY ${CASA_SOURCE_DIR}/tables/implement/Tables
    COMMAND flex -t -PTableGram TableGram.l > TableGram.lcc
    TIMEOUT 20
    ERROR_VARIABLE flex_error
    OUTPUT_QUIET
    )
else (flex_bin)
  message (SEND_ERROR "Unable to locate Flex executable!")
endif (flex_bin)

## -----------------------------------------------------------------------------
## Files processed by Bison
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }

find_program (bison_bin bison
  PATHS /usr/bin /usr/local/bin
  )

if (bison_bin)
  # Tables/RecordGram.y
  execute_process (
    WORKING_DIRECTORY ${CASA_SOURCE_DIR}/tables/implement/Tables
    COMMAND bison -p RecordGram -o RecordGram.ycc RecordGram.y
    TIMEOUT 20
    ERROR_VARIABLE bison_error
    OUTPUT_QUIET
    )
  # Tables/TableGram.y
  execute_process (
    WORKING_DIRECTORY ${CASA_SOURCE_DIR}/tables/implement/Tables
    COMMAND bison -p TableGram -o TableGram.ycc TableGram.y
    TIMEOUT 20
    ERROR_VARIABLE bison_error
    OUTPUT_QUIET
    )
else (bison_bin)
  message (SEND_ERROR "Unable to locate Bison executable!")
endif (bison_bin)

## -----------------------------------------------------------------------------
## Collect the source files for the library and the template instantiation

SET (tables_modules
  LogTables
  TablePlot
  Tables
  )

## -----------------------------------------------------------------------------
## build the library (libtables)

## collect the source files for the library

foreach (tables_module ${tables_modules})
  FILE (GLOB tables_${tables_module} ${tables_module}/*.cc ${tables_module}/*.lcc ${tables_module}/*.ycc)
  LIST(APPEND tables_sources ${tables_${tables_module}})
endforeach (tables_module)

## if necessary, remove items from the list

list (REMOVE_ITEM tables_Tables Tables/ExprFuncNode.cc)

## collect the files responsible for template instantiation

FILE (GLOB tables_templates ../../templates/tables/*.cc)

## start building the library

add_library (tables
  ${tables_LogTables}
  ${tables_TablePlot}
  ${tables_Tables}
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
