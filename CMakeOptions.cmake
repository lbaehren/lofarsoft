
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

set (HAVE_OPENMP ${OPENMP_FOUND} CACHE BOOL
  "Does the compiler support OpenMP?")

## === Anaamika =================================================================

set (ANAAMIKA_ENABLE_SCIPY ${ANAAMIKA_ENABLE_SCIPY} CACHE BOOL
  "Build SciPy as interactive environment?"                           )
set (ANAAMIKA_ENABLE_FBDSM ${ANAAMIKA_ENABLE_FBDSM} CACHE BOOL
  "Build Fortran-based version of BDSM?"                              )
set (ANAAMIKA_ENABLE_PYBDSM ${ANAAMIKA_ENABLE_PYBDSM} CACHE BOOL
  "Build Python-enabled version of BDSM?"                             )

## === CR-Tools =================================================================

set (CR_WITH_DAL ${CR_WITH_DAL} CACHE BOOL
  "Build and use the Data Access Library?"                            )
set (CR_WITH_GLISH ${CR_WITH_GLISH} CACHE BOOL
  "Support for Glish applications?"                                   )
set (CR_WITH_PYCR ${CR_WITH_PYCR} CACHE BOOL
  "Support for Python applications?"                                  )
set (CR_WITH_PLOTTING ${CR_WITH_PLOTTING} CACHE BOOL
  "Support for generation of plots?"                                  )
set (CR_WITH_GUI ${CR_WITH_GUI} CACHE BOOL
  "Build the graphical interface?"                                    )
set (CR_WITH_STARTOOLS ${CR_WITH_STARTOOLS} CACHE BOOL
  "Enable using routines from LOPES-Star?"                            )
set (CR_WITH_CASAPY ${CR_WITH_CASAPY} CACHE BOOL
  "Build Pyline library against CASApy?"                              )

## === PyCRTools =================================================================

set (PYCRTOOLS_ENABLE_TESTING ${PYCRTOOLS_ENABLE_TESTING} CACHE BOOL
  "Enable testing for the PyCRTools?" )
set (PYCRTOOLS_WITH_COMPILER_WARNINGS ${PYCRTOOLS_WITH_COMPILER_WARNINGS} CACHE BOOL
  "Compile PyCRTools with compiler warnings enabled?" )
set (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS ${PYCRTOOLS_WITH_CPU_OPTIMIZATIONS} CACHE BOOL
  "Compile PyCRTools with optimization for CPU architecture?" )
set (PYCRTOOLS_WITH_DEBUG_SYMBOLS ${PYCRTOOLS_WITH_DEBUG_SYMBOLS} CACHE BOOL
  "Compile PyCRTools with debug symbols?" )
set (PYCRTOOLS_WITH_OPTIMIZATIONS ${PYCRTOOLS_WITH_OPTIMIZATIONS} CACHE BOOL
  "Compile PyCRTools with optimization flags turned on?")
set (PYCRTOOLS_WITH_RUNTIME_WARNINGS ${PYCRTOOLS_WITH_RUNTIME_WARNINGS} CACHE BOOL
  "Compile PyCRTools with runtime warnings enabled?")
set (PYCRTOOLS_WITH_AERA ${PYCRTOOLS_WITH_AERA} CACHE BOOL
  "Compile PyCRTools with AERA data support?" )
set (PYCRTOOLS_WITH_CASACORE ${PYCRTOOLS_WITH_CASACORE} CACHE BOOL
  "Compile PyCRTools with casacore support?")
set (PYCRTOOLS_WITH_DAL ${PYCRTOOLS_WITH_DAL} CACHE BOOL
  "Compile PyCRTools with DAL support?" )
set (PYCRTOOLS_WITH_FFTW ${PYCRTOOLS_WITH_FFTW} CACHE BOOL
  "Compile PyCRTools with FFTW support?")
set (PYCRTOOLS_WITH_GSL ${PYCRTOOLS_WITH_GSL} CACHE BOOL
  "Compile PyCRTools with GSL support?" )
set (PYCRTOOLS_WITH_NUMPY ${PYCRTOOLS_WITH_NUMPY} CACHE BOOL
  "Compile PyCRTools with numpy support?" )
set (PYCRTOOLS_WITH_OPENMP ${PYCRTOOLS_WITH_OPENMP} CACHE BOOL
  "Compile PyCRTools with OpenMP support?")

## === DAL ======================================================================

set (DOCUMENTATION_ONLY ${DOCUMENTATION_ONLY} CACHE BOOL
  "Configure for building documentation only?")
set (DAL_WITH_DOCUMENTATION ${DAL_WITH_DOCUMENTATION} CACHE BOOL
  "Generate documentation for the DAL?")
set (DAL_SHARED_LIBRARY ${DAL_SHARED_LIBRARY} CACHE BOOL
  "Build DAL as shared library?")
set (DAL_ENABLE_TESTING ${DAL_ENABLE_TESTING} CACHE BOOL
  "Build and enable test programs?")
set (DAL_COMPILER_WARNINGS ${DAL_COMPILER_WARNINGS} CACHE BOOL
  "Enable standard set of compiler warnings?")
set (DAL_PYTHON_BINDINGS ${DAL_PYTHON_BINDINGS} CACHE BOOL
  "Create python bindings?")
set (DAL_DEBUGGING_MESSAGES ${DAL_DEBUGGING_MESSAGES} CACHE BOOL
  "Print debugging information?")
set (DAL_VERBOSE_CONFIGURE ${DAL_VERBOSE_CONFIGURE} CACHE BOOL
  "Verbose output during configuration?")
set (DAL_WITH_MYSQL ${DAL_WITH_MYSQL} CACHE BOOL
  "Build with support for MySQL database?")

## === WCSLIB ======================================================================

set (WCSLIB_PYTHON_WRAPPER ${WCSLIB_PYTHON_WRAPPER} CACHE BOOL
  "Generate Python wrapper for WCSLIB?")
