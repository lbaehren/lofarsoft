
message (STATUS "[CR-Tools] Adding implement/Pypeline ...")

##_______________________________________________________________________________
## Pre-processing of the source files

set (PYPELINE_PREPROCESSOR_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/preprocessor
  )

set (PYPELINE_CPP_FILES
  hftools
  )

set (PYPELINE_MODULES
  pycrtools.py
  )

set (PYPELINE_SCRIPTS
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
  crab_pulse_image.py
  hfplot.py
  pycrinit.py
  )
  
foreach (FILE ${PYPELINE_CPP_FILES})
  # Variable substitutions and iterations
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.cc

    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfiter.awk ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.cc > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc
    )

  # Split off wrapper definitions
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.def.h
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc

    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfsplit2h.awk ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc
    )

  # Execute gcc in preprocessor mode (and insert some newlines) - this file will be compiled and is quite instructive for debugging
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.cc
    PRE_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc

    COMMAND ${CMAKE_CXX_COMPILER} -E -P -DH_DEBUG_CPP  -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc | ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfinsert-newline.awk  > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.cc
    )
endforeach (FILE)

##_______________________________________________________________________________
## Create the library

include_directories (${CR_SOURCE_DIR}/implement/Pypeline)

foreach (FILE ${PYPELINE_CPP_FILES})
  set (PREPROCESSED_FILES
    ${PREPROCESSED_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.cc
    )
endforeach (FILE)

add_library (hftools MODULE
  ${PREPROCESSED_FILES}
  )

target_link_libraries (hftools
  cr
  ${DAL_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${BOOST_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${FFTW3_LIBRARIES}
  ${GSL_LIBRARIES}	
  )

## additional target properties
if (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-fpermissive -DH_DEBUG_CPP"
    LINK_FLAGS "-fPIC -flat_namespace -Wl"
    )
else (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DH_DEBUG_CPP"
    LINK_FLAGS "-fPIC -shared -Wl"
    )
endif (APPLE)

##_______________________________________________________________________________
## Create the documentation

foreach (FILE ${PYPELINE_CPP_FILES})
  # Generate the basic documentation strings that python help will use
  add_custom_command (
    TARGET hftools
    POST_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc

    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfgetpydoc.awk ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.doc.py
    )

  # Generate basic documentation in text form
  add_custom_command (
    TARGET hftools
    POST_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc

    COMMAND ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfgetdoc.awk ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.iter.cc > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.doc.tex
    )

  # Generate a file that can be processed by doxygen
  add_custom_command (
    TARGET hftools
    POST_BUILD
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc

    COMMAND ${CMAKE_CXX_COMPILER} -E -C -P -DH_DEBUG_CPP  -I${WCSLIB_INCLUDES} -I${FFTW3_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES} -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.split.cc | ${awk_executable} -f ${PYPELINE_PREPROCESSOR_DIR}/hfinsert-newline.awk  > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.doc.cc
    )
endforeach (FILE)

##_______________________________________________________________________________
## Installation

# Install Python C++ modules
install (
  TARGETS hftools
  LIBRARY DESTINATION lib/python
  )

# Create output directories
add_custom_command(
  TARGET hftools
  COMMAND ${CMAKE_COMMAND} -E make_directory ${USG_ROOT}/release/lib/python
  COMMAND ${CMAKE_COMMAND} -E make_directory ${USG_ROOT}/release/bin/python
  )

# Install Python modules
foreach(FILENAME ${PYPELINE_MODULES})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/modules/${FILENAME}")
  set(DST "${USG_ROOT}/release/lib/python/${FILENAME}")

  add_custom_command(
    TARGET hftools
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
  )
endforeach(FILENAME)

# Install Python scripts
foreach(FILENAME ${PYPELINE_SCRIPTS})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/scripts/${FILENAME}")
  set(DST "${USG_ROOT}/release/bin/python/${FILENAME}")

  add_custom_command(
    TARGET hftools
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
  )
endforeach(FILENAME)

