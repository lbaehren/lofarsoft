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
#  NUMPY_MAJOR_VERSION      = NumPy major version
#  NUMPY_MINOR_VERSION      = NumPy minor version
#  NUMPY_RELEASE_VERSION    = NumPy release/patch version

## -----------------------------------------------------------------------------
## Search locations

if (NUMPY_FIND_QUIETLY)
  set (PYTHON_FIND_QUIETLY TRUE)
endif (NUMPY_FIND_QUIETLY)

if (NOT PYTHON_FOUND)
  find_package (Python)
endif (NOT PYTHON_FOUND)
    
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
## Let's assume the Python executable is smarter about finding NumPy than we
## are, and try asking it before searching ourselves.
## This is necessary to e.g. pick up the MacPorts NumPy installation, which
## ends up in /opt/local/Library/Frameworks/Python.framework ...

execute_process (
  COMMAND ${PYTHON_EXECUTABLE} -c "import numpy, os; print os.path.dirname(numpy.__file__)"
  OUTPUT_VARIABLE numpy_path
  )
if (numpy_path)
  string (STRIP ${numpy_path} numpy_search_path)
else (numpy_path)
  set (numpy_search_path ${lib_locations})
endif (numpy_path)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (NUMPY_INCLUDES numpy/arrayobject.h numpy/ndarrayobject.h
  PATHS
  /sw/lib/python2.6/site-packages
  /Library/Frameworks/EPD64.framework/Versions/Current/lib
  ${numpy_search_path}
  /System/Library/Frameworks/Python.framework/Versions
  PATH_SUFFIXES
  python
  core/include
  python/numpy/core/include
  python${PYTHON_VERSION}
  python${PYTHON_VERSION}/site-packages/numpy
  python${PYTHON_VERSION}/site-packages/numpy/core/include
  ${PYTHON_VERSION}/Extras/lib/python/numpy/core/include
)

## -----------------------------------------------------------------------------
## Check for the library

find_library (NUMPY_MULTIARRAY_LIBRARY multiarray
  PATHS
  /sw/lib/python2.6/site-packages
  /Library/Frameworks/EPD64.framework/Versions/Current/lib
  ${numpy_search_path}
  /System/Library/Frameworks/Python.framework/Versions
  PATH_SUFFIXES
  python
  core
  python/numpy/core
  python${PYTHON_VERSION}/site-packages/numpy/core
  ${PYTHON_VERSION}/Extras/lib/python/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_MULTIARRAY_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_MULTIARRAY_LIBRARY})
endif (NUMPY_MULTIARRAY_LIBRARY)

find_library (NUMPY_SCALARMATH_LIBRARY scalarmath
  PATHS
  /sw/lib/python2.6/site-packages
  /Library/Frameworks/EPD64.framework/Versions/Current/lib
  ${numpy_search_path}
  /System/Library/Frameworks/Python.framework/Versions
  PATH_SUFFIXES
  python
  core
  python/numpy/core
  python${PYTHON_VERSION}/site-packages/numpy/core
  ${PYTHON_VERSION}/Extras/lib/python/numpy/core
  NO_DEFAULT_PATH
  )
if (NUMPY_SCALARMATH_LIBRARY)
  list (APPEND NUMPY_LIBRARIES ${NUMPY_SCALARMATH_LIBRARY})
endif (NUMPY_SCALARMATH_LIBRARY)

## -----------------------------------------------------------------------------
## Check for executables

find_program (F2PY_EXECUTABLE f2py f2py${PYTHON_VERSION} f2py-${PYTHON_VERSION}
  PATHS
  /Library/Frameworks/EPD64.framework/Versions/Current/lib
  )

## -----------------------------------------------------------------------------
## Try to determine the API version

if (PYTHON_EXECUTABLE)
  ## some basic feedback
  message (STATUS "Found version.py - running Python to import module numpy.")
  ## Run Python to import module numpy and print the version information
  execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print numpy.__version__"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE numpy_version_test_result
    OUTPUT_VARIABLE numpy_version_test_output
    ERROR_VARIABLE numpy_version_test_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else (PYTHON_EXECUTABLE)
  message (STATUS "[NumPy] Unable to process version.py script!")
endif (PYTHON_EXECUTABLE)

if (numpy_version_test_output)
  ## copy the output from the Python prompt
  set (NUMPY_API_VERSION ${numpy_version_test_output})
  ## Extract major version
  string (REGEX MATCH "[0-9]" NUMPY_MAJOR_VERSION ${numpy_version_test_output})
  ## Extract minor version
  string (REGEX REPLACE "${NUMPY_MAJOR_VERSION}." "" numpy_version_test_output ${numpy_version_test_output})
  string (REGEX MATCH "[0-9]" NUMPY_MINOR_VERSION ${numpy_version_test_output})
  ## Extract patch version
  string (REGEX REPLACE "${NUMPY_MINOR_VERSION}." "" numpy_version_test_output ${numpy_version_test_output})
  string (REGEX MATCH "[0-9]" NUMPY_RELEASE_VERSION ${numpy_version_test_output})
