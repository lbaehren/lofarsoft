##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Tests without extra requirements
##
## ==============================================================================

## Set up the list with test program sources

set (tests
  tAntenna
  tAntennaGain
  tBeamformer
  tFeed
  tGeomDelay
  tGeomPhase
  tGeomWeight
  tMDirection
  tSkymapGrid
  tSkymapOrientation
  )

## Build the test programs

foreach (src ${tests})
  ## compiler instructions
  add_executable (${src} ${src}.cc)
  ## linker instructions
  target_link_libraries (${src} cr)
  ## testing
  add_test (${src} ${src})
endforeach (src)

## ==============================================================================
##
##  Tests needing extra requirements
##
## ==============================================================================

##____________________________________________________________________
## tcasacoreImages

## compiler instructions
add_executable (tcasacoreImages tcasacoreImages.cc)
## linker instructions
if (HAVE_HDF5) 
  target_link_libraries (tcasacoreImages
    cr
    ${HDF5_LIBRARIES}
    )
else (HAVE_HDF5)
  target_link_libraries (tcasacoreImages
    cr
    )
endif (HAVE_HDF5)
## testing
add_test (tcasacoreImages tcasacoreImages)

##____________________________________________________________________
## tSkymapper

if (HAVE_DAL AND HAVE_HDF5 AND positions_lopes08)
  
  ## compiler instructions
  add_executable (tSkymapper tSkymapper.cc)
  ## linker instructions
  target_link_libraries (tSkymapper
    cr
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  if (dataset_lopes)
    add_test (tSkymapper tSkymapper ${dataset_lopes})
  else (dataset_lopes)
    add_test (tSkymapper tSkymapper)
  endif (dataset_lopes)
  ## installation
  list (APPEND tests tSkymapper)

endif (HAVE_DAL AND HAVE_HDF5 AND positions_lopes08)

## ==============================================================================
##
##  Installation
##
## ==============================================================================

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
