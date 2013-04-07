
## ==============================================================================
##
##  Handle the install of extra processing scripts
##
## ==============================================================================

add_custom_target (scripts_install
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/GRID/* ${PULSAR_SOURCE_DIR}/scripts/*.py ${PULSAR_SOURCE_DIR}/scripts/*.sh ${PULSAR_SOURCE_DIR}/scripts/*.pl ${PULSAR_SOURCE_DIR}/scripts/*.csh ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/pypulp/*.py ${CMAKE_INSTALL_PREFIX}/lib/python/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/pypulp/pulp.py ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/RTCP2
  COMMAND cp -r ${PULSAR_SOURCE_DIR}/scripts/RTCP2 ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/Pulsar_Search_LOFAR-v1.txt ${CMAKE_INSTALL_PREFIX}/doc/
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/pulp.sh
  COMMAND ln -s ${CMAKE_INSTALL_PREFIX}/bin/make_subs_SAS_Ncore_Mmodes.sh ${CMAKE_INSTALL_PREFIX}/bin/pulp.sh
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/lofar_default.inf ${PULSAR_SOURCE_DIR}/scripts/psr_cats.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/*.sh ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/*.py ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/XML-template-*txt ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/multi-template_help*txt ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/xml_generator_BF_help.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/PSR_catalog.txt ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/PSR_catalog_NA.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/3cCatalog_RADEC.txt ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/3cCatalog_psrHBA.txt ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/3cCatalog_psrLBA.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/XML-MOM/Combined_3C_PSR_catalog.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND ${CMAKE_COMMAND} -E copy ${PULSAR_SOURCE_DIR}/scripts/pulp_help.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND ${CMAKE_COMMAND} -E copy ${PULSAR_SOURCE_DIR}/scripts/master.rfi ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND ${CMAKE_COMMAND} -E copy ${PULSAR_SOURCE_DIR}/scripts/stations.txt ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND ${CMAKE_COMMAND} -E copy ${PULSAR_SOURCE_DIR}/scripts/header.puma2 ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/*.py ${CMAKE_INSTALL_PREFIX}/bin/*.sh ${CMAKE_INSTALL_PREFIX}/bin/*.pl ${CMAKE_INSTALL_PREFIX}/bin/*.csh
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/data/parfile
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/scripts
  COMMENT "Install extra processing scripts and LOFAR inf file into ${CMAKE_INSTALL_PREFIX}/bin and ${CMAKE_INSTALL_PREFIX}/data..."
  COMMENT "Copying parfiles to ${CMAKE_INSTALL_PREFIX}/data/parfile..."
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/parfiles/*.par ${CMAKE_INSTALL_PREFIX}/data/parfile/
  COMMENT "Copying Haslam 408 MHz skymap to ${CMAKE_INSTALL_PREFIX}/data..."
  COMMAND cp ${PULSAR_SOURCE_DIR}/scripts/lambda_mollweide_haslam408_dsds.fits ${CMAKE_INSTALL_PREFIX}/data/
  )

