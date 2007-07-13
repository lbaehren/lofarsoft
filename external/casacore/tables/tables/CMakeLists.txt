
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
## Run flex and bison
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }

if (flex_bin AND bison_bin)
  foreach (tables_module ${tables_modules})
    ## [1] get input files for Flex
    message (STATUS "Processing flex input files in ${casacore_SOURCE_DIR}/tables/tables/${tables_module}...")
    FILE (GLOB flex_inputs ${tables_module}/*.ll)
    foreach (flex_input ${flex_inputs})
      string (REGEX REPLACE "\\.ll" ".lcc" flex_output ${flex_input})
      ## Make processing conditional, since files also could have been generated
      ## manually; later option should be kept for providing an override machanism
      if (NOT ${flex_output})
	execute_process (
	  WORKING_DIRECTORY ${casacore_SOURCE_DIR}/tables/tables/${tables_module}
	  COMMAND "${flex_bin} --stdout ${flex_input} > ${flex_output}"
	  TIMEOUT 30
	  ERROR_VARIABLE flex_error
	  OUTPUT_QUIET
	  )
	message ("${flex_input} -> ${flex_output}")
	if (flex_error)
	  message (FATAL_ERROR "flex failed!\n${flex_error}")
	endif (flex_error)
      endif (NOT ${flex_output})
    endforeach (flex_input)
    ## [2] get input files for Bison
    FILE (GLOB bison_inputs ${tables_module}/*.yy)
    foreach (bison_input ${bison_inputs})
      string (REGEX REPLACE "\\.yy" ".ycc" bison_output ${bison_input})
      message ("${bison_input} -> ${bison_output}")
    endforeach (bison_input)
  endforeach (tables_module)
endif (flex_bin AND bison_bin)

## -----------------------------------------------------------------------------
## build the library

add_library (tables
  ${tables_sources}
)
target_link_libraries (tables casa)

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
