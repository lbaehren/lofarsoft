
include (ExternalProject)

## ==============================================================================
##
##                                                     $LOFARSOFT/external/wcslib
##
## ==============================================================================

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/wcslib.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/wcslib.cmake
  )

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (wcslib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -C ${CMAKE_CURRENT_BINARY_DIR}/wcslib.cmake
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (wcslib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/wcslib/src
    CMAKE_ARGS -C ${CMAKE_CURRENT_BINARY_DIR}/wcslib.cmake
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

