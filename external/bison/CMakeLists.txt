
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the Bison
#  lexical generator.
#
## ------------------------------------------------------------------------------

project (BISON)

## -----------------------------------------------------------------------------
## Only force rebuild of the tool, if it cannot be found on the system.

option (BISON_FORCE_BUILD "Force rebuild and local installation" 0)

set (bin_locations
  ## possible system-wide installation locations
  /usr/bin
  /usr/local/bin/
  /opt
  /opt/bin
  /sw/bin
  ## local installation as part of USG distribution
  ${BISON_SOURCE_DIR}../../release/bin
)

find_program (HAVE_BISON bison ${bin_locations})

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BISON OR BISON_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (BISON_prefix release_area.txt
  PATHS
  ${BISON_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NOT BISON_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT BISON_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the Bison configure script...")

execute_process (
  WORKING_DIRECTORY ${BISON_SOURCE_DIR}
  COMMAND ./configure --prefix=${BISON_prefix}
  TIMEOUT 300
  ERROR_VARIABLE error_configure
  )

if (error_configure)
  message (STATUS "There was an error running the Bison configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] make

message (STATUS "Building Bison ...")

execute_process (
  WORKING_DIRECTORY ${BISON_SOURCE_DIR}
  COMMAND make 
  TIMEOUT 3600
  ERROR_VARIABLE error_make
  )

if (error_make)
  message (STATUS "There was an error building Bison!")
  message (SEND_ERROR "${error_make}")
endif (error_make)

## [3] make install

if (NOT error_make)

message (STATUS "Installing Bison ...")

execute_process (
  WORKING_DIRECTORY ${BISON_SOURCE_DIR}
  COMMAND make install
  TIMEOUT 180
  ERROR_VARIABLE error_make_install
  )

if (error_make_install)
  message (STATUS "There was an error installing Bison!")
  message (SEND_ERROR "${error_make_install}")
endif (error_make_install)

endif (NOT error_make)

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_BISON OR BISON_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Bison; skipping rebuild!")
endif (NOT HAVE_BISON OR BISON_FORCE_BUILD)
