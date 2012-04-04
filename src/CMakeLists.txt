
include (ExternalProject)

## ==============================================================================
##
##  Common instructions for packages
##
## ==============================================================================

##____________________________________________________________________
##                                                Macro: build_package

macro (build_package _packageName _packageSource)

  ## Locate common options

  find_file (LUS_CMAKE_OPTIONS CMakeOptions.cmake
    PATHS ${PROJECT_BINARY_DIR}
    )

  ## get components of the path to the package's source code
  get_filename_component (_packageLocation ${_packageSource} PATH)

  ## feedback
  set (_packageBuildLocation ${PROJECT_BINARY_DIR}/${_packageName})
  
  ## create the directory within which the build is performed
  file (MAKE_DIRECTORY ${_packageBuildLocation})
  
  if (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_SOURCE_DIR}/${_packageSource} -C${LUS_CMAKE_OPTIONS} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      COMMAND ${CMAKE_BUILD_TOOL} install
      WORKING_DIRECTORY ${_packageBuildLocation}
      COMMENT "Building package ${package_name} ..."
      )
  else (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_SOURCE_DIR}/${_packageSource} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      COMMAND ${CMAKE_BUILD_TOOL} install
      WORKING_DIRECTORY ${_packageBuildLocation}
      COMMENT "Building package ${package_name} ..."
      )
  endif (LUS_CMAKE_OPTIONS)
  
endmacro (build_package _packageName _packageSource)

## Create shell script to handle package builds

configure_file (
  ${PROJECT_SOURCE_DIR}/devel_common/scripts/build_package.sh.in
  ${PROJECT_BINARY_DIR}/build_package.sh
  )

##____________________________________________________________________
##                                            Macro: lus_package_build
## Parameters:
##  _name : Name of the package as available as build target.
##  _dir  : Name of the directory containing the source code.

macro (lus_package_build _name _dir)

  ## Check for build directory
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
  endif (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})

  ## Trigger build and install
  if (LUS_CMAKE_OPTIONS)
    add_custom_target (${_name}
      COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${_dir} -C${LUS_CMAKE_OPTIONS} -DLUS_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND ${CMAKE_BUILD_TOOL} install
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir}
      )
  else (LUS_CMAKE_OPTIONS)
    add_custom_target (${_name}
      COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${_dir} -DLUS_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND ${CMAKE_BUILD_TOOL} install
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir}
      )
  endif (LUS_CMAKE_OPTIONS)

endmacro (lus_package_build)

##____________________________________________________________________
##                                     Macro: lus_package_experimental
## Parameters:
##  _name : Name of the package as available as biuld target.
##  _dir  : Name of the directory containing the source code.

macro (lus_package_experimental _name _dir)

  string (TOUPPER ${_name} varName)

  ## Check for build directory
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
  endif (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})

  ## Trigger experimental build
  add_custom_target (${_name}_Experimental
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${_dir} -D${varName}_ENABLE_TESTING=TRUE
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir}
    )

endmacro (lus_package_experimental)

## ==============================================================================
##
##                                                        $LOFARSOFT/src/Anaamika
##
## ==============================================================================

##__________________________________________________________
##                                                   Options

option (ANAAMIKA_ENABLE_SCIPY   "Build SciPy as interactive environment?"  NO  )
option (ANAAMIKA_ENABLE_FBDSM   "Build Fortran-based version of BDSM?"     NO  )
option (ANAAMIKA_ENABLE_PYBDSM  "Build Python-enabled version of BDSM?"    YES )

##__________________________________________________________
##                              Build & Testing instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)
  ## Build package from present sources
  lus_package_build (anaamika Anaamika)
  ## Experimental build
  lus_package_experimental (anaamika Anaamika)
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)
  ExternalProject_Add (anaamika
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Anaamika
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/Anaamika
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DANAAMIKA_ENABLE_SCIPY=${ANAAMIKA_ENABLE_SCIPY}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)

##__________________________________________________________
##                                      Package dependencies

add_dependencies (anaamika wcslib cfitsio boost num_util pyfits)

if (ANAAMIKA_ENABLE_SCIPY)
  add_dependencies (anaamika scipy)
endif (ANAAMIKA_ENABLE_SCIPY)

## ==============================================================================
##
##                                                         $LOFARSOFT/src/contrib
##
## ==============================================================================

