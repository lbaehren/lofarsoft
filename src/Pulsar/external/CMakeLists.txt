
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
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/external/${fftw_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external
  COMMENT "Unpacking tar archive of FFTW from ${PULSAR_SOURCE_DIR}/external/${fftw_archive} to ${PULSAR_BINARY_DIR}/external ..."
  )

add_custom_target (fftw_configure
  COMMAND ./configure  --enable-shared --enable-single --prefix=${CMAKE_INSTALL_PREFIX} --enable-sse
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${fftw_name} 
  COMMENT "Configuring project FFTW in ${PULSAR_BINARY_DIR}/external/${fftw_name} ..."
  )
add_dependencies (fftw_configure fftw_unpack)

add_custom_target (fftw_build
  COMMAND make 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${fftw_name}
  COMMENT "Building project FFTW in ${PULSAR_BINARY_DIR}/external/${fftw_name}..."
  )
add_dependencies (fftw_build fftw_configure fftw_unpack)

add_custom_target (fftw_install
  COMMAND make install
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${fftw_name}
  COMMENT "Install project FFTW into ${CMAKE_INSTALL_PREFIX}..."
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
  COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/external/${tempo_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Unpacking tar archive of Tempo from ${PULSAR_SOURCE_DIR}/external/${tempo_archive} to ${PULSAR_BINARY_DIR}/apps..."
  )

add_custom_target (tempo_build
  COMMAND make 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo_name}/src
  COMMENT "Building project Tempo in ${PULSAR_BINARY_DIR}/apps/${tempo_name}/src ..."
  )
add_dependencies (tempo_build tempo_unpack)

add_custom_target (tempo_install
  COMMAND cp src/tempo ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND sed -e \"s|CMAKE_CHANGE|${CMAKE_INSTALL_PREFIX}/bin|g\" tempo.cfg > ${CMAKE_INSTALL_PREFIX}/bin/tempo.cfg
  COMMAND cp tempo.hlp obsys.dat ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND cp -r ephem ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND cp -r clock ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND cp -r test ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND cp -r tzpar ${CMAKE_INSTALL_PREFIX}/bin
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo_name}/
  COMMENT "Copying tempo executable to ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Executing sed command: sed 's|CMAKE_CHANGE|$CMAKE_INSTALL_PREFIX|g' tempo.cfg > ${CMAKE_INSTALL_PREFIX}/bin/tempo.cfg"
  COMMENT "Copying tempo.hlp and obsys.dat ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Copying ephem directory to ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Copying clock directory to ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Copying test directory to ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Copying tzpar directory to ${CMAKE_INSTALL_PREFIX}/bin..."
  COMMENT "Installed project Tempo to ${CMAKE_INSTALL_PREFIX}/bin..."
  )
add_dependencies (tempo_install tempo_build tempo_unpack)

## ==============================================================================
##
##  Handle configuration and build of PPGPLOT (Python PGPLOT Bindings)
##
## ==============================================================================

#set (ppgplot_cmake_build TRUE)

#if (ppgplot_cmake_build)
  
#  add_subdirectory (ppgplot1.1-modified)
  
#else (ppgplot_cmake_build)
  
  set (ppgplot_version "1.1")
  set (ppgplot_name "ppgplot${ppgplot_version}-modified")
  set (ppgplot_directory "ppgplot/ppgplot${ppgplot_version}-modified")
  set (ppgplot_archive "${ppgplot_directory}.tar.gz")
  
  add_custom_target (ppgplot_unpack
    COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/external/${ppgplot_archive}
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Unpacking tar archive of PPGPLOT from ${PULSAR_SOURCE_DIR}/external/${ppgplot_archive} to ${PULSAR_BINARY_DIR}/apps..."
    )
  
  get_filename_component(GFORTRAN_LIBRARY_DIR ${GFORTRAN_LIBRARY} PATH)

configure_file (
    ${PULSAR_SOURCE_DIR}/external/ppgplot/setup.py.in
    ${PULSAR_SOURCE_DIR}/external/ppgplot/setup.py
)

add_custom_target (ppgplot_build
  COMMAND cp ${PULSAR_SOURCE_DIR}/external/ppgplot/setup.py ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}/setup.py
  COMMAND python setup.py build_ext --inplace
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}
  COMMENT "Building project PPGPLOT in ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}..."
  COMMENT "Building project PPGPLOT with command: python ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}/setup.py build_ext --inplace"
  )
add_dependencies (ppgplot_build ppgplot_unpack)


add_custom_target (ppgplot_install
  COMMAND cp ppgplot.so ${CMAKE_INSTALL_PREFIX}/lib
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}
  COMMENT "Install project PPGPLOT library ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}/ppgplot.so into ${CMAKE_INSTALL_PREFIX}/lib..."
  )
add_dependencies (ppgplot_install ppgplot_build ppgplot_unpack)

#endif (ppgplot_cmake_build)