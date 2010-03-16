##
##  CR-Tools/implement/GUI/CMakeLists.txt
##


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

if (awk_executable)

  file (GLOB hfsplit2h_awk hfsplit2h.awk)
  file (GLOB hfanalysis_h hfanalysis.h)
  file (GLOB hfanalysis_cc hfanalysis.cc)

  ## process input files
  cr_process_awk (hfanalysis.h  hfsplit2h.awk )
  cr_process_awk (hfanalysis.cc hfsplit2h.awk )
  cr_process_awk (hffuncs.h     hfiter.awk hfprep.awk)
  cr_process_awk (hffuncs.cc    hfiter.awk hfprep.awk)

  ## augment include path
  include_directories (
    ${CR_BINARY_DIR}/implement
    ${CR_BINARY_DIR}/implement/GUI
    )
else (awk_executable)
  message (WARNING "Unable to process source files -- missing awk/gawk!")
endif (awk_executable)

##_______________________________________________________________________________
## Create the library

## compiler instructions

add_library (hfget MODULE
  hfget.cc
  VectorSelector.cc
  ${CR_BINARY_DIR}/implement/GUI/hfanalysis.awk.cc
  ${CR_BINARY_DIR}/implement/GUI/hffuncs.awk.cc
  )


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

add_dependencies (hfget hfget_awk)

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

## Library

install (
  TARGETS hfget
  LIBRARY DESTINATION lib/python
  )

## Scripts

install (
  FILES hfglobal.py hfinit.py hfpywrappers.py hffuncs.py hfgui.py
  DESTINATION lib/python
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)

install (
  FILES hfstart.py hfnet.py hfcrfuncs.py
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)
