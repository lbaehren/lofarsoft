##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_MS
  MeasurementSets
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_MS ${CASACORE_MODULES_MS})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_MS} ${MODULE_MS}/*.yy)
  list (APPEND MODULE_MS_BISON ${bison_${MODULE_MS}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_MS} ${MODULE_MS}/*.ll)
  list (APPEND MODULE_MS_FLEX ${flex_${MODULE_MS}})
  ## Source files
  file (GLOB ms_${MODULE_MS} ${MODULE_MS}/*.cc ${MODULE_MS}/*.tcc)
  list (APPEND MODULE_MS_SOURCES ${ms_${MODULE_MS}})
endforeach (MODULE_MS)

## Processing of Bison input files

foreach (bison_input ${MODULE_MS_BISON})
  casacore_process_bison (${bison_input} ${MODULE_MS_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_MS_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_MS_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_ms SHARED ${MODULE_MS_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_ms STATIC ${MODULE_MS_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_ms
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_ms
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_MS_HEADERS *.h)
install (FILES ${MODULES_MS_HEADERS}
  DESTINATION include/casacore/ms
  )

foreach (MODULE_MS ${CASACORE_MODULES_MS})
  ## assemble list of header files
  FILE (GLOB MODULES_MS_HEADERS ${MODULE_MS}/*.h ${MODULE_MS}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_MS_HEADERS}
    DESTINATION include/casacore/ms/${MODULE_MS}
    )
endforeach (MODULE_MS)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_MS ${CASACORE_MODULES_MS})
    add_subdirectory (${MODULE_MS}/test)
  endforeach (MODULE_MS)
endif (CASACORE_ENABLE_TESTING)
