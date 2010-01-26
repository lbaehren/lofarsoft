##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_COORDINATES
  Coordinates
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_COORDINATES ${CASACORE_MODULES_COORDINATES})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_COORDINATES} ${MODULE_COORDINATES}/*.yy)
  list (APPEND MODULE_COORDINATES_BISON ${bison_${MODULE_COORDINATES}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_COORDINATES} ${MODULE_COORDINATES}/*.ll)
  list (APPEND MODULE_COORDINATES_FLEX ${flex_${MODULE_COORDINATES}})
  ## Source files
  file (GLOB coordinates_${MODULE_COORDINATES} ${MODULE_COORDINATES}/*.cc ${MODULE_COORDINATES}/*.tcc)
  list (APPEND MODULE_COORDINATES_SOURCES ${coordinates_${MODULE_COORDINATES}})
endforeach (MODULE_COORDINATES)

## Processing of Bison input files

foreach (bison_input ${MODULE_COORDINATES_BISON})
  casacore_process_bison (${bison_input} ${MODULE_COORDINATES_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_COORDINATES_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_COORDINATES_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_coordinates SHARED ${MODULE_COORDINATES_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_coordinates STATIC ${MODULE_COORDINATES_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions
target_link_libraries (casa_coordinates
  casa_fits
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  ${WCSLIB_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_coordinates
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_COORDINATES_HEADERS *.h)
install (FILES ${MODULES_COORDINATES_HEADERS}
  DESTINATION include/casacore/coordinates
  )

foreach (MODULE_COORDINATES ${CASACORE_MODULES_COORDINATES})
  ## assemble list of header files
  FILE (GLOB MODULES_COORDINATES_HEADERS ${MODULE_COORDINATES}/*.h ${MODULE_COORDINATES}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_COORDINATES_HEADERS}
    DESTINATION include/casacore/coordinates/${MODULE_COORDINATES}
    )
endforeach (MODULE_COORDINATES)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_COORDINATES ${CASACORE_MODULES_COORDINATES})
    add_subdirectory (${MODULE_COORDINATES}/test)
  endforeach (MODULE_COORDINATES)
endif (CASACORE_ENABLE_TESTING)
