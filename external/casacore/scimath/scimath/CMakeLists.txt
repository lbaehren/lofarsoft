
## -----------------------------------------------------------------------------
## libscimath

SET (scimath_modules
  Fitting
  Functionals
  Mathematics
  )

## collect the source files in the modules

foreach (scimath_module ${scimath_modules})
  FILE (GLOB scimath_${scimath_module} ${scimath_module}/*.cc ${scimath_module}/*.tcc)
  LIST(APPEND scimath_sources ${scimath_${scimath_module}})
endforeach (scimath_module)

## build the library

add_library (scimath
  ${scimath_sources}
)
target_link_libraries (scimath casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (scimath_module ${scimath_modules})
    LIST (APPEND tests "${scimath_module}/test")
  endforeach (scimath_module)
  ## ... and register them
  SUBDIRS (${tests})
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS scimath
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
