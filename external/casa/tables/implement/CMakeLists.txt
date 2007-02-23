
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
      string (REGEX REPLACE "\\.l" ".lcc" flex_output ${flex_input})
      ## Make processing conditional, since files also could have been generated
      ## manually; later option should be kept for providing an override machanism
      if (NOT ${flex_output})
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
      endif (NOT ${flex_output})
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
