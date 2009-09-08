
## ==============================================================================
##
##  Handle configuration and build of FFTW3
##
## ==============================================================================

set (fftw_version "3.1.2")
set (fftw_name "fftw-${fftw_version}-modified")
set (fftw_directory "fftw/fftw-${fftw_version}-modified")
set (fftw_archive "${fftw_directory}.tgz")

add_custom_target (fftw_unpack
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/data/${fftw_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement
  COMMENT "Unpacking tar archive of FFTW ..."
  )

add_custom_target (fftw_configure
  COMMAND ./configure  --enable-shared --enable-single --prefix=${CMAKE_INSTALL_PREFIX} --enable-sse
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${fftw_name} 
  COMMENT "Configuring project FFTW ..."
  )
add_dependencies (fftw_configure fftw_unpack)

add_custom_target (fftw_build
  COMMAND make 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${fftw_name}
  COMMENT "Building project FFTW ..."
  )
add_dependencies (fftw_build fftw_configure fftw_unpack)

add_custom_target (fftw_install
  COMMAND make install
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/implement/${fftw_name}
  COMMENT "Install project FFTW ..."
  )
add_dependencies (fftw_install fftw_build fftw_configure fftw_unpack)

## ==============================================================================
##
##  Handle configuration and build of Tempo
##
## ==============================================================================

set (tempo_version "11.010")
set (tempo_name "tempo${tempo_version}-modified")
set (tempo_directory "tempo/tempo${tempo_version}-modified")
set (tempo_archive "${tempo_directory}.tar.gz")
set (tempo_ephem_name "tempo_ephem${tempo_version}")
set (tempo_ephem_directory "tempo/tempo_ephem${tempo_version}")
set (tempo_ephem_archive "${tempo_ephem_directory}.tar.gz")

add_custom_target (tempo_unpack
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/data/${tempo_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Unpacking tar archive of Tempo ..."
  )

add_custom_target (tempo_ephem_unpack
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/data/${tempo_ephem_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Unpacking tar archive of Tempo Ephem ..."
  )
add_dependencies (tempo_unpack tempo_ephem_unpack)

add_custom_target (tempo_build
  COMMAND make 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo_name}/src
  COMMENT "Building project Tempo ..."
  )
add_dependencies (tempo_build tempo_unpack)

add_custom_target (tempo_install
  COMMAND cp tempo ../../../../../release/bin
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo_name}/src
  COMMENT "Install project Tempo ..."
  )
add_dependencies (tempo_install tempo_build tempo_unpack)
