## ------------------------------------------------------------------------------
## $Id::                                                                        $
## ------------------------------------------------------------------------------

##_______________________________________________________________________________
## Testing

if (contrib_ENABLE_TESTING)

  add_test (msinfo msinfo.py)

endif (contrib_ENABLE_TESTING)

##_______________________________________________________________________________
## Installation

## assemble list of files to install

file (GLOB data_inspection_sources *.py)

## installation destination

install (FILES ${data_inspection_sources}
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)
