
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
#  COMMAND ./configure  --enable-shared --enable-single --prefix=${CMAKE_INSTALL_PREFIX} --enable-sse
  COMMAND ./configure  --enable-shared --enable-single --prefix=${CMAKE_INSTALL_PREFIX}
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
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/tempo.hlp ${CMAKE_INSTALL_PREFIX}/obsys.dat ${CMAKE_INSTALL_PREFIX}/ephem ${CMAKE_INSTALL_PREFIX}/clock ${CMAKE_INSTALL_PREFIX}/test ${CMAKE_INSTALL_PREFIX}/tzpar ${CMAKE_INSTALL_PREFIX}/tztot.dat
  COMMAND cp tempo.hlp obsys.dat tztot.dat ${CMAKE_INSTALL_PREFIX}/bin
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

## ==============================================================================
##
##  Handle configuration and build of PSRCHIVE
##
## ==============================================================================

set (psrchive_version "13.3")
set (psrchive_name "psrchive-${psrchive_version}")
set (psrchive_directory "psrchive/psrchive-${psrchive_version}")

add_custom_target (psrchive_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${psrchive_name}
  COMMAND cp -r ${PULSAR_SOURCE_DIR}/external/${psrchive_directory} ${PULSAR_BINARY_DIR}/apps/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying PSRCHIVE from ${PULSAR_SOURCE_DIR}/external/${psrchive_directory} to ${PULSAR_BINARY_DIR}/apps/${psrchive_name}"
  )

#add_custom_target (psrchive_bootstrap
#  COMMAND ./bootstrap
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
#  COMMENT "Running bootstrap for project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
#  )
#add_dependencies (psrchive_bootstrap psrchive_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (psrchive_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=mac --x-includes=${X11_X11_INCLUDE_PATH} --x-libraries=${X11_X11_INCLUDE_PATH}/../lib --build=mac --enable-debug --disable-local --with-mpi --with-fftw-include-dir=${CMAKE_INSTALL_PREFIX}/include --with-fftw-lib-dir=${CMAKE_INSTALL_PREFIX}/lib
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
  COMMENT "Configuring project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
  )
#add_dependencies (psrchive_configure psrchive_bootstrap psrchive_unpack)
add_dependencies (psrchive_configure psrchive_unpack)
 else (APPLE)
add_custom_target (psrchive_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=linux
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
  COMMENT "Configuring project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
  )
#add_dependencies (psrchive_configure psrchive_bootstrap psrchive_unpack)
add_dependencies (psrchive_configure psrchive_unpack)
 endif (APPLE)
endif (UNIX)


add_custom_target (psrchive_build
  COMMAND make 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name}
  COMMENT "Building project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name}..."
  )
#add_dependencies (psrchive_build psrchive_configure psrchive_bootstrap psrchive_unpack)
add_dependencies (psrchive_build psrchive_configure psrchive_unpack)

add_custom_target (psrchive_install
  COMMAND make install
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name}
  COMMENT "Install project psrchive into ${CMAKE_INSTALL_PREFIX}..."
  )
#add_dependencies (psrchive_install psrchive_build psrchive_configure psrchive_bootstrap psrchive_unpack)
add_dependencies (psrchive_install psrchive_build psrchive_configure psrchive_unpack)

## ==============================================================================
##
##  Handle configuration and build of PSRDADA
##
## ==============================================================================

set (psrdada_version "20100817")
set (psrdada_name "psrdada-${psrdada_version}")
set (psrdada_directory "psrdada/psrdada-${psrdada_version}")

add_custom_target (psrdada_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMAND cp -r ${PULSAR_SOURCE_DIR}/external/${psrdada_directory} ${PULSAR_BINARY_DIR}/apps/
  COMMAND mkdir ${PULSAR_BINARY_DIR}/apps/${psrdada_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying psrdada from ${PULSAR_SOURCE_DIR}/external/${psrdada_directory} to ${PULSAR_BINARY_DIR}/apps/${psrdada_name}"
  )

add_custom_target (psrdada_bootstrap
  COMMAND ./bootstrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name} 
  COMMENT "Running bootstrap for project psrdada in ${PULSAR_BINARY_DIR}/apps/${psrdada_name} ..."
  )
