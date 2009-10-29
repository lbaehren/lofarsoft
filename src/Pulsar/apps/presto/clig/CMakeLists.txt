
## ==============================================================================
##
##  Handle the install of presto clig command line help files
##
## ==============================================================================

add_custom_target (presto_clig_install
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/clig
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/clig/*.1 ${PULSAR_SOURCE_DIR}/apps/presto/clig/*.cli ${CMAKE_INSTALL_PREFIX}/clig/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/clig
  COMMENT "Install presto python scripts into ${CMAKE_INSTALL_PREFIX}/clig..."
  )

