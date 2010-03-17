
foreach (external_package
    armadillo
    bison
    blitz
    boost
    casacore
    flex
    fftw3
    gsl
    hdf5
    itpp
    mathgl
    matplotlib
    numeric
    numpy
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
