
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
  ${CFITSIO_INCLUDE_DIR}
  )

## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tUseCasa
  tUseScimath
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
## Test for FFTW3

add_executable (tUseFFTW3 tUseFFTW3.cc)
target_link_libraries (tUseFFTW3 ${FFTW3_LIBRARY})

## ------------------------------------------------------------------------------
## Test for CFITSIO

add_executable (tUseCFITSIO tUseCFITSIO.cc)
target_link_libraries (tUseCFITSIO ${CFITSIO_LIBRARIES})
