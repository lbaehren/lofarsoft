
## -----------------------------------------------------------------------------
## libms

SET (ms_modules
  MeasurementSets
  )

## collect the source files in the modules

foreach (ms_module ${ms_modules})
  ## source files for the library
  FILE (GLOB ms_${ms_module} ${ms_module}/*.cc ${ms_module}/*.tcc)
  LIST (APPEND ms_sources ${ms_${ms_module}})
endforeach (ms_module)

## -----------------------------------------------------------------------------
## build the library

add_library (ms
  ${ms_sources}
)
target_link_libraries (ms casa)

## -----------------------------------------------------------------------------
## Files processed by Bison & Flex
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

set (parser_inputs
  MSAntennaGram
  MSCorrGram
  MSFieldGram
  MSScanGram
  MSSpwGram
  MSTimeGram
  MSUvDistGram
  )

if (bison_bin)
  message (STATUS "Processing bison input files...")
  foreach (src ${parser_inputs})
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/ms/ms/MeasurementSets
      COMMAND bison -p ${src} -o ${src}.ycc ${src}.yy
      TIMEOUT 20
      ERROR_VARIABLE bison_error
      OUTPUT_QUIET
      )
#    if (bison_error)
#      message (SEND_ERROR "There was an error processing ${src}.yy!")
#      message (SEND_ERROR ${bison_error})
#    endif (bison_error)
  endforeach (src)
else (bison_bin)
  message (SEND_ERROR "Unable to process bison input files!")
endif (bison_bin)

if (flex_bin)
  message (STATUS "Processing flex input files...")
  foreach (src ${parser_inputs})
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/ms/ms/MeasurementSets
      COMMAND flex -t -P${src} ${src}.ll
      TIMEOUT 20
      OUTPUT_FILE ${src}.lcc
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    if (flex_error)
      message (SEND_ERROR "There was an error processing ${src}.ll!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
  endforeach (src)
else (flex_bin)
  message (SEND_ERROR "Unable to process flex input files!")
endif (flex_bin)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (ms_module ${ms_modules})
    list (APPEND tests "${ms_module}/test")
  endforeach (ms_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_tests)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS ms
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
