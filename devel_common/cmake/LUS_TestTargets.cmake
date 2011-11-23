
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

  add_custom_target (DashboardBuild
    COMMAND make ExperimentalStart
    COMMAND make ExperimentalConfigure
    COMMAND make ExperimentalSubmit
    COMMAND make ExperimentalTest
    COMMAND make ExperimentalSubmit
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

  ##__________________________________________________________________
  ##                                                 External packages

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

    ## Define test target
    add_test (test_build_${varExternalPackage} make ${varExternalPackage})

  endforeach (varExternalPackage)

  ##__________________________________________________________________
  ##                                                      LUS packages

  ## Test building the packages from within the LUS framework

  add_test (test_build_anaamika   make anaamika  )
  add_test (test_build_dal        make dal       )
  add_test (test_build_contrib    make contrib   )
  add_test (test_build_rm         make rm        )
  add_test (test_build_pulsar     make pulsar    )
  add_test (test_build_pycrtools  make pycrtools )

  ## Run the tests defined as part of the individual packages
  
  if (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)
    add_test (
      NAME test_tests_anaamika
      WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/Anaamika/src/anaamika-build
      COMMAND make Experimental
      )
  endif (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)
  
  if (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)
    add_test (
      NAME test_tests_rm
      WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/RM/src/rm-build
      COMMAND make Experimental
      )
  endif (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)
  
endif (NOT LUS_TESTTARGETS_CMAKE)
