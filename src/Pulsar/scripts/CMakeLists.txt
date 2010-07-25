
## ==============================================================================
##
##  Handle the install of extra processing scripts
##
## ==============================================================================

#
add_custom_target (scripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/*.py ${PULSAR_SOURCE_DIR}/scripts/*.sh ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/pulp.sh
  COMMAND ln -s ${CMAKE_INSTALL_PREFIX}/bin/make_subs_SAS_Ncore_Mmodes.sh ${CMAKE_INSTALL_PREFIX}/bin/pulp.sh
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/lofar_default.inf ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/create_obs_template.sh ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/multi-templates.sh ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/xml_pipeline_kickoff.sh ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/XML-template-*txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/PSR_catalog.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/*.py ${CMAKE_INSTALL_PREFIX}/bin/*.sh
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/scripts
  COMMENT "Install extra processing scripts and LOFAR inf file into ${CMAKE_INSTALL_PREFIX}/bin and ${CMAKE_INSTALL_PREFIX}/data..."
  )

