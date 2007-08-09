
project (CASA_scimath_libscimath)

## -----------------------------------------------------------------------------
## libscimath

SET (scimath_modules
  Fitting
  Functionals
  Mathematics
  )

## collect the source files in the modules

foreach (scimath_module ${scimath_modules})
  FILE (GLOB scimath_${scimath_module} ${scimath_module}/*.cc)
  LIST(APPEND scimath_sources ${scimath_${scimath_module}})
endforeach (scimath_module)

## collect the files responsible for template instantiation

FILE (GLOB scimath_templates ../../templates/scimath/*.cc)

## build the library

add_library (scimath
  ${scimath_sources}
  ${scimath_templates}
  )

## library dependencies

target_link_libraries (scimath
  scimath_f
  casa
  ${LAPACK_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (CASA_BUILD_TESTS)
  ## Assemble the list of directories ...
  foreach (scimath_module ${scimath_modules})
    LIST (APPEND tests "${scimath_module}/test")
  endforeach (scimath_module)
  ## ... and register it
  SUBDIRS (${tests})
endif (CASA_BUILD_TESTS)

## -----------------------------------------------------------------------------
## Installation of libraries

install (TARGETS scimath
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
