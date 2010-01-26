##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Sub-modules

SET (CASACORE_MODULES_CASA
  Arrays
  BasicMath
  BasicSL
  Containers
  Exceptions
  IO
  Inputs
  Logging
  OS
  Quanta
  System
  Utilities
  )

if (CASACORE_ENABLE_HDF5)
  list (APPEND CASACORE_MODULES_CASA HDF5)
else (CASACORE_ENABLE_HDF5)
  message (STATUS "[casacore] Module HDF5 excluded from libcasa_casa.")
endif (CASACORE_ENABLE_HDF5)

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

set (casa_sources aips.cc version.cc)

foreach (MODULE_CASA ${CASACORE_MODULES_CASA})
  FILE (GLOB casa_${MODULE_CASA} ${MODULE_CASA}/*.cc ${MODULE_CASA}/*.tcc)
  LIST(APPEND casa_sources ${casa_${MODULE_CASA}})
endforeach (MODULE_CASA)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_casa SHARED ${casa_sources})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_casa STATIC ${casa_sources})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

if (CASACORE_ENABLE_HDF5)
  target_link_libraries (casa_casa ${HDF5_LIBRARIES} ${libm})
else (CASACORE_ENABLE_HDF5)
  target_link_libraries (casa_casa ${libm})
endif (CASACORE_ENABLE_HDF5)

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_casa
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## header files

FILE (GLOB casa_headers *.h)
install (FILES ${casa_headers}
  DESTINATION include/casacore/casa
  )

foreach (MODULE_CASA ${CASACORE_MODULES_CASA} HDF5)
  ## assemble list of header files
  FILE (GLOB casa_headers ${MODULE_CASA}/*.h ${MODULE_CASA}/*.tcc)
  ## set installation location
  install (FILES ${casa_headers}
    DESTINATION include/casacore/casa/${MODULE_CASA}
    )
endforeach (MODULE_CASA)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASACORE_ENABLE_TESTING)
  foreach (MODULE_CASA ${CASACORE_MODULES_CASA})
    add_subdirectory (${MODULE_CASA}/test)
  endforeach (MODULE_CASA)
endif (CASACORE_ENABLE_TESTING)
