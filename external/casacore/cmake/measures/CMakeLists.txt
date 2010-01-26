##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_MEASURES
  Measures
  TableMeasures
  )

add_subdirectory (fortran/sofa)

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_MEASURES ${CASACORE_MODULES_MEASURES})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_MEASURES} ${MODULE_MEASURES}/*.yy)
  list (APPEND MODULE_MEASURES_BISON ${bison_${MODULE_MEASURES}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_MEASURES} ${MODULE_MEASURES}/*.ll)
  list (APPEND MODULE_MEASURES_FLEX ${flex_${MODULE_MEASURES}})
  ## Source files
  file (GLOB measures_${MODULE_MEASURES} ${MODULE_MEASURES}/*.cc ${MODULE_MEASURES}/*.tcc)
  list (APPEND MODULE_MEASURES_SOURCES ${measures_${MODULE_MEASURES}})
endforeach (MODULE_MEASURES)

## Processing of Bison input files

foreach (bison_input ${MODULE_MEASURES_BISON})
  casacore_process_bison (${bison_input} ${MODULE_MEASURES_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_MEASURES_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_MEASURES_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_measures SHARED ${MODULE_MEASURES_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_measures STATIC ${MODULE_MEASURES_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_measures
  sofa
  casa_tables
  casa_scimath
  casa_casa
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_measures
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_MEASURES_HEADERS *.h)
install (FILES ${MODULES_MEASURES_HEADERS}
  DESTINATION include/casacore/measures
  )

foreach (MODULE_MEASURES ${CASACORE_MODULES_MEASURES})
  ## assemble list of header files
  FILE (GLOB MODULES_MEASURES_HEADERS ${MODULE_MEASURES}/*.h ${MODULE_MEASURES}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_MEASURES_HEADERS}
    DESTINATION include/casacore/measures/${MODULE_MEASURES}
    )
endforeach (MODULE_MEASURES)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_MEASURES ${CASACORE_MODULES_MEASURES})
    add_subdirectory (${MODULE_MEASURES}/test)
  endforeach (MODULE_MEASURES)
endif (CASACORE_ENABLE_TESTING)
