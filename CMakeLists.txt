
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

option (ENABLE_DASHBOARD    "Send a build report to the Dashboard server?"  YES )
option (POST_BUILD_CLEAN    "Clean up the build directory after a build?"   NO  )
option (MULTITHREADED_BUILD "Enable multithreading during build?"           NO  )
option (TEST_DATASETS       "Download and install sample test datasets?"    NO  )

## Options affecting the build of individual packages

option (CR_WITH_GUI         "Build CR-Tools with graphical interface?"   NO  )
option (CR_WITH_STARTOOLS   "Enable using routines from LOPES-Star?"     YES )
option (CR_WITH_FFTW3       "Require FFTW3 for CR-Tools?"                NO  )
option (DAL_PYTHON_BINDINGS "Create python bindings for DAL?"            YES )

## ==============================================================================
##
##  Basic system inspection:
##
##  - Look for essential system tools, which are required; some of the targets
##    only will be activated if the necessary tool is available.
##
## ==============================================================================

find_program (bison_executable   bison   PATHS ${bin_locations})
find_program (curl_executable    curl    PATHS ${bin_locations})
find_program (doxygen_executable doxygen PATHS ${bin_locations})
find_program (flex_executable    flex    PATHS ${bin_locations})
find_program (svn_executable     svn     PATHS ${bin_locations})
find_program (tar_executable     tar     PATHS ${bin_locations})
find_program (wget_executable    wget    PATHS ${bin_locations})

## ==============================================================================
##
##  Documentation generation
##
## ==============================================================================

if (doxygen_executable)
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
  COMMAND rm -rf external
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles
  COMMAND rm -rf casacore cfitsio cr
  COMMAND rm -rf dal flex
  COMMAND rm -rf numeric numpy num_util
  COMMAND rm -rf plplot root
  COMMAND rm -rf startools szip
  COMMAND rm -rf wcslib wcstools
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
  COMMAND rm -rf lib
  COMMAND rm -rf share
  WORKING_DIRECTORY ${USG_ROOT}/release
  COMMENT "Cleaning up release directory ..."
)

