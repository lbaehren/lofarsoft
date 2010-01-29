
## ==============================================================================
##
##  Definition of Macros
##
## ==============================================================================

find_program (bin_awk gawk awk ${bin_locations})

macro (cr_process_awk _infile _script)
  
  get_filename_component(_infile_filename  ${_infile} NAME)
  get_filename_component(_infile_name      ${_infile} NAME_WE)
  get_filename_component(_infile_extension ${_infile} EXT)

  set (_outfile_filename ${_infile_name}.awk${_infile_extension})
  set (_outfile ${CR_BINARY_DIR}/implement/GUI/${_outfile_filename})

  message (STATUS "[awk] ${_infile_filename} -> ${_outfile_filename}")
  
  execute_process (
    WORKING_DIRECTORY ${CR_SOURCE_DIR}/implement/GUI
    COMMAND ${bin_awk} -f ${_script} ${_infile}
    TIMEOUT 30
    OUTPUT_VARIABLE awk_output
    ERROR_VARIABLE awk_error
    )
  
  if (awk_output)
    file (WRITE ${_outfile} ${awk_output})
  endif (awk_output)
  
endmacro (cr_process_awk _infile _script)

##_______________________________________________________________________________
## Include directories

if (QT_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)
  set (libhfget_includes "-I${GTK_INCLUDES} -I${BOOST_INCLUDES}")
endif (QT_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)

if (QT_INCLUDES)
  include_directories (${QT_INCLUDES})
endif (QT_INCLUDES)

if (MATHGL_INCLUDES)
  include_directories (${MATHGL_INCLUDES})
endif (MATHGL_INCLUDES)

##_______________________________________________________________________________
## Pre-processing of the source files

if (gawk_executable)
  ## process input files

  cr_process_awk (hffuncs.cc hfiter.awk)
  cr_process_awk (hffuncs.h hfiter.awk)
  cr_process_awk (hfanalysis.cc hfsplit2h.awk)

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
  ${CR_BINARY_DIR}/implement/GUI/*.cc
  hfget.cc
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
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
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
