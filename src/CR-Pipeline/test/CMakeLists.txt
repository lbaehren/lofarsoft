##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------


SET (CMAKE_VERBOSE_MAKEFILE 1)

## ------------------------------------------------------------------------------
## Find external packages

if (NOT HAVE_BLITZ)
  include (../../../devel_common/cmake/FindBlitz.cmake)
endif (NOT HAVE_BLITZ)

if (NOT HAVE_CASA) 
  include (../../../devel_common/cmake/FindCASA.cmake)
endif (NOT HAVE_CASA)

if (NOT HAVE_CFITSIO)
  include (../../../devel_common/cmake/FindCFITSIO.cmake)
endif (NOT HAVE_CFITSIO)

if (NOT HAVE_FFTW3)
  include (../../../devel_common/cmake/FindFFTW3.cmake)
endif (NOT HAVE_FFTW3)

if (NOT HAVE_WCS) 
  include (../../../devel_common/cmake/FindWCS.cmake)
endif (NOT HAVE_WCS)

## ------------------------------------------------------------------------------
## Test working with CASA libraries

set (tests
  tUseCASA
  tUseScimath
  tUseMeasures
  tUseTables
  tUseLattices
  tUseImages
)

## create executable(s)

if (HAVE_CASA)
  FOREACH (src ${tests})
    ADD_EXECUTABLE (${src} ${src}.cc)
    TARGET_LINK_LIBRARIES (${src}
      ${CASA_LIBRARIES}
      ${LAPACK_LIBRARIES}
      )  
  ENDFOREACH (src)
endif (HAVE_CASA)

## installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ------------------------------------------------------------------------------
## Vectors of the standard library

#add_executable (tUseVector tUseVector.cc)

## ------------------------------------------------------------------------------
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

## ------------------------------------------------------------------------------
## Test for BLITZ

add_executable (tUseBlitz tUseBlitz.cc)
target_link_libraries (tUseBlitz ${BLITZ_LIBRARIES})

## installation

install (
  TARGETS tUseBlitz
  RUNTIME DESTINATION bin
  )

## ------------------------------------------------------------------------------
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

## ------------------------------------------------------------------------------
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

## ------------------------------------------------------------------------------
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

## ------------------------------------------------------------------------------
## Test for working with ROOT (includes working with the DAL)

if (HAVE_ROOT AND HAVE_BLITZ)
  add_executable (tUseROOT tUseROOT.cc)
  target_link_libraries (tUseROOT dal ${ROOT_LIBRARIES} ${BLITZ_LIBRARIES})
else (HAVE_ROOT AND HAVE_BLITZ)
  message (STATUS "Unable to build test program(s) for working with ROOT.")
endif (HAVE_ROOT AND HAVE_BLITZ)


