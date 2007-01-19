
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

target_link_libraries (scimath casa scimath_f)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (scimath_module ${scimath_modules})
  LIST (APPEND tests "${scimath_module}/test")
endforeach (scimath_module)

SUBDIRS (${tests})
