
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
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
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

option (SEND_REPORT "Send a report of the build to the Dashboard server?" NO )

## ==============================================================================
##
##  Cleaning up
##
## ==============================================================================

add_custom_target (clean
  COMMAND rm -rf *~
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles
  WORKING_DIRECTORY ${USG_BUILD}
)

## ==============================================================================
##
##  External packages
##
## ==============================================================================

set (packages_external
  bison
  blitz 
  boost
  cfitsio
  flex
  qt
  root
  sip
  szip
  wcstools
  )

foreach (package_name ${packages_external})
  add_custom_target (${package_name}
    COMMAND ./build_package.sh ${package_name} ${USG_EXTERNAL}/${package_name}
    WORKING_DIRECTORY ${USG_BUILD}
    )
endforeach (package_name)

add_custom_target (casacore
  COMMAND ./build.sh casacore
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (casacore hdf5 cfitsio boost wcslib)

add_custom_target (hdf5
  COMMAND ./build.sh hdf5
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (hdf5 szip)

add_custom_target (startools
  COMMAND ./build.sh startools
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (startools root)

add_custom_target (wcslib
  COMMAND ./build.sh wcslib
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (wcslib boost)

## ==============================================================================
##
##  USG packages
##
## ==============================================================================

##____________________________________________________________________
##                                                            CR-Tools

add_custom_target (cr
  COMMAND ./build_package.sh cr src/CR-Tools
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (cr dal startools mathgl)

##____________________________________________________________________
##                                           Data Access Library (DAL)

add_custom_target (dal
  COMMAND ./build_package.sh dal src/DAL
  WORKING_DIRECTORY ${USG_BUILD}
  )
add_dependencies (dal casacore hdf5 cfitsio boost)

