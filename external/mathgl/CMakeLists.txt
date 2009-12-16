
## Name of the project
project (MATHGL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATHGL_FORCE_BUILD       "Force rebuild and local installation?"  YES )
option (MATHGL_SVN_BUILD         "Build from the recent SVN version? "    NO  )
option (MATHGL_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (MATHGL_ENABLE_HDF5       "Enable support for HDF5?"               NO  )
option (MATHGL_ENABLE_PYTHON     "Enable support for Python?"             YES )
option (MATHGL_ENABLE_QT         "Enable support for Qt?"                 YES )
option (MATHGL_ENABLE_DOUBLE     "Enable double precision?"               YES )
option (MATHGL_DOWNLOAD_ARCHIVE  "Download archive of latest version?"    NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${MATHGL_SOURCE_DIR}
  ${MATHGL_SOURCE_DIR}/..
  ${MATHGL_SOURCE_DIR}/../..
  ${MATHGL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[MathGL] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindMathGL)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)

  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Tools and definitions

  list (APPEND bin_locations /sw/lib/qt4-mac/bin)
  list (APPEND bin_locations /sw/lib/qt4-x11/bin)

  find_program (tar_executable    tar    ${bin_locations} )
  find_program (ln_executable     ln     ${bin_locations} )
  find_program (qmake_executable  qmake  ${bin_locations} )

  set (MATHGL_TAR_ARCHIVE "mathgl.tgz")
  set (MATHGL_URL "${USG_DOWNLOAD}/software/${MATHGL_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## External libraries
  
  if (NOT MATHGL_VERBOSE_CONFIGURE)
    set (GSL_FIND_QUIETLY    YES )
    set (HDF5_FIND_QUIETLY   YES )
    set (PYTHON_FIND_QUIETLY YES )
    set (QT_FIND_QUIETLY     YES )
  endif (NOT MATHGL_VERBOSE_CONFIGURE)

  set (DESIRED_QT_VERSION 4)

  include (FindGSL)
  include (FindHDF5)
  include (FindPython)
  include (FindOpenGL)
  include (FindQt)

  ## Python bindings
  
  if (HAVE_PYTHON)
    
    find_path (NUMPY_INCLUDES numpy/arrayobject.h
      PATHS ${include_locations} /usr/lib /sw/lib
      PATH_SUFFIXES
      python2.6/site-packages/numpy/core/include
      python2.5/site-packages/numpy/core/include
      python2.4/site-packages/numpy/core/include
      )
    
    if (NOT NUMPY_INCLUDES)
      message (STATUS "[MathGL] Python bindings disabled - no Numpy found!")
      set (MATHGL_ENABLE_PYTHON NO)
    endif (NOT NUMPY_INCLUDES)
    
  endif (HAVE_PYTHON)

  ##__________________________________________________________________
  ## Configure and build the project using CMake

  find_file (have_mathgl_tgz 
    mathgl-1.10.tgz
    mathgl-1.9.tgz
    mathgl.tgz
    PATHS ${MATHGL_SOURCE_DIR}
    )

  ## Create configuration files from templates

  configure_file (
    ${MATHGL_SOURCE_DIR}/mathgl.cmake.in
    ${MATHGL_BINARY_DIR}/mathgl.cmake
    )

  configure_file (
    ${MATHGL_SOURCE_DIR}/setup.py.in
    ${MATHGL_BINARY_DIR}/setup.py
    )

  ## have CMake configure the project
  
  if (MATHGL_SVN_BUILD)
    ExternalProject_Add (mathgl
      PREFIX ${MATHGL_BINARY_DIR}
      DOWNLOAD_DIR ${MATHGL_BINARY_DIR}
      SOURCE_DIR ${MATHGL_BINARY_DIR}
      BINARY_DIR ${MATHGL_BINARY_DIR}/build
      SVN_REPOSITORY http://ufp.appl.sci-nnov.ru/svn/mathgl/trunk
      CMAKE_ARGS -C ${MATHGL_BINARY_DIR}/mathgl.cmake
      )
  else (MATHGL_SVN_BUILD)
    if (have_mathgl_tgz) 
      ExternalProject_Add (mathgl
	PREFIX ${MATHGL_BINARY_DIR}
	DOWNLOAD_DIR ${MATHGL_BINARY_DIR}
	SOURCE_DIR ${MATHGL_BINARY_DIR}
	BINARY_DIR ${MATHGL_BINARY_DIR}/build
	URL ${have_mathgl_tgz}
	CMAKE_ARGS -C ${MATHGL_BINARY_DIR}/mathgl.cmake
	)
    else (have_mathgl_tgz)
      message (WARNING "[MthGL] Unable to build package - missing sources!")
    endif (have_mathgl_tgz)
  endif (MATHGL_SVN_BUILD)

    ## Post-installation patch of include paths
    ExternalProject_Add_Step (mathgl mathgl_links
      COMMAND rm -f gsl
      COMMAND ${ln_executable} -s ${GSL_INCLUDES}/gsl .
      COMMAND rm -f numpy
      COMMAND ${ln_executable} -s ${NUMPY_INCLUDES}/numpy .
      COMMENT "Creating symbolic links for include directories"
      DEPENDEES patch
      DEPENDERS configure
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/include
      )
  
  ##__________________________________________________________________
  ## Configuration summary

  message ("-------------------------------------------------------------")
  message (STATUS "[MathGL] Summary of configuration status")
  message (STATUS "Forced rebuild from source  = ${MATHGL_FORCE_BUILD}")
  message (STATUS "Enable support for Python   = ${MATHGL_ENABLE_PYTHON}")
  message (STATUS "Enable support for Qt       = ${MATHGL_ENABLE_QT}")
  message (STATUS "GSL_INCLUDES                = ${GSL_INCLUDES}")
  message (STATUS "HDF5_INCLUDES               = ${HDF5_INCLUDES}")
  message (STATUS "NUMPY_INCLUDES              = ${NUMPY_INCLUDES}")
  message (STATUS "DESIRED_QT_VERSION          = ${DESIRED_QT_VERSION}")
  message (STATUS "QT3_INSTALLED               = ${QT3_INSTALLED}")
  message (STATUS "QT4_INSTALLED               = ${QT4_INSTALLED}")
  message ("-------------------------------------------------------------")

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  add_custom_target (mathgl
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    COMMENT "[MathGL] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[MathGL] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
