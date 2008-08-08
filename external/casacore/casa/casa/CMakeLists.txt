##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## libcasa

SET (casa_modules
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

if (casacore_HDF5_SUPPORT)
  list (APPEND casa_modules HDF5)
else (casacore_HDF5_SUPPORT)
  message (STATUS "[casacore] Module HDF5 excluded from libcasa_casa.")
endif (casacore_HDF5_SUPPORT)

## -----------------------------------------------------------------------------
## Build the library

## collect the source files in the modules

foreach (casa_module ${casa_modules})
  FILE (GLOB casa_${casa_module} ${casa_module}/*.cc ${casa_module}/*.tcc)
  LIST(APPEND casa_sources ${casa_${casa_module}})
endforeach (casa_module)

## compiler instructions

add_library (casa_casa
  ${casacore_BINARY_DIR}/casacore.h
  aips.cc
  ${casa_sources}
)

## linker instructions

if (casacore_HDF5_SUPPORT)
  target_link_libraries (casa_casa ${HDF5_LIBRARIES} ${libm})
else (casacore_HDF5_SUPPORT)
  target_link_libraries (casa_casa ${libm})
endif (casacore_HDF5_SUPPORT)

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

foreach (casa_module ${casa_modules} HDF5)
  ## assemble list of header files
  FILE (GLOB casa_headers ${casa_module}/*.h ${casa_module}/*.tcc)
  ## set installation location
  install (FILES ${casa_headers}
    DESTINATION include/casacore/casa/${casa_module}
    )
endforeach (casa_module)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (casa_module ${casa_modules})
    add_subdirectory (${casa_module}/test)
  endforeach (casa_module)
endif (casacore_BUILD_TESTS)
