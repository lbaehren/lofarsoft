##------------------------------------------------------------------------------
## $Id::                                                                        $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Modules inside the "CR-Pipeline" package

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

## optional modules (added depending on requested features and installed packages)

# (a) Display modules requires at least one of the external plotting libraries to
#     be available

if (HAVE_PGPLOT OR HAVE_PLPLOT)
  list (APPEND cr_modules Display)
  add_definitions (-DNEED_FORTRAN_UNDERSCORES)
endif (HAVE_PGPLOT OR HAVE_PLPLOT)

# (b) Support for usage of Glish objects and communication bus

if (CR_WITH_GLISH)
  list (APPEND cr_modules ApplicationSupport)
endif (CR_WITH_GLISH)

## -----------------------------------------------------------------------------
## Create "crtools.h"

configure_file (${USG_ROOT}/devel_common/cmake/config_crtools.h.in
  ${CR_BINARY_DIR}/crtools.h)

include_directories (${CR_BINARY_DIR})

## -----------------------------------------------------------------------------
## Build "libcr"

## [1] Assemble list of source files

foreach (cr_module ${cr_modules})
  ## list of source files for this module
  FILE (GLOB cr_${cr_module} ${cr_module}/*.cc)
  LIST (APPEND cr_sources ${cr_${cr_module}})
endforeach (cr_module)

if (CR_WITH_GLISH AND HAVE_CASA)
  list (APPEND cr_sources templates.cc)
endif (CR_WITH_GLISH AND HAVE_CASA)

if (NOT HAVE_PGPLOT)
  list (REMOVE_ITEM cr_sources Display/PGPlotterCR.cc)
  list (REMOVE_ITEM cr_sources Display/PGPlotterLocal.cc)
endif (NOT HAVE_PGPLOT)

if (NOT CR_WITH_STARTOOLS)
  list (REMOVE_ITEM cr_sources Analysis/CompletePipeline.cc)
  list (REMOVE_ITEM cr_sources Analysis/analyseLOPESevent2.cc)
endif (NOT CR_WITH_STARTOOLS)

## Assemble list of external libraries against which to link

if (HDF5_LIBRARIES)
  list (APPEND cr_link_libraries ${HDF5_LIBRARIES})
endif (HDF5_LIBRARIES)

if (CASA_LIBRARIES)
  list (APPEND cr_link_libraries ${CASA_LIBRARIES})
endif (CASA_LIBRARIES)

if (G2C_LIBRARIES)
  list (APPEND cr_link_libraries ${G2C_LIBRARIES})
endif (G2C_LIBRARIES)

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

if (PostgreSQL_LIBRARIES)
  list (APPEND cr_link_libraries ${PostgreSQL_LIBRARIES})
endif (PostgreSQL_LIBRARIES)

if (PNG_LIBRARIES)
  list (APPEND cr_link_libraries ${PNG_LIBRARIES})
endif (PNG_LIBRARIES)

if (GIF_LIBRARIES)
  list (APPEND cr_link_libraries ${GIF_LIBRARIES})
endif (GIF_LIBRARIES)

if (libgd)
  list (APPEND cr_link_libraries ${libgd})
endif (libgd)

if (HAVE_LIBUTIL)
  list (APPEND cr_link_libraries ${HAVE_LIBUTIL})
endif (HAVE_LIBUTIL)

if (HAVE_LIBM)
  list (APPEND cr_link_libraries ${HAVE_LIBM})
endif (HAVE_LIBM)

## ------------------------------------------------------------------------------
## Create the library

include_directories (
  ${CASA_INCLUDES}
  ${CASACORE_INCLUDES}
  ${CR_SOURCE_DIR}/implement
  )

add_definitions (
  -fPIC
  -ftemplate-depth-100
  )

add_library (cr ${cr_sources})

if (NOT APPLE)
  set_target_properties (cr
    PROPERTIES
    COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -fPIC -O2 -fPIC"
    LINK_FLAGS "-bind_at_load"
    )
endif (NOT APPLE)

target_link_libraries (cr
  ${cr_link_libraries}
  )

## ------------------------------------------------------------------------------
## Create Python bindings for the library

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
      COMPILE_FLAGS -DPYTHON
      LINK_FLAGS "-fPIC -shared"
      )
  endif (APPLE)
  ## installation
  install (
    TARGETS pycr
    LIBRARY DESTINATION lib/python
    )
  
endif (CR_WITH_PYTHON)

## ------------------------------------------------------------------------------
## Graphical user interface

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
    GUI/hfqt.cc moc_finddialog.cpp
    GUI/moc_gotocelldialog.cpp
    GUI/moc_mainwindow.cpp
    GUI/moc_sortdialog.cpp
    GUI/moc_spreadsheet.cpp
    GUI/moc_plotter.cpp
    GUI/moc_hfqt.cpp
    GUI/qrc_spreadsheet.cpp
    GUI/qrc_plotter.cpp
    )
  
  ## include directories
  include_directories (
    ${CR_SOURCE_DIR}/implement/GUI
    ${QT4_INCLUDES}
    )
  
  ## compiler instructions
  add_library (hfget ${libhfget_sources})
  
  ## linker instructions
  target_link_libraries (hfget
    ${QT4_LIBARY_QTGUI}
    ${QT4_LIBARY_QTCORE}
    ${PYTHON_LIBRARIES}
    ${BOOST_LIBRARIES}
    )

  ## Additional properties
  if (APPLE)
    set_target_properties (hfget
      PROPERTIES
      PREFIX "lib"
      SUFFIX .so
      COMPILE_FLAGS "-pipe -g -D_REENTRANT -w -fpermissive -DQT_GUI_LIB -DQT_CORE_LIB"
      LINK_FLAGS "-fPIC -flat_namespace"
      )
  else (APPLE)
    set_target_properties (hfget
      PROPERTIES
      PREFIX "lib"
      SUFFIX .so
      COMPILE_FLAGS "-pipe -g -D_REENTRANT -w -DQT_GUI_LIB -DQT_CORE_LIB"
      LINK_FLAGS "-fPIC -shared"
      )
  endif (APPLE)
  
endif (CR_WITH_GUI)

## ------------------------------------------------------------------------------
## Test programs

if (CR_BUILD_TESTS)
  foreach (cr_module ${cr_modules})
    ADD_SUBDIRECTORY ("${cr_module}/test")
  endforeach (cr_module)
endif (CR_BUILD_TESTS)

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (
  TARGETS cr
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## installation of header files
