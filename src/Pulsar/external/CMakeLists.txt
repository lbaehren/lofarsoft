
## ==============================================================================
##
##                                                                          FFTW3
##
## ==============================================================================

add_subdirectory (fftw)

## ==============================================================================
##
##                                                                          Tempo
##
## ==============================================================================

set (tempo_version         "20120213")
set (tempo_name            "tempo-${tempo_version}")
set (tempo_directory       "tempo/tempo-${tempo_version}")
set (tempo_archive         "${tempo_directory}.tar.gz")
set (tempo_ephem_name      "tempo_ephem${tempo_version}")
set (tempo_ephem_directory "tempo/tempo_ephem${tempo_version}")
set (tempo_ephem_archive   "${tempo_ephem_directory}.tar.gz")

add_custom_target (tempo_unpack
  COMMAND ${TAR_EXECUTABLE} -xvzf ${CMAKE_CURRENT_SOURCE_DIR}/${tempo_archive}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Unpacking tar archive of Tempo from ${PULSAR_SOURCE_DIR}/external/${tempo_archive} to ${PULSAR_BINARY_DIR}/apps..."
  )

add_custom_target (tempo_build
  COMMAND ./prepare
  COMMAND ./configure TEMPO=${CMAKE_INSTALL_PREFIX} --srcdir=${PULSAR_BINARY_DIR}/apps/${tempo_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} PGPLOT_DIR=${PGPLOT_INCLUDES}  
  COMMAND make
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo_name}/
  COMMENT "Building project Tempo in ${PULSAR_BINARY_DIR}/apps/${tempo_name}/ ..."
  )
add_dependencies (tempo_build tempo_unpack)

