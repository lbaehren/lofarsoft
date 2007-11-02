
project (PYTHON_MODULES)

## ------------------------------------------------------------------------------
## Selection of available Python modules

option (module_numarray "Build numarray module?" ON)
option (module_matplotlib "Build matplotlib module?" ON)
option (module_numeric "Build numeric module?" ON)

## ------------------------------------------------------------------------------
## check where to find the CMake scripts

find_path (PYTHON_MODULES_cmake FindPython.cmake
  PATHS 
  ${PYTHON_MODULES_SOURCE_DIR}
  ${PYTHON_MODULES_SOURCE_DIR}/..
  ${PYTHON_MODULES_SOURCE_DIR}/../..
  ${PYTHON_MODULES_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

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
