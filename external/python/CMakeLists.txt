
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1393 2008-04-03 13:58:05Z baehren                       $
##-------------------------------------------------------------------------------

## Name of the project
project (PYTHON_MODULES)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (PYTHON_MODULES_VERBOSE_CONFIGURE "Be verbose during configuration?" NO )

option (NUMPY_FORCE_BUILD      "[NumPy] Force build from local sources?"      NO )
option (MATPLOTLIB_FORCE_BUILD "[Matplotlib] Force build from local sources?" NO )

## Select the modules to be build

option (enable-matplotlib  "Build matplotlib module?"  YES )
option (enable-num_util    "Build num_util module?"    YES )
option (enable-numpy       "Build numpy module?"       YES )
option (enable-numarray    "Build numarray module?"    YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PYTHON_MODULES_SOURCE_DIR}
  ${PYTHON_MODULES_SOURCE_DIR}/..
  ${PYTHON_MODULES_SOURCE_DIR}/../..
  ${PYTHON_MODULES_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

##__________________________________________________________________
## Locate the installation area

find_path (PYTHON_MODULES_prefix release_area.txt
  PATHS
  ${PYTHON_MODULES_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (NOT PYTHON_MODULES_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT PYTHON_MODULES_prefix)

##____________________________________________________________________
## CMake modules and system tools

if (NOT PYTHON_MODULES_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY       YES )
  set (BOOST_pythonOnly         YES )
  set (BOOST_test_exec_monitor  NO  )
  set (FFTW3_FIND_QUIETLY       YES )
  set (GFORTRAN_FIND_QUIETLY    YES )
  set (GLIB_FIND_QUIETLY        YES )
  set (MATPLOTLIB_FIND_QUIETLY  YES )
  set (NUMPY_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY      YES )
endif (NOT PYTHON_MODULES_VERBOSE_CONFIGURE)

include (FindBoost)
include (FindFFTW3)
include (FindGFortran)
include (FindGLIB)
include (FindMatplotlib)
include (FindNumPy)
include (FindPython)

find_program (HAVE_WGET  wget  ${bin_locations} )
find_program (HAVE_CURL  curl  ${bin_locations} )
find_program (HAVE_TAR   tar   ${bin_locations} )

## ==============================================================================
##
##  Check build conditions
##
## ==============================================================================

##____________________________________________________________________
## Name of build platform as determined by distutils

execute_process (
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import util;print util.get_platform()"
  WORKING_DIRECTORY ${PYTHON_MODULES_SOURCE_DIR}
  RESULT_VARIABLE platform_test_result
  OUTPUT_VARIABLE platform_test_output
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

## ==============================================================================
##
##  Build the individual packages
##
## ==============================================================================

##____________________________________________________________________
##                                                               NumPy

## Check on possible system-wide installation first; if it exists and
## is of a recent enough version, there is no need - unless forced -
## to build NumPy from the provided sources.

if (HAVE_NUMPY)

  set (NUMPY_REQUIRED_VERSION "1.1.1")
  
  if (NUMPY_API_VERSION LESS NUMPY_REQUIRED_VERSION)
    ## Warning message
    message (STATUS "Version of NumPy too old - need to rebuild!")
    ## Switch on build from local sources
    set (enable-numpy YES)
  else (NUMPY_API_VERSION LESS NUMPY_REQUIRED_VERSION)
    set (enable-numpy NO)
  endif (NUMPY_API_VERSION LESS NUMPY_REQUIRED_VERSION)

endif (HAVE_NUMPY)

if (NUMPY_FORCE_BUILD)
  set (enable-numpy YES)
endif (NUMPY_FORCE_BUILD)

if (enable-numpy)

  set (NUMPY_VERSION "1.3.0")
  set (NUMPY_DIRECTORY "numpy-${NUMPY_VERSION}")
  set (NUMPY_TAR_ARCHIVE "${NUMPY_DIRECTORY}.tar.gz")
  
  find_file (have_numpy_tgz ${NUMPY_TAR_ARCHIVE} ${PYTHON_MODULES_SOURCE_DIR})
  
  if (have_numpy_tgz)

    ## copy tar archive from source to build directory
    add_custom_target (numpy_copy
      COMMAND cp ${PYTHON_MODULES_SOURCE_DIR}/${NUMPY_TAR_ARCHIVE} .
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[NumPy] Expanding tar-archive ..."
      )

    ## expand the tar-archive
    add_custom_target (numpy_expand
      COMMAND ${HAVE_TAR} -xvzf ${NUMPY_TAR_ARCHIVE}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[NumPy] Expanding tar-archive ..."
      )
    add_dependencies (numpy_expand numpy_copy)
    
    ## build the package
    add_custom_target (numpy_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUMPY_DIRECTORY}
      COMMENT "[NumPy] Starting build ..."
      )
    add_dependencies (numpy_build numpy_expand)
    
    ## build and install the package
    add_custom_target (numpy_install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUMPY_TAR_ARCHIVE}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUMPY_DIRECTORY}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUMPY_DIRECTORY}
      COMMENT "[NumPy] Installing package ..."
      )
    add_dependencies (numpy_install numpy_build)
    
  else (have_numpy_tgz)
    message (STATUS "[NumPy] Unable to install - missing tar-archive!")
  endif (have_numpy_tgz)
  
