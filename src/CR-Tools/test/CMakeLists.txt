##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Tests within the framework of the C++ standard library; this should be
## working in any case.

add_library (cr_test Templates.cc)

foreach (test tNumericalRanges tObjects tProcessingParameters tTemplates)
  ## build program
  add_executable (${test} ${test}.cc)
  target_link_libraries (${test} cr_test)
  ## testing
  add_test (${test} ${test})
  ## installation
  install (
    TARGETS ${test}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endforeach (test)

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

if (HAVE_CASA)
  FOREACH (src ${tests})
    ## build the excutables
    ADD_EXECUTABLE (${src} ${src}.cc)
    TARGET_LINK_LIBRARIES (${src}
      ${CASA_LIBRARIES}
      ${LAPACK_LIBRARIES}
      )  
    ## testing
    add_test (${src} ${src})
  ENDFOREACH (src)
  ## installation locations -------------------
  install (
    TARGETS ${tests}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
else (HAVE_CASA)
  if (HAVE_CASACORE)
    message (STATUS "Enabling test programs using casacore ...")
    ## build the executables ----------------------
    add_executable (tUseArrays tUseArrays.cc)
    target_link_libraries (tUseArrays ${CASACORE_LIBRARIES})
    ##
    add_executable (tUseCASA tUseCASA.cc)
    target_link_libraries (tUseCASA ${CASACORE_LIBRARIES})
    ##
    add_executable (tUseTables tUseTables.cc)
    target_link_libraries (tUseTables ${CASACORE_LIBRARIES})
    ## testing ------------------------------------
    add_test (tUseArrays tUseArrays)
    add_test (tUseCASA tUseCASA)
    add_test (tUseTables tUseTables)
    ## installation locations ---------------------
    install (
      TARGETS tUseArrays tUseCASA tUseTables
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      )
  endif (HAVE_CASACORE)
endif (HAVE_CASA)

## -----------------------------------------------------------------------------
## Test(s) for Blitz++

if (HAVE_BLITZ)
  ## buid test program
  add_executable (tUseBlitz tUseBlitz.cc)
  target_link_libraries (tUseBlitz ${BLITZ_LIBRARIES})
  ## testing
  add_test (tUseBlitz tUseBlitz)
  ## installation
  install (
    TARGETS tUseBlitz
    RUNTIME DESTINATION bin
    )
endif (HAVE_BLITZ)

## -----------------------------------------------------------------------------
## Tests for casacore

if (HAVE_CASACORE)
  add_executable (tUseCasacore tUseCasacore.cc)
  target_link_libraries (tUseCasacore ${CASACORE_LIBRARIES})
endif (HAVE_CASACORE)

## -----------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_CFITSIO)
  ## optional compiler settings
  if (HAVE_WCSLIB)
    add_definitions (-DHAVE_WCSLIB)
  endif (HAVE_WCSLIB)
  ## build test program
  add_executable (tUseCFITSIO tUseCFITSIO.cc)
  target_link_libraries (tUseCFITSIO ${CFITSIO_LIBRARIES})
  ## testing
  add_test (tUseCFITSIO tUseCFITSIO)
  ## installation
  install (
    TARGETS tUseCFITSIO
    RUNTIME DESTINATION bin
    )
endif (HAVE_CFITSIO)

## -----------------------------------------------------------------------------
## Test for FFTW3

if (HAVE_FFTW3)
  add_executable (tUseFFTW3 tUseFFTW3.cc)
  target_link_libraries (tUseFFTW3 ${FFTW3_LIBRARIES})
  ## testing
  add_test (tUseFFTW3 tUseFFTW3)
  ## installation
  install (
    TARGETS tUseFFTW3
    RUNTIME DESTINATION bin
    )
endif (HAVE_FFTW3)

## -----------------------------------------------------------------------------
## Test for GSL

if (HAVE_GSL)
  add_executable (tUseGSL tUseGSL.cc)
  target_link_libraries (tUseGSL ${GSL_LIBRARIES})
  ## testing
  add_test (tUseGSL tUseGSL)
  ## installation
  install (
    TARGETS tUseGSL
    RUNTIME DESTINATION bin
    )
endif (HAVE_GSL)

## -----------------------------------------------------------------------------
## Test(s) for PGPlot

if (HAVE_PGPLOT)
  add_executable (tUsePGPlot tUsePGPlot.cc)
  target_link_libraries (tUsePGPlot ${PGPLOT_LIBRARIES})
  ## testing
  add_test (tUsePGPlot tUsePGPlot)
  ## installation
  install (
    TARGETS tUsePGPlot
    RUNTIME DESTINATION bin
    )
endif (HAVE_PGPLOT)

## -----------------------------------------------------------------------------
## Test for WCSLIB

if (HAVE_WCSLIB)
  add_executable (tUseWCS tUseWCS.cc)
  target_link_libraries (tUseWCS ${WCSLIB_LIBRARIES})
  ## testing
  add_test (tUseWCS tUseWCS)
  ## installation
  install (
    TARGETS tUseWCS
    RUNTIME DESTINATION bin
    )
endif (HAVE_WCSLIB)

## -----------------------------------------------------------------------------
## Test for working with ROOT (includes working with the DAL)

if (HAVE_ROOT AND HAVE_BLITZ AND dal)
  ## build test program
  add_executable (tUseROOT tUseROOT.cc)
  target_link_libraries (tUseROOT dal ${ROOT_LIBRARIES} ${BLITZ_LIBRARIES})
  ## testing
  add_test (tUseROOT tUseROOT)
endif (HAVE_ROOT AND HAVE_BLITZ AND dal)