add_custom_target (tempo_install
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/tempo.hlp ${CMAKE_INSTALL_PREFIX}/bin/obsys.dat ${CMAKE_INSTALL_PREFIX}/bin/tztot.dat ${CMAKE_INSTALL_PREFIX}/bin/ephem ${CMAKE_INSTALL_PREFIX}/bin/clock ${CMAKE_INSTALL_PREFIX}/bin/test ${CMAKE_INSTALL_PREFIX}/bin/tzpar ${CMAKE_INSTALL_PREFIX}/bin/tztot.dat
  COMMAND make install
#  COMMAND cp src/tempo ${CMAKE_INSTALL_PREFIX}/bin
  COMMAND sed -e \"s|CMAKE_CHANGE|${CMAKE_INSTALL_PREFIX}/bin|g\" tempo.cfg.cmake > ${CMAKE_INSTALL_PREFIX}/bin/tempo.cfg
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
##                                                                         Tempo2
##
## ==============================================================================

#set (tempo2_version   "2011.7"                          )
set (tempo2_version   "2012.6.1"                        )
set (tempo2_name      "tempo2-${tempo2_version}"        )
set (tempo2_directory "tempo2/tempo2-${tempo2_version}" )
set (tempo2_archive   "${tempo2_directory}.tar.gz"      )

add_custom_target (tempo2_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${tempo2_name}
  COMMAND ${TAR_EXECUTABLE} -xvzf ${CMAKE_CURRENT_SOURCE_DIR}/${tempo2_archive}
# Don't need this anymore, as we have proper coordinates and aliases already in the Tempo2 tarball - Vlad, 26.06.2012
#  COMMAND echo lofar " t LF" >> ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/T2runtime/observatory/aliases
#  COMMAND echo "" >> ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/T2runtime/observatory/observatories.dat
#  COMMAND echo " 3826937.853   460938.478       5064630.667      LOFAR               t" >> ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/T2runtime/observatory/observatories.dat
  COMMAND cp -r ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/T2runtime/* ${CMAKE_INSTALL_PREFIX}/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Unpacking tar archive of Tempo2 from ${PULSAR_SOURCE_DIR}/external/${tempo2_archive} to ${PULSAR_BINARY_DIR}/apps..."
  )

if (UNIX)
 if (APPLE)
add_custom_target (tempo2_configure
  COMMAND ./configure TEMPO2=${CMAKE_INSTALL_PREFIX} --srcdir=${PULSAR_BINARY_DIR}/apps/${tempo2_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} PGPLOT_DIR=${PGPLOT_INCLUDES}  --with-cfitsio-include-dir=${CFITSIO_INCLUDES} --with-cfitsio-lib-dir=${CFITSIO_LIBRARY_DIR} --disable-local --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo2_name} 
  COMMENT "Configuring project Tempo2 in ${PULSAR_BINARY_DIR}/apps/${tempo2_name} ..."
  )
add_dependencies (tempo2_configure tempo2_unpack)
 else (APPLE)
add_custom_target (tempo2_configure
  COMMAND ./configure TEMPO2=${CMAKE_INSTALL_PREFIX} --srcdir=${PULSAR_BINARY_DIR}/apps/${tempo2_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} PGPLOT_DIR=${PGPLOT_INCLUDES}  --with-cfitsio-include-dir=${CFITSIO_INCLUDES} --with-cfitsio-lib-dir=${CFITSIO_LIBRARY_DIR} --disable-local LOGIN_ARCH=linux --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo2_name} 
  COMMENT "Configuring project Tempo2 in ${PULSAR_BINARY_DIR}/apps/${tempo2_name} ..."
  )
add_dependencies (tempo2_configure tempo2_unpack)
 endif (APPLE)
endif (UNIX)

add_custom_target (tempo2_install
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/plugins
  COMMAND make install
  COMMAND make plugins-install
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/
  COMMENT "Building and installing project Tempo2 in ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/..."
  )
add_dependencies (tempo2_install tempo2_configure tempo2_unpack)

#add_custom_target (tempo2_install
#  COMMAND cp .libs/lib* ${CMAKE_INSTALL_PREFIX}/lib/
#  COMMAND rm ${CMAKE_INSTALL_PREFIX}/lib/libtempo2*.o
#  COMMAND ln -sf ${CMAKE_INSTALL_PREFIX}/plugins ${CMAKE_INSTALL_PREFIX}/bin/plugins
#  COMMAND rm ${CMAKE_INSTALL_PREFIX}/lib/libtempo2pred.so ${CMAKE_INSTALL_PREFIX}/lib/libtempo2.so ${CMAKE_INSTALL_PREFIX}/lib/libsofa.so
#  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib/ && ln -sf libtempo2pred.so.0.0.0 libtempo2pred.so 
#  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib/ && ln -sf libtempo2.so.0.0.0 libtempo2.so 
#  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib/ && ln -sf libsofa.so.0.0.0 libsofa.so 
#  COMMAND make plugins-install
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${tempo2_name}/
#  COMMENT "Installaing project Tempo2 and plugins in ${CMAKE_INSTALL_PREFIX} in lib and bin..."
#  )
#add_dependencies (tempo2_install tempo2_build tempo2_configure tempo2_unpack)

## ==============================================================================
##
##  Handle configuration and build of PPGPLOT (Python PGPLOT Bindings)
##
## ==============================================================================

set (PPGPLOT_SOURCE_DIR ${PULSAR_SOURCE_DIR}/external/ppgplot)
set (PPGPLOT_BINARY_DIR ${PULSAR_BINARY_DIR}/external/ppgplot)

#set (ppgplot_cmake_build TRUE)

#if (ppgplot_cmake_build)
  
#  add_subdirectory (ppgplot1.1-modified)
  
#else (ppgplot_cmake_build)
  
  set (ppgplot_version "1.1")
  set (ppgplot_name "ppgplot${ppgplot_version}-modified")
  set (ppgplot_directory "ppgplot/ppgplot${ppgplot_version}-modified")
  set (ppgplot_archive "${ppgplot_directory}.tar.gz")
  
  add_custom_target (ppgplot_unpack
    COMMAND ${TAR_EXECUTABLE} -xvzf ${CMAKE_CURRENT_SOURCE_DIR}/${ppgplot_archive}
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Unpacking tar archive of PPGPLOT from ${PULSAR_SOURCE_DIR}/external/${ppgplot_archive} to ${PULSAR_BINARY_DIR}/apps..."
    )
  
  get_filename_component(GFORTRAN_LIBRARY_DIR ${GFORTRAN_LIBRARY} PATH)

configure_file (
    ${PPGPLOT_SOURCE_DIR}/setup.py.in
    ${PULSAR_BINARY_DIR}/apps/${ppgplot_name}/setup.py
)

add_custom_target (ppgplot_build
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

#if (NOT TARGET psrchive)

#  set (psrchive_version   "13.3")
#  set (psrchive_version   "20120316")
#  set (psrchive_version   "20120609")
#  set (psrchive_version   "20120908")
  set (psrchive_version   "20130301")
  set (psrchive_name      "psrchive-${psrchive_version}")
  set (psrchive_directory "psrchive/psrchive-${psrchive_version}")
  
  if (APPLE)
    add_custom_target (psrchive_unpack
      #  COMMAND find ./ -name entries -exec rm -rf {} "\;" 
      COMMAND rm -rf ${psrchive_name}
      COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${psrchive_directory} ${PULSAR_BINARY_DIR}/apps/
      WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
      COMMENT "Copying PSRCHIVE from ${CMAKE_CURRENT_SOURCE_DIR}/${psrchive_directory} to ${PULSAR_BINARY_DIR}/apps/${psrchive_name}"
      )
  else (APPLE)
    add_custom_target (psrchive_unpack
      #  COMMAND find ./ -name entries -exec rm -rf {} "\;" 
      COMMAND rm -rf ${psrchive_name}
      COMMAND cp -ru ${CMAKE_CURRENT_SOURCE_DIR}/${psrchive_directory} ${PULSAR_BINARY_DIR}/apps/
      WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
      COMMENT "Copying PSRCHIVE from ${CMAKE_CURRENT_SOURCE_DIR}/${psrchive_directory} to ${PULSAR_BINARY_DIR}/apps/${psrchive_name}"
      )
  endif (APPLE)
  add_dependencies (psrchive_unpack psrcat_install)

#add_custom_target (psrchive_bootstrap
#  COMMAND ./bootstrap
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
#  COMMENT "Running bootstrap for project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
#  )
#add_dependencies (psrchive_bootstrap psrchive_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (psrchive_configure
  COMMAND ./bootsrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=mac --x-includes=${X11_X11_INCLUDE_PATH} --x-libraries=${X11_X11_INCLUDE_PATH}/../lib --build=mac --enable-debug --disable-local --with-mpi --with-fftw-include-dir=${CMAKE_INSTALL_PREFIX}/include --with-fftw-lib-dir=${CMAKE_INSTALL_PREFIX}/lib --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${psrchive_name}/cmake_bootstrap.log 2>&1
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=mac --x-includes=${X11_X11_INCLUDE_PATH} --x-libraries=${X11_X11_INCLUDE_PATH}/../lib --build=mac --enable-debug --disable-local --with-mpi --with-fftw-include-dir=${CMAKE_INSTALL_PREFIX}/include --with-fftw-lib-dir=${CMAKE_INSTALL_PREFIX}/lib --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${psrchive_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
  COMMENT "Configuring project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
  )
add_dependencies (psrchive_configure psrchive_unpack psrcat_install)
 else (APPLE)
add_custom_target (psrchive_configure
  COMMAND ./bootstrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=linux --with-mpi --with-fftw-include-dir=${CMAKE_INSTALL_PREFIX}/include --with-fftw-lib-dir=${CMAKE_INSTALL_PREFIX}/lib --with-psrcat=${CMAKE_INSTALL_PREFIX}/bin --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${psrchive_name}/cmake_bootstrap.log 2>&1
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --srcdir=${PULSAR_BINARY_DIR}/apps/${psrchive_name} --prefix=${CMAKE_INSTALL_PREFIX} --exec-prefix=${CMAKE_INSTALL_PREFIX} --with-x LOGIN_ARCH=linux --with-mpi --with-psrcat=${CMAKE_INSTALL_PREFIX}/bin --enable-shared 1> ${PULSAR_BINARY_DIR}/apps/${psrchive_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name} 
  COMMENT "Configuring project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name} ..."
  )
add_dependencies (psrchive_configure psrchive_unpack psrcat_install)
 endif (APPLE)
endif (UNIX)


add_custom_target (psrchive_build
  COMMAND make 1> ${PULSAR_BINARY_DIR}/apps/${psrchive_name}/make.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name}
  COMMENT "Building project psrchive in ${PULSAR_BINARY_DIR}/apps/${psrchive_name}..."
  )
add_dependencies (psrchive_build psrchive_configure psrchive_unpack psrcat_install)

add_custom_target (psrchive_install
  COMMAND make install
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrchive_name}
  COMMENT "Install project psrchive into ${CMAKE_INSTALL_PREFIX}..."
  )
add_dependencies (psrchive_install psrchive_build psrchive_configure psrchive_unpack psrcat_install)

  ## Register last step in the chain with the actual target
#  add_custom_target (psrchive ALL)
#  add_dependencies (psrchive psrchive_install psrchive_build psrchive_configure psrchive_unpack psrcat_install)

#endif (NOT TARGET psrchive)

## ==============================================================================
##
##  Handle configuration and build of PSRDADA
##
## ==============================================================================

set (psrdada_version   "20100817")
set (psrdada_name      "psrdada-${psrdada_version}")
set (psrdada_directory "psrdada/psrdada-${psrdada_version}")

add_custom_target (psrdada_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMAND cp -r ${PULSAR_SOURCE_DIR}/external/${psrdada_directory} ${PULSAR_BINARY_DIR}/apps/
  COMMAND mkdir ${PULSAR_BINARY_DIR}/apps/${psrdada_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying psrdada from ${CMAKE_CURRENT_SOURCE_DIR}/${psrdada_directory} to ${PULSAR_BINARY_DIR}/apps/${psrdada_name}"
  )

add_custom_target (psrdada_bootstrap
  COMMAND ./bootstrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} 1> ${PULSAR_BINARY_DIR}/apps/${psrdada_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name} 
  COMMENT "Running bootstrap for project psrdada in ${PULSAR_BINARY_DIR}/apps/${psrdada_name} ..."
  )
add_dependencies (psrdada_bootstrap psrdada_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (psrdada_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=darwin --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} 1>> ${PULSAR_BINARY_DIR}/apps/${psrdada_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Configuring project psrdada (darwin) in ${PULSAR_BINARY_DIR}/apps/${psrdada_name} ..."
  )
add_dependencies (psrdada_configure psrdada_bootstrap psrdada_unpack)
 else (APPLE)
add_custom_target (psrdada_configure
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${psrdada_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} 1>> ${PULSAR_BINARY_DIR}/apps/${psrdada_name}/cmake_configure.log 2>&1
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
#  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib && find ./ -type l "\\(" -not -iname \"libfftw3f*\" "\\)" -exec rm {} "\;"
  COMMAND cd ${CMAKE_INSTALL_PREFIX}/lib && ln -sf ../psrdada/lib/* .
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrdada_name}
  COMMENT "Install project psrdada into ${CMAKE_INSTALL_PREFIX}/psrdada..."
  )
add_dependencies (psrdada_install psrdada_build psrdada_configure psrdada_bootstrap psrdada_unpack)

## ==============================================================================
##
##  Handle configuration and build of DSPSR
##
## ==============================================================================

#add_subdirectory (dspsr)
#add_dependencies (dspsr tempo2_install psrchive_install psrdada_install psrcat_install)

#if (NOT TARGET dspsr)

  set (dspsr_version   "20130301")  
#  set (dspsr_version   "20120501")  
#  set (dspsr_version   "nodal-20120501")  
#  set (dspsr_version   "20120327")  
#  set (dspsr_version   "20120322")  
#  set (dspsr_version   "20120316")
#  set (dspsr_version   "1.0")
  set (dspsr_name      "dspsr-${dspsr_version}")
  set (dspsr_directory "dspsr/dspsr-${dspsr_version}")
  
  add_custom_target (dspsr_unpack
    COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${dspsr_directory} ${PULSAR_BINARY_DIR}/apps/
    COMMAND mkdir ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Copying DSPSR from ${PULSAR_SOURCE_DIR}/external/${dspsr_directory} to ${PULSAR_BINARY_DIR}/apps/${dspsr_name}"
  )
#add_dependencies (dspsr_unpack tempo2_install psrchive_install psrdada_install)
add_dependencies (dspsr_unpack tempo2_install psrdada_install)

add_custom_target (dspsr_bootstrap
  COMMAND ./bootstrap PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada  1> ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name} 
  COMMENT "Running bootstrap for project dspsr in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
#add_dependencies (dspsr_bootstrap  tempo2_install psrchive_install psrdada_install dspsr_unpack)
add_dependencies (dspsr_bootstrap  tempo2_install psrdada_install dspsr_unpack)
#add_dependencies (dspsr_bootstrap dspsr_unpack)

if (UNIX)
 if (APPLE)
add_custom_target (dspsr_configure
#  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=darwin --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS='-ldl -ldal' LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/../../lib CXXFLAGS=-I${CMAKE_INSTALL_PREFIX}/../../include LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH} 1>> ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/cmake_configure.log 2>&1
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=darwin --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS=-ldl LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/../../lib CXXFLAGS=-I${CMAKE_INSTALL_PREFIX}/../../include LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH} 1>> ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Configuring project dspsr (darwin) in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
#add_dependencies (dspsr_configure  tempo2_install psrchive_install psrdada_install dspsr_bootstrap dspsr_unpack)
add_dependencies (dspsr_configure  tempo2_install psrdada_install dspsr_bootstrap dspsr_unpack)
#add_dependencies (dspsr_configure  dspsr_bootstrap dspsr_unpack)
 else (APPLE)
add_custom_target (dspsr_configure
#  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS='-ldl -ldal' LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/../../lib CXXFLAGS=-I${CMAKE_INSTALL_PREFIX}/../../include LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH} 1>> ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/cmake_configure.log 2>&1
  COMMAND ./configure PSRHOME=${PULSAR_BINARY_DIR}/apps/${dspsr_name} LOGIN_ARCH=linux --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS=-ldl LDFLAGS=-L${CMAKE_INSTALL_PREFIX}/../../lib CXXFLAGS=-I${CMAKE_INSTALL_PREFIX}/../../include LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH} 1>> ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/cmake_configure.log 2>&1
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Configuring project dspsr (linux) in ${PULSAR_BINARY_DIR}/apps/${dspsr_name} ..."
  )
#add_dependencies (dspsr_configure  tempo2_install psrchive_install psrdada_install dspsr_bootstrap dspsr_unpack)
add_dependencies (dspsr_configure  tempo2_install psrdada_install dspsr_bootstrap dspsr_unpack)#add_dependencies (dspsr_configure dspsr_bootstrap dspsr_unpack)
 endif (APPLE)
endif (UNIX)

add_custom_target (dspsr_build
  COMMAND make prefix=${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Building project dspsr in ${PULSAR_BINARY_DIR}/apps/${dspsr_name}..."
  )
#add_dependencies (dspsr_build  tempo2_install psrchive_install psrdada_install dspsr_configure dspsr_bootstrap dspsr_unpack)
add_dependencies (dspsr_build  tempo2_install psrdada_install dspsr_configure dspsr_bootstrap dspsr_unpack)
#add_dependencies (dspsr_build  dspsr_configure dspsr_bootstrap dspsr_unpack)

add_custom_target (dspsr_preinstall
  COMMAND make install prefix=${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build -I${CMAKE_INSTALL_PREFIX}/psrdada/include 
#  COMMAND make install prefix=${CMAKE_INSTALL_PREFIX} -I${CMAKE_INSTALL_PREFIX}/psrdada/include
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}
  COMMENT "Install project dspsr first into ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build..."
  )
#add_dependencies (dspsr_preinstall  tempo2_install psrchive_install psrdada_install dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
add_dependencies (dspsr_preinstall  tempo2_install psrdada_install dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
#add_dependencies (dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)

add_custom_target (dspsr_install
  COMMAND cp -r bin/* ${CMAKE_INSTALL_PREFIX}/bin/
  COMMAND cp -r include/* ${CMAKE_INSTALL_PREFIX}/include/
  COMMAND cp -r lib/* ${CMAKE_INSTALL_PREFIX}/lib/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${dspsr_name}/build
  COMMENT "Install project dspsr into ${CMAKE_INSTALL_PREFIX}..."
  )
#add_dependencies (dspsr_install tempo2_install psrchive_install psrdada_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
add_dependencies (dspsr_install tempo2_install psrdada_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
#add_dependencies (dspsr_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)

  ## Register last step in the chain with the actual target
#  add_custom_target (dspsr ALL)
#  add_dependencies (dspsr_install tempo2_install psrchive_install psrdada_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
#  add_dependencies (dspsr_install tempo2_install psrdada_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)
#  add_dependencies (dspsr_install dspsr_preinstall dspsr_build dspsr_configure dspsr_bootstrap dspsr_unpack)

#endif (NOT TARGET dspsr)


## ==============================================================================
##
##  Handle configuration and build of SSPS (Single Pulse Library code, Thijs Coenen)
##
## ==============================================================================

#set (ssps_cmake_build TRUE)

#if (ssps_cmake_build)
  
#  add_subdirectory (ssps0.0)
  
#else (ssps_cmake_build)
  
  set (ssps_version   "0.1"                               )
  set (ssps_name      "ssps-release-${ssps_version}"      )
  set (ssps_directory "ssps/ssps-release-${ssps_version}" )
  set (ssps_archive   "${ssps_directory}.tar.gz"          )
  
  add_custom_target (ssps_unpack
    COMMAND cd ${PULSAR_BINARY_DIR}/apps
    COMMAND rm -rf ${ssps_name}
    COMMAND ${TAR_EXECUTABLE} -xvzf ${CMAKE_CURRENT_SOURCE_DIR}/${ssps_archive}
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Unpacking tar archive of SSPS from ${CMAKE_CURRENT_SOURCE_DIR}/${ssps_archive} to ${PULSAR_BINARY_DIR}/apps/${ssps_name}..."
    )
  
add_custom_target (ssps_build
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMAND python setup.py build --build-lib=build/lib
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${ssps_name}
  COMMENT "Building project SSPS in ${PULSAR_BINARY_DIR}/apps/${ssps_name}..."
  COMMENT "Building project SSPS with command: python ${PULSAR_BINARY_DIR}/apps/${ssps_name}/setup.py build"
  )
add_dependencies (ssps_build ssps_unpack fftw)

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

#set (psrcat_version "V1.32")
set (psrcat_version "V1.43")
set (psrcat_name "psrcat-${psrcat_version}")
set (psrcat_directory "psrcat/psrcat-${psrcat_version}")

add_custom_target (psrcat_unpack
  COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/psrcat
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${psrcat_directory}.tar.gz ${PULSAR_BINARY_DIR}/apps/
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/
  COMMAND tar xvfz ${psrcat_name}.tar.gz
  COMMAND rm ${psrcat_name}.tar.gz
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
  COMMENT "Copying PSRCAT from ${CMAKE_CURRENT_SOURCE_DIR}/${psrcat_directory}.tar.gz to ${PULSAR_BINARY_DIR}/apps/${psrcat_name}.tar.gz"
  )

add_custom_target (psrcat_build
  COMMAND ./makeit 1> ${PULSAR_BINARY_DIR}/apps/psrcat/cmake_configure.log 2>&1
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

## ==============================================================================
##
##  Handle configuration and build of PSRFITS_UTILS
##
## ==============================================================================

#add_subdirectory (psrfits_utils)

#if (NOT TARGET psrfits_utils)
  
  set (psrfits_version   "utils-525dbed")
  set (psrfits_name      "psrfits_${psrfits_version}")
  set (psrfits_directory "psrfits_utils/psrfits_${psrfits_version}")
  
  add_custom_target (psrfits_unpack
    COMMAND rm -rf ${PULSAR_BINARY_DIR}/apps/${psrfits_name}
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${psrfits_directory} ${PULSAR_BINARY_DIR}/apps/
    COMMAND mkdir ${PULSAR_BINARY_DIR}/apps/${psrfits_name}/build
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps
    COMMENT "Copying psrfits from ${CMAKE_CURRENT_SOURCE_DIR}/${psrfits_directory} to ${PULSAR_BINARY_DIR}/apps/${psrfits_name}"
    )
  
  add_custom_target (psrfits_prepare
    COMMAND ./prepare 1> ${PULSAR_BINARY_DIR}/apps/${psrfits_name}/cmake_configure.log 2>&1
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrfits_name} 
    COMMENT "Running prepare for project psrfits in ${PULSAR_BINARY_DIR}/apps/${psrfits_name} ..."
    )
  add_dependencies (psrfits_prepare psrfits_unpack)
  
  if (UNIX)
    if (APPLE)
      add_custom_target (psrfits_configure
	COMMAND ./configure LDFLAGS=-L${CFITSIO_LIBRARY_DIR} CPPFLAGS=-I${CFITSIO_INCLUDES} --bindir=${CMAKE_INSTALL_PREFIX}/bin --libdir=${CMAKE_INSTALL_PREFIX}/lib --includedir=${CMAKE_INSTALL_PREFIX}/include --prefix=${CMAKE_INSTALL_PREFIX} 1>> ${PULSAR_BINARY_DIR}/apps/${psrfits_name}/cmake_configure.log 2>&1
	WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrfits_name}
	COMMENT "Configuring project psrfits (darwin) in ${PULSAR_BINARY_DIR}/apps/${psrfits_name} ..."
	)
      add_dependencies (psrfits_configure psrfits_prepare psrfits_unpack)
    else (APPLE)
      add_custom_target (psrfits_configure
	COMMAND ./configure LDFLAGS=-L${CFITSIO_LIBRARY_DIR} CPPFLAGS=-I${CFITSIO_INCLUDES} --bindir=${CMAKE_INSTALL_PREFIX}/bin --libdir=${CMAKE_INSTALL_PREFIX}/lib --includedir=${CMAKE_INSTALL_PREFIX}/include --prefix=${CMAKE_INSTALL_PREFIX} 1>> ${PULSAR_BINARY_DIR}/apps/${psrfits_name}/cmake_configure.log 2>&1
	WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrfits_name}
	COMMENT "Configuring project psrfits (linux) in ${PULSAR_BINARY_DIR}/apps/${psrfits_name} ..."
	)
      add_dependencies (psrfits_configure psrfits_prepare psrfits_unpack)
    endif (APPLE)
  endif (UNIX)
  
  add_custom_target (psrfits_build
    COMMAND make prefix=${PULSAR_BINARY_DIR}/apps/${psrfits_name}/build
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrfits_name}
    COMMENT "Building project psrfits in ${PULSAR_BINARY_DIR}/apps/${psrfits_name}..."
    )
  add_dependencies (psrfits_build psrfits_configure)
  
  add_custom_target (psrfits_install
    COMMAND make install 
    WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/${psrfits_name}
    COMMENT "Install project psrfits into ${CMAKE_INSTALL_PREFIX}..."
    )
  add_dependencies (psrfits_install psrfits_build)

  ## Register last step in the chain with the actual target
#  add_custom_target (psrfits_utils ALL)
#  add_dependencies (psrfits_utils psrfits_install)
  
#endif (NOT TARGET psrfits_utils)

## ==============================================================================
##
##  Handle configuration and build of DAL2 (pydal2)
##
## ==============================================================================

#set (dal2_version   "f15358f")
#set (dal2_name      "nextgen-astrodata-DAL-${dal2_version}")
#set (dal2_directory "dal2/${dal2_name}")
#
#add_custom_target (dal2_unpack
#  COMMAND rm -rf ${PULSAR_BINARY_DIR}/external/${dal2_name}
#  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${dal2_directory} ${PULSAR_BINARY_DIR}/external/
#  COMMAND mkdir ${PULSAR_BINARY_DIR}/external/${dal2_name}/build
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external
#  COMMENT "Copying dal2 from ${CMAKE_CURRENT_SOURCE_DIR}/${dal2_directory} to ${PULSAR_BINARY_DIR}/external/${dal2_name}"
#  COMMENT "Running mkdir ${PULSAR_BINARY_DIR}/external/${dal2_name}/build"
#  )
#
#add_custom_target (dal2_bootstrap
#  COMMAND cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$PWD -DPYTHON_DEST=$PWD/python
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${dal2_name}/build
#  COMMENT "Running bootstrap for project dal2 in ${PULSAR_BINARY_DIR}/external/${dal2_name}/build ..."
#  )
#add_dependencies (dal2_bootstrap dal2_unpack)
#
#add_custom_target (dal2_build
#  COMMAND make install
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${dal2_name}/build
#  COMMENT "Building project dal2 in ${PULSAR_BINARY_DIR}/external/${dal2_name}..."
#  )
#add_dependencies (psrdada_build psrdada_unpack)
#
#add_custom_target (dal2_install
#  COMMAND echo install dal2
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/external/${dal2_name}
#  COMMENT "Install project dal2 into ${CMAKE_INSTALL_PREFIX}/dal2 -- not operation yet..."
#  )
#add_dependencies (dal2_install dal2_build dal2_bootstrap dal2_unpack)


