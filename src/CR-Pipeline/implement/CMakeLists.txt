
## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

set (cr_modules
  Coordinates
  Data
  Detection
  Imaging
  Math
  Nodes
  )

subdirs (${cr_modules})

## -----------------------------------------------------------------------------
## Path to the various header files

include_directories (
  .
  ..
  ${CR_SOURCE_DIR}/include
  ${BLITZ_INCLUDES}
  ${WCS_INCLUDES}
  )

## -----------------------------------------------------------------------------
## collect the source files in the modules

foreach (cr_module ${cr_modules})
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## -----------------------------------------------------------------------------
## build the library

add_library (cr
  ${cr_sources}
  )
target_link_libraries (cr
  ${CASA_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )
