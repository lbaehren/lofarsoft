
## -----------------------------------------------------------------------------
## libfits

SET (fits_modules
  FITS
  )

## collect the source files in the modules

foreach (fits_module ${fits_modules})
  FILE (GLOB fits_${fits_module} ${fits_module}/*.cc ${fits_module}/*.tcc)
  LIST(APPEND fits_sources ${fits_${fits_module}})
endforeach (fits_module)

## build the library

add_library (fits ${fits_sources})
target_link_libraries (fits measures casa ${CFITSIO_LIBRARIES})

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (fits_module ${fits_modules})
  LIST (APPEND tests "${fits_module}/test")
endforeach (fits_module)

SUBDIRS (${tests})

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS fits
  RUNTIME DESTINATION ${installation_prefix}/bin
  LIBRARY DESTINATION ${installation_prefix}/lib
  ARCHIVE DESTINATION ${installation_prefix}/lib
  )
