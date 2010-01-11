
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

##____________________________________________________________________
##                                                           getPhases

if (HAVE_CASA OR HAVE_CASACORE)
    add_executable (getPhases getPhases.cc)
    ## linker instructions
    target_link_libraries (getPhases cr)
    ## installation instructions
  list (APPEND cr_apps getPhases)
else (HAVE_CASA OR HAVE_CASACORE)
  message (STATUS "[CR-Tools] Unable to build executable getPhases.")
endif (HAVE_CASA OR HAVE_CASACORE)

##____________________________________________________________________
##                                                          reas2event

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
else (HAVE_CASA OR HAVE_CASACORE)
  message (STATUS "[CR-Tools] Unable to build executable reas2event.")
endif (HAVE_CASA OR HAVE_CASACORE)

##____________________________________________________________________
##                                                      simStationBeam

if (HAVE_CASACORE)
  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (simStationBeam simStationBeam.cc)
    ## linker instructions
    target_link_libraries (simStationBeam cr)
    ## installation instructions
    list (APPEND cr_apps simStationBeam)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)
else (HAVE_CASACORE)
  message (STATUS "[CR-Tools] Unable to build executable simStationBeam.")
endif (HAVE_CASACORE)

##____________________________________________________________________
##                                                              imager

if (HAVE_CASACORE AND HAVE_DAL AND HAVE_BOOST)
  ## compiler instructions
  add_executable (imager imager.cc)
  ## linker instructions
  target_link_libraries (imager
    cr
    ${HAVE_LIBZ}
    ${BOOST_PROGRAM_OPTIONS_LIBRARY}
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps imager)
  ## testing 
  add_test (imager_test01 imager --help)
  add_test (imager_test02 imager --infile ${dataset_lopes_example})
  add_test (imager_test03 imager --infile ${dataset_lopes_example} --blocksize 4096)
endif (HAVE_CASACORE AND HAVE_DAL AND HAVE_BOOST)

##____________________________________________________________________
##                                                             trigrun

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

if (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (call_pipeline
      call_pipeline.cc
      ${CR_SOURCE_DIR}/implement/Analysis/RootDict.C
      )
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (call_pipeline
	PROPERTIES
	COMPILE_FLAGS "-fsigned-char -fno-common"
	LINK_FLAGS "-bind_at_load"
	)
    endif (APPLE)
    ## linker instructions
    target_link_libraries (call_pipeline
      cr
      ${STARTOOLS_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps call_pipeline)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

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

  if (DAL_LIBRARIES AND HDF5_LIBRARIES AND CR_WITH_LEGACY AND CR_WITH_PLOTTING)

    ## compiler instructions
    add_executable (tbbTest tbbTest.cc)
    ## linker instructions
    target_link_libraries (tbbTest
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps tbbTest)
    
  else (DAL_LIBRARIES AND HDF5_LIBRARIES AND CR_WITH_LEGACY AND CR_WITH_PLOTTING)
    message (STATUS "[CR-Tools] Unable to build executable tbbTest.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES AND CR_WITH_LEGACY AND CR_WITH_PLOTTING)
  
endif (HAVE_CASA OR HAVE_CASACORE)

## -------------------------------------------------------------------
##                                                 testLOPESskymapping

if (HAVE_CASACORE AND HAVE_DAL)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (testLOPESskymapping testLOPESskymapping.cc)
    ## linker instructions
    target_link_libraries (testLOPESskymapping 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps testLOPESskymapping)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable testLOPESskymapping.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL)

## -------------------------------------------------------------------
##                                                          CalDynSpec

if (HAVE_CASA OR HAVE_CASACORE)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (CalDynSpec CalDynSpec.cc)
    ## linker instructions
    target_link_libraries (CalDynSpec
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps CalDynSpec)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable CalDynSpec.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASA OR HAVE_CASACORE)

##_______________________________________________________________________________
##                                                     snstestlightningskymapping

if (HAVE_CASACORE AND HAVE_DAL)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (snstestlightningskymapping snstestlightningskymapping.cc)
    ## linker instructions
    target_link_libraries (snstestlightningskymapping 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps snstestlightningskymapping)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable snstestlightningskymapping.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL)

##_______________________________________________________________________________
##                                                           snstestnewskymapping

if (HAVE_CASACORE AND HAVE_DAL)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (snstestnewskymapping snstestnewskymapping.cc)
    ## linker instructions
    target_link_libraries (snstestnewskymapping 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps snstestnewskymapping)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable snstestnewskymapping.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL)

##_______________________________________________________________________________
##                                                                    imagetofits

if (HAVE_CASACORE AND HAVE_DAL)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (imagetofits imagetofits.cc)
    ## linker instructions
    target_link_libraries (imagetofits 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps imagetofits)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable imagetofits.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL)

##_______________________________________________________________________________
##                                                                     checkimage

