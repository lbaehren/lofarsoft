
project (PyCRTools)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

option (PYCRTOOLS_ENABLE_TESTING         "Enable testing for the PyCRTools?"                         YES )
option (PYCRTOOLS_WITH_OPENMP            "Compile PyCRTools with OpenMP enabled?"                    YES )
option (PYCRTOOLS_WITH_COMPILER_WARNINGS "Compile PyCRTools with compiler warnings enabled?"         YES )
option (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS "Compile PyCRTools with optimization for CPU architecture?" NO )
option (PYCRTOOLS_WITH_DEBUG_SYMBOLS     "Compile PyCRTools with debug symbols?"                     YES )
option (PYCRTOOLS_WITH_OPTIMIZATIONS     "Compile PyCRTools with optimization flags turned on?"      YES )
option (PYCRTOOLS_WITH_RUNTIME_WARNINGS  "Compile PyCRTools with runtime warnings enabled?"          YES )
option (PYCRTOOLS_WITH_AERA              "Compile PyCRTools with AERA data support?"                 NO )
option (PYCRTOOLS_WITH_CASACORE          "Compile PyCRTools with CASACore support?"                  YES )
option (PYCRTOOLS_WITH_DAL1              "Compile PyCRTools with DAL1 support?"                      YES )
option (PYCRTOOLS_WITH_FFTW              "Compile PyCRTools with FFTW support?"                      YES )
option (PYCRTOOLS_WITH_GSL               "Compile PyCRTools with GSL support?"                       YES )
option (PYCRTOOLS_WITH_NUMPY             "Compile PyCRTools with Numpy support?"                     YES )

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
include (FindDAL)
include (FindDAL1)
include (FindCASACORE)
include (FindFFTW3)
include (FindGSL)
include (FindSphinx)
include (FindPython)
include (FindNumPy)
include (FindNumUtil)
include (FindTMF)
include (FindWCSLIB)
include (FindCFITSIO)

if (PYCRTOOLS_WITH_OPENMP)
    include (FindOpenMP)
endif (PYCRTOOLS_WITH_OPENMP)

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
#  set (PYCRTOOLS_CXX_FLAGS "-fpermissive")
#  set (PYCRTOOLS_LINKER_FLAGS "-fPIC -flat_namespace")
  set (PYCRTOOLS_CXX_FLAGS "")
  set (PYCRTOOLS_LINKER_FLAGS "-fPIC")
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

set (PYCRTOOLS_OPTIMIZATION_FLAGS_GCC_4_4 "-falign-functions -falign-jumps -falign-labels -falign-loops -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg -fcaller-saves -fcommon -fcprop-registers -fcrossjumping -fcse-follow-jumps -fcse-skip-blocks -fdce -fdefer-pop -fdelete-null-pointer-checks -fdse -fearly-inlining -fexpensive-optimizations -fforward-propagate -fgcse -fgcse-lm -fguess-branch-probability -fif-conversion -fif-conversion2 -findirect-inlining -finline-functions-called-once -finline-small-functions -fipa-cp -fipa-pure-const -fipa-reference -fivopts -fjump-tables -fmath-errno -fmerge-constants -fmove-loop-invariants -fno-threadsafe-statics -foptimize-register-move -foptimize-sibling-calls -fpeephole -fpeephole2 -fprefetch-loop-arrays -fregmove -frename-registers -freorder-blocks -freorder-functions -frerun-cse-after-loop -frtti -fsched-interblock -fsched-spec -fsched-stalled-insns-dep -fschedule-insns -fschedule-insns2 -fshort-enums -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types -fstrict-aliasing -fstrict-overflow -fthread-jumps -ftoplevel-reorder -ftrapping-math -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copy-prop -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-fre -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize -ftree-pre -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-sra -ftree-switch-conversion -ftree-ter -ftree-vect-loop-version -ftree-vrp -funit-at-a-time -fvar-tracking -fvect-cost-model -fweb")
set (PYCRTOOLS_OPTIMIZATION_FLAGS_GCC_4_6 "${PYCRTOOLS_OPTIMIZATION_FLAGS_GCC_4_4} -fcombine-stack-adjustments -fcompare-elim -fdevirtualize -fipa-profile -fipa-sra -fpartial-inlining -fsched-critical-path-heuristic -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec-insn-heuristic -ftree-bit-ccp -ftree-forwprop -ftree-loop-if-convert -ftree-phiprop -ftree-pta -ftree-slp-vectorize -fvar-tracking-assignments")

