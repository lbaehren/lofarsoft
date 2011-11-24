
## Include guard
if (NOT LUS_TESTTARGETS_CMAKE)

  ## Include guard
  set (LUS_TESTTARGETS_CMAKE TRUE)

  ## ============================================================================
  ##
  ##  Options
  ##
  ## ============================================================================

  option (LUS_TEST_EXTERNAL_PACKAGES "Test building external packages?" YES)

  ## ============================================================================
  ##
  ##  Definition of macros & custom targets
  ##
  ## ============================================================================
  
  ##__________________________________________________________________
  ## Custom target to run the collection of tests and submit the
  ## results to the Dashboard server.

  file (WRITE ${PROJECT_BINARY_DIR}/testbuild
"${CMAKE_CTEST_COMMAND} -D ExperimentalStart
${CMAKE_CTEST_COMMAND} -D ExperimentalConfigure
${CMAKE_CTEST_COMMAND} -D ExperimentalTest
${CMAKE_CTEST_COMMAND} -D ExperimentalSubmit"
    )
  
  add_custom_target (DashboardBuild
    COMMAND sh testbuild
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Starting test builds to be submitted to Dashboard ..."
    SOURCES ${PROJECT_SOURCE_DIR}/CTestConfig.cmake
    )
  
  ## ============================================================================
  ##
  ##  Test targets for find modules
  ##
  ## ============================================================================

  ## ============================================================================
  ##
  ##  Test targets for building packages
  ##
  ## ============================================================================
  
  if (LUS_TEST_EXTERNAL_PACKAGES)
    
    ##________________________________________________________________
    ##                                               External packages
    
    foreach (varExternalPackage
	armadillo
	bison
	blitz
	boost
	casacore
	cfitsio
	cppunit
	fftw3
	flex
	gsl
	hdf5
	ipython
	itpp
	matplotlib
	numpy
	pil
	plplot
	pyfits
	sip
	sphinx
	swig
	tmf
	wcslib
	wcstools
	)
      
      ## Set up a test in case the corresponding target does exist
      if (TARGET ${varExternalPackage})
	add_test (test_build_${varExternalPackage} make ${varExternalPackage})
      endif (TARGET ${varExternalPackage})
      
    endforeach (varExternalPackage)
    
  endif (LUS_TEST_EXTERNAL_PACKAGES)
  
  ##__________________________________________________________________
  ##                                                      LUS packages
  
  ## Test building the packages from within the LUS framework
  
  add_test (test_build_contrib    ${CMAKE_BUILD_TOOL} contrib   )
  add_test (test_build_dal        ${CMAKE_BUILD_TOOL} dal       )
  add_test (test_build_pycrtools  ${CMAKE_BUILD_TOOL} pycrtools )

  ## Pulsar Tools
  
  if (TARGET pulsar)
    
    add_test (test_build_pulsar ${CMAKE_BUILD_TOOL} pulsar )
    
    if (EXISTS ${LUS_BINARY_DIR}/pulsar)
      add_test (
	NAME test_tests_pulsar
	WORKING_DIRECTORY ${LUS_BINARY_DIR}/pulsar
	COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
	)
    endif (EXISTS ${LUS_BINARY_DIR}/pulsar)
    
  endif (TARGET pulsar)
  
endif (NOT LUS_TESTTARGETS_CMAKE)
