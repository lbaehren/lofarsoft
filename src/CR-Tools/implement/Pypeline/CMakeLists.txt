
message (STATUS "[CR-Tools] Adding implement/Pypeline ...")

##-------------------------------------------------------------------------------
## Optional dependencies
if (CR_WITH_OPENMP AND OPENMP_FOUND)
  message (STATUS "[CR-Tools] Adding OpenMP support to Pypeline.")
  set (PYPELINE_CXX_FLAGS ${OpenMP_CXX_FLAGS})
  set (PYPELINE_LINKER_FLAGS ${OpenMP_CXX_FLAGS})
endif (CR_WITH_OPENMP AND OPENMP_FOUND)

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

set (PYPELINE_PY_CORE_MODULES
  __init__.py
  config.py
  datareader.py
  types.py
  vector.py
  harray.py
  help.py
  tasks.py
  workspaces.py
  )

set (PYPELINE_PY_MODULES
  __init__.py
  metadata.py
  bfdata.py
  imager.py
  interfaces.py
  calibration.py
  IO.py
  srcfind.py
  PPF.py
  matching.py
  )

set (PYPELINE_PY_SCRIPTS
  test.py
  testfft.py
  testrfi.py
  testseti2.py
  testcr2.py
  testimage.py
  testrfi2.py
  testcrab.py
  testimageCrab2.py
  testseti.py
  testimager.py
  crab_pulse_image.py
  hfplot.py
  pycrinit.py
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
    COMMAND ${PYPELINE_PREPROCESSOR_DIR}/hfCheckInterfaceChange.sh ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}
    COMMENT "Wrapper preprocessing module ${FILE}: Splitting off wrapper definitions"
    )
endforeach (FILE ${PYPELINE_CPP_MODULES})

foreach (FILE ${PYPELINE_CPP_MODULES})

  ## ________________________________________________________________________
  ##                           Check all header files for the bindings module
  if (FILE STREQUAL "Bindings")
    file (GLOB PYPELINE_CPP_MODULE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}*.h)
    file (GLOB PYPELINE_CPP_MODULE_WRAPPER_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}*.def.h)
  else (FILE STREQUAL "Bindings")
    set (PYPELINE_CPP_MODULE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.h")
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
            ${CMAKE_CURRENT_SOURCE_DIR}/${PYPELINE_CPP_MODULE_PREFIX}Module.h
    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP -w  -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${TMF_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${PYPELINE_CXX_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.split.cc > ${CMAKE_CURRENT_BINARY_DIR}/${PYPELINE_CPP_MODULE_PREFIX}${FILE}.wpp.cc
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
if (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "${PYPELINE_CXX_FLAGS} -fpermissive -DH_DEBUG_CPP"
    LINK_FLAGS "${PYPELINE_LINKER_FLAGS} -fPIC -flat_namespace -Wl"
    )
else (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "${PYPELINE_CXX_FLAGS} -DH_DEBUG_CPP -g -O2 -w"
    LINK_FLAGS "${PYPELINE_LINKER_FLAGS} -fPIC -shared -Wl"
    )
endif (APPLE)

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

# Create output directories
add_custom_command(
  TARGET hftools
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LUS_ROOT}/release/lib/python/pycrtools
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LUS_ROOT}/release/lib/python/pycrtools/core
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LUS_ROOT}/release/bin/python
  )

# Install Python core modules
foreach(FILENAME ${PYPELINE_PY_CORE_MODULES})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/modules/core/${FILENAME}")
  set(DST "${LUS_ROOT}/release/lib/python/pycrtools/core/${FILENAME}")

  add_custom_command(
    TARGET hftools
    COMMAND ${CMAKE_COMMAND} -E remove ${DST}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
  )
endforeach(FILENAME)

# Install Python modules
foreach(FILENAME ${PYPELINE_PY_MODULES})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/modules/${FILENAME}")
  set(DST "${LUS_ROOT}/release/lib/python/pycrtools/${FILENAME}")

  add_custom_command(
    TARGET hftools
    COMMAND ${CMAKE_COMMAND} -E remove ${DST}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
  )
endforeach(FILENAME)

# Install Python scripts
foreach(FILENAME ${PYPELINE_PY_SCRIPTS})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/scripts/${FILENAME}")
  set(DST "${LUS_ROOT}/release/bin/python/${FILENAME}")

  add_custom_command(
    TARGET hftools
    COMMAND ${CMAKE_COMMAND} -E remove ${DST}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
  )
endforeach(FILENAME)

