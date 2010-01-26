##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_TABLES
  LogTables
  Tables
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_TABLES ${CASACORE_MODULES_TABLES})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_TABLES} ${MODULE_TABLES}/*.yy)
  list (APPEND MODULE_TABLES_BISON ${bison_${MODULE_TABLES}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_TABLES} ${MODULE_TABLES}/*.ll)
  list (APPEND MODULE_TABLES_FLEX ${flex_${MODULE_TABLES}})
  ## Source files
  file (GLOB tables_${MODULE_TABLES} ${MODULE_TABLES}/*.cc ${MODULE_TABLES}/*.tcc)
  list (APPEND MODULE_TABLES_SOURCES ${tables_${MODULE_TABLES}})
endforeach (MODULE_TABLES)

## Processing of Bison input files

foreach (bison_input ${MODULE_TABLES_BISON})
  casacore_process_bison (${bison_input} ${MODULE_TABLES_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_TABLES_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_TABLES_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_tables SHARED ${MODULE_TABLES_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_tables STATIC ${MODULE_TABLES_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_tables
  casa_casa
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_tables
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_TABLES_HEADERS *.h)
install (FILES ${MODULES_TABLES_HEADERS}
  DESTINATION include/casacore/tables
  )

foreach (MODULE_TABLES ${CASACORE_MODULES_TABLES})
  ## assemble list of header files
  FILE (GLOB MODULES_TABLES_HEADERS ${MODULE_TABLES}/*.h ${MODULE_TABLES}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_TABLES_HEADERS}
    DESTINATION include/casacore/tables/${MODULE_TABLES}
    )
endforeach (MODULE_TABLES)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_TABLES ${CASACORE_MODULES_TABLES})
    add_subdirectory (${MODULE_TABLES}/test)
  endforeach (MODULE_TABLES)
endif (CASACORE_ENABLE_TESTING)
