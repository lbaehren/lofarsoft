
##_______________________________________________________________________________
##                                                             Load CMake modules

include (ExternalProject)

##_______________________________________________________________________________
## List of external packages

set (external_packages
  aera
  armadillo
  bison
  blas
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
  num_util
  numpy
  scipy
  sphinx
  plplot
  pyfits
  pyqt
  python
  ipython
  root
  sip
  swig
  szip
  tmf
  valgrind
  wcslib
  wcstools
  dal1
  dal
  )

if (OPENGL_FOUND)
  list (APPEND external_packages mathgl)
else (OPENGL_FOUND)
  add_custom_target (mathgl
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    COMMENT "[MathGL] Unable to find OpenGL; skipping build!"
    )
endif (OPENGL_FOUND)

if (ROOT_FOUND)
  list (APPEND external_packages startools)
endif (ROOT_FOUND)

##_______________________________________________________________________________
##                                                  Targets for external packages

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

##_______________________________________________________________________________
##                                         Dependencies between external packages

add_dependencies (casacore hdf5 cfitsio wcslib bison flex)
add_dependencies (hdf5 szip)
add_dependencies (mathgl hdf5 swig num_util gsl)
add_dependencies (matplotlib numpy)
add_dependencies (num_util boost numpy)
add_dependencies (numpy cblas)
add_dependencies (plplot numpy swig)
add_dependencies (pyqt sip)
add_dependencies (wcslib boost flex)

if (ROOT_FOUND)
  add_dependencies (startools fftw3)
endif (ROOT_FOUND)