if (CMAKE_COMPILER_IS_GNUCC)
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if (PYCRTOOLS_WITH_OPTIMIZATIONS)
  if (APPLE)
    set (PYCRTOOLS_OPTIMIZATION_FLAGS "-O2")
  else (APPLE)
    if (GCC_VERSION)
      if (GCC_VERSION VERSION_EQUAL "4.4" OR GCC_VERSION VERSION_GREATER "4.4")
        if (GCC_VERSION VERSION_LESS "4.6")
          set (PYCRTOOLS_OPTIMIZATION_FLAGS "${PYCRTOOLS_OPTIMIZATION_FLAGS_GCC_4_4}")
        else (GCC_VERSION VERSION_LESS "4.6")
          set (PYCRTOOLS_OPTIMIZATION_FLAGS "${PYCRTOOLS_OPTIMIZATION_FLAGS_GCC_4_6}")
        endif (GCC_VERSION VERSION_LESS "4.6")
      else (GCC_VERSION VERSION_EQUAL "4.4" OR GCC_VERSION VERSION_GREATER "4.4")
        set (PYCRTOOLS_OPTIMIZATION_FLAGS "-O2")
      endif (GCC_VERSION VERSION_EQUAL "4.4" OR GCC_VERSION VERSION_GREATER "4.4")
    else (GCC_VERSION)
      set (PYCRTOOLS_OPTIMIZATION_FLAGS "-O2")
    endif (GCC_VERSION)
  endif (APPLE)
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} ${PYCRTOOLS_OPTIMIZATION_FLAGS}")
  if (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS)
    set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} -march=native -ftree-vectorize")
  endif (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS)
endif (PYCRTOOLS_WITH_OPTIMIZATIONS)

##-------------------------------------------------------------------------------
## Optional dependencies

if (OPENMP_FOUND AND PYCRTOOLS_WITH_OPENMP)
  message (STATUS "[PyCRTools] OpenMP requested and enabled")
  set (PYCRTOOLS_CXX_FLAGS "${PYCRTOOLS_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (PYCRTOOLS_CPP_FLAGS "${PYCRTOOLS_CPP_FLAGS} ${OpenMP_CXX_FLAGS} ")
  set (PYCRTOOLS_LINKER_FLAGS "${PYCRTOOLS_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (OPENMP_FOUND AND PYCRTOOLS_WITH_OPENMP)

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
    set (PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES)
    foreach (COMPONENT ${PYCRTOOLS_CPP_MODULES})
      if (NOT COMPONENT STREQUAL "Bindings")
	set (PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES "${PYCRTOOLS_CPP_MODULE_WRAPPER_HEADER_FILES}"
	  "${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${COMPONENT}.def.h")
      endif (NOT COMPONENT STREQUAL "Bindings")
    endforeach (COMPONENT)
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
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP ${PYCRTOOLS_CPP_FLAGS} -w -I${FFTW3_INCLUDES} -I${HDF5_INCLUDES} -I${DAL1_INCLUDES} -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${TMF_INCLUDES} -I${CASACORE_INCLUDES} -I${CMAKE_CURRENT_BINARY_DIR} -I${NUM_UTIL_INCLUDES} -I${NUMPY_INCLUDES} -I${WCSLIB_INCLUDES} -I${CFITSIO_INCLUDES} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.pp.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYCRTOOLS_CPP_MODULE_PREFIX}${FILE}.wpp.cc
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
  ${DAL_INCLUDES}
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
  ${DAL_LIBRARIES}
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
##  Testing
##
## =============================================================================

if (PYCRTOOLS_ENABLE_TESTING)
  set (CTEST_BUILD_TARGET "install")
  add_test(NAME tImportPyCRTools
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/tImportPyCRTools.py)
endif (PYCRTOOLS_ENABLE_TESTING)

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
