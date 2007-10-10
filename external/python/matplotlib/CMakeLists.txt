
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

project (PYTHON_MATPLOTLIB)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYTHON_MATPLOTLIB_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (PYTHON_MATPLOTLIB_cmake FindPython.cmake
  PATHS 
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/..
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/../..
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYTHON_MATPLOTLIB_cmake)
  set (PYTHON_FIND_QUIETLY 1)
  include (${PYTHON_MATPLOTLIB_cmake}/FindPython.cmake)
endif (PYTHON_MATPLOTLIB_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (PYTHON_MATPLOTLIB_prefix release_area.txt
  PATHS
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (NOT PYTHON_MATPLOTLIB_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT PYTHON_MATPLOTLIB_prefix)

## ------------------------------------------------------------------------------
## External dependencies
##
##  You will need to have recent versions of freetype (>= 2.1.7), libpng
##  and zlib installed on your system.  If you are using a package
##  manager, also make sure the devel versions of these packages are
##  also installed (eg freetype-devel).

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

find_library (libfreetype freetype ${lib_locations})
find_library (libgtk gtk ${lib_locations})
find_library (libpng png ${lib_locations})
find_library (libz z ${lib_locations})

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the Python setup script for this module ...")

execute_process (
  WORKING_DIRECTORY ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  COMMAND python setup.py build install --home=${PYTHON_MATPLOTLIB_prefix}
  TIMEOUT 1800
  ERROR_VARIABLE error_configure
  ERROR_FILE error.log
  )

if (error_configure)
  message (STATUS "There was an error running the matplotlib configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] build

execute_process (
  WORKING_DIRECTORY ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  COMMAND python setup.py build --home=${PYTHON_MATPLOTLIB_prefix
  TIMEOUT 1800
  ERROR_VARIABLE error_build
  ERROR_FILE build.log
  )

## [3] install

execute_process (
  WORKING_DIRECTORY ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  COMMAND python setup.py install --home=${PYTHON_MATPLOTLIB_prefix
  TIMEOUT 1800
  ERROR_VARIABLE error_install
  ERROR_FILE install.log
  )

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)
  message (STATUS "Found system-wide installation of matplotlib; skipping rebuild!")
endif (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)
