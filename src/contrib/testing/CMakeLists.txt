
## ==============================================================================
##
##  Tests for casacore
##
## ==============================================================================

if (HAVE_CASACORE)

  include_directories (${CASA_INCLUDES})

  set (tests
    casacoreArrayMath
    casacoreMatrixAccess02
    casacoreMatrixAccess03
    casacoreMatrixAccess04
    )

  foreach (src ${tests})
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    ## linker instructions
    target_link_libraries (${src} ${CASA_LIBRARIES})
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
  
endif (HAVE_CASACORE)

## ==============================================================================
##
##  Tests for Boost
##
## ==============================================================================

if (HAVE_BOOST)

  include_directories (${BOOST_INCLUDES})

  ## compiler instructions
  add_executable (boostProgramOptions boostProgramOptions.cc)
  ## linker instructions
  target_link_libraries (boostProgramOptions ${BOOST_LIBRARIES})
  ## testing
  add_test (boostProgramOptions_test1 boostProgramOptions)
  add_test (boostProgramOptions_test2 boostProgramOptions --help)
  add_test (boostProgramOptions_test3 boostProgramOptions --blocksize 10)
  add_test (boostProgramOptions_test4 boostProgramOptions --refcode J2000)
  add_test (boostProgramOptions_test5 boostProgramOptions --projection SIN)
  add_test (boostProgramOptions_test6 boostProgramOptions --blocksize 2048 --refcode B1950)

endif (HAVE_BOOST)
