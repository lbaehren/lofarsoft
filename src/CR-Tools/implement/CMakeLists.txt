##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## =============================================================================
##
##  `libcr` library
##
## =============================================================================

##__________________________________________________________
## Core modules

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Imaging
  IO
  Math
  Utilities
  )

##__________________________________________________________
## Optional modules depending on user selection

if (CR_WITH_GLISH)
  list (APPEND cr_modules ApplicationSupport)
  ## additional sources-files
  if (HAVE_CASA)
    list (APPEND cr_sources templates.cc)
  endif (HAVE_CASA)
endif (CR_WITH_GLISH)

#if (HAVE_FFTW3)
#  list (APPEND cr_modules Analysis/Utils)
#endif (HAVE_FFTW3)

##__________________________________________________________
## Collect source files to be compiled into the library

foreach (cr_module ${cr_modules})
  ## list of source files for this module
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  ## add to global source files list
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## ------------------------------------------------------------------------------
## Optional modules (added depending on requested features and installed packages)

## Enable using routines from LOPES-Star?

if (CR_WITH_STARTOOLS)
  LIST (APPEND cr_sources ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C)
else (CR_WITH_STARTOOLS)
  message (STATUS "[CR-Tools] Removing source files requiring STAR-Tools installation.")
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/CompletePipeline.cc)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/analyseLOPESevent2.cc)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/PulseProperties.cc)
endif (CR_WITH_STARTOOLS)

if (NOT CR_WITH_PLOTTING)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Utilities/SimplePlot.cc)
endif (NOT CR_WITH_PLOTTING)


## ------------------------------------------------------------------------------
## Assemble list of external libraries against which to link

if (CASA_LIBRARIES)
  list (APPEND cr_link_libraries ${CASA_LIBRARIES})
endif (CASA_LIBRARIES)

if (GFORTRAN_LIBRARIES)
  list (APPEND cr_link_libraries ${GFORTRAN_LIBRARIES})
endif (GFORTRAN_LIBRARIES)

if (LAPACK_LIBRARIES)
  list (APPEND cr_link_libraries ${LAPACK_LIBRARIES})
endif (LAPACK_LIBRARIES)

if (CFITSIO_LIBRARIES)
  list (APPEND cr_link_libraries ${CFITSIO_LIBRARIES})
endif (CFITSIO_LIBRARIES)

if (WCSLIB_LIBRARIES)
  list (APPEND cr_link_libraries ${WCSLIB_LIBRARIES})
endif (WCSLIB_LIBRARIES)

if (X11_LIBRARIES)
  list (APPEND cr_link_libraries ${X11_LIBRARIES})
endif (X11_LIBRARIES)

if (PGPLOT_LIBRARIES)
  list (APPEND cr_link_libraries ${PGPLOT_LIBRARIES})
endif (PGPLOT_LIBRARIES)

if (PLPLOT_LIBRARIES)
  list (APPEND cr_link_libraries ${PLPLOT_LIBRARIES})
endif (PLPLOT_LIBRARIES)

if (PNG_LIBRARIES)
  list (APPEND cr_link_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (FFTW3_LIBRARIES)
  list (APPEND cr_link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (HAVE_LIBUTIL)
  list (APPEND cr_link_libraries ${HAVE_LIBUTIL})
endif (HAVE_LIBUTIL)

if (HAVE_LIBM)
  list (APPEND cr_link_libraries ${HAVE_LIBM})
endif (HAVE_LIBM)

## ------------------------------------------------------------------------------
## Create the library

add_library (cr
  ${CR_BINARY_DIR}/crtools.h
  ${cr_sources}
  )

if (APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "-fPIC -ftemplate-depth-100"
    LINK_FLAGS "-bind_at_load"
    )
else (APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC -ftemplate-depth-100"
    LINK_FLAGS "-bind_at_load"
    )
endif (APPLE)

## Linker instructions

target_link_libraries (cr
  ${cr_link_libraries}
  )

## Installation

install (
  TARGETS cr
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## =============================================================================
##
##  Python bindings to the library
##
## =============================================================================

if (CR_WITH_PYTHON)

  ## compiler instructions
  add_library (pycr MODULE
    Analysis/DynamicSpectrum.cc
    Coordinates/CoordinateType.cc
    Coordinates/TimeFreq.cc
    Data/LOFAR_TBB.cc
    IO/DataIterator.cc
    IO/DataReader.cc
    Math/BasicFilter.cc
    Math/HanningFilter.cc
    Coordinates/MConversions.cc
    BindingsBoostPython.cc
    )
  ## linker instructions
  target_link_libraries(pycr
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${DAL_LIBRARIES}
    ${cr_link_libraries}
    )
  ## additional target properties
  if (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON -fpermissive"
      LINK_FLAGS "-fPIC -flat_namespace"
      )
  else (APPLE)
    set_target_properties (pycr
      PROPERTIES
      PREFIX ""
      SUFFIX .so
      COMPILE_FLAGS "-DPYTHON"
      LINK_FLAGS "-fPIC -shared -Wl"
      )
  endif (APPLE)
  ## installation
  install (
    TARGETS pycr
    LIBRARY DESTINATION lib/python
    )
  
endif (CR_WITH_PYTHON)

## =============================================================================
##
##  Graphical user interface
##
## =============================================================================

if (CR_WITH_GUI)
  add_subdirectory (GUI)
endif (CR_WITH_GUI)

## =============================================================================
##
##  Test programs
##
## =============================================================================

if (CR_BUILD_TESTS)
  foreach (cr_module ${cr_modules})
    add_subdirectory ("${cr_module}/test")
  endforeach (cr_module)
endif (CR_BUILD_TESTS)
