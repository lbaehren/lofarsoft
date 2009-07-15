# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

# - Check for the presence of NUMPY
#
# The following variables are set when NUMPY is found:
#  HAVE_NUMPY               = Set to true, if all components of NUMPY have been
#                             found.
#  NUMPY_INCLUDES           = Include path for the header files of NUMPY
#  NUMPY_MULTIARRAY_LIBRARY = Path to the multiarray shared library
#  NUMPY_SCALARMATH_LIBRARY = Path to the scalarmath shared library
#  NUMPY_LIBRARIES          = Link these to use NUMPY
#  F2PY_EXECUTABLE          = The f2py executable
#  NUMPY_LFLAGS             = Linker flags (optional)
#  NUMPY_API_VERSION        = API version of the installed and available NumPy
#                             package

## -----------------------------------------------------------------------------
## Search locations

include (CMakeSettings)

include (FindPython)

## -----------------------------------------------------------------------------
## As the shared libraries of a Python module typically do not contain the 
## usual prefix, we need to remove it while searching for the NumPy libraries.
## In order however not to affect other CMake modules we need to swap back in the
## original prefixes once the end of this module is reached.

set (TMP_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})

set (CMAKE_FIND_LIBRARY_PREFIXES "" CACHE STRING
  "Library prefixes"
  FORCE
  )

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NUMPY_ARRAYOBJECT_H numpy/arrayobject.h
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python
  python/numpy/core/include
  NO_DEFAULT_PATH
)

find_path (NUMPY_NDARRAYOBJECT_H numpy/ndarrayobject.h
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python
  python/numpy/core/include
  NO_DEFAULT_PATH
)

find_path (NUMPY_INCLUDES numpy/__multiarray_api.h numpy/multiarray_api.txt
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python
  python/numpy/core/include
  python${PYTHON_VERSION}
  python${PYTHON_VERSION}/site-packages/numpy
  python${PYTHON_VERSION}/site-packages/numpy/core/include
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the library

find_library (NUMPY_MULTIARRAY_LIBRARY multiarray
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python
  python/numpy/core
  python${PYTHON_VERSION}/site-packages/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_MULTIARRAY_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_MULTIARRAY_LIBRARY})
endif (NUMPY_MULTIARRAY_LIBRARY)

find_library (NUMPY_SCALARMATH_LIBRARY scalarmath
  PATHS
  ${lib_locations}
  PATH_SUFFIXES
  python
  python/numpy/core
  python${PYTHON_VERSION}/site-packages/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_SCALARMATH_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_SCALARMATH_LIBRARY})
endif (NUMPY_SCALARMATH_LIBRARY)

## -----------------------------------------------------------------------------
## Check for executables

