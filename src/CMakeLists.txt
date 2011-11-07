
include (ExternalProject)

## ==============================================================================
##
##                                                         $LOFARSOFT/src/contrib
##
## ==============================================================================

##__________________________________________________________
## Build instructions

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

## ==============================================================================
##
##                                                              $LOFARSOFT/src/RM
##
## ==============================================================================

##__________________________________________________________
## Options

option (RM_ENABLE_ITPP       "Enable using IT++ library?"       NO  )
option (RM_ENABLE_ARMADILLO  "Enable using Armadillo library?"  YES )

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    DOWNLOAD_DIR download
    SOURCE_DIR source
    URL ${CMAKE_CURRENT_SOURCE_DIR}/RM
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DRM_ENABLE_ITPP=${RM_ENABLE_ITPP} -DRM_ENABLE_ARMADILLO=${RM_ENABLE_ARMADILLO}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    DOWNLOAD_DIR download
    SOURCE_DIR source
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/RM
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DRM_ENABLE_ITPP=${RM_ENABLE_ITPP} -DRM_ENABLE_ARMADILLO=${RM_ENABLE_ARMADILLO}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)

##__________________________________________________________
## Package dependencies

add_dependencies (rm dal)

if (RM_ENABLE_ARMADILLO AND EXISTS armadillo)
  add_dependencies (rm armadillo)
endif (RM_ENABLE_ARMADILLO AND EXISTS armadillo)

if (RM_ENABLE_ITPP)
  add_dependencies (rm itpp)
endif (RM_ENABLE_ITPP)

