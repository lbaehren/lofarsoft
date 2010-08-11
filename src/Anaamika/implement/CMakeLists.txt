
include_directories (
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
  ${NUMPY_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${NUM_UTIL_INCLUDES}/..
  )

## ==============================================================================
##
##  Preprocessing
##
## ==============================================================================

#  As Fortran does not know precompiler statements, encapsulation of code is not
#  posible in the straight-forward manner a this can be done in C/C++. As a
#  consequence of this, the fBDSM source code contains quite a few instructions 
#  which only can be compiled successfully, if the required external packages
#  are available.
#
#  Running the following sequence of command line instructions from within the
#  current directory, will produce a list of source files which make use of
#  functionality from the PGPlot library:
#
#    grep "call pg" * | tr ":" "\n" | grep "\.f" | grep -v "call pg" | uniq
#
#  Regular expressions for CMake
#
#    ^(.*\n)?call pg

## Check file for usage of PGPlot ________________

if (BDSM_WITH_FBDSM AND NOT HAVE_PGPLOT)

  ## Report on activity
  message (STATUS "Processing source files to remove PGPlot function calls ...")

  foreach (_module fbdsm shapelets)
    
    file (GLOB _sources ${_module}/*.f)
    
    foreach (_src ${_sources})
      ## Store the name of the file
      get_filename_component(_filename ${_src} NAME)
      ## Process the file
      execute_process (
	COMMAND "sed '/call pg/s/^/c/g'"
	COMMAND "sed '/pgopen/s/^/c/g'"
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_module}
	INPUT_FILE ${_src}
	OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${_module}/${_filename}
	)
    endforeach (_src)
    
  endforeach (_module)
  
endif (BDSM_WITH_FBDSM AND NOT HAVE_PGPLOT)

## ==============================================================================
##
##  Projet subdirectories
##
## ==============================================================================

configure_file (
  ${BDSM_SOURCE_DIR}/implement/create.in
  ${BDSM_BINARY_DIR}/implement/bdsm/create
  )

add_custom_target (create
  COMMAND ./create
  WORKING_DIRECTORY ${BDSM_BINARY_DIR}/implement/bdsm
  )

## Fortran-based version

if (BDSM_WITH_FBDSM)
  add_subdirectory (fbdsm)
endif (BDSM_WITH_FBDSM)

## Python-based version

if (BDSM_WITH_PYBDSM)
  add_subdirectory (minpack)
  add_subdirectory (port3)
  add_subdirectory (pybdsm)
endif (BDSM_WITH_PYBDSM)
