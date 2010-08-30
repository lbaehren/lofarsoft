
## Name of the project
project (BOOST)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BOOST_FORCE_BUILD       "Force rebuild and local installation?"  NO )
option (BOOST_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (BOOST_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BOOST_SOURCE_DIR}
  ${BOOST_SOURCE_DIR}/..
  ${BOOST_SOURCE_DIR}/../..
  ${BOOST_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "[Boost++] Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (BOOST_FIND_QUIETLY YES )

include (FindBoost)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  
  ## Package information

  set (Boost_VERSION        "1_43_0" )
  set (Boost_SOURCE_ARCHIVE "boost_${Boost_VERSION}.tar.gz" )
  set (Boost_URL            "http://downloads.sourceforge.net/project/boost/boost/1.43.0/${Boost_SOURCE_ARCHIVE}" )

  ## Check for package source
  
  find_file (Boost_SOURCE ${Boost_SOURCE_ARCHIVE}
    PATHS ${Boost_SOURCE_DIR}
    )
  
  if (NOT Boost_SOURCE)
    set (Boost_SOURCE ${Boost_URL})
  endif (NOT Boost_SOURCE)
  
  ExternalProject_Add (boost
    PREFIX ${PROJECT_BINARY_DIR}
    DOWNLOAD_DIR download
    SOURCE_DIR source
    URL ${Boost_SOURCE}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/bootstrap.sh
    BUILD_COMMAND ${PROJECT_BINARY_DIR}/source/bjam
    INSTALL_COMMAND ${PROJECT_BINARY_DIR}/source/bjam install --prefix=${USG_INSTALL_PREFIX}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  add_custom_target (boost
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Boost++] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Boost++] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
