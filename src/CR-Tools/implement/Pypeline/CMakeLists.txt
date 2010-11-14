
message (STATUS "[CR-Tools] Adding implement/Pypeline ...")

##-------------------------------------------------------------------------------
## Compiler options

option (PYPELINE_WITH_COMPILER_WARNINGS "Compile Pypeline with compiler warnings enabled." ON)
option (PYPELINE_WITH_RUNTIME_WARNINGS "Compile Pypeline with runtime warnings enabled." ON)
option (PYPELINE_WITH_DEBUG_SYMBOLS "Compile Pypeline with debug symbols." ON)
option (PYPELINE_WITH_OPTIMIZATION "Compile Pypeline with optimization flags turned on." OFF)
option (PYPELINE_WITH_CPU_OPTIMIZATION "Compile Pypeline with optimization for CPU architecture." OFF)

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

message (STATUS "Compiling Pypeline with flags: ${CMAKE_CXX_FLAGS} ${PYPELINE_CXX_FLAGS}")

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
  Filter
  Fitting
  Imaging
  IO
  RF
  Bindings
  )

##_______________________________________________________________________________
## Perform Wrapper Preprocessing of module files

foreach (FILE ${PYPELINE_CPP_MODULES})
  ## ________________________________________________________________________
  ##                                    Variable substitutions and iterations
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfiter.awk ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Substituting variables and processing iterations"
    )

  ## ________________________________________________________________________
  ##                                            Split off wrapper definitions
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc
    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfsplit2h.awk ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Splitting off wrapper definitions"
    )

  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.def.h
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc
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
    set (PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES "${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.def.h")
  endif (FILE STREQUAL "Bindings")

  ## ________________________________________________________________________
  ##                                                 execute gcc preprocessor
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc
            ${PYPELINE_CPP_MODULE_HEADER_FILES}
            ${PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/core.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/hfppnew.h
            ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}Module.h
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP -w  -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${TMF_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${PYPELINE_CPP_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Executing wrapper preprocessor"
    )

  ## ________________________________________________________________________
  ##                                                      Inserting new lines
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfinsert-newline.awk ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.cc
    COMMENT "Wrapper preprocessing module ${FILE}: Finalizing wrapper preprocessing"
    )

endforeach (FILE ${PYPELINE_CPP_MODULES})

##_______________________________________________________________________________
## Create the library

include_directories (
  ${CR_SOURCE_DIR}/implement/Pypeline
  ${CMAKE_CURRENT_BINARY_DIR}
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
  )

##to speed up computations use -g -O2 or -g0 -O3 if no debuggng is needed.
## additional target properties
set_target_properties (hftools
  PROPERTIES
  PREFIX ""
  SUFFIX .so
  COMPILE_FLAGS "${PYPELINE_CXX_FLAGS}"
  LINK_FLAGS "${PYPELINE_LINKER_FLAGS}"
  )

##_______________________________________________________________________________
## Create the documentation

# Start with empty documentation files.
add_custom_command (
  TARGET hftools
  POST_BUILD
  COMMAND echo "" > ${CMAKE_CURRENT_BINARY_DIR}/hftools.doc.py
  COMMAND echo "" > ${CMAKE_CURRENT_BINARY_DIR}/hftools.doc.tex

  COMMENT "Create documentation for python bindings"
  )

foreach (FILE ${PYPELINE_CPP_MODULES})
   # Generate the basic documentation strings that python help will use
   add_custom_command (
     TARGET hftools
     POST_BUILD
     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc

     COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfgetpydoc.awk ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc >> ${CMAKE_CURRENT_BINARY_DIR}/hftools.doc.py

#     COMMENT "Generating python documentation for module ${FILE}"
     )

   # Generate basic documentation in text form
   add_custom_command (
     TARGET hftools
     POST_BUILD
     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc

     COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfgetdoc.awk ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.iter.cc >> ${CMAKE_CURRENT_BINARY_DIR}/hftools.doc.tex

#     COMMENT "Generating text documentation for module ${FILE}"
     )

  # # Generate a file that can be processed by doxygen
  # add_custom_command (
  #   TARGET hftools
  #   POST_BUILD
  #   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc

  #   COMMAND ${CMAKE_CXX_COMPILER} -w -E -C -P -DH_DEBUG_CPP  -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES} -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc | ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfinsert-newline.awk  > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.doc.cc

  #   COMMENT "Generating doxygen parsable documentation for module ${FILE}"
  #   )
endforeach (FILE)

##_______________________________________________________________________________
## Installation

# Install Python C++ modules
install (
  TARGETS hftools
  LIBRARY DESTINATION lib/python/pycrtools/core
  )

# Install Python modules
install (CODE "message (STATUS \"Install Python modules...\")")
install (CODE "execute_process(COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/modules ${LUS_ROOT}/release/lib/python/pycrtools)")

# Install Python scripts
install (CODE "message (STATUS \"Install Python scripts...\")")
install (CODE "execute_process(COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCreateLinks.sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${LUS_ROOT}/release/bin/python)")

