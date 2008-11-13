##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## =============================================================================
##
##  `libcr` library
##
## =============================================================================

## -----------------------------------------------------------------------------
## Core modules which should be included in any case

set (cr_modules
  Analysis
  Calibration
  Coordinates
  Data
  Imaging
  IO
  LopesLegacy
  Math
  Observation
  Utilities
  )

foreach (cr_module ${cr_modules})
  ## list of source files for this module
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

## ------------------------------------------------------------------------------
## Optional modules (added depending on requested features and installed packages)

if (CR_WITH_PLOTTING)
  ## add source files to the library
  FILE (GLOB cr_Display Display/*.cc)
  ## addition to the lists
  list (APPEND cr_modules Display)
  LIST (APPEND cr_sources ${cr_Display})
endif (CR_WITH_PLOTTING)

## Support for generation of plots?

if (CR_WITH_GLISH)
  ## add source files to the library
  FILE (GLOB cr_ApplicationSupport ApplicationSupport/*.cc)
  if (HAVE_CASA)
    list (APPEND cr_sources templates.cc)
  endif (HAVE_CASA)
  ## addition to the lists
  list (APPEND cr_modules ApplicationSupport)
  LIST (APPEND cr_sources ${cr_ApplicationSupport})
endif (CR_WITH_GLISH)

## Enable using routines from LOPES-Star?

if (CR_WITH_STARTOOLS)
  LIST (APPEND cr_sources ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C)
else (CR_WITH_STARTOOLS)
  message (STATUS "[CR-Tools] Removing source files requiring STAR-Tools installation.")
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/CompletePipeline.cc)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/analyseLOPESevent2.cc)
  list (REMOVE_ITEM cr_sources ${CR_SOURCE_DIR}/implement/Analysis/PulseProperties.cc)
endif (CR_WITH_STARTOOLS)

## ------------------------------------------------------------------------------
## Assemble list of directories containing header files

set (cr_includes "")

if (HAVE_GIF)
  set (cr_includes "${cr_includes} -I${GIF_INCLUDES}")
endif (HAVE_GIF)

if (HAVE_GSL)
  set (cr_includes "${cr_includes} -I${GSL_INCLUDES}")
endif (HAVE_GSL)

## ------------------------------------------------------------------------------
## Assemble list of external libraries against which to link

if (HDF5_LIBRARIES)
  list (APPEND cr_link_libraries ${HDF5_LIBRARIES})
endif (HDF5_LIBRARIES)

if (CASA_LIBRARIES)
  list (APPEND cr_link_libraries ${CASA_LIBRARIES})
endif (CASA_LIBRARIES)

if (GFORTRAN_LIBRARIES)
  list (APPEND cr_link_libraries ${GFORTRAN_LIBRARIES})
else (GFORTRAN_LIBRARIES)
  if (G2C_LIBRARIES)
    list (APPEND cr_link_libraries ${G2C_LIBRARIES})
  endif (G2C_LIBRARIES)
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

if (FFTW3_LIBRARIES)
  list (APPEND cr_link_libraries ${FFTW3_LIBRARIES})
endif (FFTW3_LIBRARIES)

if (ROOT_LIBRARIES)
  list (APPEND cr_link_libraries ${ROOT_LIBRARIES})
endif (ROOT_LIBRARIES)

if (POSTGRESQL_LIBRARIES)
  list (APPEND cr_link_libraries ${POSTGRESQL_LIBRARIES})
endif (POSTGRESQL_LIBRARIES)

if (PNG_LIBRARIES)
  list (APPEND cr_link_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (GIF_LIBRARIES)
  list (APPEND cr_link_libraries ${GIF_LIBRARIES})
endif (GIF_LIBRARIES)

if (HAVE_LIBGD)
  list (APPEND cr_link_libraries ${HAVE_LIBGD})
endif (HAVE_LIBGD)

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
    COMPILE_FLAGS "-fPIC -ftemplate-depth-100 ${cr_includes}"
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
    Coordinates/TimeFreq.cc
    Data/LOFAR_DipoleDataset.cc
    IO/DataIterator.cc
    IO/DataReader.cc
    BindingsBoostPython.cc
    )
  ## linker instructions
  target_link_libraries(pycr
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
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
  
  ## list of source files in this module
  file (GLOB libhfget_sources 
    GUI/cell.cpp
    GUI/finddialog.cpp
    GUI/gotocelldialog.cpp
    GUI/mainwindow.cpp
    GUI/sortdialog.cpp
    GUI/spreadsheet.cpp
    GUI/plotter.cpp
    GUI/VectorSelector.cc
    GUI/hffuncs.cc
    GUI/hfget.cc
    GUI/hfqt.cc
    GUI/qrc_spreadsheet.cpp
    GUI/qrc_plotter.cpp
    GUI/moc_finddialog.cpp
    GUI/moc_gotocelldialog.cpp
    GUI/moc_mainwindow.cpp
    GUI/moc_sortdialog.cpp
    GUI/moc_spreadsheet.cpp
    GUI/moc_plotter.cpp
    GUI/moc_hfqt.cpp
    )
  
  ## include directories
  
  if (QT4_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)
    set (libhfget_includes "-I${GTK_INCLUDES} -I${BOOST_INCLUDES}")
  endif (QT4_INCLUDES AND GTK_INCLUDES AND BOOST_INCLUDES)

  if (QT4_INCLUDES)
    include_directories (${QT4_INCLUDES})
  endif (QT4_INCLUDES)

  ## compiler instructions
  add_library (hfget MODULE
    ${libhfget_sources}
    )
  
  ## linker instructions
  target_link_libraries (hfget
    ${QT4_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    )

  ## Additional properties
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
  
  ## installation
  install (
    TARGETS hfget
    LIBRARY DESTINATION lib/python
    )
  
endif (CR_WITH_GUI)

## =============================================================================
##
##  Test programs
##
## =============================================================================

if (CR_BUILD_TESTS)
  foreach (cr_module ${cr_modules})
    ADD_SUBDIRECTORY ("${cr_module}/test")
  endforeach (cr_module)
endif (CR_BUILD_TESTS)
