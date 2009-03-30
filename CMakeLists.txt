
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1620 2008-06-06 09:26:35Z baehren                       $
##-------------------------------------------------------------------------------

## Name of the project handled by CMake
project (LUS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## Enforced CMake policy 
cmake_policy (VERSION 2.6)

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

option (SEND_BUILD_REPORT   "Send a build report to the Dashboard server?"  NO  )
option (POST_BUILD_CLEAN    "Clean up the build directory after a build?"   NO  )
option (MULTITHREADED_BUILD "Enable multithreading during build?"           NO  )
option (SYSTEM_PACKAGES     "Installation of required system packages?"     YES )

## Options affecting the build of individual packages

option (CR_WITH_GUI         "Build CR-Tools with graphical interface?"   NO  )
option (CR_WITH_STARTOOLS   "Enable using routines from LOPES-Star?"     NO  )
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
find_program (doxygen_executable doxygen PATHS ${bin_locations})
find_program (fink_executable    fink    PATHS ${bin_locations})
find_program (flex_executable    flex    PATHS ${bin_locations})
find_program (tar_executable     tar     PATHS ${bin_locations})
find_program (wget_executable    wget    PATHS ${bin_locations})

## ==============================================================================
##
##  Installation of required system packages
##
## ==============================================================================

if (SYSTEM_PACKAGES)
  
  ## Mute the output of the find scripts
  set (FFTW3_FIND_QUIETLY     YES )
  set (GFORTRAN_FIND_QUIETLY  YES )
  set (GSL_FIND_QUIETLY       YES )
  set (HDF5_FIND_QUIETLY      YES )
  
  include (FindFFTW3)
  include (FindGFortran)
  include (FindGSL)
  include (FindHDF5)

  if (APPLE)
    
    if (fink_executable)

      ##______________________________________________________________
      ## Set up the list of Fink packages

      set (fink_packages
	atlas
	bison
	flex
	fftw3
	fftw3-shlibs
	gfortran-shlibs
	numpy-py26
	qt4-mac
	sip-py26
	pyqt4-py26
	)

      if (NOT doxygen_executable)
	list (APPEND fink_packages doxygen)
      endif (NOT doxygen_executable)

      if (NOT HAVE_GSL)
	list (APPEND fink_packages gsl)
	list (APPEND fink_packages gsl-shlibs)
      endif (NOT HAVE_GSL)

      if (NOT SWIG_FOUND)
	list (APPEND fink_packages swig)
      endif (NOT SWIG_FOUND)

      ##______________________________________________________________
      ## Define installation targets for the packages

      foreach (fink_package ${fink_packages})
	## define target for individual Fink package
	add_custom_target (fink-${fink_package}
	  COMMAND ${fink_executable} install ${fink_package}
	  WORKING_DIRECTORY ${USG_BUILD}
	  COMMENT "Installating Fink package ${fink_package} ..."
	  )
	## target dependencies
	list (APPEND fink-packages-depends fink-${fink_package})
      endforeach (fink_package)

      ## Master target to install all of the above Fink packages
      add_custom_target (fink-packages
	COMMAND ${fink_executable} remove hdf5 hdf5-shlibs
	WORKING_DIRECTORY ${USG_BUILD}
	COMMENT "Installating Fink packages ..."
	)
      add_dependencies (fink-packages ${fink-packages-depends})
      
    else (fink_executable)
      message (STATUS "Unable to install Fink packages!")
    endif (fink_executable)
    
  endif (APPLE)
  
endif (SYSTEM_PACKAGES)

## ==============================================================================
##
##  Documentation generation
##
## ==============================================================================

if (doxygen_executable)
  add_custom_target (doc
    COMMAND ${doxygen_executable}
    WORKING_DIRECTORY ${USG_ROOT}/doc  
    COMMENT "Creating Doxygen documentation for source code ..."
    )
else (doxygen_executable)
  message (STATUS "Disabled generation of source documentation - missing Doxygen!")
endif (doxygen_executable)

add_custom_target (doc-cmake
  COMMAND cmake --help-html CMake.html
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  COMMENT "Creating browsable documentation for CMake ..."
  )

add_custom_target (doc-ctest
  COMMAND ctest --help-html CTest.html
  WORKING_DIRECTORY ${USG_ROOT}/doc  
  COMMENT "Creating browsable documentation for CTest ..."
  )

## ==============================================================================
##
##  Cleaning up
##
## ==============================================================================

add_custom_target (clean-build
  COMMAND rm -rf *~
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeFiles
  COMMAND rm -rf bison blitz boost
  COMMAND rm -rf casacore cfitsio cr
  COMMAND rm -rf dal flex hdf5 mathgl
  COMMAND rm -rf plplot python root
  COMMAND rm -rf startools swig szip
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

add_custom_target (update
  COMMAND svn up
  WORKING_DIRECTORY ${USG_ROOT}
  COMMENT "Updating working version of the source code from repository ..."
  )

## ==============================================================================
##
##  External packages
##
## ==============================================================================

set (packages_external
  blitz 
  boost
  casacore
  cfitsio
  hdf5
  mathgl
  plplot
  pyrap
  python
  qt
  root
  sip
  startools
  szip
  swig
  wcslib
  wcstools
  zlib
  )

if (NOT bison_executable)
  list (APPEND packages_external bison)
endif (NOT bison_executable)

if (NOT flex_executable)
  list (APPEND packages_external flex)
endif (NOT flex_executable)

if (NOT wget_executable)
  list (APPEND packages_external wget)
endif (NOT wget_executable)

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

add_dependencies (casacore hdf5 cfitsio boost wcslib bison)

add_dependencies (hdf5 szip)

add_dependencies (mathgl hdf5 python swig)

add_dependencies (pyrap casacore python)

add_dependencies (python boost)

add_dependencies (startools root)

add_dependencies (wcslib boost flex)

add_dependencies (plplot python swig)

##____________________________________________________________________
## Packages/Targets needing extra attention

add_custom_target (cmake-modules
  COMMAND mkdir -p cmake_modules
  COMMAND cd cmake_modules && cmake ${USG_ROOT}/devel_common/cmake
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Running test of CMake-modules ..."
  )

add_custom_target (test-data
  COMMAND wget -c http://usg.lofar.org/download/datasets/usg_data_test.tgz
  COMMAND tar -xvzf usg_data_test.tgz
  COMMAND rm -f usg_data_test.tgz
  WORKING_DIRECTORY ${USG_DATA}
  COMMENT "Retrieving test data-sets ..."
  )
add_dependencies (test-data wget)

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
  COMMAND ./build_package.sh dal ${USG_ROOT}/src/DAL -DDAL_PYTHON_BINDINGS:BOOL=${DAL_PYTHON_BINDINGS}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package DAL ..."
  )
add_dependencies (dal casacore)

##____________________________________________________________________
##                                                            CR-Tools

add_custom_target (cr
  COMMAND ./build_package.sh cr ${USG_ROOT}/src/CR-Tools -DCR_WITH_GUI:BOOL=${CR_WITH_GUI}
  WORKING_DIRECTORY ${USG_BUILD}
  COMMENT "Building package CR-Tools ..."
  )

if (CR_WITH_STARTOOLS)
  if (CR_WITH_GUI)
    add_dependencies (cr dal startools mathgl)
  else (CR_WITH_GUI)
    add_dependencies (cr dal startools)
  endif (CR_WITH_GUI)
else (CR_WITH_STARTOOLS)
  if (CR_WITH_GUI)
    add_dependencies (cr dal mathgl)
  else (CR_WITH_GUI)
    add_dependencies (cr dal)
  endif (CR_WITH_GUI)
endif (CR_WITH_STARTOOLS)

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

