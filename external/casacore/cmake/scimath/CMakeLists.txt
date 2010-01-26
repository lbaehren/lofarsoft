##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_SCIMATH
  Fitting
  Functionals
  Mathematics
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_SCIMATH ${CASACORE_MODULES_SCIMATH})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_SCIMATH} ${MODULE_SCIMATH}/*.yy)
  list (APPEND MODULE_SCIMATH_BISON ${bison_${MODULE_SCIMATH}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_SCIMATH} ${MODULE_SCIMATH}/*.ll)
  list (APPEND MODULE_SCIMATH_FLEX ${flex_${MODULE_SCIMATH}})
  ## Source files
  file (GLOB scimath_${MODULE_SCIMATH} ${MODULE_SCIMATH}/*.cc ${MODULE_SCIMATH}/*.tcc)
  list (APPEND MODULE_SCIMATH_SOURCES ${scimath_${MODULE_SCIMATH}})
endforeach (MODULE_SCIMATH)

## Processing of Bison input files

foreach (bison_input ${MODULE_SCIMATH_BISON})
  casacore_process_bison (${bison_input} ${MODULE_SCIMATH_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_SCIMATH_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_SCIMATH_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_scimath SHARED ${MODULE_SCIMATH_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_scimath STATIC ${MODULE_SCIMATH_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_scimath
  casa_scimath_f
  casa_casa
  ${LAPACK_LIBRARIES}
  ${GFORTRAN_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_scimath
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_SCIMATH_HEADERS *.h)
install (FILES ${MODULES_SCIMATH_HEADERS}
  DESTINATION include/casacore/scimath
  )

foreach (MODULE_SCIMATH ${CASACORE_MODULES_SCIMATH})
  ## assemble list of header files
  FILE (GLOB MODULES_SCIMATH_HEADERS ${MODULE_SCIMATH}/*.h ${MODULE_SCIMATH}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_SCIMATH_HEADERS}
    DESTINATION include/casacore/scimath/${MODULE_SCIMATH}
    )
endforeach (MODULE_SCIMATH)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_SCIMATH ${CASACORE_MODULES_SCIMATH})
    add_subdirectory (${MODULE_SCIMATH}/test)
  endforeach (MODULE_SCIMATH)
endif (CASACORE_ENABLE_TESTING)
