
## -----------------------------------------------------------------------------
## libtables

SET (tables_modules
  LogTables
  Tables
  )

## collect the source files in the modules

foreach (tables_module ${tables_modules})
  FILE (GLOB tables_${tables_module} ${tables_module}/*.cc ${tables_module}/*.tcc)
  LIST(APPEND tables_sources ${tables_${tables_module}})
endforeach (tables_module)

## -----------------------------------------------------------------------------
## build the library

add_library (tables
  ${tables_sources}
)
target_link_libraries (tables casa)

## -----------------------------------------------------------------------------
## Files processed by Flex
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

if (flex_bin)
  message (STATUS "Processing flex input files...")
  # Tables/RecordGram.ll
  execute_process (
    WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
    COMMAND flex -t -PRecordGram RecordGram.ll > RecordGram.lcc
    TIMEOUT 20
    ERROR_VARIABLE flex_error
    OUTPUT_QUIET
    )
  if (flex_error)
    message (SEND_ERROR "There was an error processing RecordGram.ll!")
    message (SEND_ERROR ${flex_error})
  endif (flex_error)
  # Tables/TableGram.ll
  execute_process (
    WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
    COMMAND flex -t -PTableGram TableGram.ll > TableGram.lcc
    TIMEOUT 20
    ERROR_VARIABLE flex_error
    OUTPUT_QUIET
    )
  if (flex_error)
    message (SEND_ERROR "There was an error processing TableGram.ll!")
    message (SEND_ERROR ${flex_error})
  endif (flex_error)
else (flex_bin)
  message (SEND_ERROR "Unable to process flex input files!")
endif (flex_bin)

## -----------------------------------------------------------------------------
## Files processed by Bison
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }

if (bison_bin)
  message (STATUS "Processing bison input files...")
  # Tables/RecordGram.yy
  execute_process (
    WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
    COMMAND bison -p RecordGram -o RecordGram.ycc RecordGram.yy
    TIMEOUT 20
    ERROR_VARIABLE bison_error
    OUTPUT_QUIET
    )
  # Tables/TableGram.yy
  execute_process (
    WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
    COMMAND bison -p TableGram -o TableGram.ycc TableGram.yy
    TIMEOUT 20
    ERROR_VARIABLE bison_error
    OUTPUT_QUIET
    )
else (bison_bin)
  message (SEND_ERROR "Unable to process bison input files!")
endif (bison_bin)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (tables_module ${tables_modules})
  LIST (APPEND tests "${tables_module}/test")
endforeach (tables_module)

SUBDIRS (${tests})

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS tables
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
