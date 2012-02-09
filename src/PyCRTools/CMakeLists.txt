## Name of the project
project (PyCRTools)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

if (LUS_SOURCE_DIR)
  include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
else (LUS_SOURCE_DIR)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_SOURCE_DIR)

##-------------------------------------------------------------------------------
## Find libraries

include (FindBoost)
include (FindHDF5)
include (FindDAL1)
include (FindCASACORE)
include (FindFFTW3)
include (FindGSL)
include (FindSphinx)
include (FindPython)
include (FindNumPy)
include (FindNumUtil)
include (FindOpenMP)
include (FindTMF)
include (FindCRTools)
include (FindWCSLIB)
include (FindCFITSIO)

##-------------------------------------------------------------------------------
## Testing

message (STATUS "PYCRTOOLS_ENABLE_TESTING: ${PYCRTOOLS_ENABLE_TESTING}")
message (STATUS "PYCRTOOLS_WITH_GSL: ${PYCRTOOLS_WITH_GSL}")

if (PYCRTOOLS_ENABLE_TESTING)
  set (UPDATE_TYPE "svn")
  set (CTEST_TESTING_TIMEOUT "500")
  set (DART_TESTING_TIMEOUT 500)
  include (CTest)
  enable_testing()
endif (PYCRTOOLS_ENABLE_TESTING)

##-------------------------------------------------------------------------------
## Options

# Debug level
set (PYCRTOOLS_DEBUG_LEVEL 0 CACHE STRING "PyCRTools debugging level")

# Turn off all predefined options
set (CMAKE_CXX_FLAGS "")
set (PYCRTOOLS_INCLUDES)
set (PYCRTOOLS_LIBRARIES)

if (APPLE)
  set (PYCRTOOLS_CXX_FLAGS "-fpermissive")
  set (PYCRTOOLS_LINKER_FLAGS "-fPIC -flat_namespace -Wl")
else (APPLE)
  set (PYCRTOOLS_CXX_FLAGS "")
  set (PYCRTOOLS_LINKER_FLAGS "-fPIC -shared -Wl")
endif (APPLE)

if (PYCRTOOLS_WITH_COMPILER_WARNINGS)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -Wall")
endif (PYCRTOOLS_WITH_COMPILER_WARNINGS)

if (PYCRTOOLS_WITH_RUNTIME_WARNINGS)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -DWARNINGS")
endif (PYCRTOOLS_WITH_RUNTIME_WARNINGS)

if (PYCRTOOLS_WITH_DEBUG_SYMBOLS)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -g -DH_DEBUG_CPP")
else (PYCRTOOLS_WITH_DEBUG_SYMBOLS)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -g0")
endif (PYCRTOOLS_WITH_DEBUG_SYMBOLS)

if (PYCRTOOLS_WITH_OPTIMIZATIONS)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -O2")
  if (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS)
    message (STATUS "Attempting to find best optimization settings for current CPU architecture.")

    include (OptimizeForArchitecture)
    OptimizeForArchitecture()

    set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -ftree-vectorize")
  endif (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS)
endif (PYCRTOOLS_WITH_OPTIMIZATIONS)

##-------------------------------------------------------------------------------
## Optional dependencies

if (OPENMP_FOUND)
  message (STATUS "[PyCRTools] OpenMP requested and enabled")
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (PYCRTOOLS_CPP_FLAGS "${PYCRTOOLS_CPP_FLAGS} ${OpenMP_CXX_FLAGS} ")
  set (PYCRTOOLS_LINKER_FLAGS "${PYCRTOOLS_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (OPENMP_FOUND)

if (PYCRTOOLS_WITH_AERA)
  include (FindAERA)
  if (AERA_FOUND)
    message (STATUS "[PyCRTools] AERA support requested and enabled")
    list (APPEND PYCRTOOLS_INCLUDES ${AERA_INCLUDES})
    list (APPEND PYCRTOOLS_LIBRARIES ${AERA_LIBRARIES})
  else (AERA_FOUND)
    message (WARNING "[PyCRTools] AERA support requested but not enabled: AERA not found")
  endif (AERA_FOUND)
endif (PYCRTOOLS_WITH_AERA)

## _____________________________________________________________________________
## Configuration of source files

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/pycrtools_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/pycrtools_config.h
)

##_______________________________________________________________________________
## Pre-processing of the source files

