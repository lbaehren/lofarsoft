
project (PYTHON_MODULES)

## ------------------------------------------------------------------------------
## Selection of available Python modules

option (module_matplotlib "Build matplotlib module?" ON)
option (nodule_numpy "Build numpy module?" ON)
option (module_num_util "Build num_util module?" ON)

## the following modules are outdated and have become replaced by newer ones; 
## we might want to removed them completely at some point in the future.

option (module_numarray "Build numarray module?" OFF)
option (module_numeric "Build numeric module?" OFF)

## ------------------------------------------------------------------------------
## check where to find the CMake scripts

find_path (PYTHON_MODULES_cmake CMakeSettings.cmake
  PATHS 
  ${PYTHON_MODULES_SOURCE_DIR}
  ${PYTHON_MODULES_SOURCE_DIR}/..
  ${PYTHON_MODULES_SOURCE_DIR}/../..
  ${PYTHON_MODULES_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYTHON_MODULES_cmake)
  list (APPEND CMAKE_MODULE_PATH ${PYTHON_MODULES_cmake})
else (PYTHON_MODULES_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (PYTHON_MODULES_cmake)

## --- set verbosity during configuration ------
set (BOOST_FIND_QUIETLY ON)
set (BOOST_pythonOnly ON)
set (BOOST_test_exec_monitor OFF)
set (PYTHON_FIND_QUIETLY ON)

include (FindPython)
include (FindBoost)

message (STATUS "--------------------------------------------------")
message (STATUS "BOOST          = ${HAVE_BOOST}")
message (STATUS "BOOST includes = ${BOOST_INCLUDES}")
message (STATUS "BOOST libs     = ${BOOST_LIBRARIES}")
message (STATUS "NumPy includes = ${NUMPY_INCLUDES}")
message (STATUS "--------------------------------------------------")

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

## [3] NumPy

# NumPy is a replacement of Numeric Python that adds the features of numarray.  
# To install:
#   python setup.py install
# The setup.py script will take advantage of fast BLAS on your system if it can
# find it.  You can help the process with a site.cfg file.
# If fast BLAS and LAPACK cannot be found, then a slower default version is used. 

if (nodule_numpy)
  add_subdirectory (numpy)
endif (nodule_numpy)

## [5] num_util

if (module_num_util)
  add_subdirectory (num_util)
endif (module_num_util)

## ------------------------------------------------------------------------------
## Installation option for Python packages

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
