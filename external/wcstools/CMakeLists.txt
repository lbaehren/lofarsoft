
include (ExternalProject)

## ==============================================================================
##
##                                                        $LOFARSOFT/src/Anaamika
##
## ==============================================================================

##__________________________________________________________
## Options


##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ExternalProject_Add (wcstools
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
    DOWNLOAD_DIR download
    SOURCE_DIR source
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
  message (STATUS "[wcstools] Unable to find source directory!")
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)

##__________________________________________________________
## Package dependencies

