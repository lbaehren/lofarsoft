
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1393 2008-04-03 13:58:05Z baehren                       $
##-------------------------------------------------------------------------------

## Name of the project
project (PYTHON_MODULES)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Options

option (PYTHON_MODULES_VERBOSE_CONFIGURE "Be verbose during configuration?" NO )

## Select the modules to be build

option (module_matplotlib "Build matplotlib module?" YES )
option (module_num_util   "Build num_util module?"   YES )
option (module_numpy      "Build numpy module?"      YES )

## the following modules are outdated and have become replaced by newer ones; 
## we might want to remove them completely at some point in the future.

option (module_numarray "Build numarray module?" NO )
option (module_numeric  "Build numeric module?"  NO )

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

## --- set verbosity during configuration

if (NOT PYTHON_MODULES_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY      YES )
  set (BOOST_pythonOnly        YES )
  set (BOOST_test_exec_monitor NO  )
  set (GLIB_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY     YES )
endif (NOT PYTHON_MODULES_VERBOSE_CONFIGURE)

## --- load CMake modules to locate external packages 

include (FindPython)
include (FindBoost)

## ------------------------------------------------------------------------------
## Configuration and build

## [1] numarray

if (module_numarray)
  add_subdirectory (numarray)
endif (module_numarray)

## [2] matplotlib

#  matplotlib works with with Numeric or numarray.  At compile time,
#  setup.py will look for both packages and compile the appropriate
#  extensions into matplotlib.  At runtime, the correct extension code
#  will be chosen based on your numerix setting in matplotlibrc.  If
#  you want to be able to use either Numeric or numarray efficiently
#  with matplotlib, it is important that you have *both* present and in
#  your PYTHONPATH when you compile matplotlib.

if (module_matplotlib)
  add_subdirectory (matplotlib)
endif (module_matplotlib)

## [3] numeric

if (module_numeric)
  add_subdirectory (numeric)
endif (module_numeric)

## [4] NumPy

# NumPy is a replacement of Numeric Python that adds the features of numarray.  
# To install:
#   python setup.py install
# The setup.py script will take advantage of fast BLAS on your system if it can
# find it.  You can help the process with a site.cfg file.
# If fast BLAS and LAPACK cannot be found, then a slower default version is used. 

if (module_numpy)
  add_subdirectory (numpy)
endif (module_numpy)

## [5] num_util

if (module_num_util)
  add_subdirectory (num_util)
endif (module_num_util)

## ------------------------------------------------------------------------------
## Configuration summary

if (PYTHON_MODULES_VERBOSE_CONFIGURE)
  message (STATUS ">------------------------------------------------------------<")
  message (STATUS "> [Python modules] Configuration summary.                    <")
  message (STATUS ">------------------------------------------------------------<")
  message (STATUS "Build matplotlib module = ${module_matplotlib}")
  message (STATUS "Build num_util module   = ${module_num_util}")
  message (STATUS "Build numpy module      = ${module_numpy}")
  message (STATUS " ")
  message (STATUS "BOOST_INCLUDES          = ${BOOST_INCLUDES}")
  message (STATUS "PYTHON_INCLUDES         = ${PYTHON_INCLUDES}")
  message (STATUS "NUMPY_INCLUDES          = ${NUMPY_INCLUDES}")
  message (STATUS "+------------------------------------------------------------+")
endif (PYTHON_MODULES_VERBOSE_CONFIGURE)


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