add_custom_target (clean-src
  COMMAND rm -rf *~ */*~ */*/*~ */*/*/*~
  WORKING_DIRECTORY ${USG_ROOT}/src
  COMMENT "Cleaning up build directory ..."
)

if (svn_executable)
  add_custom_target (update
    COMMAND svn up
    COMMAND cd ${USG_ROOT}/build && ./bootstrap
    WORKING_DIRECTORY ${USG_ROOT}
    COMMENT "Updating working version of the source code from repository ..."
    )
else (svn_executable)
  message (STATUS "Disabling update of working copy -- missing Subversion!")
endif (svn_executable)

## ==============================================================================
##
##  External packages
##
## ==============================================================================

## Packages with build handled through shell script

set (packages_external
  blas
  casacore
  cfitsio
  numpy
  num_util
  pyrap
  root
  sip
  szip
  wcslib
  wcstools
  )

if (NOT flex_executable)
  list (APPEND packages_external flex)
endif (NOT flex_executable)

## Packages handled directly through CMake

add_subdirectory (${USG_ROOT}/external)
add_subdirectory (${USG_ROOT}/devel_common/scripts)

##____________________________________________________________________
## generic build instructions for the packages

foreach (package_name ${packages_external})
  add_custom_target (${package_name}
    COMMAND mkdir -p ${package_name}
    COMMAND ./build_package.sh ${package_name} ${USG_EXTERNAL}/${package_name}
    WORKING_DIRECTORY ${USG_BUILD}
    COMMENT "Building package ${package_name} ..."
    )
endforeach (package_name)

##____________________________________________________________________
##                                        Dependencies of the packages

add_dependencies (casacore hdf5 cfitsio wcslib bison)

add_dependencies (hdf5 szip)

add_dependencies (mathgl hdf5 swig num_util)

add_dependencies (matplotlib numpy)

add_dependencies (num_util boost numpy)

add_dependencies (pyrap casacore num_util)

#add_dependencies (startools root)

add_dependencies (wcslib boost flex)

add_dependencies (plplot numpy swig)

##____________________________________________________________________
## Packages/Targets needing extra attention

add_custom_target (cmake-modules
  COMMAND mkdir -p cmake_modules
  COMMAND cd cmake_modules && cmake ${USG_ROOT}/devel_common/cmake
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Running test of CMake-modules ..."
  )

if (wget_executable)
  add_custom_target (test-data
    COMMAND ${wget_executable} -c ${USG_DOWNLOAD}/datasets/usg_data_test.tgz
    COMMAND tar -xvzf usg_data_test.tgz
    COMMAND rm -f usg_data_test.tgz
    WORKING_DIRECTORY ${USG_DATA}
    COMMENT "Retrieving test data-sets ..."
    )
else (wget_executable)
  if (curl_executable)
    add_custom_target (test-data
      COMMAND ${curl_executable} ${USG_DOWNLOAD}/datasets/usg_data_test.tgz -o usg_data_test.tgz -C -
      COMMAND tar -xvzf usg_data_test.tgz
      COMMAND rm -f usg_data_test.tgz
      WORKING_DIRECTORY ${USG_DATA}
      COMMENT "Retrieving test data-sets ..."
      )
  else (curl_executable)
    message (STATUS "WARNING: Unable to retrieve sample datasets - missing wget and curl!")
  endif (curl_executable)
endif (wget_executable)

##____________________________________________________________________
## Update version of casacore

if (svn_executable AND tar_executable)
  add_custom_target (casacore-update
    COMMAND rm -rf casacore
    COMMAND ${svn_executable} co http://casacore.googlecode.com/svn/trunk casacore
    COMMAND ${tar_executable} -cvzf ${LUS_SOURCE_DIR}/external/casacore/casacore.tgz casacore
    COMMAND rm -rf casacore
    WORKING_DIRECTORY ${USG_BUILD}
  )
endif (svn_executable AND tar_executable)

## ==============================================================================
##
##  USG packages
##
## ==============================================================================

##____________________________________________________________________
##                                                                BDSM

add_custom_target (bdsm
  COMMAND ./build_package.sh bdsm ${USG_ROOT}/src/BDSM
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package BDSM ..."
  )
add_dependencies (bdsm cfitsio wcslib)

##____________________________________________________________________
##                                           Data Access Library (DAL)

add_custom_target (dal
  COMMAND ./build_package.sh dal ${USG_ROOT}/src/DAL -DDAL_PYTHON_BINDINGS:BOOL=${DAL_PYTHON_BINDINGS} -DDAL_ENABLE_DASHBOARD:BOOL=${ENABLE_DASHBOARD}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package DAL ..."
  )
if (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal casacore num_util test-data)
  else(TEST_DATASETS)
    add_dependencies (dal casacore num_util)
  endif (TEST_DATASETS)
else (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal casacore test-data)
  else(TEST_DATASETS)
    add_dependencies (dal casacore)
  endif (TEST_DATASETS)
endif (DAL_PYTHON_BINDINGS)

##____________________________________________________________________
##                                                            CR-Tools

add_custom_target (cr
  COMMAND ./build_package.sh cr ${USG_ROOT}/src/CR-Tools -DCR_WITH_GUI:BOOL=${CR_WITH_GUI}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package CR-Tools ..."
  )

add_dependencies (cr dal plplot)

if (CR_WITH_FFTW3)
  add_dependencies (cr fftw3)
endif (CR_WITH_FFTW3)

if (CR_WITH_GUI)
  add_dependencies (cr mathgl)
endif (CR_WITH_GUI)

if (CR_WITH_STARTOOLS)
  add_dependencies (cr startools)
endif (CR_WITH_STARTOOLS)

##____________________________________________________________________
##                                                              Pulsar

add_custom_target (pulsar
  COMMAND ./build_package.sh pulsar ${USG_ROOT}/src/Pulsar
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package Pulsar ..."
  )
add_dependencies (pulsar cfitsio)

##____________________________________________________________________
##                                                              pybdsm

add_custom_target (pybdsm
  COMMAND ./build_package.sh pybdsm ${USG_ROOT}/src/pybdsm
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package pybdsm ..."
  )
add_dependencies (pybdsm boost num_util wcslib)

##____________________________________________________________________
##                                                                  RM

add_custom_target (rm
  COMMAND ./build_package.sh rm ${USG_ROOT}/src/RM
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package RM ..."
  )
add_dependencies (rm itpp dal)

##____________________________________________________________________
##                                                             smosaic

add_custom_target (smosaic
  COMMAND ./build_package.sh smosaic ${USG_ROOT}/src/smosaic
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package smosaic ..."
  )
add_dependencies (smosaic casacore cfitsio wcslib)

##____________________________________________________________________
##                                                             contrib

add_custom_target (contrib
  COMMAND ./build_package.sh contrib ${USG_ROOT}/src/contrib
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building user-contributed software (programs, scripts, ...)"
  )
add_dependencies (contrib dal)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "------------------------------------------------------------")
message (STATUS "LOFARSOFT          = $ENV{LOFARSOFT}")
message (STATUS "USG_ROOT           = ${USG_ROOT}")
message (STATUS "USG_INSTALL_PREFIX = ${USG_INSTALL_PREFIX}")
message (STATUS "USG_PYTHONPATH     = ${USG_PYTHONPATH}")
message (STATUS "USG_DATA           = ${USG_DATA}")
message (STATUS "------------------------------------------------------------")

