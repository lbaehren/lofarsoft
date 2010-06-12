
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1620 2008-06-06 09:26:35Z baehren                       $
##-------------------------------------------------------------------------------

## Name of the project handled by CMake
project (LUS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${LUS_SOURCE_DIR}
  ${LUS_SOURCE_DIR}/..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  ## load custom modules
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  ## define custom variables
  set (USG_BUILD ${USG_ROOT}/build)
  set (USG_EXTERNAL ${USG_ROOT}/external)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ==============================================================================
##
##  Build options
##
## ==============================================================================

option (LUS_VERBOSE_CONFIGURE   "Be verbose during configuration?"            NO )
option (POST_BUILD_CLEAN       "Clean up the build directory after a build?"  NO )
option (MULTITHREADED_BUILD    "Enable multithreading during build?"          NO )
option (TEST_DATASETS          "Download and install sample test datasets?"   NO )
option (DOXYGEN_GENERATE_LATEX "Have Doxygen generate LaTeX output?"          NO )

## Options affecting the build of individual packages

option (CR_WITH_GUI          "Build CR-Tools with graphical interface?"   NO  )
option (DAL_PYTHON_BINDINGS  "Create python bindings for DAL?"            YES )
option (RM_ENABLE_ITPP       "Enable using IT++ library?"                 NO  )
option (RM_ENABLE_ARMADILLO  "Enable using Armadillo library?"            YES )

## ==============================================================================
##
##  Basic system inspection:
##
##  - Look for essential system tools, which are required; some of the targets
##    only will be activated if the necessary tool is available.
##
## ==============================================================================

find_program (bash_executable       bash    PATHS ${bin_locations})
find_program (bison_executable      bison   PATHS ${bin_locations})
find_program (curl_executable       curl    PATHS ${bin_locations})
find_program (doxygen_executable    doxygen PATHS ${bin_locations})
find_program (flex_executable       flex    PATHS ${bin_locations})
find_program (perl_executable       perl    PATHS ${bin_locations})
find_program (tar_executable        tar     PATHS ${bin_locations})
find_program (wget_executable       wget    PATHS ${bin_locations})

set (BLITZ_FIND_QUIETLY           YES )
set (OPENGL_FIND_QUIETLY          YES )
set (ROOT_FIND_QUIETLY            YES )
set (VERSION_CONTROL_FIND_QUIETLY YES )

include (BuildPackage)
include (FindBlitz)
include (FindOpenGL)
include (FindROOT)
include (FindVersionControl)

## ==============================================================================
##
##  Documentation generation
##
## ==============================================================================

if (doxygen_executable)
  if (DOXYGEN_GENERATE_LATEX)
    ## Locate required executables
    find_program (latex_executable      latex      PATHS ${bin_locations})
    find_program (makeindex_executable  makeindex  PATHS ${bin_locations})
    find_program (pdflatex_executable   pdflatex   PATHS ${bin_locations})
    ## Is LaTeX available?
    if (latex_executable AND makeindex_executable)
      set (DOXYGEN_GENERATE_LATEX YES)
    endif (latex_executable AND makeindex_executable)
    ## Is PDFLaTeX available?
    if (pdflatex_executable)
      set (DOXYGEN_USE_PDFLATEX YES)
    endif (pdflatex_executable)
  endif (DOXYGEN_GENERATE_LATEX)
  ## Creation configuration file for Doxygen
  configure_file (${USG_ROOT}/doc/Doxyfile.in ${USG_ROOT}/doc/Doxyfile)
  ## Create documentation
  add_custom_target (doc
    COMMAND cmake --help-html CMake.html
    COMMAND ctest --help-html CTest.html
    COMMAND ${doxygen_executable}
    COMMAND rm -f *.tmp
    WORKING_DIRECTORY ${USG_ROOT}/doc
    COMMENT "Creating browsable documentation in ${USG_ROOT}/doc ..."
    )
  ## Source file dependency of build target
else (doxygen_executable)
  add_custom_target (doc
    COMMAND cmake --help-html CMake.html
    COMMAND ctest --help-html CTest.html
    WORKING_DIRECTORY ${USG_ROOT}/doc
    COMMENT "Creating browsable documentation in ${USG_ROOT}/doc ..."
    )
endif (doxygen_executable)

## ==============================================================================
##
##  Cleaning up
##
## ==============================================================================

add_custom_target (clean-build
  COMMAND rm -rf *~
  COMMAND rm -rf contrib devel_common external
  COMMAND rm -rf bdsm cr dal lasa pulsar pybdsm smosaic
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Cleaning up build directory ..."
)

add_custom_target (clean-doc
  COMMAND rm -f *~
  COMMAND rm -f html/formula*
  COMMAND rm -f html/tab*
  COMMAND rm -f html/search.*
  COMMAND rm -f html/*.png
  COMMAND rm -f html/*.map
  COMMAND rm -f html/*.html
  COMMAND rm -f html/*.md5
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  COMMENT "Cleaning up documentation directory ..."
  )

add_custom_target (clean-release
  COMMAND rm -rf bin
  COMMAND rm -rf etc
  COMMAND rm -rf include
  COMMAND rm -rf info
  COMMAND rm -rf lib lib64
  COMMAND rm -rf plugins
  COMMAND rm -rf share
  WORKING_DIRECTORY ${USG_ROOT}/release
  COMMENT "Cleaning up release directory ..."
)

add_custom_target (clean-src
  COMMAND rm -rf *~ */*~ */*/*~ */*/*/*~
  WORKING_DIRECTORY ${USG_ROOT}/src
  COMMENT "Cleaning up build directory ..."
)

## ==============================================================================
##
##  External packages
##
## ==============================================================================

## Packages with build handled through shell script

configure_file (
  ${USG_ROOT}/devel_common/scripts/build_package.sh.in
  ${USG_BUILD}/build_package.sh
  )

set (packages_external
  blas
  num_util
  wcslib
  wcstools
  )

if (HAVE_ROOT)
  list (APPEND packages_external startools)
endif (HAVE_ROOT)

## Packages handled directly through CMake

add_subdirectory (${USG_ROOT}/data)
add_subdirectory (${USG_ROOT}/external)
add_subdirectory (${USG_ROOT}/devel_common)

## Build instructions for packages

foreach (package_name ${packages_external})
  build_package (${package_name} external/${package_name})
endforeach (package_name)

##____________________________________________________________________
##                                        Dependencies of the packages

add_dependencies (casacore hdf5 cfitsio wcslib bison flex)

add_dependencies (hdf5 szip)

add_dependencies (mathgl hdf5 swig num_util gsl)

add_dependencies (matplotlib numpy)

add_dependencies (num_util boost numpy)

#add_dependencies (pyrap casacore num_util)

add_dependencies (pyqt sip)

add_dependencies (wcslib boost flex)

add_dependencies (plplot numpy swig)

if (HAVE_ROOT)
  add_dependencies (startools fftw3)
endif (HAVE_ROOT)

## ==============================================================================
##
##  USG packages
##
## ==============================================================================

##____________________________________________________________________
##                                                                BDSM

build_package (bdsm src/BDSM)

add_dependencies (bdsm wcslib cfitsio boost num_util)

##____________________________________________________________________
##                                           Data Access Library (DAL)

build_package (dal src/DAL)

add_dependencies (dal casacore pil)

if (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal num_util test-data)
  else(TEST_DATASETS)
    add_dependencies (dal num_util)
  endif (TEST_DATASETS)
else (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal test-data)
  endif (TEST_DATASETS)
endif (DAL_PYTHON_BINDINGS)

##____________________________________________________________________
##                                                            CR-Tools

add_custom_target (cr
  COMMAND ./build_package.sh cr ${USG_ROOT}/src/CR-Tools -DCR_WITH_GUI:BOOL=${CR_WITH_GUI}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package CR-Tools ..."
  )

add_dependencies (cr dal plplot fftw3 gsl)

if (CR_WITH_GUI)
  add_dependencies (cr mathgl)
endif (CR_WITH_GUI)

if (HAVE_ROOT)
  add_dependencies (cr startools)
endif (HAVE_ROOT)

if (CR_WITH_GUI)
  add_dependencies (cr pyqt)
endif (CR_WITH_GUI)

##____________________________________________________________________
##                                       LASA (LOFAR Air-Shower Array)

build_package (lasa src/LASA)

##____________________________________________________________________
##                                                              Pulsar

build_package (pulsar src/Pulsar)

add_dependencies (pulsar cfitsio)

##____________________________________________________________________
##                                                                  RM

add_custom_target (rm
  COMMAND ./build_package.sh rm ${USG_ROOT}/src/RM -DRM_ENABLE_ARMADILLO=${RM_ENABLE_ARMADILLO} -DRM_ENABLE_ITPP=${RM_ENABLE_ITPP}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package RM ..."
  )

add_dependencies (rm dal)

if (RM_ENABLE_ARMADILLO)
  add_dependencies (rm armadillo)
endif (RM_ENABLE_ARMADILLO)

if (RM_ENABLE_ITPP)
  add_dependencies (rm itpp)
endif (RM_ENABLE_ITPP)

##____________________________________________________________________
##                                                             smosaic

build_package (smosaic src/smosaic)

add_dependencies (smosaic casacore cfitsio wcslib)

##____________________________________________________________________
##                                                             contrib

build_package (contrib src/contrib)

add_dependencies (contrib dal)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

if (LUS_VERBOSE_CONFIGURE)
  message (STATUS "------------------------------------------------------------")
  message (STATUS "LOFARSOFT          = $ENV{LOFARSOFT}")
  message (STATUS "USG_ROOT           = ${USG_ROOT}")
  message (STATUS "USG_INSTALL_PREFIX = ${USG_INSTALL_PREFIX}")
  message (STATUS "USG_PYTHONPATH     = ${USG_PYTHONPATH}")
  message (STATUS "USG_DATA           = ${USG_DATA}")
  message (STATUS "------------------------------------------------------------")
endif (LUS_VERBOSE_CONFIGURE)
