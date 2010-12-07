## ==============================================================================
##
##  Handle the install of the lpps_search python modules
##
## ==============================================================================

add_custom_target (lpps_install
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/python/lpps_search
  COMMAND cp -r lpps_search ${CMAKE_INSTALL_PREFIX}/lib/python/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps
  COMMENT "Install project LPPS python modules ${PULSAR_SOURCE_DIR}/apps/lpps_search into ${CMAKE_INSTALL_PREFIX}/lib/python/lpps_search..."
  )
add_dependencies (lpps_install scripts_install)