endif (enable-numpy)

##____________________________________________________________________
##                                                            num_util

if (enable-num_util)

  set (NUM_UTIL_VERSION "2.0")
  set (NUM_UTIL_DIRECTORY "num_util-${NUM_UTIL_VERSION}")
  set (NUM_UTIL_TAR_ARCHIVE "${NUM_UTIL_DIRECTORY}.tar.gz")
  
  find_file (have_num_util_tgz ${NUM_UTIL_TAR_ARCHIVE} ${PYTHON_MODULES_SOURCE_DIR})
  
  if (have_num_util_tgz)
    
    ## copy tar archive from source to build directory
    add_custom_target (num_util_copy
      COMMAND cp ${PYTHON_MODULES_SOURCE_DIR}/${NUM_UTIL_TAR_ARCHIVE} .
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Num_util] Expanding tar-archive ..."
      )
    
    ## expand the tar-archive
    add_custom_target (num_util_expand
      COMMAND ${HAVE_TAR} -xvzf ${NUM_UTIL_TAR_ARCHIVE}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Num_util] Expanding tar-archive ..."
      )
    if (enable-numpy)
      add_dependencies (num_util_expand num_util_copy numpy_build)
    else (enable-numpy)
      add_dependencies (num_util_expand num_util_copy)
    endif (enable-numpy)
    
    ## configure the package
    add_custom_target (num_util_configure
      COMMAND rm -f makefile
      COMMAND cmake -C ${PYTHON_MODULES_BINARY_DIR}/python.cmake .
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUM_UTIL_DIRECTORY}
      COMMENT "[Num_util] Configuring package ..."
      )
    add_dependencies (num_util_configure num_util_expand)
    
    ## build the package
    add_custom_target (num_util_build
      COMMAND make
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUM_UTIL_DIRECTORY}
      COMMENT "[Num_util] Building package ..."
      )
    add_dependencies (num_util_build num_util_configure)
    
    ## build the package
    add_custom_target (num_util_install
      COMMAND make install
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUM_UTIL_DIRECTORY}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUM_UTIL_TAR_ARCHIVE}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUM_UTIL_DIRECTORY}
      COMMENT "[Num_util] Installting the package ..."
      )
    add_dependencies (num_util_install num_util_build)
    
  else (have_num_util_tgz)
    message (STATUS "[Num_util] Unable to install - missing tar-archive!")
    set (enable-num_util OFF)
  endif (have_num_util_tgz)
  
endif (enable-num_util)

##____________________________________________________________________
##                                                          matplotlib

#  matplotlib works with with Numeric or numarray.  At compile time,
#  setup.py will look for both packages and compile the appropriate
#  extensions into matplotlib.  At runtime, the correct extension code
#  will be chosen based on your numerix setting in matplotlibrc.  If
#  you want to be able to use either Numeric or numarray efficiently
#  with matplotlib, it is important that you have *both* present and in
#  your PYTHONPATH when you compile matplotlib.

if (MATPLOTLIB_LIBRARIES)
  message (STATUS "MATPLOTLIB_API_VERSION = ${MATPLOTLIB_API_VERSION}")
endif (MATPLOTLIB_LIBRARIES)

if (enable-matplotlib)
  
  set (MATPLOTLIB_VERSION "0.98.5.2")
  set (MATPLOTLIB_DIRECTORY "matplotlib-${MATPLOTLIB_VERSION}")
  set (MATPLOTLIB_TAR_ARCHIVE "${MATPLOTLIB_DIRECTORY}.tar.gz")

  find_file (have_matplotlib_tgz ${MATPLOTLIB_TAR_ARCHIVE} ${PYTHON_MODULES_SOURCE_DIR})

  if (have_matplotlib_tgz)

    ## copy tar archive from source to build directory
    add_custom_target (matplotlib_copy
      COMMAND cp ${PYTHON_MODULES_SOURCE_DIR}/${MATPLOTLIB_TAR_ARCHIVE} .
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Matplotlib] Expanding tar-archive ..."
      )

    ## expand the tar-archive
    add_custom_target (matplotlib_expand
      COMMAND ${HAVE_TAR} -xvzf ${MATPLOTLIB_TAR_ARCHIVE}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Matplotlib] Expanding tar-archive ..."
      )
    add_dependencies (matplotlib_expand matplotlib_copy)    

    ## build the package
    add_custom_target (matplotlib_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${MATPLOTLIB_DIRECTORY}
      COMMENT "[Matplotlib] Starting build ..."
      )
    if (enable-numpy)
      add_dependencies (matplotlib_build matplotlib_expand numpy_install)
    else (enable-numpy)
      add_dependencies (matplotlib_build matplotlib_expand)
    endif (enable-numpy)
    
    ## install the package
    add_custom_target (matplotlib_install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${MATPLOTLIB_TAR_ARCHIVE}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${MATPLOTLIB_DIRECTORY}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${MATPLOTLIB_DIRECTORY}
      COMMENT "[Matplotlib] Installing package ..."
      )
    add_dependencies (matplotlib_install matplotlib_build)
    
  else (have_matplotlib_tgz)
    message (STATUS "[Matplotlib] Unable to install - missing tar-archive!")
  endif (have_matplotlib_tgz)
  
