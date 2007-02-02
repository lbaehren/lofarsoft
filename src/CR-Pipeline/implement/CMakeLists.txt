
subdirs (
  Imaging
  Math
  )

#include_directories (
#  .
#  ..
#  ${CR_SOURCE_DIR}/include
#  )

## -----------------------------------------------------------------------------
## libcr

set (cr_modules
  Imaging
  Math
  )

## collect the source files in the modules

foreach (cr_module ${cr_modules})
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## build the library

add_library (cr
  ${cr_sources}
  )