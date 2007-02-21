# $LastChangedDate$

## ------------------------------------------------------------------------------
## Find external packages

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

## ------------------------------------------------------------------------------
## Include directories

include_directories (
  .
  ..
  ../../implement
  ${CASA_INCLUDES}
  ${CFITSIO_INCLUDES}
  )

## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tUseCasa
  tUseScimath
  tUseMeasures
  tUseTables
  tUseLattices
  tUseImages
)

## ------------------------------------------------------------------------------
## create executable(s)

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src}
    ${CASA_LIBRARIES}
    ${LAPACK_LIBRARIES}
    )  
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Test for BLITZ

add_executable (tUseBlitz tUseBlitz.cc)
target_link_libraries (tUseBlitz ${BLITZ_LIBRARIES})

## ------------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_CFITSIO)
  add_executable (tUseCFITSIO tUseCFITSIO.cc)
  target_link_libraries (tUseCFITSIO ${CFITSIO_LIBRARIES})
else (HAVE_CFITSIO)
  message (STATUS "Unable to build test program for CFITSIO.")
endif (HAVE_CFITSIO)

## ------------------------------------------------------------------------------
## Test for FFTW3

if (HAVE_FFTW3)
  add_executable (tUseFFTW3 tUseFFTW3.cc)
  target_link_libraries (tUseFFTW3 ${FFTW3_LIBRARY})
endif (HAVE_FFTW3)

## ------------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_WCS)
  add_executable (tUseWCS tUseWCS.cc)
  target_link_libraries (tUseWCS ${WCS_LIBRARIES})
else (HAVE_WCS)
  message (STATUS "Unable to build test program for WCS.")
endif (HAVE_WCS)

