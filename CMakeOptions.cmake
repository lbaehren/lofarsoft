
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
set (PYCRTOOLS_WITH_DAL1 ${PYCRTOOLS_WITH_DAL1} CACHE BOOL
  "Compile PyCRTools with DAL1 support?" )
set (PYCRTOOLS_WITH_FFTW ${PYCRTOOLS_WITH_FFTW} CACHE BOOL
  "Compile PyCRTools with FFTW support?")
set (PYCRTOOLS_WITH_GSL ${PYCRTOOLS_WITH_GSL} CACHE BOOL
  "Compile PyCRTools with GSL support?" )
set (PYCRTOOLS_WITH_NUMPY ${PYCRTOOLS_WITH_NUMPY} CACHE BOOL
  "Compile PyCRTools with numpy support?" )
set (PYCRTOOLS_WITH_OPENMP ${PYCRTOOLS_WITH_OPENMP} CACHE BOOL
  "Compile PyCRTools with OpenMP support?")

