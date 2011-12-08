
include (ExternalProject)

## ==============================================================================
##
##                                                   $LOFARSOFT/external/num_util
##
## ==============================================================================

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (num_util
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:BOOL=${LUS_CMAKE_MODULES}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (num_util
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/num_util/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:BOOL=${LUS_CMAKE_MODULES}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

