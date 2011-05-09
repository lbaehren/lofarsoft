
message (STATUS "[CR-Tools] Adding implement/Pypeline ...")

##-------------------------------------------------------------------------------
## Find libraries
include (FindPython)
include (FindBoost)
include (FindNumPy)
include (FindNumUtil)
include (FindAERA)

##-------------------------------------------------------------------------------
## Options

option (PYPELINE_WITH_COMPILER_WARNINGS "Compile Pypeline with compiler warnings enabled." ON)
option (PYPELINE_WITH_RUNTIME_WARNINGS "Compile Pypeline with runtime warnings enabled." ON)
option (PYPELINE_WITH_DEBUG_SYMBOLS "Compile Pypeline with debug symbols." ON)
option (PYPELINE_WITH_OPTIMIZATION "Compile Pypeline with optimization flags turned on." OFF)
option (PYPELINE_WITH_CPU_OPTIMIZATION "Compile Pypeline with optimization for CPU architecture." OFF)
option (PYPELINE_WITH_AERA "Compile Pypeline with support for AERA data files" OFF)

option (PYCRTOOLS_WITH_WARNINGS "" ON)
option (PYCRTOOLS_WITH_NUMPY "" ON)
option (PYCRTOOLS_WITH_CASACORE "" ON)
option (PYCRTOOLS_WITH_DAL "" ON)
option (PYCRTOOLS_WITH_GSL "" ON)
option (PYCRTOOLS_WITH_FFTW "" ON)
option (PYCRTOOLS_WITH_AERA "" ON)

# Debug level
set (PYCRTOOLS_DEBUG_LEVEL 0)

# Configuration file
configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/pycrtools_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/pycrtools_config.h
)

# Turn off all predefined options
set (CMAKE_CXX_FLAGS "")

if (APPLE)
  set (PYPELINE_CXX_FLAGS "-fpermissive")
  set (PYPELINE_LINKER_FLAGS "-fPIC -flat_namespace -Wl")
else (APPLE)
  set (PYPELINE_CXX_FLAGS "")
  set (PYPELINE_LINKER_FLAGS "-fPIC -shared -Wl")
endif (APPLE)

if (PYPELINE_WITH_COMPILER_WARNINGS)
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -Wall")
endif (PYPELINE_WITH_COMPILER_WARNINGS)

if (PYPELINE_WITH_RUNTIME_WARNINGS)
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -DWARNINGS")
endif (PYPELINE_WITH_RUNTIME_WARNINGS)

if (PYPELINE_WITH_DEBUG_SYMBOLS)
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -g -DH_DEBUG_CPP")
else (PYPELINE_WITH_DEBUG_SYMBOLS)
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -g0")
endif (PYPELINE_WITH_DEBUG_SYMBOLS)

if (PYPELINE_WITH_OPTIMIZATION)
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -O2")
  if (PYPELINE_WITH_CPU_OPTIMIZATION)
    message (STATUS "Attempting to find best optimization settings for current CPU architecture.")

    include (OptimizeForArchitecture)
    OptimizeForArchitecture()

    set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} -ftree-vectorize")
  endif (PYPELINE_WITH_CPU_OPTIMIZATION)
endif (PYPELINE_WITH_OPTIMIZATION)

##-------------------------------------------------------------------------------
## Optional dependencies


if (CR_WITH_OPENMP AND OPENMP_FOUND)
  message (STATUS "[CR-Tools] Adding OpenMP support to Pypeline.")
  set (PYPELINE_CXX_FLAGS "${PYPELINE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (PYPELINE_CPP_FLAGS ${OpenMP_CXX_FLAGS})
  set (PYPELINE_LINKER_FLAGS "${PYPELINE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (CR_WITH_OPENMP AND OPENMP_FOUND)

if (PYPELINE_WITH_AERA AND AERA_FOUND)
  set (PYPELINE_CXX_FLAGS "-DHAVE_AERA ${PYPELINE_CXX_FLAGS}")
  set (PYPELINE_CPP_FLAGS "-DHAVE_AERA ${PYPELINE_CPP_FLAGS}")
  list (APPEND PYPELINE_INCLUDES ${AERA_INCLUDES})
  list (APPEND PYPELINE_LIBRARIES ${AERA_LIBRARIES})
endif (PYPELINE_WITH_AERA AND AERA_FOUND)

##_______________________________________________________________________________
## Pre-processing of the source files

set (PYPELINE_PREPROCESSOR_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/preprocessor
  )

set (PYPELINE_CPP_FILES
  core
  )

set (PYPELINE_CPP_MODULE_PREFIX "m")

set (PYPELINE_CPP_MODULES
  Array
  Vector
  Math
  FFT
  FFTW
  Filter
  Fitting
  Imaging
  IO
  RF
  Numpy
  EndPointRadiation
  TBB
  )

if (PYPELINE_WITH_AERA AND AERA_FOUND)
  list (APPEND PYPELINE_CPP_MODULES IO_AERA)
endif (PYPELINE_WITH_AERA AND AERA_FOUND)

list (APPEND PYPELINE_CPP_MODULES Bindings)

foreach (include ${PYPELINE_INCLUDES})
  set (PYPELINE_INCLUDE_FLAGS "-I${include} ${PYPELINE_INCLUDE_FLAGS}")
endforeach (include ${PYPELINE_INCLUDES})

message (STATUS "Preprocessing Pypeline with flags: ${CMAKE_CXX_FLAGS} ${PYPELINE_CPP_FLAGS}")
message (STATUS "Compiling Pypeline with flags: ${CMAKE_CXX_FLAGS} ${PYPELINE_CXX_FLAGS}")

##_______________________________________________________________________________
## Perform Wrapper Preprocessing of module files

foreach (FILE ${PYPELINE_CPP_MODULES})
  ## ________________________________________________________________________
  ##                                                             Code parsing
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.pp.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
            ${PYPELINE_PREPROCESSOR_DIR}/crtools_code_parser.py
    COMMAND python ${PYPELINE_PREPROCESSOR_DIR}/crtools_code_parser.py -q ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.pp.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Preprocessing code"
    )

  ## ________________________________________________________________________
  ##                                                       Interface checking
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.def.h
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.pp.cc
    COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCheckInterfaceChange.sh ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}
    COMMENT "Wrapper preprocessing module ${FILE}: Checking interfaces of wrappers"
    )
