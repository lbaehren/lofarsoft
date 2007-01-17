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
endforeach (casa_module)

## collect the files responsible for template instantiation

FILE (GLOB casa_templates ../../templates/casa/*.cc)

## build the library

ADD_LIBRARY (casa
  aips.cc
  version2.cc
  ${casa_Arrays}
  ${casa_BasicMath}
  ${casa_BasicSL}
  ${casa_Containers}
  ${casa_Exceptions}
  ${casa_IO}
  ${casa_Inputs}
  ${casa_Logging}
  ${casa_OS}
  ${casa_Quanta}
  ${casa_System}
  ${casa_Utilities}
  ## -- Template instantiation
  ${casa_templates}
)

## -----------------------------------------------------------------------------
## Directories containing the test programs

SUBDIRS (
  Arrays/test
  BasicMath/test
  BasicSL/test
  Containers/test
  Exceptions/test
  Inputs/test
  IO/test
  Logging/test
  OS/test
  Quanta/test
  System/test
  Utilities/test
  )

