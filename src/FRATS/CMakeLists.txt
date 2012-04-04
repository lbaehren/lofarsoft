
## Name of the project
project (FRATS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/CMakeSettings.cmake
    PATHS
    ${CR_SOURCE_DIR}
    ${CR_SOURCE_DIR}/..
    ${CR_SOURCE_DIR}/../..
    ${CR_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )

  if (LUS_SOURCE_DIR)
    include (${LUS_SOURCE_DIR}/devel_common/cmake/CMakeSettings.cmake)
  else (LUS_SOURCE_DIR)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_SOURCE_DIR)
endif (NOT LUS_SOURCE_DIR)

