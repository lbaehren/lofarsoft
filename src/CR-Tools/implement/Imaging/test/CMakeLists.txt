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
  tBeamformer
  tFeed
  tGeomDelay
  tGeometricalDelay
  tGeometricalPhase
  tGeometricalWeight
  tMDirection
  tSkymapGrid
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
## tSkymapCoordinates

if (HAVE_DAL AND HAVE_HDF5)

  ## compiler instructions
  add_executable (tSkymapCoordinates tSkymapCoordinates.cc)
  ## linker instructions
  target_link_libraries (tSkymapCoordinates
    cr
    ${DAL_LIBRARIES}
    ${HDF5_LIBRARIES}
    )
  ## testing
  add_test (tSkymapCoordinates tSkymapCoordinates)
  ## installation
  list (APPEND tests tSkymapCoordinates)

endif (HAVE_DAL AND HAVE_HDF5)

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
  add_test (tSkymapper tSkymapper)
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
