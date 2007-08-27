
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

add_library (casa_scimath
  ${scimath_sources}
)
target_link_libraries (casa_scimath casa_casa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  ## build up the list of directories ...
  foreach (scimath_module ${scimath_modules})
    LIST (APPEND tests "${scimath_module}/test")
  endforeach (scimath_module)
  ## ... and register them
  SUBDIRS (${tests})
endif (casacore_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS casa_scimath
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
