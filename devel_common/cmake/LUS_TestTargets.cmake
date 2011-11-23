
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

  foreach (varInternalPackage
      dal
      contrib
      rm
      anaamika
      pulsar
      )
    add_test (test_build_${varInternalPackage} make ${varInternalPackage})
  endforeach (varInternalPackage)

endif (NOT LUS_TESTTARGETS_CMAKE)
