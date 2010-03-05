
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
  COMMAND ${awk_executable} -f ${CR_SOURCE_DIR}/implement/GUI/hfiter.awk ${hftools_cc}
  COMMAND ${awk_executable} -f ${CR_SOURCE_DIR}/implement/GUI/hfgetdoc.awk
  COMMAND sort
  OUTPUT_FILE ${CR_BINARY_DIR}/implement/Pypeline/hftools.doc.tex
  TIMEOUT 20
  )

## Splitting off certain snippets of code

execute_process (
  COMMAND ${awk_executable} -f ${CR_SOURCE_DIR}/implement/GUI/hfiter.awk ${hftools_cc}
  COMMAND ${awk_executable} -f ${CR_SOURCE_DIR}/implement/GUI/hfsplit2h.awk 
  OUTPUT_FILE ${CR_BINARY_DIR}/implement/Pypeline/hftools.awk.cc
  TIMEOUT 20
  )

#add_custom_command (
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc
#  DEPENDS ${hftools_cc}
#  COMMAND ${CMAKE_CXX_COMPILER}
#  ARGS -E  -P -DH_DEBUG_CPP -D_H_NL_=\" \" -I${WCSLIB_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} #-I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${CASACORE_INCLUDES} #-I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} -o #${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc ${CMAKE_CURRENT_BINARY_DIR}/hftools.awk.cc
#  )


add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc
  DEPENDS ${hftools_cc}
  COMMAND ${CMAKE_CXX_COMPILER}
  ARGS -E -DH_DEBUG_CPP -I${WCSLIB_INCLUDES} -I${DAL_INCLUDES}/.. -I${CFITSIO_INCLUDES} -I${HDF5_INCLUDES}  -I${DAL_INCLUDES} -I${PYTHON_INCLUDES} -I${BOOST_INCLUDES} -I${CASACORE_INCLUDES} -I${CR_SOURCE_DIR}/implement/Pypeline -I${CR_SOURCE_DIR}/implement -I${CR_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/hftools.awk.cc | ${awk_executable} -f ${CR_SOURCE_DIR}/implement/GUI/hfinsert-newline.awk  > ${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc
    )

set_source_files_properties (
  ${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc
  PROPERTIES
  OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hftools.awk.cc
  )

########### is this needed???
########### in a way yes, but it doesn't seem to work...
set_source_files_properties (
  ${CMAKE_CURRENT_BINARY_DIR}/hftools.awk.cc
  PROPERTIES
  OBJECT_DEPENDS ${hftools_cc}
  OBJECT_DEPENDS ${CR_SOURCE_DIR}/implement/GUI/hfiter.awk
  OBJECT_DEPENDS ${CR_SOURCE_DIR}/implement/GUI/hfsplit2h.awk
  )

##_______________________________________________________________________________
## Create the library

include_directories (${CR_SOURCE_DIR}/implement/Pypeline)

add_library (hftools MODULE
  ${CMAKE_CURRENT_BINARY_DIR}/hftools.hfpp.cc
  )

target_link_libraries (hftools
  cr
  ${DAL_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CASA_LIBRARIES}
  ${BOOST_LIBRARIES}
  ${PYTHON_LIBRARIES}
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
## Installation

install (
  TARGETS hftools
  LIBRARY DESTINATION lib/python
  )
