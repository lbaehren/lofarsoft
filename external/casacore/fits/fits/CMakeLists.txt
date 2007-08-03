
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
  ${G2C_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
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
#  |       |-- <fits/FITS/fits.h>
#  |       |   `-- <fits/FITS/FITSError.h>
#  |       `-- <fits/FITS/blockio.h>
#  |           `-- <fits/FITS/FITSError.h>
#  |-- <fits/FITS/fitsio.h>
#  |   |-- <fits/FITS/fits.h>
#  |   |   `-- <fits/FITS/FITSError.h>
#  |   |-- <fits/FITS/blockio.h>
#  |   |   `-- <fits/FITS/FITSError.h>
#  |   `-- <fits/FITS/hdu.h>
#  |       |-- <fits/FITS/fits.h>
#  |       |   `-- <fits/FITS/FITSError.h>
#  |       `-- <fits/FITS/blockio.h>
#  |           `-- <fits/FITS/FITSError.h>
#  `-- <fits/FITS/hdu.h>
#      |-- <fits/FITS/fits.h>
#      `-- <fits/FITS/blockio.h>

## -----------------------------------------------------------------------------
## Debugging information
##
## [1] This is the compile command, as used with the "casa" package, as derived
## directly from CASA:
##
##  /usr/bin/c++   -I/Users/lars/Code/projects/lofar/usg/external/casa/. 
##                 -I/Users/lars/Code/projects/lofar/usg/external/casa
##                 -I/Users/lars/Code/projects/lofar/usg/external/casa/include
##                 -I/sw/include/cfitsio
##                 -DAIPS_STDLIB
##                 -DAIPS_NO_TEMPLATE_SRC
##                 -DAIPS_AUTO_STL
##                 -DAIPS_NO_LEA_MALLOC
##                 -DAIPS_DARWIN
##                 -DAIPS_BIG_ENDIAN
##                 -DNATIVE_EXCP
##                 -D_FILE_OFFSET_BITS=64
##                 -D_LARGEFILE_SOURCE
##                 -O3
##                 -fPIC
##                 -pipe
##                 -Wall
##                 -Wno-comment
##                 -fexceptions
##                 -Wcast-align
##                 -Wno-non-template-friend
##                 -Woverloaded-virtual
##                 -o fits/implement/CMakeFiles/fits.dir/FITS/BasicFITS.o
##                 -c /Users/lars/Code/projects/lofar/usg/external/casa/fits/implement/FITS/BasicFITS.cc
##
## [2] This is the corresponding compile command when building "libfits" from
## "casacore":
## 