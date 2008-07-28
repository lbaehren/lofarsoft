## ------------------------------------------------------------------------------
## $Id::                                                                        $
## ------------------------------------------------------------------------------

## Assemble the list files to install

file (GLOB data_inspection_sources *.py)

## ------------------------------------------------------------------------------
## Installation

## python scripts

install (FILES ${data_inspection_sources}
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)
