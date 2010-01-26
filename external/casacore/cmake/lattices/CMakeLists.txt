##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_LATTICES
  LatticeMath
  Lattices
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_LATTICES ${CASACORE_MODULES_LATTICES})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_LATTICES} ${MODULE_LATTICES}/*.yy)
  list (APPEND MODULE_LATTICES_BISON ${bison_${MODULE_LATTICES}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_LATTICES} ${MODULE_LATTICES}/*.ll)
  list (APPEND MODULE_LATTICES_FLEX ${flex_${MODULE_LATTICES}})
  ## Source files
  file (GLOB lattices_${MODULE_LATTICES} ${MODULE_LATTICES}/*.cc ${MODULE_LATTICES}/*.tcc)
  list (APPEND MODULE_LATTICES_SOURCES ${lattices_${MODULE_LATTICES}})
endforeach (MODULE_LATTICES)

## Processing of Bison input files

foreach (bison_input ${MODULE_LATTICES_BISON})
  casacore_process_bison (${bison_input} ${MODULE_LATTICES_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_LATTICES_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_LATTICES_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_lattices SHARED ${MODULE_LATTICES_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_lattices STATIC ${MODULE_LATTICES_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

target_link_libraries (casa_lattices
  sofa
  casa_tables
  casa_scimath
  casa_casa
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_lattices
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_LATTICES_HEADERS *.h)
install (FILES ${MODULES_LATTICES_HEADERS}
  DESTINATION include/casacore/lattices
  )

foreach (MODULE_LATTICES ${CASACORE_MODULES_LATTICES} HDF5)
  ## assemble list of header files
  FILE (GLOB MODULES_LATTICES_HEADERS ${MODULE_LATTICES}/*.h ${MODULE_LATTICES}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_LATTICES_HEADERS}
    DESTINATION include/casacore/lattices/${MODULE_LATTICES}
    )
endforeach (MODULE_LATTICES)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_LATTICES ${CASACORE_MODULES_LATTICES})
    add_subdirectory (${MODULE_LATTICES}/test)
  endforeach (MODULE_LATTICES)
endif (CASACORE_ENABLE_TESTING)
