
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

## -------------------------------------------------------------------
## imagefromfits  WCSLIB MIRLIB LAPACK F77 CFITSIO

#if (HAVE_CASACORE OR HAVE_CASA)
#  if (HAVE_CFITSIO)
#    add_executable (imagefromfits imagefromfits.cc)
#    target_link_libraries (imagefromfits
#      ${CASA_LIBRARIES}
#      ${CFITSIO_LIBRARIES}
#      ${LAPACK_LIBRARIES}
#      ${CPGPLOT_LIBRARIES}
#      )
#  endif (HAVE_CFITSIO)
#endif (HAVE_CASACORE OR HAVE_CASA)
  
## -------------------------------------------------------------------
## treetest  GLISH CFITSIO WCSLIB

if (LAPACK_LIBRARIES AND G2C_LIBRARIES)
  ## compiler instructions
  add_executable (treetest treetest.cc)
  ## linker instructions
  target_link_libraries (treetest cr)
  ## installation instructions
  list (APPEND cr_apps treetest)
else (LAPACK_LIBRARIES AND G2C_LIBRARIES)
  message (STATUS "[CR-Tools] Unable to build executable treetest.")
endif (LAPACK_LIBRARIES AND G2C_LIBRARIES)

## -------------------------------------------------------------------
## trigrun   GLISH POPT LAPACK F77 CFITSIO WCSLIB

if (HAVE_POPT AND LAPACK_LIBRARIES)
  ## compiler instructions
  add_executable (trigrun trigrun.cc)
  ## additional properties
  set_target_properties (trigrun
    PROPERTIES
    COMPILE_FLAGS "-I${POPT_INCLUDES}"
    )
  ## linker instructions
  target_link_libraries (trigrun
    cr
    ${POPT_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps trigrun)
endif (HAVE_POPT AND LAPACK_LIBRARIES)

## -------------------------------------------------------------------
## simStationBeam

if (HAVE_CASACORE)
  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (simStationBeam simStationBeam.cc)
    ## linker instructions
    target_link_libraries (simStationBeam cr)
    ## installation instructions
    list (APPEND cr_apps simStationBeam)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)
endif (HAVE_CASACORE)

## -------------------------------------------------------------------
## skymapper

if (HAVE_CASACORE AND HAVE_DAL AND HAVE_BOOST)
  ## compiler instructions
  add_executable (skymapper skymapper.cc)
  ## linker instructions
  target_link_libraries (skymapper cr ${BOOST_LIBRARIES})
  ## installation instructions
  list (APPEND cr_apps skymapper)
endif (HAVE_CASACORE AND HAVE_DAL AND HAVE_BOOST)

## -------------------------------------------------------------------
## reas2event

if (HAVE_CASA OR HAVE_CASACORE)
  add_executable (reas2event reas2event.cc)
  ## linker instructions
  target_link_libraries (reas2event cr)
  ## additional properties to consider
  set_target_properties (reas2event
    PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    )
  ## installation instructions
  list (APPEND cr_apps reas2event)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## calcSourcePosition

if (HAVE_CASA OR HAVE_CASACORE)
  include_directories (${CR_SOURCE_DIR}/implement)
  ## compiler instructions
  add_executable (calcSourcePosition calcSourcePosition.cc)
  ## linker instructions
  target_link_libraries (calcSourcePosition cr)
  ## installation instructions
  list (APPEND cr_apps calcSourcePosition)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## call_pipeline

if (CR_WITH_STARTOOLS)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (call_pipeline call_pipeline.cc ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (call_pipeline
	PROPERTIES
	LINK_FLAGS "-bind_at_load"
	)
    endif (APPLE)
    ## linker instructions
    target_link_libraries (call_pipeline
      cr
      ${STARTOOLS_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${FFTW3_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps call_pipeline)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_STARTOOLS)

## -------------------------------------------------------------------
## readLopesCalTable

if (HAVE_CASA OR HAVE_CASACORE)
    add_executable (readLopesCalTable readLopesCalTable.cc)
    ## linker instructions
    target_link_libraries (readLopesCalTable cr)
    ## installation instructions
  list (APPEND cr_apps readLopesCalTable)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## writeLopesCalTable

if (HAVE_CASA OR HAVE_CASACORE)
    add_executable (writeLopesCalTable writeLopesCalTable.cc)
    ## linker instructions
    target_link_libraries (writeLopesCalTable cr)
    ## installation instructions
  list (APPEND cr_apps writeLopesCalTable)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## getPhases

if (HAVE_CASA OR HAVE_CASACORE)
    add_executable (getPhases getPhases.cc)
    ## linker instructions
    target_link_libraries (getPhases cr)
    ## installation instructions
  list (APPEND cr_apps getPhases)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## tbbctlResort

if (HAVE_CASA OR HAVE_CASACORE)
  add_executable (tbbctlResort tbbctlResort.cc)
  ## linker instructions
  target_link_libraries (tbbctlResort cr)
  ## installation instructions
  list (APPEND cr_apps tbbctlResort)
else (HAVE_CASA OR HAVE_CASACORE)
  message (STATUS "[CR-Tools] Unable to build executable tbbctlResort.")
endif (HAVE_CASA OR HAVE_CASACORE)


## -------------------------------------------------------------------
## tbbTest

if (HAVE_CASA OR HAVE_CASACORE)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (tbbTest tbbTest.cc)
    ## linker instructions
    target_link_libraries (tbbTest
      cr
      ${DAL_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps tbbTest)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable tbbTest.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## seftest

if (HAVE_CASA OR HAVE_CASACORE)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (seftest seftest.cc)
    ## linker instructions
    target_link_libraries (seftest
      cr
      ${DAL_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps seftest)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable seftest.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## tbbStatistics

if (HAVE_CASA OR HAVE_CASACORE)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (tbbStatistics tbbStatistics.cc)
    ## linker instructions
    target_link_libraries (tbbStatistics
      cr
      ${DAL_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps tbbStatistics)
    ## automated execution within test environment
    if (dataset_tbb)
      add_test (tbbStatistics tbbStatistics ${dataset_tbb})
    endif (dataset_tbb)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable tbbStatistics.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
## calculatePhaseBehaviour

if (HAVE_STARTOOLS AND HAVE_PLPLOT)
  add_executable (calculatePhaseBehaviour calculatePhaseBehaviour.cc)
  ## extra properties to take care of
  if (APPLE)
    set_target_properties (calculatePhaseBehaviour
      PROPERTIES
      LINK_FLAGS "-bind_at_load"
      )
  endif (APPLE)
  ## linker instructions
  target_link_libraries (calculatePhaseBehaviour
    cr
    ${STARTOOLS_LIBRARIES}
    ${ROOT_LIBRARIES}
    ${FFTW3_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps calculatePhaseBehaviour)
endif (HAVE_STARTOOLS AND HAVE_PLPLOT)



## -------------------------------------------------------------------

## Glish client programs

if (CR_WITH_GLISH AND HAVE_GLISH)
  add_subdirectory (GlishClients)
endif (CR_WITH_GLISH AND HAVE_GLISH)

## ==============================================================================
##
##  Installation
##
## ==============================================================================

install (TARGETS ${cr_apps}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
