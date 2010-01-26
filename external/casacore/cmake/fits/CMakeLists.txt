##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_FITS
  FITS
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_FITS ${CASACORE_MODULES_FITS})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_FITS} ${MODULE_FITS}/*.yy)
  list (APPEND MODULE_FITS_BISON ${bison_${MODULE_FITS}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_FITS} ${MODULE_FITS}/*.ll)
  list (APPEND MODULE_FITS_FLEX ${flex_${MODULE_FITS}})
  ## Source files
  file (GLOB fits_${MODULE_FITS} ${MODULE_FITS}/*.cc ${MODULE_FITS}/*.tcc)
  list (APPEND MODULE_FITS_SOURCES ${fits_${MODULE_FITS}})
endforeach (MODULE_FITS)

## Processing of Bison input files

foreach (bison_input ${MODULE_FITS_BISON})
  casacore_process_bison (${bison_input} ${MODULE_FITS_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_FITS_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_FITS_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_fits SHARED ${MODULE_FITS_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_fits STATIC ${MODULE_FITS_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_fits
  ${CFITSIO_LIBRARIES}
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_fits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_FITS_HEADERS *.h)
install (FILES ${MODULES_FITS_HEADERS}
  DESTINATION include/casacore/fits
  )

foreach (MODULE_FITS ${CASACORE_MODULES_FITS})
  ## assemble list of header files
  FILE (GLOB MODULES_FITS_HEADERS ${MODULE_FITS}/*.h ${MODULE_FITS}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_FITS_HEADERS}
    DESTINATION include/casacore/fits/${MODULE_FITS}
    )
endforeach (MODULE_FITS)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_FITS ${CASACORE_MODULES_FITS})
    add_subdirectory (${MODULE_FITS}/test)
  endforeach (MODULE_FITS)
endif (CASACORE_ENABLE_TESTING)
