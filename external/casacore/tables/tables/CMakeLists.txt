##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## libtables

SET (tables_modules
  LogTables
  Tables
  )

## -----------------------------------------------------------------------------
## Files processed by Flex
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

find_path (HAVE_RECORDGRAM_LCC RecordGram.lcc
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  )
find_path (HAVE_TABLEGRAM_LCC TableGram.lcc
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  )

if (flex_bin)
  # Tables/RecordGram.ll
  if (NOT HAVE_RECORDGRAM_LCC)
    message (STATUS "[casacore] Processing tables/tables/Tables/RecordGram.ll ...")
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
      COMMAND flex -t -PRecordGram RecordGram.ll
      TIMEOUT 20
      OUTPUT_FILE RecordGram.lcc
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    if (flex_error)
      message (SEND_ERROR "There was an error processing RecordGram.ll!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
  endif (NOT HAVE_RECORDGRAM_LCC)
  # Tables/TableGram.ll
  if (NOT HAVE_TABLEGRAM_LCC)
    message (STATUS "[casacrore] Processing tables/tables/Tables/TableGram.ll ...")
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
      COMMAND flex -t -PTableGram TableGram.ll
      TIMEOUT 20
      OUTPUT_FILE TableGram.lcc
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    if (flex_error)
      message (SEND_ERROR "There was an error processing TableGram.ll!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
  endif (NOT HAVE_TABLEGRAM_LCC)
else (flex_bin)
  message (SEND_ERROR "[casacore] Unable to process flex input files!")
endif (flex_bin)

## -----------------------------------------------------------------------------
## Files processed by Bison
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }

find_path (HAVE_RECORDGRAM_YCC RecordGram.ycc
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  )
find_path (HAVE_TABLEGRAM_YCC TableGram.ycc
  ${casacore_SOURCE_DIR}/tables/tables/Tables
  )

if (bison_bin)
  # Tables/RecordGram.yy
  if (NOT HAVE_RECORDGRAM_YCC)
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
      COMMAND bison -p RecordGram -o RecordGram.ycc RecordGram.yy
      TIMEOUT 20
      ERROR_VARIABLE bison_error
      OUTPUT_QUIET
      )
  endif (NOT HAVE_RECORDGRAM_YCC)
  # Tables/TableGram.yy
  if (NOT HAVE_TABLEGRAM_YCC)
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/Tables
      COMMAND bison -p TableGram -o TableGram.ycc TableGram.yy
      TIMEOUT 20
      ERROR_VARIABLE bison_error
      OUTPUT_QUIET
      )
  endif (NOT HAVE_TABLEGRAM_YCC)
else (bison_bin)
  message (SEND_ERROR "Unable to process bison input files!")
endif (bison_bin)

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (tables_module ${tables_modules})
  FILE (GLOB tables_${tables_module} ${tables_module}/*.cc ${tables_module}/*.tcc)
  LIST(APPEND tables_sources ${tables_${tables_module}})
endforeach (tables_module)

## build the library

add_library (casa_tables
  ${tables_sources}
)
target_link_libraries (casa_tables casa_casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (tables_module ${tables_modules})
    add_subdirectory (${tables_module}/test)
  endforeach (tables_module)
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation

## Library

install (TARGETS casa_tables
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB tables_headers *.h)
install (FILES ${tables_headers}
  DESTINATION include/casacore/tables
  )

foreach (tables_module ${tables_modules})
  ## assemble list of header files
  FILE (GLOB tables_headers ${tables_module}/*.h ${tables_module}/*.tcc)
  ## set installation location
  install (FILES ${tables_headers}
    DESTINATION include/casacore/tables/${tables_module}
    )
endforeach (tables_module)