##__________________________________________________________
##                              Build & Testing instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  ## Build package from present sources
  lus_package_build (contrib contrib)
  ## Experimental build
  lus_package_experimental (contrib contrib)
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  ExternalProject_Add (contrib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/contrib
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

##__________________________________________________________
##                                      Package dependencies

add_dependencies (contrib dal1)

##__________________________________________________________
##                                                   Testing

if (EXISTS ${LUS_BINARY_DIR}/src/contrib/src/contrib-build)

  add_test (
    NAME contrib_Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/contrib/src/contrib-build
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    )

endif (EXISTS ${LUS_BINARY_DIR}/src/contrib/src/contrib-build)

## ==============================================================================
##
##                                                        $LOFARSOFT/src/CR-Tools
##
## ==============================================================================

set (CR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CR-Tools)
set (CR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cr)

##__________________________________________________________
##                              Build & Testing instructions

if (IS_DIRECTORY ${CR_SOURCE_DIR})
  ## Build package from present sources
  lus_package_build (cr CR-Tools)
  ## Experimental build
  lus_package_experimental (cr CR-Tools)
else (IS_DIRECTORY ${CR_SOURCE_DIR})
  ExternalProject_Add (cr
    PREFIX ${CR_BINARY_DIR}
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/CR-Tools
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCR_WITH_STARTOOLS=${CR_WITH_STARTOOLS} -DCR_WITH_GUI=${CR_WITH_GUI}
    )
endif (IS_DIRECTORY ${CR_SOURCE_DIR})

##__________________________________________________________
##                                      Package dependencies

add_dependencies (cr plplot fftw3 gsl)

if (CR_WITH_DAL)
  add_dependencies (cr dal1)
endif (CR_WITH_DAL)

if (CR_WITH_GUI)
  add_dependencies (cr mathgl)
endif (CR_WITH_GUI)

if (CR_WITH_AERA)
  add_dependencies(cr aera)
endif (CR_WITH_AERA)

if (ROOT_FOUND AND CR_WITH_STARTOOLS)
  add_dependencies (cr startools)
endif (ROOT_FOUND AND CR_WITH_STARTOOLS)

if (CR_WITH_GUI)
  add_dependencies (cr pyqt)
endif (CR_WITH_GUI)

## ==============================================================================
##
##                                                            $LOFARSOFT/src/LASA
##
## ==============================================================================

set (LASA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/LASA)
set (LASA_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/lasa)

if (IS_DIRECTORY ${LASA_SOURCE_DIR})
  ## Build package from present sources
  lus_package_build (lasa LASA)
  ## Experimental build
  lus_package_experimental (lasa LASA)
else (IS_DIRECTORY ${LASA_SOURCE_DIR})
  ExternalProject_Add (lasa
    PREFIX ${LASA_BINARY_DIR}
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/LASA
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
endif (IS_DIRECTORY ${LASA_SOURCE_DIR})

## ==============================================================================
##
##                                                          $LOFARSOFT/src/Pulsar
##
##  Due to its internal organization (or lack thereof), the Pulsar-Tools bundle
##  cannot be build and installed by calling a single target; therefore in
##  order to hide this internal setup from the outside world and present a
##  standard command sequence within the LUS framework, the internal build steps
##  are collected into a common custom target.
##
## ==============================================================================

set (PULSAR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Pulsar)
set (PULSAR_BINARY_DIR ${LUS_BINARY_DIR}/pulsar)

##__________________________________________________________
##                              Build & Testing instructions

if (IS_DIRECTORY ${PULSAR_SOURCE_DIR})
  ## Check for build directory
  if (NOT EXISTS ${PULSAR_BINARY_DIR})
    file (MAKE_DIRECTORY ${PULSAR_BINARY_DIR})
  endif (NOT EXISTS ${PULSAR_BINARY_DIR})
  ## Trigger build and install
  add_custom_target (pulsar
    COMMAND ${CMAKE_COMMAND} ${PULSAR_SOURCE_DIR}
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} install
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} psrchive_install
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} ppgplot_bindings_install
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} presto_makewisdom
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} dspsr_install
    COMMAND date
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}
    )
  ## Experimental build
  add_custom_target (pulsar_Experimental
    COMMAND date
    COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} ${PULSAR_SOURCE_DIR}
    COMMAND date
    COMMAND ${CMAKE_BUILD_TOOL} Experimental
    COMMAND date
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}
    )
else (IS_DIRECTORY ${PULSAR_SOURCE_DIR})
  ExternalProject_Add (pulsar
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Pulsar
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/Pulsar
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
endif (IS_DIRECTORY ${PULSAR_SOURCE_DIR})

##__________________________________________________________
##                                      Package dependencies

add_dependencies (pulsar cfitsio wcstools swig)

## ==============================================================================
##
##                                                       $LOFARSOFT/src/FRATS
##
## ==============================================================================

##__________________________________________________________
##                              Build & Testing instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FRATS)
  ## Build package from present sources
  build_package (frats src/FRATS)
  ## Experimental build
  lus_package_experimental (frats FRATS)
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/FRATS)

##__________________________________________________________
##                                      Package dependencies

#add_dependencies (frats casacore)

## ==============================================================================
##
##                                                       $LOFARSOFT/src/PyCRTools
##
## ==============================================================================

##__________________________________________________________
##                              Build & Testing instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/PyCRTools)
  ## Build package from present sources
  build_package (pycrtools src/PyCRTools)
  ## Experimental build
  lus_package_experimental (pycrtools PyCRTools)
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/PyCRTools)

##__________________________________________________________
##                                      Package dependencies

add_dependencies (pycrtools dal1 casacore fftw3 gsl tmf num_util)

if (PYCRTOOLS_WITH_AERA)
  add_dependencies (pycrtools aera)
endif (PYCRTOOLS_WITH_AERA)

## ==============================================================================
##
##                                                              $LOFARSOFT/src/RM
##
## ==============================================================================

set (RM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RM)
set (RM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/RM)

##__________________________________________________________
## Options

option (RM_WITH_ITPP       "Enable using IT++ library?"       NO  )
option (RM_WITH_ARMADILLO  "Enable using Armadillo library?"  YES )

##__________________________________________________________
##                              Build & Testing instructions

## Create configuration file for initial CMake cache
configure_file (
  ${RM_SOURCE_DIR}/rm.cmake.in
  ${RM_BINARY_DIR}/rm.cmake
  )

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ## Build package from present sources
  lus_package_build (rm RM)
  ## Experimental build
  lus_package_experimental (rm RM)
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/RM
    CMAKE_ARGS -C ${RM_BINARY_DIR}/rm.cmake
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)

##__________________________________________________________
## Package dependencies

add_dependencies (rm wcslib cfitsio dal1 casacore)

if (RM_WITH_ARMADILLO)
  add_dependencies (rm armadillo)
endif (RM_WITH_ARMADILLO)

if (RM_WITH_ITPP)
  add_dependencies (rm itpp)
endif (RM_WITH_ITPP)
