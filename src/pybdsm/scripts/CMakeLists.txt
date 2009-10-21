
## ==============================================================================
##
## Installation
##
## ==============================================================================

file (GLOB python_scripts *.py)

install (FILES ${python_scripts}
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
)
