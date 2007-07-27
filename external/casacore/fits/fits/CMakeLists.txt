
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

add_library (fits
  ${fits_sources}
  )
target_link_libraries (fits
  measures
  scimath
  scimath_f
  tables
  casa
  ${CFITSIO_LIBRARIES}
  ${G2C_LIBRARIES}
  wcs
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (fits_module ${fits_modules})
    list (APPEND tests "${fits_module}/test")
  endforeach (fits_module)
  ## ... and register them
  subdirs (${tests})
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS fits
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## -----------------------------------------------------------------------------
## Include hierarchy 

#  <fits/FITS/BasicFITS.cc>
#  |-- <fits/FITS/BasicFITS.h>
#  |-- <fits/FITS/FITS2.h>
#  |   `-- <fits/FITS/hdu.h>
#  |-- <fits/FITS/fitsio.h>
#  |   |-- <fits/FITS/fits.h>
#  |   |-- <fits/FITS/blockio.h>
#  |   `-- <fits/FITS/hdu.h>
#  `-- <fits/FITS/hdu.h>
#      |-- <fits/FITS/fits.h>
#      `-- <fits/FITS/blockio.h>
