##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_COMPONENTS
  ComponentModels
  SpectralComponents
  )

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (MODULE_COMPONENTS ${CASACORE_MODULES_COMPONENTS})
  ## Files processed by Bison
  file (GLOB bison_${MODULE_COMPONENTS} ${MODULE_COMPONENTS}/*.yy)
  list (APPEND MODULE_COMPONENTS_BISON ${bison_${MODULE_COMPONENTS}})
  ## Files processed by Flex
  file (GLOB flex_${MODULE_COMPONENTS} ${MODULE_COMPONENTS}/*.ll)
  list (APPEND MODULE_COMPONENTS_FLEX ${flex_${MODULE_COMPONENTS}})
  ## Source files
  file (GLOB components_${MODULE_COMPONENTS} ${MODULE_COMPONENTS}/*.cc ${MODULE_COMPONENTS}/*.tcc)
  list (APPEND MODULE_COMPONENTS_SOURCES ${components_${MODULE_COMPONENTS}})
endforeach (MODULE_COMPONENTS)

## Processing of Bison input files

foreach (bison_input ${MODULE_COMPONENTS_BISON})
  casacore_process_bison (${bison_input} ${MODULE_COMPONENTS_SOURCES})
endforeach (bison_input)

## Processing of Flex input files

foreach (flex_input ${MODULE_COMPONENTS_FLEX})
  casacore_process_flex (${flex_input} ${MODULE_COMPONENTS_SOURCES})
endforeach (flex_input)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_components SHARED ${MODULE_COMPONENTS_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_components STATIC ${MODULE_COMPONENTS_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions
target_link_libraries (casa_components
  casa_coordinates
  casa_fits
  casa_measures
  casa_tables
  casa_scimath
  casa_casa
  ${WCSLIB_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_components
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB MODULES_COMPONENTS_HEADERS *.h)
install (FILES ${MODULES_COMPONENTS_HEADERS}
  DESTINATION include/casacore/components
  )

foreach (MODULE_COMPONENTS ${CASACORE_MODULES_COMPONENTS})
  ## assemble list of header files
  FILE (GLOB MODULES_COMPONENTS_HEADERS ${MODULE_COMPONENTS}/*.h ${MODULE_COMPONENTS}/*.tcc)
  ## set installation location
  install (FILES ${MODULES_COMPONENTS_HEADERS}
    DESTINATION include/casacore/components/${MODULE_COMPONENTS}
    )
endforeach (MODULE_COMPONENTS)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_COMPONENTS ${CASACORE_MODULES_COMPONENTS})
    add_subdirectory (${MODULE_COMPONENTS}/test)
  endforeach (MODULE_COMPONENTS)
endif (CASACORE_ENABLE_TESTING)
