
project (casacore_scimath C Fortran)

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
target_link_libraries (casa_scimath
  casa_scimath_f
  casa_casa
  ${LAPACK_LIBRARIES}
  )
set_target_properties (casa_scimath
  PROPERTIES
  LINKER_LANGUAGE Fortran
  )

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
## Installation

## Library

install (TARGETS casa_scimath
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Header files

FILE (GLOB scimath_headers *.h)
install (FILES ${scimath_headers}
  DESTINATION include/casacore/scimath
  )

foreach (scimath_module ${scimath_modules})
  ## assemble list of header files
  FILE (GLOB scimath_headers ${scimath_module}/*.h ${scimath_module}/*.tcc)
  ## set installation location
  install (FILES ${scimath_headers}
    DESTINATION include/casacore/scimath/${scimath_module}
    )
endforeach (scimath_module)
