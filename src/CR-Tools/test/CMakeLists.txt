##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Tests within the framework of the C++ standard library; this should be
## working in any case.

if (HAVE_CASACORE OR HAVE_CASA)
  ## location of the header files
  include_directories (${CASA_INCLUDES})
  ## compiler instructions
  add_library (cr_test
    ${CR_BINARY_DIR}/crtools.h
    Templates.cc
    Casacore.cc
    )
  ## linker instructions
  target_link_libraries (cr_test
    ${CASA_LIBARIES}
    )
else (HAVE_CASACORE OR HAVE_CASA)
  add_library (cr_test
    Templates.cc
    )
endif (HAVE_CASACORE OR HAVE_CASA)

foreach (test tNumericalRanges tObjects tTemplates)
  ## compiler instructions
  add_executable (${test} ${test}.cc)
  ## linker instructions
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

## -------------------------------------------------------------------
## Tests for working with casacore

if (HAVE_CASACORE AND HAVE_WCSLIB)

  message (STATUS "[CR-Tools] Tests for casacore enabled.")
  
  set (tests_casacore
    tcasa_casa
    tcasa_casa_Quanta
    tcasa_casa_Containers
    tcasa_tables
    tcasa_coordinates
    tcasa_images
    tcasa_lattices
    tcasa_measures
    )
  
  
  foreach (test ${tests_casacore})
    ## compiler instructions
    add_executable (${test} ${test}.cc)
    ## additional properties
    if (HAVE_HDF5)
      set_target_properties (${test}
	PROPERTIES
	COMPILE_FLAGS "-I${HDF5_INCLUDES} -I${CR_BINARY_DIR}"
	)
    else (HAVE_HDF5)
      set_target_properties (${test}
	PROPERTIES
	COMPILE_FLAGS "-I${CR_BINARY_DIR}"
	)
    endif (HAVE_HDF5)
    ## linker instructions
    target_link_libraries (${test}
      cr_test
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${LAPACK_LIBRARIES}
      ${WCSLIB_LIBRARIES}
      ${G2C_LIBRARIES}
      ${HAVE_LIBM}
      )
    ## testing
    add_test (${test} ${test})
  endforeach (test ${tests_casacore})
  
endif (HAVE_CASACORE AND HAVE_WCSLIB)

## -----------------------------------------------------------------------------
## Test(s) for Blitz++

if (HAVE_BLITZ)
  ## compiler instructions
  add_executable (tUseBlitz ${CR_BINARY_DIR}/crtools.h tUseBlitz.cc)
  set_target_properties (tUseBlitz
    PROPERTIES
    COMPILE_FLAGS "-I${BLITZ_INCLUDES}"
    )
  ## linker instructions
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
## Test for CFITSIO

if (HAVE_CFITSIO)
  ## build test program
  add_executable (tUseCFITSIO tUseCFITSIO.cc ${CR_BINARY_DIR}/crtools.h)
  set_target_properties (tUseCFITSIO
    PROPERTIES
    COMPILE_FLAGS "-I${CFITSIO_INCLUDES}"
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
  ## compiler instructions
  add_executable (tUseFFTW3 tUseFFTW3.cc ${CR_BINARY_DIR}/crtools.h)
  ## additional properties
  set_target_properties (tUseFFTW3
    PROPERTIES
    COMPILE_FLAGS "-I${FFTW3_INCLUDES}"
    )
  ## linker instructions
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
  ## compiler instructions
  add_executable (tUseGSL tUseGSL.cc)
  set_target_properties (tUseGSL
    PROPERTIES
    COMPILE_FLAGS "-I${GSL_INCLUDES}"
    )
  ## linker instructions
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
  ## compiler instructions
  add_executable (tUseWCS tUseWCS.cc)
  ## additional properties
  set_target_properties (tUseWCS
    PROPERTIES
    COMPILE_FLAGS "-I${WCSLIB_INCLUDES}"
    )
  ## linker instructions
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

if (HAVE_ROOT AND HAVE_CASACORE AND FFTW3_LIBRARIES)
  if (HAVE_DAL)
    ## compiler instructions
    add_executable (tUseROOT tUseROOT.cc)
    ## linker instructions
    if (HAVE_LIBUTIL AND HAVE_LIBDL)
      target_link_libraries (tUseROOT
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${CASA_LIBRARIES}
	${DAL_LIBRARIES}
	${HAVE_LIBUTIL}
	${HAVE_LIBDL}
	)
    else (HAVE_LIBUTIL AND HAVE_LIBDL)
      target_link_libraries (tUseROOT
	${FFTW3_LIBRARIES}
	${ROOT_LIBRARIES}
	${PYTHON_LIBRARIES}
	${CASA_LIBRARIES}
	${DAL_LIBRARIES}
	)
    endif (HAVE_LIBUTIL AND HAVE_LIBDL)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (tUseROOT
	PROPERTIES
	COMPILE_FLAGS "-I${DAL_INCLUDES} -I${ROOT_INCLUDES} -I${FFTW3_INCLUDES}"
	LINK_FLAGS "-bind_at_load"
	)
    else (APPLE)
      set_target_properties (tUseROOT
	PROPERTIES
	COMPILE_FLAGS "-I${DAL_INCLUDES} -I${ROOT_INCLUDES} -I${FFTW3_INCLUDES}"
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
endif (HAVE_ROOT AND HAVE_CASACORE AND FFTW3_LIBRARIES)

## -----------------------------------------------------------------------------
## Test working with the Data Access Library (DAL)

if (HAVE_DAL AND HAVE_CASACORE AND HDF5_MINOR_VERSION LESS 8)
  ## compiler instructions
  add_executable (tUseDAL tUseDAL.cc)
  ## linker instructions
  target_link_libraries (tUseDAL
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${CASA_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    )
  ## additional properties
  set_target_properties (tUseDAL
    PROPERTIES
    COMPILE_FLAGS "-I${DAL_INCLUDES} -I${HDF5_INCLUDES}"
    )
endif (HAVE_DAL AND HAVE_CASACORE AND HDF5_MINOR_VERSION LESS 8)

## -----------------------------------------------------------------------------
## Test working with the Python libraries

if (HAVE_PYTHON)
  ## compiler instructions
  add_executable (tUsePython tUsePython.cc)
  ## additional properties
  set_target_properties (tUsePython
    PROPERTIES
    COMPILE_FLAGS "-I${PYTHON_INCLUDES}"
    )
  ## linker instructions
  target_link_libraries (tUsePython ${PYTHON_LIBRARIES})
  ## testing
  add_test (tUsePython tUsePython)
endif (HAVE_PYTHON)
