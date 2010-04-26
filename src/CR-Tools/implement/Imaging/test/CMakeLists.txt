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
  target_link_libraries (${src}
    cr
    ${cr_link_libraries}
    )
  ## testing
  add_test (${src} ${src})
endforeach (src)

## ==============================================================================
##
##  Tests needing extra requirements
##
## ==============================================================================

##____________________________________________________________________
## tSkymapper

if (HAVE_DAL AND HAVE_HDF5 AND positions_lopes08)
  
  configure_file (${CR_SOURCE_DIR}/implement/Imaging/test/tSkymapper.sh
    ${CR_BINARY_DIR}/implement/Imaging/test/tSkymapper.sh
    )

  ## compiler instructions
  add_executable (tSkymapper tSkymapper.cc)
  ## linker instructions
  target_link_libraries (tSkymapper
    cr
    ${cr_link_libraries}
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
