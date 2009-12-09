
add_subdirectory (blitz)
add_subdirectory (boost)
add_subdirectory (fftw3)
add_subdirectory (itpp)
add_subdirectory (matplotlib)
add_subdirectory (numeric)
add_subdirectory (plplot)
add_subdirectory (swig)
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
