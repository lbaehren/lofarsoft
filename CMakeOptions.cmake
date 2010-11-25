
set (CMAKE_INSTALL_PREFIX ${LUS_INSTALL_PREFIX} CACHE PATH
  "CMake installation area."
  FORCE
  )
set (LUS_ENABLE_TESTING ${LUS_ENABLE_TESTING} CACHE BOOL
  "Enable testing?"
  FORCE
  )
set (LUS_ENABLE_DASHBOARD ${LUS_ENABLE_DASHBOARD} CACHE BOOL
  "Enable Experimental/Nightly builds with results send to Dashboard?"
  FORCE
  )

## === System description =======================================================

option (HAVE_OPENMP ${OPENMP_FOUND} CACHE BOOL
  "Does the compiler support OpenMP?")

## === Anaamika =================================================================

option (ANAAMIKA_ENABLE_SCIPY ${ANAAMIKA_ENABLE_SCIPY} CACHE BOOL
  "Build SciPy as interactive environment?"                           )
option (ANAAMIKA_ENABLE_FBDSM ${ANAAMIKA_ENABLE_FBDSM} CACHE BOOL
  "Build Fortran-based version of BDSM?"                              )
option (ANAAMIKA_ENABLE_PYBDSM ${ANAAMIKA_ENABLE_PYBDSM} CACHE BOOL
  "Build Python-enabled version of BDSM?"                             )

## === CR-Tools =================================================================

option (CR_WITH_DAL ${CR_WITH_DAL} CACHE BOOL
  "Build and use the Data Access Library?"                            )
set (CR_WITH_GLISH ${CR_WITH_GLISH} CACHE BOOL
  "Support for Glish applications?"                                   )
set (CR_WITH_PYCR ${CR_WITH_PYCR} CACHE BOOL
  "Support for Python applications?"                                  )
set (CR_WITH_PLOTTING ${CR_WITH_PLOTTING} CACHE BOOL
  "Support for generation of plots?"                                  )
set (CR_WITH_GUI ${CR_WITH_GUI} CACHE BOOL
  "Build the graphical interface?"                                    )
set (CR_WITH_PYPELINE ${CR_WITH_PYPELINE} CACHE BOOL
  "Build the Pypeline library?"                                       )
set (CR_WITH_STARTOOLS ${CR_WITH_STARTOOLS} CACHE BOOL
  "Enable using routines from LOPES-Star?"                            )
set (CR_WITH_CASAPY ${CR_WITH_CASAPY} CACHE BOOL
  "Build Pyline library against CASApy?"                              )

## === DAL ======================================================================

set (DAL_BUILD_TESTS ${DAL_BUILD_TESTS} CACHE BOOL
  "Build the test programs?"                                          )
set (DAL_BUILD_APPS ${DAL_BUILD_APPS} CACHE BOOL
  "Build the applications?"                                           )
set (DAL_ENABLE_TESTING ${DAL_ENABLE_TESTING} CACHE BOOL
  "Build the test programs?"                                          )
set (DAL_ENABLE_DASHBOARD ${DAL_ENABLE_DASHBOARD} CACHE BOOL
  "Send a build report to the Dashboard server?"                      )
set (DAL_COMPILER_WARNINGS ${DAL_COMPILER_WARNINGS} CACHE BOOL
  "Enable standard set of compiler warnings?"                         )
set (DAL_HDF5_MACROS ${DAL_HDF5_MACROS} CACHE BOOL
  "HDF5 compatibility macros version"                                 )
set (DAL_PYTHON_BINDINGS ${DAL_PYTHON_BINDINGS} CACHE BOOL
  "Create python bindings?"                                           )
set (DAL_DEBUGGING_MESSAGES ${DAL_DEBUGGING_MESSAGES} CACHE BOOL
  "Print debugging information?"                                      )
set (DAL_VERBOSE_CONFIGURE ${DAL_VERBOSE_CONFIGURE} CACHE BOOL
  "Verbose output during configuration?"                              )

## === RM Synthesis =============================================================

set (RM_ENABLE_ITPP ${RM_ENABLE_ITPP} CACHE BOOL
  "Enable using IT++ library?"                                        )
set (RM_ENABLE_ARMADILLO ${RM_ENABLE_ARMADILLO} CACHE BOOL
  "Enable using Armadillo library?"                                   )

