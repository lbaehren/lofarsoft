##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Test working with CASA libraries
## Most of the test programs are not in need of a full CASA installation, but
## can be build using casacore; therefore we first check for the existence of 
## casacore - if it is not available, we switch to CASA.

set (tests
  tUseArrays
  tUseCASA
  tUseTables
  tUseScimath
  tUseMeasures
  tUseLattices
  tUseImages
)

if (casa AND tables)
  ## build the executables ----------------------
  add_executable (tUseArrays tUseArrays.cc)
  target_link_libraries (tUseArrays casa tables)
  ##
  add_executable (tUseCASA tUseCASA.cc)
  target_link_libraries (tUseCASA casa tables)
  ##
  add_executable (tUseTables tUseTables.cc)
  target_link_libraries (tUseTables casa tables)
  ## installation locations ---------------------
  install (
    TARGETS tUseArrays tUseCASA tUseTables
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (casa AND tables)

if (HAVE_CASA)
  ## build the excutables ---------------------
  FOREACH (src ${tests})
    ADD_EXECUTABLE (${src} ${src}.cc)
    TARGET_LINK_LIBRARIES (${src}
      ${CASA_LIBRARIES}
      ${LAPACK_LIBRARIES}
      )  
  ENDFOREACH (src)
  ## installation locations -------------------
  install (
    TARGETS ${tests}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (HAVE_CASA)

## -----------------------------------------------------------------------------
## Test working with casacore

if (HAVE_CASACORE)
endif (HAVE_CASACORE)

## -----------------------------------------------------------------------------
## Vectors of the standard library

#add_executable (tUseVector tUseVector.cc)

## -----------------------------------------------------------------------------
## Numerical ranges of the various data types
## Relationship between the various control parameters

add_executable (tNumericalRanges tNumericalRanges.cc)
add_executable (tObjects tObjects.cc)
add_executable (tProcessingParameters tProcessingParameters.cc)

## installation

install (
  TARGETS tNumericalRanges tObjects tProcessingParameters
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## -----------------------------------------------------------------------------
## Test for BLITZ

add_executable (tUseBlitz tUseBlitz.cc)
target_link_libraries (tUseBlitz ${BLITZ_LIBRARIES})

## installation

install (
  TARGETS tUseBlitz
  RUNTIME DESTINATION bin
  )

## -----------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_CFITSIO)
  add_executable (tUseCFITSIO tUseCFITSIO.cc)
  target_link_libraries (tUseCFITSIO ${CFITSIO_LIBRARIES})
  ## installation
  install (
    TARGETS tUseCFITSIO
    RUNTIME DESTINATION bin
    )
else (HAVE_CFITSIO)
  message (STATUS "Unable to build test program for CFITSIO.")
endif (HAVE_CFITSIO)

## -----------------------------------------------------------------------------
## Test for FFTW3

if (HAVE_FFTW3)
  add_executable (tUseFFTW3 tUseFFTW3.cc)
  target_link_libraries (tUseFFTW3 ${FFTW3_LIBRARY})
  ## installation
  install (
    TARGETS tUseFFTW3
    RUNTIME DESTINATION bin
    )
else (HAVE_FFTW3)
  message (STATUS "Unable to build test program for FFTW3.")
endif (HAVE_FFTW3)

## -----------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_WCS)
  add_executable (tUseWCS tUseWCS.cc)
  target_link_libraries (tUseWCS ${WCS_LIBRARIES})
  ## installation
  install (
    TARGETS tUseWCS
    RUNTIME DESTINATION bin
    )
else (HAVE_WCS)
  message (STATUS "Unable to build test program for WCS.")
endif (HAVE_WCS)

## -----------------------------------------------------------------------------
## Test for working with ROOT (includes working with the DAL)

if (HAVE_ROOT AND HAVE_BLITZ)
  add_executable (tUseROOT tUseROOT.cc)
  target_link_libraries (tUseROOT dal ${ROOT_LIBRARIES} ${BLITZ_LIBRARIES})
else (HAVE_ROOT AND HAVE_BLITZ)
  message (STATUS "Unable to build test program(s) for working with ROOT.")
endif (HAVE_ROOT AND HAVE_BLITZ)


