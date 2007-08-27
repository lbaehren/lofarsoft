
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the HDF5
#  library.
#
## ------------------------------------------------------------------------------

project (libhdf5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

## check where to find the CMake scripts

find_path (libhdf5_cmake FindHDF5.cmake
  PATHS 
  ${libhdf5_SOURCE_DIR}
  ${libhdf5_SOURCE_DIR}/..
  ${libhdf5_SOURCE_DIR}/../..
  ${libhdf5_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (libhdf5_cmake)
  include (${libhdf5_cmake}/FindHDF5.cmake)
endif (libhdf5_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (libhdf5_prefix release_area.txt
  PATHS
  ${libhdf5_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NOT libhdf5_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT libhdf5_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the HDF5 configure script...")

execute_process (
  WORKING_DIRECTORY ${libhdf5_SOURCE_DIR}
  COMMAND ./configure --prefix=${libhdf5_prefix} --includedir=${libhdf5_prefix}/hdf5
  TIMEOUT 180
  ERROR_VARIABLE error_configure
  )

if (error_configure)
  message (STATUS "There was an error running the HDF5 configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] make

message (STATUS "Building HDF5 library...")

execute_process (
  WORKING_DIRECTORY ${libhdf5_SOURCE_DIR}
  COMMAND make 
  TIMEOUT 3600
  ERROR_VARIABLE error_make
  )

if (error_make)
  message (STATUS "There was an error building/installing the HDF5 library!")
  message (SEND_ERROR "${error_make}")
endif (error_make)

## [3] make install

if (NOT error_make)

message (STATUS "Installing HDF5 library...")

execute_process (
  WORKING_DIRECTORY ${libhdf5_SOURCE_DIR}
  COMMAND make install
  TIMEOUT 180
  ERROR_VARIABLE error_make_install
  )

if (error_make_install)
  message (STATUS "There was an error building/installing the HDF5 library!")
  message (SEND_ERROR "${error_make_install}")
endif (error_make_install)

endif (NOT error_make)

## <-- end build condition ----------------------------------------------------->

endif (NOT HAVE_HDF5)
