
## ------------------------------------------------------------------------------
## Find external packages



# basic stuff is already handled a top level

## ------------------------------------------------------------------------------
## Include directories

include_directories (
  .
  ..
  ../../implement
  ${CASA_INCLUDE_DIR}
  ${CFITSIO_INCLUDES}
  )

## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tUseCasa
#  tUseScimath
)

## ------------------------------------------------------------------------------
## create executable(s)

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src}
    ${CASA_LIBRARY}
    ${SCIMATH_LIBRARY}
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
  message (ERROR "Unable to build test program for CFITSIO.")
endif (HAVE_CFITSIO)

## ------------------------------------------------------------------------------
## Test for FFTW3

if (HAVE_FFTW3)
  add_executable (tUseFFTW3 tUseFFTW3.cc)
  target_link_libraries (tUseFFTW3 ${FFTW3_LIBRARY})
endif (HAVE_FFTW3)
