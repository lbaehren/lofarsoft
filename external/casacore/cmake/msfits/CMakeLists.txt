##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_MSFITS
  MSFits
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_MSFITS ${CASACORE_MODULES_MSFITS})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_MSFITS} ${MODULE_MSFITS}/*.yy)
  list (APPEND MODULE_MSFITS_BISON ${bison_${MODULE_MSFITS}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_MSFITS} ${MODULE_MSFITS}/*.ll)
  list (APPEND MODULE_MSFITS_FLEX ${flex_${MODULE_MSFITS}})
  ## Source files
  file (GLOB msfits_${MODULE_MSFITS} ${MODULE_MSFITS}/*.cc ${MODULE_MSFITS}/*.tcc)
  list (APPEND MODULE_MSFITS_SOURCES ${msfits_${MODULE_MSFITS}})
endforeach (MODULE_MSFITS)

## Processing of Bison input files

foreach (bison_input ${MODULE_MSFITS_BISON})
  casacore_process_bison (${bison_input} ${MODULE_MSFITS_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_MSFITS_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_MSFITS_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_msfits SHARED ${MODULE_MSFITS_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_msfits STATIC ${MODULE_MSFITS_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_msfits
  ${CMSFITSIO_LIBRARIES}
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_msfits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_MSFITS_HEADERS *.h)
install (FILES ${MODULES_MSFITS_HEADERS}
  DESTINATION include/casacore/msfits
  )

foreach (MODULE_MSFITS ${CASACORE_MODULES_MSFITS})
  ## assemble list of header files
  FILE (GLOB MODULES_MSFITS_HEADERS ${MODULE_MSFITS}/*.h ${MODULE_MSFITS}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_MSFITS_HEADERS}
    DESTINATION include/casacore/msfits/${MODULE_MSFITS}
    )
endforeach (MODULE_MSFITS)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_MSFITS ${CASACORE_MODULES_MSFITS})
    add_subdirectory (${MODULE_MSFITS}/test)
  endforeach (MODULE_MSFITS)
endif (CASACORE_ENABLE_TESTING)
