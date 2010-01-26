##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Build the library

enable_language(Fortran)

## collect the source files in the modules

file (GLOB sofa_sources *.f *.c)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (sofa SHARED ${sofa_sources})
else (CASACORE_ENABLE_SHARED)
  add_library (sofa STATIC ${sofa_sources})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

set_target_properties (sofa
  PROPERTIES
  COMPILE_FLAGS "-fPIC -O2 -fPIC"
  LINKER_LANGUAGE Fortran
  )

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS sofa
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
