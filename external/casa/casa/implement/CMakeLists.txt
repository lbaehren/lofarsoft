PROJECT (CASA_casa_libcasa)

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
  FILE (GLOB casa_${casa_module} ${casa_module}/*.cc)
  LIST(APPEND casa_sources ${casa_${casa_module}})
endforeach (casa_module)

## collect the files responsible for template instantiation

FILE (GLOB casa_templates ../../templates/casa/*.cc)

## build the library

ADD_LIBRARY (casa
  aips.cc
  version2.cc
  ${casa_sources}
  ${casa_templates}
)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (casa_module ${casa_modules})
  LIST (APPEND tests "${casa_module}/test")
endforeach (casa_module)

SUBDIRS (${tests})
