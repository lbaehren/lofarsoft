
## ==============================================================================
##
##  Handle the install of python scripts for presto
##
## ==============================================================================

add_custom_target (presto_pyscripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/bin/*.py ${PULSAR_SOURCE_DIR}/apps/presto/bin/a2x ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/bin
  COMMENT "Install presto python scripts into ${CMAKE_INSTALL_PREFIX}/bin..."
  )

