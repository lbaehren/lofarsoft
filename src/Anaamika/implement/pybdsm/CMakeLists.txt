## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

## ==============================================================================
##
##  Boost.Python modules
##
## ==============================================================================

## list of source files
file (GLOB cbdsm_sources *.cc)

## compiler instructions
add_library (_cbdsm MODULE
  ${cbdsm_sources}
  )

## linker instructions
target_link_libraries (_cbdsm 
  minpack
  port3
  ${NUM_UTIL_LIBRARIES}
  ${BOOST_PYTHON_LIBRARY}
  )

## additional target properties
set_target_properties (_cbdsm PROPERTIES PREFIX ""
  COMPILE_FLAGS "-Wall -Wextra -Wno-unused"
  LINKER_LANGUAGE C
  )

if (APPLE)
  set_target_properties(_cbdsm PROPERTIES 
    LINK_FLAGS "-undefined dynamic_lookup")
endif (APPLE)

## ==============================================================================
##
##  Fortran modules
##
## ==============================================================================

## We should use f2py to generate these modules, otherwise the required Python
## "glue" will be missing.
## For example:
##
##   $ f2py -c -m <module-name> <Fortran-sources>
##
## This will generate a shared library (module) called <module-name>.so in the
## current directory. The proper way to implement this in CMake is to use
## a combination of add_custom_command() and add_custom_target() and
## add_dependencies().
##

##__________________________________________________________
## Module : _pytesselate

## compiler instructions
add_library (_pytesselate MODULE
  pytess_simple.f
  pytess_roundness.f
  dummy.f
  )
## linker instructions
target_link_libraries (_pytesselate
  ${PYTHON_LIBRARIES}
  )
## additional target properties
set_target_properties (_pytesselate
  PROPERTIES PREFIX "" 
  COMPILE_FLAGS "-Wall -Wextra -Wno-unused"
  LINKER_LANGUAGE Fortran
  )

##__________________________________________________________
## Module: _py2fbdsm

## compiler instructions
add_library (_py2fbdsm MODULE
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/iland_mat2list.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/get_keyword.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/gaul2gaulbin.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/writearray.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/writefitshead.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/sub_prog.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/readarray.f
  ${PROJECT_SOURCE_DIR}/implement/fbdsm/check.f
  )
## linker instructions
target_link_libraries (_py2fbdsm
  ${PYTHON_LIBRARIES}
  )
## additional target properties
set_target_properties (_py2fbdsm
  PROPERTIES PREFIX "" 
  COMPILE_FLAGS "-Wall -Wextra -Wno-unused"
  LINKER_LANGUAGE Fortran
  ) 

## ==============================================================================
##
##  Installation
##
## ==============================================================================

install (TARGETS _cbdsm _pytesselate _py2fbdsm
  DESTINATION lib/python/bdsm
  )
