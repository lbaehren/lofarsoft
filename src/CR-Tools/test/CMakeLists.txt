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
  tUseMeasures
  tUseLattices
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
endif (HAVE_CASA)

## -------------------------------------------------------------------
## Tests for working with casacore

if (HAVE_CASACORE)

  message (STATUS "[CR] Tests for casacore enabled.")
  
  set (tests_casacore
    tcasa_casa
    tcasa_casa_Quanta
    tcasa_casa_Containers
    tcasa_tables
    tcasa_coordinates
    tcasa_images
    )
  
  foreach (test ${tests_casacore})
    ## compiler instructions
    add_executable (${test} ${test}.cc)
    ## linker instructions
    target_link_libraries (${test}
      ${LAPACK_LIBRARIES}
      ${CASA_LIBRARIES}
      ${WCSLIB_LIBRARIES}
      ${G2C_LIBRARIES}
      ${libm}
      )
    ## testing
    add_test (${test} ${test})
  endforeach (test ${tests_casacore})

endif (HAVE_CASACORE)

## -----------------------------------------------------------------------------
## Test(s) for Blitz++

if (HAVE_BLITZ)
  ## compiler instructions
  add_executable (tUseBlitz tUseBlitz.cc)
  set_target_properties (tUseBlitz
    PROPERTIES
    COMPILE_FLAGS "-DHAVE_BLITZ -I${BLITZ_INCLUDES}"
    )
  ## linker instructions
  target_link_libraries (tUseBlitz ${BLITZ_LIBRARIES})
  ## testing
  if (CR_ENABLE_TESTING) 
    add_test (tUseBlitz tUseBlitz)
  endif (CR_ENABLE_TESTING)
  ## installation
  install (
    TARGETS tUseBlitz
    RUNTIME DESTINATION bin
    )
endif (HAVE_BLITZ)

## -----------------------------------------------------------------------------
## Test for CFITSIO

if (HAVE_CFITSIO)
  ## build test program
  add_executable (tUseCFITSIO tUseCFITSIO.cc)
  set_target_properties (tUseCFITSIO
    PROPERTIES
    COMPILE_FLAGS "-DHAVE_CFITSIO -I${CFITSIO_INCLUDES}"
    )
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
  ## compiler instructions
#  add_executable (tUsePGPlot tUsePGPlot.cc)
  ## linker instructions
#  target_link_libraries (tUsePGPlot ${PGPLOT_LIBRARIES} ${G2C_LIBRARIES})
#  ## testing
#  add_test (tUsePGPlot tUsePGPlot)
#  ## installation
#  install (
#    TARGETS tUsePGPlot
#    RUNTIME DESTINATION bin
#    )
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

if (HAVE_ROOT AND HAVE_CASACORE)
  if (HAVE_DAL)
    ## compiler instructions
    add_executable (tUseROOT tUseROOT.cc)
    ## linker instructions
    if (libutil AND libdl)
      target_link_libraries (tUseROOT
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${CASA_LIBRARIES}
	${DAL_LIBRARIES}
	${libutil}
	${libdl}
	)
    else (libutil AND libdl)
      target_link_libraries (tUseROOT
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${CASA_LIBRARIES}
	${DAL_LIBRARIES}
	)
    endif (libutil AND libdl)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (tUseROOT
	PROPERTIES
	LINK_FLAGS "-bind_at_load"
	)
    endif (APPLE)
    ## testing
    if (dataset_event)
      add_test (tUseROOT tUseROOT ${dataset_event})
    else (dataset_event)
      add_test (tUseROOT tUseROOT) 
   endif (dataset_event)
  else (HAVE_DAL)
    message (STATUS "[CR-Tools] Unable to build test program tUseROOT.")
  endif (HAVE_DAL)
endif (HAVE_ROOT AND HAVE_CASACORE)

## -----------------------------------------------------------------------------
## Test working with the Data Access Library (DAL)

if (HAVE_DAL AND HAVE_CASACORE)
  ## compiler instructions
  add_executable (tUseDAL tUseDAL.cc)
  ## linker instructions
  target_link_libraries (tUseDAL
    cr
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${CASA_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    )
  ## additional properties
  set_target_properties (tUseDAL
    PROPERTIES
    COMPILE_FLAGS "-DHAVE_DAL -I${DAL_INCLUDES} -I${CR_SOURCE_DIR}/implement"
    )
endif (HAVE_DAL AND HAVE_CASACORE)