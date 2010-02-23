
message (STATUS "[CR-Tools] Adding implement/Pypeline ...")

##_______________________________________________________________________________
## Include directories

if (GTK_INCLUDES AND BOOST_INCLUDES)
  set (libhfget_includes "-I${GTK_INCLUDES} -I${BOOST_INCLUDES}")
endif (GTK_INCLUDES AND BOOST_INCLUDES)

if (QT_INCLUDES)
  include_directories (${QT_INCLUDES})
endif (QT_INCLUDES)

if (MATHGL_INCLUDES)
  include_directories (${MATHGL_INCLUDES})
endif (MATHGL_INCLUDES)

##_______________________________________________________________________________
## Pre-processing of the source files

file (GLOB hftools_cc hftools.cc)

## Generating some basic documentation

execute_process (
  COMMAND ${bin_awk} -f ${CR_SOURCE_DIR}/implement/GUI/hfiter.awk ${hftools_cc}
  COMMAND ${bin_awk} -f ${CR_SOURCE_DIR}/implement/GUI/hfgetdoc.awk
  COMMAND sort
  OUTPUT_FILE ${CR_BINARY_DIR}/implement/Pypeline/hftools.doc.tex
  TIMEOUT 20
  )

## Splitting off certain snippets of code

execute_process (
  COMMAND ${bin_awk} -f ${CR_SOURCE_DIR}/implement/GUI/hfiter.awk ${hftools_cc}
  COMMAND ${bin_awk} -f ${CR_SOURCE_DIR}/implement/GUI/hfsplit2h.awk 
  OUTPUT_FILE ${CR_BINARY_DIR}/implement/Pypeline/hftools.awk.cc
  TIMEOUT 20
  )

execute_process (
  COMMAND ${CMAKE_CXX_COMPILER} -E  -P -D H_DEBUG_CPP -I${CR_SOURCE_DIR}/implement/Pypeline ${CR_BINARY_DIR}/implement/Pypeline/hftools.awk.cc
  COMMAND ${bin_awk} '{gsub(" *\\<_H_NL_\\> *","\n"); print}'
  OUTPUT_FILE ${CR_BINARY_DIR}/implement/Pypeline/hftools.hfpp.cc
  TIMEOUT 20
  )

#set_source_files_properties (
#  ${CR_BINARY_DIR}/implement/Pypeline/hftools.awk.cc
#  PROPERTIES
#  KEEP_EXTENSION TRUE
#  LANGUAGE CXX
#  OBJECT_DEPENDS ${hftools_cc}
#  COMPILE_DEFINITIONS E;P
#  COMPILE_FLAGS "-DH_DEBUG_CPP"
#  )

##_______________________________________________________________________________
## Create the library

include_directories (${CR_SOURCE_DIR}/implement/Pypeline)

add_library (hftools MODULE
  ${CR_BINARY_DIR}/implement/Pypeline/hftools.hfpp.cc
  )

target_link_libraries (hftools
  cr
  ${DAL_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${BOOST_LIBRARIES}
  )

## additional target properties
if (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX "lib"
    SUFFIX .so
    COMPILE_FLAGS "-fpermissive -DH_DEBUG_CPP"
    LINK_FLAGS "-fPIC -flat_namespace -Wl"
    )
else (APPLE)
  set_target_properties (hftools
    PROPERTIES
    PREFIX "lib"
    SUFFIX .so
    COMPILE_FLAGS "-DH_DEBUG_CPP"
    LINK_FLAGS "-fPIC -shared -Wl"
    )
endif (APPLE)


##_______________________________________________________________________________
## Installation

install (
  TARGETS hftools
  LIBRARY DESTINATION lib/python
  )