set (PYCRTOOLS_PREPROCESSOR_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/preprocessor
  )

set (PYCRTOOLS_CPP_FILES
  core
  legacy
  ElementResponse
  )

set (PYCRTOOLS_CPP_MODULE_PREFIX "m")

set (PYCRTOOLS_CPP_MODULES
  Array
  Vector
  Math
  FFT
  FFTW
  Filter
  Fitting
  IO
  RF
  EndPointRadiation
  TBB
  Imaging
  Calibration
  )

list (APPEND PYCRTOOLS_CPP_MODULES Bindings)

string (STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)
string (STRIP "${PYCRTOOLS_CPP_FLAGS}" PYCRTOOLS_CPP_FLAGS)
string (STRIP "${PYCRTOOLS_CXX_FLAGS}" PYCRTOOLS_CXX_FLAGS)

message (STATUS "Preprocessing PyCRTools with flags: ${CMAKE_CXX_FLAGS} ${PYCRTOOLS_CPP_FLAGS}")
message (STATUS "Compiling PyCRTools with flags: ${CMAKE_CXX_FLAGS} ${PYCRTOOLS_CXX_FLAGS}")

##_______________________________________________________________________________
## Perform Wrapper Preprocessing of module files

foreach (FILE ${PYCRTOOLS_CPP_MODULES})
  ## ________________________________________________________________________
  ##                                                             Code parsing
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.cc
            ${PYCRTOOLS_PREPROCESSOR_DIR}/crtools_code_parser.py
    COMMAND python ${PYCRTOOLS_PREPROCESSOR_DIR}/crtools_code_parser.py -q ${CMAKE_CURRENT_SOURCE_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.cc ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Preprocessing code"
    )

  ## ________________________________________________________________________
  ##                                                       Interface checking
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.def.h
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc
    COMMAND ${PYCRTOOLS_PREPROCESSOR_DIR}/hfCheckInterfaceChange.sh ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}
    COMMENT "Wrapper preprocessing module ${FILE}: Checking interfaces of wrappers"
    )
endforeach (FILE ${PYCRTOOLS_CPP_MODULES})

