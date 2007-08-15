##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

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

## -----------------------------------------------------------------------------
## build the library

add_library (fits
  ${fits_sources}
  )

target_link_libraries (fits measures
  ${G2C_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_BUILD_TESTS)
  foreach (fits_module ${fits_modules})
    add_subdirectory (${fits_module}/test)
  endforeach (fits_module)
endif (casacore_BUILD_TESTS)

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
##  g++ -o build_darwin_i386/opt/FITS/BasicFITS.o 
##      -c
##      -DAIPS_STDLIB
##      -DAIPS_AUTO_STL
##      -DAIPS_DARWIN
##      -DAIPS_NO_LEA_MALLOC
##      -DAIPS_LITTLE_ENDIAN
##      -O2
##      -I.
##      -I/sw/share/casacore/stage/include/casacore
##      fits/FITS/BasicFITS.cc
##
