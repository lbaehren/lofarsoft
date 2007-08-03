##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
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

## collect the source files in the modules

foreach (casa_module ${casa_modules})
  FILE (GLOB casa_${casa_module} ${casa_module}/*.cc ${casa_module}/*.tcc)
  LIST(APPEND casa_sources ${casa_${casa_module}})
endforeach (casa_module)

## build the library

add_library (casa
  aips.cc
  ${casa_sources}
)
target_link_libraries (casa ${libm})

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (casa_module ${casa_modules})
    list (APPEND tests "${casa_module}/test")
  endforeach (casa_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_tests)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS casa
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ------------------------------------------------------------------------------
## Dependency tree for basic aips.h header file
##
## <casa/aips.h>
## |-- <casa/aipsenv.h>    // Define compiler specific flags
## |-- <casa/aipstype.h>   // Define the standard types used by AIPS++.
## |-- <casa/aipsxtype.h>  // Define the extra non-standard types used by aips++
## `-- <casa/aipsdef.h>    // Define the special aips++ macros

