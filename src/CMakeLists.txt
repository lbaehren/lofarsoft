
include (ExternalProject)

##_______________________________________________________________________________
## Sub-project: Collection of contributed tools and script

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  ExternalProject_Add (contrib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/contrib
    DOWNLOAD_DIR download
    SOURCE_DIR source
    URL ${CMAKE_CURRENT_SOURCE_DIR}/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  ExternalProject_Add (contrib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/contrib
    DOWNLOAD_DIR download
    SOURCE_DIR source
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

## Package dependencies
add_dependencies (contrib dal)

