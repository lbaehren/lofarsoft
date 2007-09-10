
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the FLEX
#  lexical generator.
#
## ------------------------------------------------------------------------------

project (FLEX)

## -----------------------------------------------------------------------------
## Only force rebuild of the tool, if it cannot be found on the system.

option (FLEX_FORCE_REBUILD "Force rebuild and local installation" 0)

set (bin_locations
  ## possible system-wide installation locations
  /usr/bin
  /usr/local/bin/
  /opt
  /opt/bin
  /sw/bin
  ## local installation as part of USG distribution
  ${FLEX_SOURCE_DIR}../../release/bin
)

find_program (HAVE_FLEX flex ${bin_locations})

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_FLEX OR FLEX_FORCE_REBUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (FLEX_prefix release_area.txt
  PATHS
  ${FLEX_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NOT FLEX_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT FLEX_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the Flex configure script...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND ./configure --prefix=${FLEX_prefix}
  TIMEOUT 180
  ERROR_VARIABLE error_configure
  )

if (error_configure)
  message (STATUS "There was an error running the Flex configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] make

message (STATUS "Building Flex ...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND make 
  TIMEOUT 3600
  ERROR_VARIABLE error_make
  )

if (error_make)
  message (STATUS "There was an error building Flex!")
  message (SEND_ERROR "${error_make}")
endif (error_make)

## [3] make install

if (NOT error_make)

message (STATUS "Installing Flex ...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND make install
  TIMEOUT 180
  ERROR_VARIABLE error_make_install
  )

if (error_make_install)
  message (STATUS "There was an error installing Flex!")
  message (SEND_ERROR "${error_make_install}")
endif (error_make_install)

endif (NOT error_make)

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_FLEX OR FLEX_FORCE_REBUILD)
  message (STATUS "Found system-wide installation of Flex; skipping rebuild!")
endif (NOT HAVE_FLEX OR FLEX_FORCE_REBUILD)