if (HAVE_CASACORE AND HAVE_DAL)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (checkimage checkimage.cc)
    ## linker instructions
    target_link_libraries (checkimage 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps checkimage)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable checkimage.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL)

##_______________________________________________________________________________
##                                                                        cccheck

if (HAVE_CASACORE AND HAVE_DAL AND CR_WITH_PLOTTING)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (cccheck cccheck.cc)
    ## linker instructions
    target_link_libraries (cccheck 
      cr 
      ${DAL_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps cccheck)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable cccheck.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL AND CR_WITH_PLOTTING)

##_______________________________________________________________________________
##                                                                       cccheck2

if (HAVE_CASACORE AND HAVE_DAL AND CR_WITH_PLOTTING)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (cccheck2 cccheck2.cc)
    ## linker instructions
    target_link_libraries (cccheck2 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps cccheck2)
  else (DAL_LIBRARIES AND HDF5_LIBRARIES)
    message (STATUS "[CR-Tools] Unable to build executable cccheck2.")
  endif (DAL_LIBRARIES AND HDF5_LIBRARIES)
endif (HAVE_CASACORE AND HAVE_DAL AND CR_WITH_PLOTTING)

##_______________________________________________________________________________
##                                                                  tbbStatistics

if (HAVE_CASA OR HAVE_CASACORE)
  if (DAL_LIBRARIES AND HDF5_LIBRARIES)
    ## compiler instructions
    add_executable (tbbStatistics tbbStatistics.cc)
    ## linker instructions
    target_link_libraries (tbbStatistics
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
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

##_______________________________________________________________________________
##                                                        calculatePhaseBehaviour

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
    ${POSTGRESQL_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps calculatePhaseBehaviour)
endif (HAVE_STARTOOLS AND HAVE_PLPLOT)

##_______________________________________________________________________________
##                                                                   plotLopesRaw

if (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (plotLopesRaw plotLopesRaw.cc)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (plotLopesRaw
        PROPERTIES
        COMPILE_FLAGS "-fsigned-char -fno-common"
        LINK_FLAGS "-bind_at_load"
        )
    endif (APPLE)
    ## linker instructions
    target_link_libraries (plotLopesRaw
      cr
      ${STARTOOLS_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps plotLopesRaw)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

##_______________________________________________________________________________
##                                                                   simpleSkymap

if (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)
  ## compiler instructions
  add_executable (simpleSkymap simpleSkymap.cc)
  ## linker instructions
  target_link_libraries (simpleSkymap
    cr
     ${STARTOOLS_LIBRARIES}
     ${ROOT_LIBRARIES}
     ${FFTW3_LIBRARIES}
     ${POSTGRESQL_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps simpleSkymap)
endif (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

##_______________________________________________________________________________
##                                                                dynamicSpectrum

if (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

  if (HAVE_PGPLOT OR HAVE_PLPLOT)
    add_executable (dynamicSpectrum dynamicSpectrum.cc)
    ## extra properties to take care of
    if (APPLE)
      set_target_properties (dynamicSpectrum
        PROPERTIES
        COMPILE_FLAGS "-fsigned-char -fno-common"
        LINK_FLAGS "-bind_at_load"
        )
    endif (APPLE)
    ## linker instructions
    target_link_libraries (dynamicSpectrum
      cr
      ${STARTOOLS_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${FFTW3_LIBRARIES}
      ${POSTGRESQL_LIBRARIES}
      )
    ## installation instructions
    list (APPEND cr_apps dynamicSpectrum)
  endif (HAVE_PGPLOT OR HAVE_PLPLOT)

endif (CR_WITH_STARTOOLS AND CR_WITH_LEGACY)

##_______________________________________________________________________________
##                                                                   genEventlist

if (HAVE_ROOT)
  add_executable (genEventlist genEventlist.cc)
  ## linker instructions
  target_link_libraries (genEventlist
      ${ROOT_LIBRARIES}
      )
  ## installation instructions
  list (APPEND cr_apps genEventlist)
endif (HAVE_ROOT)

##_______________________________________________________________________________
##                                                             tbbTriggerAnalysis

if (HAVE_CASACORE AND DAL_LIBRARIES AND HDF5_LIBRARIES AND HAVE_BOOST)
  
  ## compiler instructions
  add_executable (tbbTriggerAnalysis tbbTriggerAnalysis.cc)
  ## linker instructions
  target_link_libraries (tbbTriggerAnalysis
    cr
    ${HAVE_LIBZ}
    ${DAL_LIBRARIES}
    ${CASA_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${BOOST_LIBRARIES}
    )
  ## installation instructions
  list (APPEND cr_apps tbbTriggerAnalysis)
  
endif (HAVE_CASACORE AND DAL_LIBRARIES AND HDF5_LIBRARIES AND HAVE_BOOST)


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
