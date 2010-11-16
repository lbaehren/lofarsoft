## Name of the project
project (TMF)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${DAL_SOURCE_DIR}
  ${DAL_SOURCE_DIR}/..
  ${DAL_SOURCE_DIR}/../..
  ${DAL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## Options
option (TMF_BUILD_TESTS       "Build tests?"                           YES )
option (TMF_BUILD_PYTMF       "Build PyTMF if possible?"               YES )

## List of source files
FILE (GLOB tmf_sources src/*.cc)

## List of header files
FILE (GLOB tmf_headers src/*.h)

## Includes
include_directories (src)

## Build the library
add_library (tmf ${tmf_sources})

## Add definitions
add_definitions (-fPIC -O2)

## Library versioning
set_target_properties (tmf PROPERTIES VERSION 1.0)

## Install libray
install (TARGETS tmf
  DESTINATION lib
  )

## Install header files
install (FILES ${tmf_headers}
  DESTINATION include
  )

## Tests
if (TMF_BUILD_TESTS)
    add_subdirectory (test)
endif (TMF_BUILD_TESTS)

## Python bindings
if (TMF_BUILD_PYTMF)
    include (FindPython)
    include (FindBoost)

    if (HAVE_PYTHON AND HAVE_BOOST)
        add_subdirectory (pytmf)
    endif (HAVE_PYTHON AND HAVE_BOOST)
endif (TMF_BUILD_PYTMF)

