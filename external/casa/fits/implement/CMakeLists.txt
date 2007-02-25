PROJECT (CASA_fits_libfits)

## -----------------------------------------------------------------------------
## Additional required external software


## -----------------------------------------------------------------------------
## libfits

SET (fits_modules
  FITS
  )

## collect the source files in the modules

foreach (fits_module ${fits_modules})
  FILE (GLOB fits_${fits_module} ${fits_module}/*.cc)
  LIST(APPEND fits_sources ${fits_${fits_module}})
endforeach (fits_module)

## collect the files responsible for template instantiation

file (GLOB fits_templates ../../templates/fits/*.cc)

## build the library

add_library (fits
  ${fits_sources}
  ${fits_templates}
)
target_link_libraries (fits ${CFITSIO_LIBRARIES} wcs)

## -----------------------------------------------------------------------------
## Directories containing the test programs

foreach (fits_module ${fits_modules})
  LIST (APPEND tests "${fits_module}/test")
endforeach (fits_module)

SUBDIRS (${tests})
