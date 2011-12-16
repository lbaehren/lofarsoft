
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (NUM_UTIL_FORCE_BUILD       "Force rebuild and local installation?"      NO  )
option (NUM_UTIL_CMAKE_BUILD       "Use CMake to configure and build NUM_UTIL?" YES )
option (NUM_UTIL_VERBOSE_CONFIGURE "Be verbose during configuration?"           NO  )
option (NUM_UTIL_VERBOSE_BUILD     "Be verbose during build process?"           NO  )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    ${PROJECT_SOURCE_DIR}/../../../..
    ENV LOFARSOFT
    )
  
  if (LUS_SOURCE_DIR)
    ## Load common CMake settings
    include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
    ## Set verbosity level of search modules
    if (NOT NUM_UTIL_VERBOSE_CONFIGURE)
      set (BOOST_FIND_QUIETLY     YES )
      set (NUMPY_FIND_QUIETLY     YES )
      set (NUM_UTIL_FIND_QUIETLY  YES )
      set (PYTHON_FIND_QUIETLY    YES )
    endif (NOT NUM_UTIL_VERBOSE_CONFIGURE)
    ## Load search modules
    find_package (Boost)
    find_package (NumPy)
    find_package (NumUtil)
  else (LUS_SOURCE_DIR)
    message (WARNING "[num_util] Failed to locate custom CMake modules!")
  endif (LUS_SOURCE_DIR)
endif (NOT LUS_SOURCE_DIR)

## Check if required external packages are available

if (NOT BOOST_FOUND)
  message (WARNING "[num_util] Missing installation of Boost++")
endif (NOT BOOST_FOUND)

if (NOT NUMPY_FOUND)
  message (WARNING "[num_util] Missing installation of NumPy")
endif (NOT NUMPY_FOUND)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

#  simple_ext.so
#  |-- num_util.o
#  `-- simpletest.o           [T]
#      |-- num_util.o         [T]
#      |   |-- num_util.cpp
#      |   `-- num_util.h
#      |-- num_util.h
#      `-- simpletest.cpp

include_directories (
  ${BOOST_INCLUDES}
  ${NUMPY_INCLUDES}
  ${NUMPY_INCLUDES}/numpy
  ${PYTHON_INCLUDES}
  )

add_definitions (-fPIC)

add_library (num_util STATIC
  num_util.cpp
  )

target_link_libraries (num_util
  ${BOOST_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## Installation of the library
install (TARGETS num_util
  DESTINATION lib
  )

## Installation of the header files
install (FILES num_util.h
  DESTINATION include/python/num_util
  )

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "------------------------------------------------------------")
message (STATUS "[num_util] Configuration summary.")
message (STATUS "------------------------------------------------------------")
message (STATUS " System configuration:"                                      )
message (STATUS " .. Processor type ............. = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " .. CMake executable ........... = ${CMAKE_COMMAND}"         )
message (STATUS " .. CMake version .............. = ${CMAKE_VERSION}"         )
message (STATUS " .. System name ................ = ${CMAKE_SYSTEM}"          )
message (STATUS " .. C++ compiler ............... = ${CMAKE_CXX_COMPILER}"    )
message (STATUS " .. C compiler ................. = ${CMAKE_C_COMPILER}"      )
message (STATUS " .. size(void*) ................ = ${CMAKE_SIZEOF_VOID_P}"   )
message (STATUS " Required external packages:"                                )
message (STATUS " .. Boost++ .................... = ${BOOST_FOUND}"           )
message (STATUS "    - Include directory ........ = ${BOOST_INCLUDES}"        )
message (STATUS "------------------------------------------------------------")
