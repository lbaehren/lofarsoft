
## ==============================================================================
##
##  Tests for casacore
##
## ==============================================================================

if (HAVE_CASACORE)

  include_directories (${CASA_INCLUDES})

  set (tests
    casacoreArrayAccess
    casacoreArrayMath
    casacoreMatrixAccess02
    casacoreMatrixAccess03
    casacoreMatrixAccess04
    casacoreMeasures
    casacoreCoordinates
    casacore_Coordinates_ObsInfo
    tcasa_casa
    )

  foreach (src ${tests})
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src}
      ${CASA_LIBRARIES}
      ${WCSLIB_LIBRARIES}
      ${LAPACK_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## testing
    add_test (${src} ${src})
  endforeach (src)

  ## compiler instructions
  add_executable (casacoreMatrixAccess casacoreMatrixAccess.cc)
  ## linker instructions
  target_link_libraries (casacoreMatrixAccess ${CASA_LIBRARIES})
  ## testing
  add_test (casacoreMatrixAccess_test1 casacoreMatrixAccess test1)
  add_test (casacoreMatrixAccess_test2 casacoreMatrixAccess test2)
  add_test (casacoreMatrixAccess_test3 casacoreMatrixAccess test3)
  add_test (casacoreMatrixAccess_test4 casacoreMatrixAccess test4)
  add_test (casacoreMatrixAccess_test5 casacoreMatrixAccess test5)
  add_test (casacoreMatrixAccess_test6 casacoreMatrixAccess test6)
  
  ## compiler instructions
  add_executable (casacoreCubeAccess casacoreCubeAccess.cc)
  ## linker instructions
  target_link_libraries (casacoreCubeAccess ${CASA_LIBRARIES})
  ## testing
  add_test (casacoreCubeAccess_test1 casacoreCubeAccess test1)
  add_test (casacoreCubeAccess_test2 casacoreCubeAccess test2)

  if (HAVE_CRTOOLS)
  ## compiler instructions
  add_executable (tcasa_images tcasa_images.cc)
  ## linker instructions
  target_link_libraries (tcasa_images ${CASA_LIBRARIES} ${CRTOOLS_CR_LIBRARY})
  ## testing
    add_test (tcasa_images_test02 tcasa_images --hdf5)
    add_test (tcasa_images_test03 tcasa_images --lofar)
  endif (HAVE_CRTOOLS)
  
else (HAVE_CASACORE)
  message (STATUS "Missing casacore - skipping tests!")
endif (HAVE_CASACORE)

## ==============================================================================
##
##  Tests for casacore/CASA
##
## ==============================================================================

if (HAVE_CASACORE AND HAVE_WCSLIB AND HAVE_CFITSIO)

  set (tests_casacore
    tcasa_casa_Quanta
    tcasa_casa_Containers
    tcasa_tables
    tcasa_lattices
    tcasa_measures
    )
  
  set (tests_casacore_link_libs ${LAPACK_LIBRARIES})
  list (APPEND tests_casacore_link_libs ${CFITSIO_LIBRARIES})
  list (APPEND tests_casacore_link_libs ${WCSLIB_LIBRARIES})
  list (APPEND tests_casacore_link_libs ${LAPACK_LIBRARIES})
  
  if (GFORTRAN_LIBRARIES) 
    list (APPEND tests_casacore_link_libs ${GFORTRAN_LIBRARIES})
  endif (GFORTRAN_LIBRARIES)
  
  foreach (test ${tests_casacore})
    ## compiler instructions
    add_executable (${test} ${test}.cc)
    ## additional properties
    if (HAVE_HDF5)
      set_target_properties (${test}
	PROPERTIES
	COMPILE_FLAGS "-I${HDF5_INCLUDES}"
	)
      ## linker instructions
      target_link_libraries (${test}
	${CASA_LIBRARIES}
	${WCSLIB_LIBRARIES}
	${LAPACK_LIBRARIES}
	${HDF5_LIBRARIES}
	)
    else (HAVE_HDF5)
      set_target_properties (${test}
	PROPERTIES
	COMPILE_FLAGS "-I${CR_BINARY_DIR}"
	)
      ## linker instructions
      target_link_libraries (${test}
	${CASA_LIBRARIES}
	${WCSLIB_LIBRARIES}
	${LAPACK_LIBRARIES}
	)
    endif (HAVE_HDF5)
    ## testing
    add_test (${test} ${test})
  endforeach (test ${tests_casacore})
  
  
else (HAVE_CASACORE AND HAVE_WCSLIB AND HAVE_CFITSIO)
  message (STATUS "Missing casacore/CASA - skipping tests!")
endif (HAVE_CASACORE AND HAVE_WCSLIB AND HAVE_CFITSIO)

## ==============================================================================
##
##  Tests for Boost
##
## ==============================================================================

if (HAVE_BOOST)

  include_directories (${BOOST_INCLUDES})

  ## compiler instructions
  add_executable (boostProgramOptions boostProgramOptions.cc)
  add_executable (boostThread boostThread.cc)
  ## linker instructions
  target_link_libraries (boostProgramOptions ${BOOST_LIBRARIES})
  target_link_libraries (boostThread ${BOOST_LIBRARIES})
  ## testing
  add_test (boostProgramOptions_test1 boostProgramOptions)
  add_test (boostProgramOptions_test2 boostProgramOptions --help)
  add_test (boostProgramOptions_test3 boostProgramOptions --infile testdata.h5)
  add_test (boostProgramOptions_test4 boostProgramOptions --infile testdata.h5 --blocksize 10)
  add_test (boostProgramOptions_test5 boostProgramOptions --infile testdata.h5 --refcode J2000)
  add_test (boostProgramOptions_test6 boostProgramOptions --infile testdata.h5 --projection SIN)
  add_test (boostProgramOptions_test7 boostProgramOptions --infile testdata.h5 --blocksize 2048 --refcode B1950)

else (HAVE_BOOST)
  message (STATUS "Missing Bosst - skipping tests!")
endif (HAVE_BOOST)

## ==============================================================================
##
##  Tests for HDF5
##
## ==============================================================================

if (HAVE_HDF5)

  include_directories (${HDF5_INCLUDES})

  ##__________________________________________________________________
  ## Custom tests

  add_executable (hdf5_attributes hdf5_attributes.cc)
  target_link_libraries (hdf5_attributes ${HDF5_LIBRARIES})
  add_test (hdf5_attributes hdf5_attributes)
  
  ##__________________________________________________________________
  ## Third party tests
  
  set (tests_hdf5
    hdf5_attrs
    hdf5_attrsarray
    hdf5_attrvstr
    hdf5_cmpvlstrar
    hdf5_crtstr1
    hdf5_grpatt
    hdf5_h5cmpstr
    hdf5_h5time
    hdf5_strarray
    hdf5_vlstra
    hdf5_vlstratt
    )
  
  foreach (test_hdf5 ${tests_hdf5}) 
    ## locate the source file
    find_file (HAVE_${test_hdf5} ${test_hdf5}.c PATHS .)
    ## remove test from list of source file not found
    if (NOT HAVE_${test_hdf5})
      list (REMOVE_ITEM tests_hdf5 ${test_hdf5})
    endif (NOT HAVE_${test_hdf5})
  endforeach (test_hdf5)
  
  foreach (src ${tests_hdf5})
    ## compiler instructions
    add_executable (${src} ${src}.c)
    ## linker instructions
    target_link_libraries (${src} ${HDF5_LIBRARIES})
    ## additional properties
    set_target_properties (${src} PROPERTIES
      COMPILE_FLAGS "-DH5_USE_16_API"
      )
    ## Testing 
    add_test (${src} ${src})
  endforeach (src)  
  
else (HAVE_HDF5)
  message (STATUS "Missing HDF5 library - skipping tests!")
endif (HAVE_HDF5)

## ==============================================================================
##
##  Tests for Blitz++
##
## ==============================================================================

if (HAVE_BLITZ)

  ## compiler instructions
  add_executable (blitz blitz.cc)
  set_target_properties (blitz
    PROPERTIES
    COMPILE_FLAGS "-I${BLITZ_INCLUDES}"
    )

  ## linker instructions
  target_link_libraries (blitz ${BLITZ_LIBRARIES})

  ## testing
  add_test (blitz blitz)

else (HAVE_BLITZ)
  message (STATUS "Missing Blitz++ - skipping tests!")
endif (HAVE_BLITZ)

## ==============================================================================
##
##  Tests for CFITSIO
##
## ==============================================================================

if (HAVE_CFITSIO)

  ## compiler instructions
  add_executable (test_cfitsio test_cfitsio.cc)
  set_target_properties (test_cfitsio
    PROPERTIES
    COMPILE_FLAGS "-I${CFITSIO_INCLUDES}"
    )
  ## linker instructions
  target_link_libraries (test_cfitsio ${CFITSIO_LIBRARIES})
  ## testing
  add_test (test_cfitsio test_cfitsio)
  ## installation
  install (
    TARGETS test_cfitsio
    RUNTIME DESTINATION bin
    )

endif (HAVE_CFITSIO)

## ==============================================================================
##
## Tests for the Data Access Library (DAL)
##
## ==============================================================================

if (HAVE_DAL AND HAVE_CASACORE AND HAVE_HDF5)
  ## compiler instructions
  add_executable (test_dal test_dal.cc)
  ## linker instructions
  target_link_libraries (test_dal
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${CASA_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    )
  ## additional properties
  set_target_properties (test_dal
    PROPERTIES
    COMPILE_FLAGS "-I${DAL_INCLUDES} -I${HDF5_INCLUDES}"
    )
  ## testing
  add_test (test_dal test_dal ${dataset_tbb})
endif (HAVE_DAL AND HAVE_CASACORE AND HAVE_HDF5)

## ==============================================================================
##
##  Tests for GNU Scientific Library (GSL)
##
## ==============================================================================

if (HAVE_GSL)

  add_executable (gsl gsl.cc)

  target_link_libraries (gsl ${GSL_LIBRARIES})

  add_test (gsl gsl)

else (HAVE_GSL)
  message (STATUS "Missing GNU Scientic Library (GSL) - skipping tests!")
endif (HAVE_GSL)

