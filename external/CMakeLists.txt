
## List of external packages _____________________

set (external_packages
  armadillo
  bison
  boost
  casacore
  cfitsio
  fftw3
  flex
  gsl
  hdf5
  itpp
  matplotlib
  numeric
  numpy
  pil
  plplot
  pyqt
  python
  qt
  root
  sip
  swig
  szip
  valgrind
  )

if (NOT HAVE_BLITZ)
  list (APPEND external_packages blitz)
endif (NOT HAVE_BLITZ)

if (OPENGL_FOUND)
  list (APPEND external_packages mathgl)
else (OPENGL_FOUND)
  add_custom_target (mathgl
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    COMMENT "[MathGL] Unable to find OpenGL; skipping build!"
    )
endif (OPENGL_FOUND)

## Targets for external packages _________________

foreach (external_package ${external_packages}
    )
  ## search for CMakeLists.txt file
  find_file (external_package_${external_package}
    NAMES CMakeLists.txt
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/${external_package}
    )
  ## add the package to the list
  if (external_package_${external_package})
    ## Feedback
    message (STATUS "Adding external package ${external_package} ...")
    ## Add sub-directory
    add_subdirectory (${external_package})
  endif (external_package_${external_package})
endforeach (external_package armadillo)

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
