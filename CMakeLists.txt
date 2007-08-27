
project (LUS)

## ------------------------------------------------------------------------------
## 

option (BUILD_DAL "Create package for the Data Access Library" 0)
option (BUILD_CASACORE "Create package for casacore" 0)

## ------------------------------------------------------------------------------
## Package for the Data Access Library

if (BUILD_DAL)
  ## installation location
  set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR}/release)
  ## external components using CMake
  add_subdirectory (external/wcslib)
  add_subdirectory (external/cfitsio)
  add_subdirectory (external/casacore)
  ## external components using their own configuration
#  add_subdirectory (external/boost)
  add_subdirectory (external/hdf5)
  ## The Data Access Library
  add_subdirectory (src/DAL)
endif (BUILD_DAL)

## ------------------------------------------------------------------------------
## Package for casacore

if (BUILD_CASACORE)
  ## installation location
  set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR}/release) 
  ## required external components
  add_subdirectory (external/wcslib)
  add_subdirectory (external/cfitsio)
  ## casacore
  add_subdirectory (external/casacore)
endif (BUILD_CASACORE)
