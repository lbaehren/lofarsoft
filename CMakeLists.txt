
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1620 2008-06-06 09:26:35Z baehren                       $
##-------------------------------------------------------------------------------


## Name of the project handled by CMake
project (LUS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

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
  include (GetCasacoreData)
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

option (SEND_REPORT "Send a report of the build to the Dashboard server?" NO  )
option (POST_BUILD_CLEAN "Clean up the build directory after a build?"    NO  )

## ==============================================================================
##
##  Cleaning up
##
## ==============================================================================

add_custom_target (cleanup
  COMMAND rm -rf *~
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles
  WORKING_DIRECTORY ${USG_BUILD}
)

add_custom_target (update
  COMMAND svn up
  WORKING_DIRECTORY ${USG_ROOT}
  )

add_custom_target (doc
  COMMAND doxygen
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  )

add_custom_target (doc_clean
  COMMAND rm -f *~
  COMMAND rm -f html/formula*
  COMMAND rm -f html/tab*
  COMMAND rm -f html/search.*
  COMMAND rm -f html/*.png
  COMMAND rm -f html/*.map
  COMMAND rm -f html/*.html
  COMMAND rm -f html/*.md5
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  )

add_custom_target (doc_cmake
  COMMAND cmake --help-html CMake.html
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  )


## ==============================================================================
##
##  External packages
##
## ==============================================================================

##____________________________________________________________________
## generic build instructions for the packages

set (packages_external
  bison
  blitz 
  boost
  casacore
  cfitsio
  flex
  hdf5
  mathgl
  pyrap
  python
  qt
  root
  sip
  startools
  szip
  wcslib
  wcstools
  wget
  zlib
  )

foreach (package_name ${packages_external})
  add_custom_target (${package_name}
    COMMAND ./build_package.sh ${package_name} ${USG_EXTERNAL}/${package_name}
    WORKING_DIRECTORY ${USG_BUILD}
    COMMENT "Building package ${package_name} ..."
    )
endforeach (package_name)

##____________________________________________________________________
##                                        Dependencies of the packages

add_dependencies (casacore hdf5 cfitsio boost wcslib bison)

add_dependencies (hdf5 szip)

add_dependencies (mathgl hdf5)

add_dependencies (pyrap casacore python)

add_dependencies (python boost)

add_dependencies (startools root)

add_dependencies (wcslib boost flex)

##____________________________________________________________________
## Packages needing extra attention

add_custom_target (plplot
  COMMAND mkdir -p ${USG_BUILD}/plplot 
  COMMAND cd plplot && cmake -C ${USG_ROOT}/devel_common/cmake/SettingsPLplot.cmake ${USG_ROOT}/external/plplot
  COMMAND cd ${USG_BUILD} && ./build_package.sh plplot ${USG_EXTERNAL}/plplot
  WORKING_DIRECTORY ${USG_BUILD}
  )

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
  COMMAND ./build_package.sh dal ${USG_ROOT}/src/DAL
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package DAL ..."
  )
add_dependencies (dal python plplot casacore)

##____________________________________________________________________
##                                                            CR-Tools

add_custom_target (cr
  COMMAND ./build_package.sh cr ${USG_ROOT}/src/CR-Tools
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package CR-Tools ..."
  )
add_dependencies (cr dal startools mathgl)

##____________________________________________________________________
##                                                                  RM

add_custom_target (rm
  COMMAND ./build_package.sh rm ${USG_ROOT}/src/RM
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package RM ..."
  )
add_dependencies (rm dal)

##____________________________________________________________________
##                                                             contrib

add_custom_target (contrib
  COMMAND ./build_package.sh contrib ${USG_ROOT}/src/contrib
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (contrib dal)