endforeach (FILE ${PYPELINE_CPP_MODULES})

foreach (FILE ${PYPELINE_CPP_MODULES})
  ## ________________________________________________________________________
  ##                           Check all header files for the bindings module
  file (GLOB PYPELINE_CPP_MODULE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}*.h)
  if (FILE STREQUAL "Bindings")
    file (GLOB PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}*.def.h)
  else (FILE STREQUAL "Bindings")
    set (PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES
      "${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.def.h"
      "${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}Array.def.h"
      "${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}Math.def.h")
  endif (FILE STREQUAL "Bindings")

  ## ________________________________________________________________________
  ##                                                 execute gcc preprocessor
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.pp.cc
            ${PYPELINE_CPP_MODULE_HEADER_FILES}
            ${PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/hfppnew-generatewrappers.def
            ${CMAKE_CURRENT_SOURCE_DIR}/core.h
            ${CMAKE_CURRENT_SOURCE_DIR}/hfppnew.h
            ${CMAKE_CURRENT_SOURCE_DIR}/numpy_ptr.h
            ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}Module.h
            ${PYPELINE_PREPROCESSOR_DIR}/crtools_newline_parser.py
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP ${PYPELINE_CPP_FLAGS} -w -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${TMF_INCLUDES} -I${CASACORE_INCLUDES} -I${CMAKE_CURRENT_BINARY_DIR} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} -I${NUM_UTIL_INCLUDES} -I${NUMPY_INCLUDES} ${PYPELINE_INCLUDE_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.pp.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMAND python ${PYPELINE_PREPROCESSOR_DIR}/crtools_newline_parser.py ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    COMMAND rm -r ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Executing wrapper preprocessor"
    )

endforeach (FILE ${PYPELINE_CPP_MODULES})

##_______________________________________________________________________________
## Create the library

include_directories (
  ${CR_SOURCE_DIR}/implement/Pypeline
  ${CMAKE_CURRENT_BINARY_DIR}
  ${NUMPY_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${PYPELINE_INCLUDES}
  )

# Core files
foreach (FILE ${PYPELINE_CPP_FILES} )
  set (PREPROCESSED_FILES
    ${PREPROCESSED_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.cc
    )
endforeach (FILE ${PYPELINE_CPP_FILES})

# Module files
foreach (FILE ${PYPELINE_CPP_MODULES} )
  set (PREPROCESSED_FILES
    ${PREPROCESSED_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    )
endforeach (FILE ${PYPELINE_CPP_MODULES})


add_library (hftools MODULE
  ${PREPROCESSED_FILES}
  )

target_link_libraries (hftools
  cr
  ${TMF_LIBRARIES}
  ${DAL_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${BOOST_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${FFTW3_LIBRARIES}
  ${GSL_LIBRARIES}
  ${NUM_UTIL_LIBRARIES}
  ${PYPELINE_LIBRARIES}
  )

##to speed up computations use -g -O2 or -g0 -O3 if no debuggng is needed.
## additional target properties
set_target_properties (hftools
  PROPERTIES
  PREFIX "_"
  SUFFIX .so
  COMPILE_FLAGS "${PYPELINE_CXX_FLAGS}"
  LINK_FLAGS "${PYPELINE_LINKER_FLAGS}"
  )

##_______________________________________________________________________________
## Create the documentation


foreach (FILE ${PYPELINE_CPP_MODULES})

  list (APPEND PYPELINE_PYTHON_DOCS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.py)

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
install (CODE "execute_process(COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/modules ${LUS_ROOT}/release/lib/python/pycrtools)")

install (CODE "message (STATUS \"Installing Python module documentation...\")")
install (FILES ${PYPELINE_PYTHON_DOCS} DESTINATION ${LUS_ROOT}/release/lib/python/pycrtools/core/hftools)

# Install Python scripts
install (CODE "message (STATUS \"Installing Python scripts...\")")
install (CODE "execute_process(COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${LUS_ROOT}/release/bin/python)")

## =============================================================================
##
##  Configuration summary
##
## =============================================================================

message (STATUS "============================================================")
message (STATUS "  CR-Tools: PyCRTools")
message (STATUS "------------------------------------------------------------")

message (STATUS "  AERA..................... : " ${AERA_FOUND})
message (STATUS "  OpenMP................... : " ${OPENMP_FOUND})

message (STATUS "------------------------------------------------------------")

message (STATUS "  Compiler warnings........ : " ${PYPELINE_WITH_COMPILER_WARNINGS})
message (STATUS "  Runtime warnings..........: " ${PYPELINE_WITH_RUNTIME_WARNINGS})
message (STATUS "  Debug symbols.............: " ${PYPELINE_WITH_DEBUG_SYMBOLS})
message (STATUS "  Optimization..............: " ${PYPELINE_WITH_OPTIMIZATION})
message (STATUS "  CPU optimization..........: " ${PYPELINE_WITH_CPU_OPTIMIZATION})

message (STATUS "------------------------------------------------------------")

