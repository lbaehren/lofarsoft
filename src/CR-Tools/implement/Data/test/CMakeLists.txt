##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## ==============================================================================
##
##  Tests without extra requirements
##
## ==============================================================================

##____________________________________________________________________
## Set up the list with test program sources

set (tests
  tData
  tDataFlag
  tITS_Beam
  tITSMetadata
  tLopesEvent
  tLopesEventIn
  tNDABeam
  tObservationData
  tTIM40Header
  ttbbctlIn
  trawSubbandIn
  tUpSampledDR
  )

##____________________________________________________________________
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
##  Installation
##
## ==============================================================================

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
