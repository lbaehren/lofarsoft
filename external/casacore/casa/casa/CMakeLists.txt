
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

foreach (casa_module ${casa_modules})
  LIST (APPEND tests "${casa_module}/test")
endforeach (casa_module)

SUBDIRS (${tests})

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (TARGETS casa
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
