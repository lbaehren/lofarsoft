
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the
#  PYTHON_NUMARRAY library.
#
#  ---
#
#  matplotlib is a pure python 2D plotting library with a Matlab(TM)
#  syntax which produces publication quality figures using in a
#  variety of hardcopy formats (PNG, JPG, TIFF, PS) and interactive
#  GUI environments (WX, GTK) across platforms. matplotlib can be used
#  in python scripts, interactively from the python shell (ala matlab
#  or mathematica), in web application servers generating dynamic
#  charts, or embedded in GTK or WX applications.
#
#  matplotlib requires at a minimum python 2.2+, Numeric or numarray
#  and freetype.  To get the most out of matplotlib, you will want to
#  build some of the optional GUI and image extensions, discussed
#  below.  Matplotlib is known to work on linux, unix, win32 and OS X
#  platforms.
#
#  matplotlib works with with Numeric or numarray.  At compile time,
#  setup.py will look for both packages and compile the appropriate
#  extensions into matplotlib.  At runtime, the correct extension code
#  will be chosen based on your numerix setting in matplotlibrc.  If
#  you want to be able to use either Numeric or numarray efficiently
#  with matplotlib, it is important that you have *both* present and in
#  your PYTHONPATH when you compile matplotlib.
#
#  The latest installation instructions can be found at
#
#    http://matplotlib.sourceforge.net/installing.html
#
## ------------------------------------------------------------------------------

project (MATPLOTLIB)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATPLOTLIB_FORCE_BUILD "Force rebuild and local installation" 0)
option (MATPLOTLIB_CMAKE_BUILD "Use CMake port to configure and build Boost?" 0)

## check where to find the CMake scripts

find_path (MATPLOTLIB_cmake CMakeSettings.cmake
  PATHS 
  ${MATPLOTLIB_SOURCE_DIR}
  ${MATPLOTLIB_SOURCE_DIR}/..
  ${MATPLOTLIB_SOURCE_DIR}/../..
  ${MATPLOTLIB_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (MATPLOTLIB_cmake)
  ## --- Python ----------------------------------
  set (PYTHON_FIND_QUIETLY 1)
  include (${MATPLOTLIB_cmake}/FindPython.cmake)
  ## includes
  if (PYTHON_INCLUDES)
    include_directories (${PYTHON_INCLUDES})
  endif (PYTHON_INCLUDES)
  ## linking
  if (PYTHON_LIBRARIES)
    list (APPEND matplotlib_link_libraries ${PYTHON_LIBRARIES})
  endif (PYTHON_LIBRARIES)
  ## --- Numarray --------------------------------
  set (NUMARRAY_FIND_QUIETLY 1)
  include (${MATPLOTLIB_cmake}/FindNumarray.cmake)
  if (NUMARRAY_INCLUDES)
    include_directories (${NUMARRAY_INCLUDES})
  endif (NUMARRAY_INCLUDES)
  ## linking
  if (NUMARRAY_LIBRARIES)
    list (APPEND matplotlib_link_libraries ${NUMARRAY_LIBRARIES})
  endif (NUMARRAY_LIBRARIES)
  ## --- PyGtk -----------------------------------
  set (PYGTK_FIND_QUIETLY 1)
  include (${MATPLOTLIB_cmake}/FindPyGTK.cmake)
  if (PYGTK_INCLUDES)
    include_directories (${PYGTK_INCLUDES})
  endif (PYGTK_INCLUDES)
  ## linking
  if (PYGTK_LIBRARIES)
    list (APPEND matplotlib_link_libraries ${PYGTK_LIBRARIES})
  endif (PYGTK_LIBRARIES)
  ## --- Glib ------------------------------------
#  set (GLIB_FIND_QUIETLY 1)
  include (${MATPLOTLIB_cmake}/FindGLIB.cmake)
  if (GLIB_INCLUDES)
    include_directories (${GLIB_INCLUDES})
  endif (GLIB_INCLUDES)
  ## linking
  if (GLIB_LIBRARIES)
    list (APPEND matplotlib_link_libraries ${GLIB_LIBRARIES})
  endif (GLIB_LIBRARIES)
  ## --- GTK -------------------------------------
  set (GTK_FIND_QUIETLY 1)
  include (${MATPLOTLIB_cmake}/FindGTK.cmake)
  if (GTK_INCLUDES)
    include_directories (${GTK_INCLUDES})
  endif (GTK_INCLUDES)
  ## linking
  if (GTK_LIBRARIES)
    list (APPEND matplotlib_link_libraries ${GTK_LIBRARIES})
  endif (GTK_LIBRARIES)
  ## --- Summary ---------------------------------
  message (STATUS "Python   = ${HAVE_PYTHON}")
  message (STATUS "Numarray = ${HAVE_NUMARRAY}")
  message (STATUS "PyGtk    = ${HAVE_PYGTK}")
  message (STATUS "GLib     = ${HAVE_GLIB}")
  message (STATUS "GTK      = ${HAVE_GTK}")
endif (MATPLOTLIB_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (MATPLOTLIB_prefix release_area.txt
  PATHS
  ${MATPLOTLIB_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (MATPLOTLIB_prefix)
  message (STATUS "Located installation area: ${MATPLOTLIB_prefix}")
else (MATPLOTLIB_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (MATPLOTLIB_prefix)

## ------------------------------------------------------------------------------
## External dependencies
##
##  You will need to have recent versions of freetype (>= 2.1.7), libpng
##  and zlib installed on your system.  If you are using a package
##  manager, also make sure the devel versions of these packages are
##  also installed (eg freetype-devel).

set (lib_locations
  ## local installation
  ${MATPLOTLIB_prefix}/lib
  ## system-wide installation
  /lib
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

set (include_locations
  ## local installation
  ${MATPLOTLIB_prefix}/include
  ${MATPLOTLIB_prefix}/include/python
  ## system-wide installation
  /include
  /usr/include
  /usr/local/include
  /opt/include
  /sw/include
  )

find_library (libfreetype freetype ${lib_locations})
find_library (libgtk gtk ${lib_locations})
find_library (libpng png ${lib_locations})
find_library (libz z ${lib_locations})

## ------------------------------------------------------------------------------
## CMake-based installation

if (MATPLOTLIB_CMAKE_BUILD)
  add_subdirectory (src)
endif (MATPLOTLIB_CMAKE_BUILD)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] build

execute_process (
  WORKING_DIRECTORY ${MATPLOTLIB_SOURCE_DIR}
  COMMAND ${PYTHON_EXECUTABLE} setup.py build
  TIMEOUT 1800
  ERROR_VARIABLE error_build
  ERROR_FILE matplotlib_build.log
  )

## [2] install

execute_process (
  WORKING_DIRECTORY ${MATPLOTLIB_SOURCE_DIR}
  COMMAND ${PYTHON_EXECUTABLE} setup.py install --home=${MATPLOTLIB_prefix}
  TIMEOUT 1800
  ERROR_VARIABLE error_install
  ERROR_FILE matplotlib_install.log
  )

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
  message (STATUS "Found system-wide installation of matplotlib; skipping rebuild!")
endif (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
