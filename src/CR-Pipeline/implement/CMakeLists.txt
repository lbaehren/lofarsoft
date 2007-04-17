
## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Imaging
  IO
  Math
  )

subdirs (${cr_modules})

## -----------------------------------------------------------------------------
## Path to the various header files

# these settings should be handled one level up

## -----------------------------------------------------------------------------
## collect the source files in the modules

foreach (cr_module ${cr_modules})
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## -----------------------------------------------------------------------------
## build the library

if (CASA_LIBRARIES AND LAPACK_LIBRARIES)
  add_library (cr
    ${cr_sources}
    )
  target_link_libraries (cr
    ${CASA_LIBRARIES}
    ${LAPACK_LIBRARIES}
    )
endif (CASA_LIBRARIES AND LAPACK_LIBRARIES)
