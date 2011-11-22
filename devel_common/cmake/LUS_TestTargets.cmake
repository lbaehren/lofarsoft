
## Include guard
if (NOT LUS_TESTTARGETS_CMAKE)

  ## Include guard
  set (LUS_TESTTARGETS_CMAKE TRUE)

  ## ============================================================================
  ##
  ##  Definition of macros
  ##
  ## ============================================================================

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

  ## External packages

  add_test (test_build_bison    make bison    )
  add_test (test_build_flex     make flex     )
  add_test (test_build_gsl      make gsl      )
  add_test (test_build_hdf5     make hdf5     )
  add_test (test_build_itpp     make itpp     )
  add_test (test_build_numpy    make numpy    )
  add_test (test_build_pil      make pil      )
  add_test (test_build_swig     make swig     )

  ## LUS packages

  add_test (test_build_dal      make dal      )
  add_test (test_build_contrib  make contrib  )
  add_test (test_build_rm       make rm       )
  add_test (test_build_anaamika make anaamika )
  add_test (test_build_pulsar   make pulsar   )

endif (NOT LUS_TESTTARGETS_CMAKE)
