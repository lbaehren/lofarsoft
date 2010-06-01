
## ==============================================================================
##
##  Handle the install of extra processing scripts
##
## ==============================================================================

#
add_custom_target (scripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/*.py ${PULSAR_SOURCE_DIR}/scripts/*.sh ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/*.py ${CMAKE_INSTALL_PREFIX}/bin/*.sh
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/lofar_default.inf ${CMAKE_INSTALL_PREFIX}/data/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/scripts
  COMMENT "Install extra processing scripts and LOFAR inf file into ${CMAKE_INSTALL_PREFIX}/bin..."
  )