add_dependencies (psrdada_bootstrap psrdada_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (psrdada_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=darwin --with-psrchive-dir=${CMAKE_INSTALL_PREFIX}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Configuring project psrdada (darwin) in ${PULSAR_BINARY_DIR}/apps/${psrdada_name} ..."
  )
add_dependencies (psrdada_configure psrdada_bootstrap psrdada_unpack)
 else (APPLE)
add_custom_target (psrdada_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Configuring project psrdada (linux) in ${PULSAR_BINARY_DIR}/apps/${psrdada_name} ..."
  )
add_dependencies (psrdada_configure psrdada_bootstrap psrdada_unpack)
 endif (APPLE)
endif (UNIX)

add_custom_target (psrdada_build
  COMMAND make prefix=${PULSAR_BINARY_DIR}/apps/${psrdada_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Building project psrdada in ${PULSAR_BINARY_DIR}/apps/${psrdada_name}..."
  )
add_dependencies (psrdada_build psrdada_configure psrdada_unpack)

add_custom_target (psrdada_install
  COMMAND make install prefix=${CMAKE_INSTALL_PREFIX}/psrdada
  COMMAND cd ${CMAKE_INSTALL_PREFIX}/include && find ./ -type l -exec rm {} "\;"
  COMMAND cd ${CMAKE_INSTALL_PREFIX}/include && find ../psrdada/include "\\(" -iname \"*.h\" -not -iname \"ascii_header.h\" "\\)" -exec ln -s {} . "\;"
  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib && find ./ -type l "\\(" -not -iname \"libfftw3f*\" "\\)" -exec rm {} "\;"
  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib && ln -s ../psrdada/lib/* .
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Install project psrdada into ${CMAKE_INSTALL_PREFIX}/psrdada..."
  )
add_dependencies (psrdada_install psrdada_build psrdada_configure psrdada_bootstrap psrdada_unpack)

## ==============================================================================
##
##  Handle configuration and build of DSPSR
##
## ==============================================================================

set (dspsr_version "1.0")
set (dspsr_name "dspsr-${dspsr_version}")
set (dspsr_directory "dspsr/dspsr-${dspsr_version}")

add_custom_target (dspsr_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMAND cp -r ${PULSAR_SOURCE_DIR}/external/${dspsr_directory} ${PULSAR_BINARY_DIR}/apps/
  COMMAND mkdir ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying DSPSR from ${PULSAR_SOURCE_DIR}/external/${dspsr_directory} to ${PULSAR_BINARY_DIR}/apps/${dspsr_name}"
  )

add_custom_target (dspsr_bootstrap
  COMMAND ./bootstrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name} 
  COMMENT "Running bootstrap for project dspsr in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
add_dependencies (dspsr_bootstrap dspsr_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (dspsr_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=darwin --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Configuring project dspsr (darwin) in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
add_dependencies (dspsr_configure dspsr_bootstrap dspsr_unpack)
 else (APPLE)
add_custom_target (dspsr_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Configuring project dspsr (linux) in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
add_dependencies (dspsr_configure dspsr_bootstrap dspsr_unpack)
 endif (APPLE)
endif (UNIX)

add_custom_target (dspsr_build
  COMMAND make prefix=${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Building project dspsr in ${PULSAR_BINARY_DIR}/apps/${dspsr_name}..."
  )
add_dependencies (dspsr_build dspsr_configure dspsr_unpack)

add_custom_target (dspsr_install
  COMMAND make install prefix=${CMAKE_INSTALL_PREFIX} -I${CMAKE_INSTALL_PREFIX}/psrdada/include 
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Install project dspsr into ${CMAKE_INSTALL_PREFIX}..."
  )
add_dependencies (dspsr_install dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)

## ==============================================================================
##
##  Handle configuration and build of SSPS (Single Pulse Library code, Thijs Coenen)
##
## ==============================================================================

#set (ssps_cmake_build TRUE)

#if (ssps_cmake_build)
  
#  add_subdirectory (ssps0.0)
  
#else (ssps_cmake_build)
  
  set (ssps_version "0.1")
  set (ssps_name "ssps-release-${ssps_version}")
  set (ssps_directory "ssps/ssps-release-${ssps_version}")
  set (ssps_archive "${ssps_directory}.tar.gz")
  
  add_custom_target (ssps_unpack
    COMMAND cd ${PULSAR_BINARY_DIR}/apps
    COMMAND rm -rf ${ssps_name}
    COMMAND ${tar_executable} -xvzf ${PULSAR_SOURCE_DIR}/external/${ssps_archive}
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Unpacking tar archive of SSPS from ${PULSAR_SOURCE_DIR}/external/${ssps_archive} to ${PULSAR_BINARY_DIR}/apps/${ssps_name}..."
    )
  
add_custom_target (ssps_build
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMAND python setup.py build --build-lib=build/lib
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMENT "Building project SSPS in ${PULSAR_BINARY_DIR}/apps/${ssps_name}..."
  COMMENT "Building project SSPS with command: python ${PULSAR_BINARY_DIR}/apps/${ssps_name}/setup.py build"
  )
add_dependencies (ssps_build ssps_unpack)

add_custom_target (ssps_build_docs
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/${ssps_name}/docs
  COMMAND make html
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ssps_name}/docs
  COMMENT "Building docs from project SSPS in ${PULSAR_BINARY_DIR}/apps/${ssps_name}/docs..."
  COMMENT "Building docs from project SSPS with command: make html"
  )
add_dependencies (ssps_build_docs ssps_build ssps_unpack)

add_custom_target (ssps_install_docs
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/doc/ssps
  COMMAND mkdir ${CMAKE_INSTALL_PREFIX}/doc/ssps
  COMMAND cp -r ${PULSAR_BINARY_DIR}/apps/${ssps_name}/docs/_build/html/* ${CMAKE_INSTALL_PREFIX}/doc/ssps/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMENT "Install docs for SSPS ${PULSAR_BINARY_DIR}/apps/${ssps_name}/docs/_build/html into ${CMAKE_INSTALL_PREFIX}/doc/ssps..."
  )
add_dependencies (ssps_install_docs ssps_build_docs ssps_build ssps_unpack)

add_custom_target (ssps_install
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/lib/python/ssps
  COMMAND cp -r ${PULSAR_BINARY_DIR}/apps/${ssps_name}/build/lib/ssps ${CMAKE_INSTALL_PREFIX}/lib/python/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMENT "Install project SSPS library ${PULSAR_BINARY_DIR}/apps/${ssps_name}/build/lib/ssps into ${CMAKE_INSTALL_PREFIX}/lib/python/ssps..."
  )
#add_dependencies (ssps_install ssps_install_docs ssps_build ssps_unpack)
add_dependencies (ssps_install ssps_build ssps_unpack)

#endif (ssps_cmake_build)

## ==============================================================================
##
##  Handle configuration and build of PSRCAT
##
## ==============================================================================

set (psrcat_version "V1.32")
set (psrcat_name "psrcat-${psrcat_version}")
set (psrcat_directory "psrcat/psrcat-${psrcat_version}")

add_custom_target (psrcat_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/psrcat
  COMMAND cp ${PULSAR_SOURCE_DIR}/external/${psrcat_directory}.tar.gz ${PULSAR_BINARY_DIR}/apps/
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/
  COMMAND tar xvfz ${psrcat_name}.tar.gz
  COMMAND rm ${psrcat_name}.tar.gz
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying PSRCAT from ${PULSAR_SOURCE_DIR}/external/${psrcat_directory}.tar.gz to ${PULSAR_BINARY_DIR}/apps/${psrcat_name}.tar.gz"
  )

add_custom_target (psrcat_build
  COMMAND ./makeit
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/psrcat
  COMMENT "Building project psrcat in ${PULSAR_BINARY_DIR}/apps/psrcat..."
  )
add_dependencies (psrcat_build psrcat_unpack)

add_custom_target (psrcat_install
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/psrcat/psrcat.db ${CMAKE_INSTALL_PREFIX}/data/
  COMMAND cp ${PULSAR_BINARY_DIR}/apps/psrcat/psrcat ${CMAKE_INSTALL_PREFIX}/bin/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/psrcat
  COMMENT "Install project psrcat into ${CMAKE_INSTALL_PREFIX}/data and ${CMAKE_INSTALL_PREFIX}/bin..."
  )
add_dependencies (psrcat_install psrcat_build psrcat_unpack)


