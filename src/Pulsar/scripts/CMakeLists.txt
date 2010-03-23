
## ==============================================================================
##
##  Handle the install of extra processing scripts
##
## ==============================================================================

#
add_custom_target (scripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/*.py ${PULSAR_SOURCE_DIR}/scripts/*.sh ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/scripts
  COMMENT "Install extra processing scripts into ${CMAKE_INSTALL_PREFIX}/bin..."
  )

