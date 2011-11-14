
include (ExternalProject)

## ==============================================================================
##
##                                                  $LOFARSOFT/external/startools
##
## ==============================================================================

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (startools
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (startools
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/startools/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

