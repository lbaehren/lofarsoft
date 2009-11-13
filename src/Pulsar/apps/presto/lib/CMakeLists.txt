
## ==============================================================================
##
##  Handle the install of python library scripts for presto
##
## ==============================================================================

add_custom_target (presto_pylibscripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/lib/python/*.py ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/lib
  COMMENT "Install presto python library scripts into ${CMAKE_INSTALL_PREFIX}/bin..."
  )
add_dependencies (presto_pylibscripts_install presto_pyscripts_install)