endif (enable-matplotlib)

##____________________________________________________________________
##                                                            numarray

if (enable-numarray)
  
  set (NUMARRAY_VERSION "1.5.2")
  set (NUMARRAY_DIRECTORY "numarray-${NUMARRAY_VERSION}")
  set (NUMARRAY_TAR_ARCHIVE "${NUMARRAY_DIRECTORY}.tar.gz")
  
  find_file (have_numarray_tgz ${NUMARRAY_TAR_ARCHIVE} ${PYTHON_MODULES_SOURCE_DIR})

  if (have_numarray_tgz)
    
    ## copy tar archive from source to build directory
    add_custom_target (numarray_copy
      COMMAND cp ${PYTHON_MODULES_SOURCE_DIR}/${NUMARRAY_TAR_ARCHIVE} .
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Numarray] Expanding tar-archive ..."
      )
    
    ## expand the tar-archive
    add_custom_target (numarray_expand
      COMMAND ${HAVE_TAR} -xvzf ${NUMARRAY_TAR_ARCHIVE}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
      COMMENT "[Numarray] Expanding tar-archive ..."
      )
    add_dependencies (numarray_expand numarray_copy)    
  
    ## build the package
    add_custom_target (numarray_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUMARRAY_DIRECTORY}
      COMMENT "[Numarray] Starting build ..."
      )
    add_dependencies (numarray_build numarray_expand)
    
    ## install the package
    add_custom_target (numarray_install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUMARRAY_TAR_ARCHIVE}
      COMMAND rm -rf ${PYTHON_MODULES_BINARY_DIR}/${NUMARRAY_DIRECTORY}
      WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}/${NUMARRAY_DIRECTORY}
      COMMENT "[Numarray] Installing package ..."
      )
    add_dependencies (numarray_install numarray_build)
    
  else (have_numarray_tgz)
    message (STATUS "[Numarray] Unable to continue - missing tar-archive!")
  endif (have_numarray_tgz)
  
endif (enable-numarray)

##____________________________________________________________________
## Presets for CMake cache

configure_file (
  ${PYTHON_MODULES_SOURCE_DIR}/python.cmake.in
  ${PYTHON_MODULES_BINARY_DIR}/python.cmake
  )

##____________________________________________________________________
## Installation target

add_custom_target (install
  COMMENT "[Python modules] Installing packages"
  WORKING_DIRECTORY ${PYTHON_MODULES_BINARY_DIR}
  )

if (enable-numpy)
  if (enable-matplotlib)
    if (enable-num_util)
      add_dependencies (install numpy_install matplotlib_install num_util_install)
    else (enable-num_util)
      add_dependencies (install numpy_install matplotlib_install)
    endif (enable-num_util)
  else (enable-matplotlib)
    if (enable-num_util)
      add_dependencies (install numpy_install num_util_install)
    else (enable-num_util)
      add_dependencies (install numpy_install)
    endif (enable-num_util)
  endif (enable-matplotlib)
else (enable-numpy)
  if (enable-matplotlib)
    if (enable-num_util)
      add_dependencies (install matplotlib_install num_util_install)
    else (enable-num_util)
      add_dependencies (install matplotlib_install)
    endif (enable-num_util)
  else (enable-matplotlib)
    if (enable-num_util)
      add_dependencies (install num_util_install)
    endif (enable-num_util)
  endif (enable-matplotlib)
endif (enable-numpy)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "------------------------------------------------------------")

