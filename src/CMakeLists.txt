
include (ExternalProject)

##_______________________________________________________________________________
## Sub-project: Collection of contributed tools and script

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  
  add_subdirectory (contrib)
  
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

  ## Package handling instructions
  ExternalProject_Add (contrib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/contrib
    DOWNLOAD_DIR download
    SOURCE_DIR source
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
  
  ## Package dependencies
  add_dependencies (contrib dal)

endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
