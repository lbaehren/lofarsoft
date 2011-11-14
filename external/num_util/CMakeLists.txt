
include (ExternalProject)

## ==============================================================================
##
##                                                     $LOFARSOFT/external/wcslib
##
## ==============================================================================

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (num_util
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (num_util
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/num_util/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

