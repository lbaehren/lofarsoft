
##_______________________________________________________________________________
## Include directories

if (QT4_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)
  set (libhfget_includes "-I${GTK_INCLUDES} -I${BOOST_INCLUDES}")
endif (QT4_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)

if (QT4_INCLUDES)
  include_directories (${QT4_INCLUDES})
endif (QT4_INCLUDES)

if (MATHGL_INCLUDES)
  include_directories (${MATHGL_INCLUDES})
endif (MATHGL_INCLUDES)

##_______________________________________________________________________________
## Pre-processing of the source files

find_program (gawk_executable gawk awk ${bin_locations})

if (gawk_executable)
  ## process input files
  add_custom_target (hfget_awk
    COMMAND ${gawk_executable} -f hfiter.awk hffuncs.cc | ${gawk_executable} -f hfprep.awk > ${CR_BINARY_DIR}/implement/GUI/hffuncs.awk.cc
    COMMAND ${gawk_executable} -f hfiter.awk hffuncs.h | ${gawk_executable} -f hfprep.awk > ${CR_BINARY_DIR}/implement/GUI/hffuncs.awk.h
    COMMAND ${gawk_executable} -f hfsplit2h.awk hfanalysis.cc
    WORKING_DIRECTORY ${CR_SOURCE_DIR}/implement/GUI
    COMMENT "Running awk to generate source files ..."
    )
  ## augment include path
  include_directories (
    ${CR_BINARY_DIR}/implement
    ${CR_BINARY_DIR}/implement/GUI
    )
else (gawk_executable)
  message (WARNING "Unable to process source files -- missing awk/gawk!")
endif (gawk_executable)

##_______________________________________________________________________________
## Create the library

## List of source files

file (GLOB libhfget_sources 
  VectorSelector.cc
  hfanalysis.cc
  hfget.cc
  ${CR_BINARY_DIR}/implement/GUI/*.cc
  )

## compiler instructions

add_library (hfget MODULE ${libhfget_sources})
add_dependencies (hfget hfget_awk)

## linker instructions

target_link_libraries (hfget
  cr
  ${DAL_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${QT4_LIBRARIES}
  ${MATHGL_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${BOOST_LIBRARIES}
  )

## Additional target properties
if (APPLE)
  set_target_properties (hfget
    PROPERTIES
    PREFIX "lib"
    SUFFIX .so
    COMPILE_FLAGS "-pipe -g -D_REENTRANT -w -fpermissive -DQT_GUI_LIB -DQT_CORE_LIB -I${CR_SOURCE_DIR}/implement/GUI ${libhfget_includes}"
    LINK_FLAGS "-fPIC -flat_namespace -Wl"
    )
else (APPLE)
  set_target_properties (hfget
    PROPERTIES
    PREFIX "lib"
    SUFFIX .so
    COMPILE_FLAGS "-pipe -g -D_REENTRANT -w -DQT_GUI_LIB -DQT_CORE_LIB -I${CR_SOURCE_DIR}/implement/GUI ${libhfget_includes}"
    LINK_FLAGS "-fPIC -shared -shared -Wl"
    )
endif (APPLE)

##_______________________________________________________________________________
## Installation

install (
  TARGETS hfget
  LIBRARY DESTINATION lib/python
  )
