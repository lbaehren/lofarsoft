
include (ExternalProject)

## ==============================================================================
##
##                                                     $LOFARSOFT/external/wcslib
##
## ==============================================================================

##__________________________________________________________
## Options


##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (wcslib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  message (STATUS "[wcslib] Unable to find source directory!")
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

