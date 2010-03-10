
add_subdirectory (bison)
add_subdirectory (blitz)
add_subdirectory (boost)
add_subdirectory (casacore)
add_subdirectory (flex)
add_subdirectory (fftw3)
add_subdirectory (hdf5)
add_subdirectory (itpp)
add_subdirectory (mathgl)
add_subdirectory (matplotlib)
add_subdirectory (numeric)
add_subdirectory (numpy)
add_subdirectory (plplot)
add_subdirectory (pyqt)
add_subdirectory (python)
add_subdirectory (qt)
add_subdirectory (root)
add_subdirectory (sip)
add_subdirectory (swig)
add_subdirectory (szip)
add_subdirectory (valgrind)

##____________________________________________________________________
## Packages which are only available on Mac OS X

if (APPLE)
  add_subdirectory (${USG_EXTERNAL}/fink)
endif (APPLE)

##____________________________________________________________________
## Packages not available on Mac OS X

if (NOT APPLE)
  ## packages
  add_subdirectory (texinfo)
  add_subdirectory (glibc)
  ## dependencies
  add_dependencies (glibc texinfo)
endif (NOT APPLE)