message (STATUS "[Python modules] Configuration summary:")
message (STATUS "Operating system           = ${CMAKE_SYSTEM}")
message (STATUS "System processor           = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "Python platform            = ${platform_test_output}")
message (STATUS "Install prefix             = ${USG_INSTALL_PREFIX}")
message (STATUS "GFortran compiler          = ${GFORTRAN_EXECUTABLE}")
message (STATUS "BOOST_INCLUDES             = ${BOOST_INCLUDES}")
message (STATUS "FFTW3_INCLUDES             = ${FFTW3_INCLUDES}")
message (STATUS "FFTW3_LIBRARIES            = ${FFTW3_LIBRARIES}")
message (STATUS "GLIB_INCLUDES              = ${GLIB_INCLUDES}")
message (STATUS "GLIB_LIBRARIES             = ${GLIB_LIBRARIES}")
message (STATUS "MATPLOTLIB_INCLUDES        = ${MATPLOTLIB_INCLUDES}")
message (STATUS "MATPLOTLIB_NXUTILS_LIBRARY = ${MATPLOTLIB_NXUTILS_LIBRARY}")
message (STATUS "MATPLOTLIB_TTCONV_LIBRARY  = ${MATPLOTLIB_TTCONV_LIBRARY}")
message (STATUS "PYTHON_INCLUDES            = ${PYTHON_INCLUDES}")
message (STATUS "PYTHON_LIBRARIES           = ${PYTHON_LIBRARIES}")

if (enable-numarray)
  message (STATUS "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ")
  message (STATUS "NUMARRAY_VERSION           = ${NUMARRAY_VERSION}")
  message (STATUS "NUMARRAY_DIRECTORY         = ${NUMARRAY_DIRECTORY}")
  message (STATUS "NUMARRAY_TAR_ARCHIVE       = ${NUMARRAY_TAR_ARCHIVE}")
endif (enable-numarray)

if (enable-num_util)
  message (STATUS "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ")
  message (STATUS "NUM_UTIL_VERSION           = ${NUM_UTIL_VERSION}")
  message (STATUS "NUM_UTIL_DIRECTORY         = ${NUM_UTIL_DIRECTORY}")
  message (STATUS "NUM_UTIL_TAR_ARCHIVE       = ${NUM_UTIL_TAR_ARCHIVE}")
endif (enable-num_util)

if (enable-numpy)
  message (STATUS "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ")
  message (STATUS "NUMPY_VERSION              = ${NUMPY_VERSION}")
  message (STATUS "NUMPY_DIRECTORY            = ${NUMPY_DIRECTORY}")
  message (STATUS "NUMPY_TAR_ARCHIVE          = ${NUMPY_TAR_ARCHIVE}")
else (enable-numpy)
  message (STATUS "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ")
  message (STATUS "NUMPY_INCLUDES             = ${NUMPY_INCLUDES}")
  message (STATUS "NUMPY_LIBRARIES            = ${NUMPY_LIBRARIES}")
  message (STATUS "NUMPY_API_VERSION          = ${NUMPY_API_VERSION}")
endif (enable-numpy)

if (enable-matplotlib)
  message (STATUS "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ")
  message (STATUS "MATPLOTLIB_VERSION         = ${MATPLOTLIB_VERSION}")
  message (STATUS "MATPLOTLIB_DIRECTORY       = ${MATPLOTLIB_DIRECTORY}")
  message (STATUS "MATPLOTLIB_TAR_ARCHIVE     = ${MATPLOTLIB_TAR_ARCHIVE}")
endif (enable-matplotlib)

message (STATUS "------------------------------------------------------------")

## ==============================================================================
##
## Installation option for Python packages
##
## ==============================================================================


# [1] python setup.py build install --root=`pwd`/../install_with_root
#
# install_with_root
# `-- usr
#     |-- include
#     |   `-- python2.4
#     |       `-- numarray
#     `-- lib
#         `-- python2.4
#             `-- site-packages
#                 `-- numarray
#                     |-- codegenerator
#                     |-- convolve
#                     |-- fft
#                     |-- image
#                     |-- linear_algebra
#                     |-- ma
#                     |-- matrix
#                     |-- mlab
#                     |-- nd_image
#                     `-- random_array
#
# [2] python setup.py build install --home=`pwd`/../install_with_home
# 
# install_with_home
# |-- include
# |   `-- python
# |       `-- numarray
# `-- lib
#     `-- python
#         `-- numarray
#             |-- codegenerator
#             |-- convolve
#             |-- fft
#             |-- image
#             |-- linear_algebra
#             |-- ma
#             |-- matrix
#             |-- mlab
#             |-- nd_image
#             `-- random_array
#
# [3] python setup.py build install --prefix=`pwd`/../install_with_prefix
#
# install_with_prefix
# |-- include
# |   `-- python2.4
# |       `-- numarray
# `-- lib
#     `-- python2.4
#         `-- site-packages
#             `-- numarray
#                 |-- codegenerator
#                 |-- convolve
#                 |-- fft
#                 |-- image
#                 |-- linear_algebra
#                 |-- ma
#                 |-- matrix
#                 |-- mlab
#                 |-- nd_image
#                 `-- random_array