find_program (F2PY_EXECUTABLE f2py f2py${PYTHON_VERSION}
  PATHS ${bin_locations}
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Try to determine the API version

## Try to locate numpy/version.py first; if this script is not available, loading
## the module in order to query its version does not make any sense.

find_file (NUMPY_VERSION_PY version.py
  PATHS
  ${lib_locations}
  /usr/lib64
  /usr/local/lib64
  PATH_SUFFIXES
  python/numpy
  python${PYTHON_VERSION}/site-packages/numpy
  NO_DEFAULT_PATH
  )

if (NUMPY_VERSION_PY)
  execute_process (
    COMMAND python -c "import numpy; print numpy.__version__"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE numpy_version_test_result
    OUTPUT_VARIABLE numpy_version_test_output
    ERROR_VARIABLE numpy_version_test_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else (NUMPY_VERSION_PY)
  message (STATUS "[NumPy] Unable to locate version.py script!")
endif (NUMPY_VERSION_PY)

if (numpy_version_test_output)

  set (NUMPY_API_VERSION ${numpy_version_test_output})
  
else (numpy_version_test_output)
  
  if (NUMPY_NDARRAYOBJECT_H)
    file (STRINGS ${NUMPY_NDARRAYOBJECT_H} NPY_VERSION
      REGEX "NPY_VERSION"
      )
    if (NPY_VERSION)
      string (REGEX REPLACE "#define NPY_VERSION 0x" "" NUMPY_VERSION ${NPY_VERSION})
    else (NPY_VERSION)
      message (STATUS "[NumPy] Unable to extract version from header file.")
    endif (NPY_VERSION)
  endif (NUMPY_NDARRAYOBJECT_H)
  
  find_file (NUMPY_TEST_PROGRAM TestNumPyVersion.cc
    PATHS ${CMAKE_MODULE_PATH} ${USG_ROOT}
    PATH_SUFFIXES devel_common/cmake
    )
  
  if (NUMPY_TEST_PROGRAM AND PYTHON_INCLUDES)
    ## try to compile and run
    try_run (
      NUMPY_TEST_RUN_RESULT
      NUMPY_TEST_COMPILE_RESULT
      ${CMAKE_BINARY_DIR}
      ${NUMPY_TEST_PROGRAM}
      COMPILE_DEFINITIONS -I${PYTHON_INCLUDES} -I${NUMPY_INCLUDES}
      COMPILE_OUTPUT_VARIABLE NUMPY_TEST_COMPILE
      RUN_OUTPUT_VARIABLE NUMPY_TEST_RUN
      )
    ## display results
    if (NOT NUMPY_FIND_QUIETLY)
      message (STATUS "NUMPY_TEST_RUN_RESULT     = ${NUMPY_TEST_RUN_RESULT}")
      message (STATUS "NUMPY_TEST_COMPILE_RESULT = ${NUMPY_TEST_COMPILE_RESULT}")
      message (STATUS "NUMPY_TEST_RUN            = ${NUMPY_TEST_RUN}")
    endif (NOT NUMPY_FIND_QUIETLY)
  else (NUMPY_TEST_PROGRAM AND PYTHON_INCLUDES)
    message (STATUS "Unable to locate test program!")
  endif (NUMPY_TEST_PROGRAM AND PYTHON_INCLUDES)
  
endif (numpy_version_test_output)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (NUMPY_INCLUDES AND NUMPY_LIBRARIES)
  set (HAVE_NUMPY TRUE)
else (NUMPY_INCLUDES AND NUMPY_LIBRARIES)
  set (HAVE_NUMPY FALSE)
  if (NOT NUMPY_FIND_QUIETLY)
    if (NOT NUMPY_INCLUDES)
      message (STATUS "Unable to find NUMPY header files!")
    endif (NOT NUMPY_INCLUDES)
    if (NOT NUMPY_LIBRARIES)
      message (STATUS "Unable to find NUMPY library files!")
    endif (NOT NUMPY_LIBRARIES)
  endif (NOT NUMPY_FIND_QUIETLY)
endif (NUMPY_INCLUDES AND NUMPY_LIBRARIES)

if (HAVE_NUMPY)
  if (NOT NUMPY_FIND_QUIETLY)
    message (STATUS "Found components for NUMPY")
    message (STATUS "NUMPY_INCLUDES  = ${NUMPY_INCLUDES}")
    message (STATUS "NUMPY_LIBRARIES = ${NUMPY_LIBRARIES}")
  endif (NOT NUMPY_FIND_QUIETLY)
else (HAVE_NUMPY)
  if (NUMPY_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find NUMPY!")
  endif (NUMPY_FIND_REQUIRED)
endif (HAVE_NUMPY)

## -----------------------------------------------------------------------------
## Mark advanced variables

mark_as_advanced (
  NUMPY_INCLUDES
  NUMPY_LIBRARIES
  NUMPY_MULTIARRAY_LIBRARY
  NUMPY_SCALARMATH_LIBRARY
  F2PY_EXECUTABLE
  NUMPY_API_VERSION
  )

## -----------------------------------------------------------------------------
## Reinstate the original value of CMAKE_FIND_LIBRARY_PREFIXES

set (CMAKE_FIND_LIBRARY_PREFIXES ${TMP_FIND_LIBRARY_PREFIXES} CACHE STRING
  "Library prefixes"
  FORCE
  )

