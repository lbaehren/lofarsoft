
## ==============================================================================
##
##  Handle configuration and build of FFTW3
##
## ==============================================================================

set (fftw_version "3.1.2")
set (fftw_directory "fftw-${fftw_version}-modified")
set (fftw_archive "${fftw_directory}.tgz")

add_custom_target (fftw_unpack
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/data/${fftw_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement
  COMMENT "Unpacking tar archive of FFTW ..."
  )

add_custom_target (fftw_configure
  COMMAND ./configure
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${fftw_directory}
  COMMENT "Configuring project FFTW ..."
  )
add_dependencies (fftw_configure fftw_unpack)

add_custom_target (fftw_build
  COMMAND make
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${fftw_directory}
  COMMENT "Configuring project FFTW ..."
  )
add_dependencies (fftw_build fftw_configure)

## ==============================================================================
##
##  Handle configuration and build of SigProc
##
## ==============================================================================

set (sigproc_version "3.5")
set (sigproc_directory "sigproc-${sigproc_version}")
set (sigproc_archive "${sigproc_directory}_64bit_mods.tgz")

add_custom_target (sigproc_unpack
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/data/${sigproc_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement
  COMMENT "Unpacking tar archive of SigProc ..."
  )

add_custom_target (sigproc_configure
  COMMAND ./configure
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${sigproc_directory}
  COMMENT "Configuring project SigProc ..."
  )
add_dependencies (sigproc_configure sigproc_unpack)

