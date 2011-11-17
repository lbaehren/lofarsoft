
include (ExternalProject)

## ==============================================================================
##
##                                                        $LOFARSOFT/src/Anaamika
##
## ==============================================================================

##__________________________________________________________
## Options

option (ANAAMIKA_ENABLE_SCIPY   "Build SciPy as interactive environment?"  NO  )
option (ANAAMIKA_ENABLE_FBDSM   "Build Fortran-based version of BDSM?"     NO  )
option (ANAAMIKA_ENABLE_PYBDSM  "Build Python-enabled version of BDSM?"    YES )

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)
  ExternalProject_Add (anaamika
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Anaamika
    URL ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DANAAMIKA_ENABLE_SCIPY=${ANAAMIKA_ENABLE_SCIPY}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)
  ExternalProject_Add (anaamika
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Anaamika
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/Anaamika
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DANAAMIKA_ENABLE_SCIPY=${ANAAMIKA_ENABLE_SCIPY}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Anaamika)

##__________________________________________________________
## Package dependencies

add_dependencies (anaamika wcslib cfitsio boost num_util pyfits)

if (ANAAMIKA_ENABLE_SCIPY)
  add_dependencies (anaamika scipy)
endif (ANAAMIKA_ENABLE_SCIPY)

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
    URL ${CMAKE_CURRENT_SOURCE_DIR}/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)
  ExternalProject_Add (contrib
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/contrib
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/contrib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

## Package dependencies
add_dependencies (contrib dal)

## ==============================================================================
##
##                                                            $LOFARSOFT/src/LASA
##
## ==============================================================================

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)
  ExternalProject_Add (lasa
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LASA
    URL ${CMAKE_CURRENT_SOURCE_DIR}/LASA
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)
  ExternalProject_Add (lasa
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LASA
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/LASA
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    CMAKE_CACHE_ARGS -DLUS_CMAKE_MODULES:PATH=${LUS_CMAKE_MODULES}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LASA)

## ==============================================================================
##
##                                                              $LOFARSOFT/src/RM
##
## ==============================================================================

##__________________________________________________________
## Options

option (RM_WITH_ITPP       "Enable using IT++ library?"       NO  )
option (RM_WITH_ARMADILLO  "Enable using Armadillo library?"  YES )

##__________________________________________________________
## Build instructions

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    URL ${CMAKE_CURRENT_SOURCE_DIR}/RM
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DRM_WITH_ITPP=${RM_WITH_ITPP} -DRM_WITH_ARMADILLO=${RM_WITH_ARMADILLO}
    )
else (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)
  ExternalProject_Add (rm
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RM
    SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/src/RM
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DRM_WITH_ITPP=${RM_WITH_ITPP} -DRM_WITH_ARMADILLO=${RM_WITH_ARMADILLO}
    )
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RM)

##__________________________________________________________
## Package dependencies

add_dependencies (rm wcslib cfitsio dal)

if (RM_WITH_ARMADILLO)
  add_dependencies (rm armadillo)
endif (RM_WITH_ARMADILLO)

if (RM_WITH_ITPP)
  add_dependencies (rm itpp)
endif (RM_WITH_ITPP)

