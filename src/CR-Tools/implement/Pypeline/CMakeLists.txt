
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

##_______________________________________________________________________________
## Create the library

include_directories (${CR_SOURCE_DIR}/implement/Pypeline)

file (GLOB hftools_sources ${CR_BINARY_DIR}/implement/Pypeline/*.cc)

add_library (hftools ${hftools_sources} )

set_target_properties (PROPERTIES
  COMPILE_FLAGS "-E -P -D H_DEBUG_CPP  -Wall -Wno-comment -Woverloaded-virtual -Wno-non-template-friend -bundle -headerpad_max_install_names -fPIC -flat_namespace -Wl"
)
