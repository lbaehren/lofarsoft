##-------------------------------------------------------------------------------
## $Id::                                                                        $
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the BOOST
#  library.
#
## ------------------------------------------------------------------------------

project (BOOST)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BOOST_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (BOOST_cmake FindBOOST.cmake FindPython.cmake
  PATHS 
  ${BOOST_SOURCE_DIR}
  ${BOOST_SOURCE_DIR}/..
  ${BOOST_SOURCE_DIR}/../..
  ${BOOST_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (BOOST_cmake)
  include (${BOOST_cmake}/FindBoost.cmake)
  include (${BOOST_cmake}/FindPython.cmake)
endif (BOOST_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (BOOST_prefix release_area.txt
  PATHS
  ${BOOST_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NOT BOOST_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT BOOST_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure ---------------------------------

## To be fully consistent at this point, I think we have to ensure that we are
## building against the correct python version.

message (STATUS "Running the BOOST configure script...")

execute_process (
  WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
  COMMAND ./configure --with-libraries=python --with-python=${PYTHON_EXECUTABLE} --prefix=${BOOST_prefix}
  TIMEOUT 300
  ERROR_VARIABLE error_configure
  ERROR_FILE error.log
  )

if (error_configure)
  message (STATUS "There was an error running the BOOST configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] make --------------------------------------

message (STATUS "Building BOOST library...")

execute_process (
  WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
  COMMAND make 
  TIMEOUT 3600
  ERROR_VARIABLE error_make
  ERROR_FILE error.log
  )

if (error_make)
  message (STATUS "There was an error building/installing the BOOST library!")
  message (SEND_ERROR "${error_make}")
endif (error_make)

## [3] make install ------------------------------

if (NOT error_make)

message (STATUS "Installing BOOST library...")

execute_process (
  WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
  COMMAND make install
  TIMEOUT 180
  ERROR_VARIABLE error_make_install
  ERROR_FILE error.log
  )

if (error_make_install)
  message (STATUS "There was an error building/installing the BOOST library!")
  message (SEND_ERROR "${error_make_install}")
endif (error_make_install)

endif (NOT error_make)

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  message (STATUS "Found system-wide installation of BOOST; skipping rebuild!")
endif (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