else (numpy_version_test_output)

  find_file (NUMPY_TEST_PROGRAM TestNumPyVersion.cc
    PATHS ${CMAKE_MODULE_PATH} ${LUS_SOURCE_DIR}
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

if (NUMPY_INCLUDES AND NUMPY_LIBRARIES AND F2PY_EXECUTABLE)
  set (HAVE_NUMPY  TRUE)
  set (NUMPY_FOUND TRUE)
else (NUMPY_INCLUDES AND NUMPY_LIBRARIES AND F2PY_EXECUTABLE)
  set (HAVE_NUMPY  FALSE)
  set (NUMPY_FOUND FALSE)
  if (NOT NUMPY_FIND_QUIETLY)
    if (NOT NUMPY_INCLUDES)
      message (STATUS "Unable to find NUMPY header files!")
    endif (NOT NUMPY_INCLUDES)
    if (NOT NUMPY_LIBRARIES)
      message (STATUS "Unable to find NUMPY library files!")
    endif (NOT NUMPY_LIBRARIES)
    if (NOT F2PY_EXECUTABLE)
      message(STATUS "Unable to find F2PY executable!")
    endif (NOT F2PY_EXECUTABLE)
  endif (NOT NUMPY_FIND_QUIETLY)
endif (NUMPY_INCLUDES AND NUMPY_LIBRARIES AND F2PY_EXECUTABLE)

if (HAVE_NUMPY)
  if (NOT NUMPY_FIND_QUIETLY)
    message (STATUS "Found components for NUMPY")
    message (STATUS "NUMPY_INCLUDES  = ${NUMPY_INCLUDES}")
    message (STATUS "NUMPY_LIBRARIES = ${NUMPY_LIBRARIES}")
    message (STATUS "F2PY_EXECUTABLE = ${F2PY_EXECUTABLE}")
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

## -----------------------------------------------------------------------------
## Macro to generate a Python interface module from one or more Fortran sources
##
## Usage: add_f2py_module(<module-name> <src1>..<srcN> DESTINATION <install-dir>
##
macro (add_f2py_module _name)

  # Precondition check.
  if(NOT F2PY_EXECUTABLE)
    message(FATAL_ERROR "add_f2py_module: f2py executable is not available!")
  endif(NOT F2PY_EXECUTABLE)

  # Parse arguments.
  string(REGEX REPLACE ";?DESTINATION.*" "" _srcs "${ARGN}")
  string(REGEX MATCH "DESTINATION;.*" _dest_dir "${ARGN}")
  string(REGEX REPLACE "^DESTINATION;" "" _dest_dir "${_dest_dir}")

  # Sanity checks.
  if(_srcs MATCHES "^$")
    message(FATAL_ERROR "add_f2py_module: no source files specified")
  endif(_srcs MATCHES "^$")
  if(_dest_dir MATCHES "^$" OR _dest_dir MATCHES ";")
    message(FATAL_ERROR "add_f2py_module: destination directory invalid")
  endif(_dest_dir MATCHES "^$" OR _dest_dir MATCHES ";")

  # Get the compiler-id and map it to compiler vendor as used by f2py.
  # Currently, we only check for GNU, but this can easily be extended. 
  # Cache the result, so that we only need to check once.
  if(NOT F2PY_FCOMPILER)
    if(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
      if(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
        set(_fcompiler "gnu95")
      else(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
        set(_fcompiler "gnu")
      endif(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
    else(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
      set(_fcompiler "F2PY_FCOMPILER-NOTFOUND")
    endif(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
    set(F2PY_FCOMPILER ${_fcompiler} CACHE STRING
      "F2PY: Fortran compiler type by vendor" FORCE)
    if(NOT F2PY_FCOMPILER)
      message(STATUS "[F2PY]: Could not determine Fortran compiler type. "
                     "Troubles ahead!")
    endif(NOT F2PY_FCOMPILER)
  endif(NOT F2PY_FCOMPILER)

  # Set f2py compiler options: compiler vendor and path to Fortran77/90 compiler.
  if(F2PY_FCOMPILER)
    set(_fcompiler_opts "--fcompiler=${F2PY_FCOMPILER}")
    list(APPEND _fcompiler_opts "--f77exec=${CMAKE_Fortran_COMPILER}")
    if(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
      list(APPEND _fcompiler_opts "--f90exec=${CMAKE_Fortran_COMPILER}")
    endif(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
  endif(F2PY_FCOMPILER)

  # Make the source filenames absolute.
  set(_abs_srcs)
  foreach(_src ${_srcs})
    get_filename_component(_abs_src ${_src} ABSOLUTE)
    list(APPEND _abs_srcs ${_abs_src})
  endforeach(_src ${_srcs})

  # Get a list of the include directories.
  # The f2py --include_paths option, used when generating a signature file,
  # needs a colon-separated list. The f2py -I option, used when compiling
  # the sources, must be repeated for every include directory.
  get_directory_property(_inc_dirs INCLUDE_DIRECTORIES)
  string(REPLACE ";" ":" _inc_paths "${_inc_dirs}")
  set(_inc_opts)
  foreach(_dir ${_inc_dirs})
    list(APPEND _inc_opts "-I${_dir}")
  endforeach(_dir)

  # Define the command to generate the Fortran to Python interface module. The
  # output will be a shared library that can be imported by python.
  add_custom_command(OUTPUT ${_name}.so
    COMMAND ${F2PY_EXECUTABLE} --quiet -m ${_name} -h ${_name}.pyf
            --include_paths ${_inc_paths} --overwrite-signature ${_abs_srcs}
    COMMAND ${F2PY_EXECUTABLE} --quiet -m ${_name} -c ${_name}.pyf
            ${_fcompiler_opts} ${_inc_opts} ${_abs_srcs}
    DEPENDS ${_srcs}
    COMMENT "[F2PY] Building Fortran to Python interface module ${_name}")

  # Add a custom target <name> to trigger the generation of the python module.
  add_custom_target(${_name} ALL DEPENDS ${_name}.so)

  # Install the python module
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_name}.so
    DESTINATION ${_dest_dir})

endmacro (add_f2py_module)
