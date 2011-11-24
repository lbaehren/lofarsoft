
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
  if (${_packageLocation} STREQUAL "external")
    message (STATUS "Adding external package ${_packageName} ...")
    set (_packageBuildLocation ${PROJECT_BINARY_DIR}/external/${_packageName})
  else (${_packageLocation} STREQUAL "external")
    set (_packageBuildLocation ${PROJECT_BINARY_DIR}/${_packageName})
  endif (${_packageLocation} STREQUAL "external")

  ## create the directory within which the build is performed
  file (MAKE_DIRECTORY ${_packageBuildLocation})

  if (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_ROOT}/${_packageSource} -C${LUS_CMAKE_OPTIONS} -DCMAKE_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND make install
      WORKING_DIRECTORY ${_packageBuildLocation}
      COMMENT "Building package ${package_name} ..."
      )
  else (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_ROOT}/${_packageSource} -DCMAKE_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND make install
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
##  _name : Name of the package as available as biuld target.
##  -dir  : Name of the directory containing the source code.

macro (lus_package_build _name _dir)

  ## Check for build directory
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
  endif (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})

  ## Trigger build and install
  add_custom_target (${_name}
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${_dir}
    COMMAND ${CMAKE_BUILD_TOOL} install
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir}
    )

endmacro (lus_package_build)

##____________________________________________________________________
##                                     Macro: lus_package_experimental
## Parameters:
##  _name : Name of the package as available as biuld target.
##  _dir  : Name of the directory containing the source code.

macro (lus_package_experimental _name _dir)

  ## Check for build directory
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir})
  endif (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_dir})

  ## Trigger experimental build
  add_custom_target (${_name}_Experimental
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${_dir}
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_dir}
    )

endmacro (lus_package_experimental)

## ==============================================================================
##
##                                                             $LOFARSOFT/src/DAL
##
## ==============================================================================

add_subdirectory (DAL)

add_custom_target (dal_depends DEPENDS casacore git)
add_dependencies (dal casacore git)

if (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal_depends num_util test-data)
    add_dependencies (dal num_util test-data)
  else(TEST_DATASETS)
    add_dependencies (dal_depends num_util)
    add_dependencies (dal num_util)
  endif (TEST_DATASETS)
else (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal_depends test-data)
    add_dependencies (dal test-data)
  endif (TEST_DATASETS)
endif (DAL_PYTHON_BINDINGS)

## ==============================================================================
##
##                                                        $LOFARSOFT/src/Anaamika
##
## ==============================================================================

##__________________________________________________________
## Options

option (ANAAMIKA_ENABLE_SCIPY   "Build SciPy as interactive environment?"  NO  )
option (ANAAMIKA_ENABLE_FBDSM   "Build Fortran-based version of BDSM?"     NO  )
option (ANAAMIKA_ENABLE_PYBDSM  "Build Python-enabled version of BDSM?"    YES )

##__________________________________________________________
##                                        Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)
  ExternalProject_Add (anaamika
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Anaamika
    URL ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DANAAMIKA_ENABLE_SCIPY=${ANAAMIKA_ENABLE_SCIPY}
    )
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

##__________________________________________________________
##                                                   Testing

if (EXISTS ${LUS_BINARY_DIR}/src/Anaamika/src/anaamika-build)

  ## Custom target to trigger test build
  add_custom_target (anaamika_Experimental
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/Anaamika/src/anaamika-build
    )

  add_test (
    NAME anaamika_Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/Anaamika/src/anaamika-build
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    )

endif (EXISTS ${LUS_BINARY_DIR}/src/Anaamika/src/anaamika-build)

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

add_dependencies (contrib dal)

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

build_package (cr src/CR-Tools)

add_dependencies (cr plplot fftw3 gsl)

if (CR_WITH_DAL)
  add_dependencies (cr dal)
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

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)
  ExternalProject_Add (lasa
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LASA
    URL ${CMAKE_CURRENT_SOURCE_DIR}/LASA
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)
  ExternalProject_Add (lasa
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LASA
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/LASA
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)

## ==============================================================================
##
##                                                          $LOFARSOFT/src/Pulsar
##
## ==============================================================================

build_package (pulsar src/Pulsar)

add_dependencies (pulsar cfitsio wcstools swig)

##__________________________________________________________
## Testing

if (EXISTS ${LUS_BINARY_DIR}/pulsar)

  ## Custom target to trigger test build
  add_custom_target (pulsar_Experimental
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/Pulsar
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/pulsar
    ) 
  
endif (EXISTS ${LUS_BINARY_DIR}/pulsar)

## ==============================================================================
##
##                                                       $LOFARSOFT/src/PyCRTools
##
## ==============================================================================

build_package (pycrtools src/PyCRTools)

add_dependencies (pycrtools dal casacore fftw3 gsl tmf num_util)
if (PYCRTOOLS_WITH_AERA)
  add_dependencies (pycrtools aera)
endif (PYCRTOOLS_WITH_AERA)

## ==============================================================================
##
##                                                              $LOFARSOFT/src/RM
##
## ==============================================================================

##__________________________________________________________
## Options

option (RM_WITH_ITPP       "Enable using IT++ library?"       NO  )
option (RM_WITH_ARMADILLO  "Enable using Armadillo library?"  YES )

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ## Build package from present sources
  lus_package_build (rm RM)
  ## Experimental build
  lus_package_experimental (rm RM)
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/RM
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DRM_WITH_ITPP=${RM_WITH_ITPP} -DRM_WITH_ARMADILLO=${RM_WITH_ARMADILLO}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)

##__________________________________________________________
## Package dependencies

add_dependencies (rm wcslib cfitsio dal casacore)

if (RM_WITH_ARMADILLO)
  add_dependencies (rm armadillo)
endif (RM_WITH_ARMADILLO)

if (RM_WITH_ITPP)
  add_dependencies (rm itpp)
endif (RM_WITH_ITPP)

##__________________________________________________________
## Testing

if (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)

  ## Custom target to trigger test build
  add_custom_target (rm_Experimental
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/RM/src/rm-build
    ) 

  add_test (
    NAME rm_Experimental
    WORKING_DIRECTORY ${LUS_BINARY_DIR}/src/RM/src/rm-build
    COMMAND ${CMAKE_CTEST_COMMAND} -D Experimental
    )

endif (EXISTS ${LUS_BINARY_DIR}/src/RM/src/rm-build)