foreach (FILE ${PYCRTOOLS_CPP_MODULES})
  ## ________________________________________________________________________
  ##                           Check all header files for the bindings module
  file (GLOB PYCRTOOLS_CPP_MODULE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}*.h)
  if (FILE STREQUAL "Bindings")
    file (GLOB PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}*.def.h)
  else (FILE STREQUAL "Bindings")
    set (PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES
      "${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.def.h"
      "${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}Array.def.h"
      "${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}Math.def.h")
  endif (FILE STREQUAL "Bindings")

  ## ________________________________________________________________________
  ##                                                 execute gcc preprocessor
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc
            ${PYCRTOOLS_CPP_MODULE_HEADER_FILES}
            ${PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/hfppnew-generatewrappers.def
            ${CMAKE_CURRENT_SOURCE_DIR}/core.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/pycrtools_config.h.in
            ${CMAKE_CURRENT_SOURCE_DIR}/hfppnew.h
            ${CMAKE_CURRENT_SOURCE_DIR}/numpy_ptr.h
            ${CMAKE_CURRENT_SOURCE_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}Module.h
            ${PYCRTOOLS_PREPROCESSOR_DIR}/crtools_newline_parser.py
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP ${PYCRTOOLS_CPP_FLAGS} -w -I${FFTW3_INCLUDES} -I${HDF5_INCLUDES} -I${DAL1_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${TMF_INCLUDES} -I${CASACORE_INCLUDES} -I${CMAKE_CURRENT_BINARY_DIR} -I${NUM_UTIL_INCLUDES} -I${NUMPY_INCLUDES} -I${WCSLIB_INCLUDES} -I${CFITSIO_INCLUDES} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMAND python ${PYCRTOOLS_PREPROCESSOR_DIR}/crtools_newline_parser.py ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.wpp.cc ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.cc
    COMMAND rm -r ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Executing wrapper preprocessor"
    )

endforeach (FILE ${PYCRTOOLS_CPP_MODULES})

##_______________________________________________________________________________
## Create the library

include_directories (
  ${FFTW3_INCLUDES}
  ${HDF5_INCLUDES}
  ${DAL1_INCLUDES}
  ${PYTHON_INCLUDES}
  ${BOOST_INCLUDES}
  ${TMF_INCLUDES}
  ${CASACORE_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${NUM_UTIL_INCLUDES}
  ${NUMPY_INCLUDES}
  ${WCSLIB_INCLUDES}
  ${CFITSIO_INCLUDES}
  )

# Core files
foreach (FILE ${PYCRTOOLS_CPP_FILES} )
  set (PREPROCESSED_FILES
    ${PREPROCESSED_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.cc
    )
endforeach (FILE ${PYCRTOOLS_CPP_FILES})

# Module files
foreach (FILE ${PYCRTOOLS_CPP_MODULES} )
  set (PREPROCESSED_FILES
    ${PREPROCESSED_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.cc
    )
endforeach (FILE ${PYCRTOOLS_CPP_MODULES})


add_library (hftools MODULE
  ${PREPROCESSED_FILES}
  )

# if (PYCRTOOLS_WITH_AERA AND AERA_FOUND)
#   add_dependencies(hftools aera)
# endif(PYCRTOOLS_WITH_AERA AND AERA_FOUND)

target_link_libraries (hftools
  ${CRTOOLS_LIBRARIES}
  ${TMF_LIBRARIES}
  ${DAL1_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${BOOST_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${FFTW3_LIBRARIES}
  ${GSL_LIBRARIES}
  ${NUM_UTIL_LIBRARIES}
  ${PYCRTOOLS_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  )

##to speed up computations use -g -O2 or -g0 -O3 if no debuggng is needed.
## additional target properties
set_target_properties (hftools
  PROPERTIES
  PREFIX "_"
  SUFFIX .so
  COMPILE_FLAGS "${PYCRTOOLS_CXX_FLAGS}"
  LINK_FLAGS "${PYCRTOOLS_LINKER_FLAGS}"
  )

##_______________________________________________________________________________
## Create the documentation


foreach (FILE ${PYCRTOOLS_CPP_MODULES})

  list (APPEND PYCRTOOLS_PYTHON_DOCS ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.py)

endforeach (FILE)

##_______________________________________________________________________________
## Installation

# Install Python C++ modules
install (
  TARGETS hftools
  LIBRARY DESTINATION lib/python/pycrtools/core/hftools
  )

# Install Python modules
install (CODE "message (STATUS \"Installing Python modules...\")")
install (CODE "execute_process(COMMAND ${PYCRTOOLS_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/modules ${LUS_SOURCE_DIR}/release/lib/python/pycrtools)")

install (CODE "message (STATUS \"Installing Python module documentation...\")")
install (FILES ${PYCRTOOLS_PYTHON_DOCS} DESTINATION ${LUS_SOURCE_DIR}/release/lib/python/pycrtools/core/hftools)

# Install Python scripts
install (CODE "message (STATUS \"Installing Python scripts...\")")
install (CODE "execute_process(COMMAND ${PYCRTOOLS_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${LUS_SOURCE_DIR}/release/bin/python)")

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

message (STATUS "============================================================")
message (STATUS "  PyCRTools")
message (STATUS "------------------------------------------------------------")

message (STATUS "  Compiler warnings........ : " ${PYCRTOOLS_WITH_COMPILER_WARNINGS})
message (STATUS "  Runtime warnings..........: " ${PYCRTOOLS_WITH_RUNTIME_WARNINGS})
message (STATUS "  Debug symbols.............: " ${PYCRTOOLS_WITH_DEBUG_SYMBOLS})
message (STATUS "  Optimizations.............: " ${PYCRTOOLS_WITH_OPTIMIZATIONS})
message (STATUS "  CPU optimizations.........: " ${PYCRTOOLS_WITH_CPU_OPTIMIZATIONS})
message (STATUS "  OpenMP................... : " ${OPENMP_FOUND})

message (STATUS "------------------------------------------------------------")

message (STATUS "  AERA data support.........: " ${PYCRTOOLS_WITH_AERA} )
message (STATUS "  CASACore support..........: " ${PYCRTOOLS_WITH_CASACORE} )
message (STATUS "  DAL1 support...............: " ${PYCRTOOLS_WITH_DAL1} )
message (STATUS "  FFTW support..............: " ${PYCRTOOLS_WITH_FFTW} )
message (STATUS "  GSL support...............: " ${PYCRTOOLS_WITH_GSL} )
message (STATUS "  Numpy support.............: " ${PYCRTOOLS_WITH_NUMPY} )

message (STATUS "------------------------------------------------------------")
