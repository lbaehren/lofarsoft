
##____________________________________________________________________
## Load CMake modules

include (ExternalProject)

##____________________________________________________________________
## List of external packages

set (external_packages
  aera
  armadillo
  bison
  blas
  blitz
  boost
  casacore
  cfitsio
  cppunit
  fftw3
  flex
  git
  gsl
  hdf5
  itpp
  matplotlib
  numeric
  numpy
  scipy
  sphinx
  pelican
  pil
  plplot
  pyfits
  pyqt
  python
  ipython
  qt
  root
  sip
  swig
  szip
  tmf
  valgrind
  wcstools
  )

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

