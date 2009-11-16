
## Name of the project
project (MATHGL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATHGL_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (MATHGL_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (MATHGL_VERBOSE_BUILD     "Be verbose during build process?"       NO  )
option (MATHGL_ENABLE_HDF5       "Enable support for HDF5?"               NO  )
option (MATHGL_ENABLE_PYTHON     "Enable support for Python?"             YES )
option (MATHGL_ENABLE_QT         "Enable support for Qt?"                 YES )
option (MATHGL_ENABLE_DOUBLE     "Enable double precision?"               YES )
option (MATHGL_DOWNLOAD_ARCHIVE  "Download archive of latest version?"    NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${MATHGL_SOURCE_DIR}
  ${MATHGL_SOURCE_DIR}/..
  ${MATHGL_SOURCE_DIR}/../..
  ${MATHGL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[MathGL] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindMathGL)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (MATHGL_prefix release_area.txt
    PATHS
    ${MATHGL_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT MATHGL_prefix)
    message (FATAL_ERROR "[MathGL] Unable to locate the installation area")
  endif (NOT MATHGL_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (MATHGL_TAR_ARCHIVE "mathgl.tgz")
  set (MATHGL_URL "${USG_DOWNLOAD}/software/${MATHGL_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## External libraries
  
  if (NOT MATHGL_VERBOSE_CONFIGURE)
    set (GSL_FIND_QUIETLY YES)
    set (HDF5_FIND_QUIETLY YES)
  endif (NOT MATHGL_VERBOSE_CONFIGURE)

  set (DESIRED_QT_VERSION 4)

  include (FindGSL)
  include (FindHDF5)
  include (FindOpenGL)
  include (FindQt)

  ##__________________________________________________________________
  ## Python bindings
  
  if (MATHGL_ENABLE_PYTHON)
    
    find_path (NUMPY_INCLUDES numpy/arrayobject.h
      PATHS ${include_locations} /usr/lib /sw/lib
      PATH_SUFFIXES
      python2.6/site-packages/numpy/core/include
      python2.5/site-packages/numpy/core/include
      python2.4/site-packages/numpy/core/include
      )
    
    if (NOT NUMPY_INCLUDES)
      message (STATUS "[MathGL] Python bindings disabled - no Numpy found!")
      set (MATHGL_ENABLE_PYTHON NO)
    endif (NOT NUMPY_INCLUDES)
    
  endif (MATHGL_ENABLE_PYTHON)

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_mathgl_tgz 
    mathgl-1.10.tgz
    mathgl-1.9.tgz
    mathgl.tgz
    PATHS ${MATHGL_SOURCE_DIR}
    )

  if (NOT have_mathgl_tgz)
    set (MATHGL_DOWNLOAD_ARCHIVE YES)
  else (NOT have_mathgl_tgz)
    ## if the tar-archive with the source code cannot be found on the system, 
    ## we try to fetch it from the USG server
    if (MATHGL_DOWNLOAD_ARCHIVE)
      if (HAVE_WGET)
	add_custom_target (download_archive ALL
          COMMAND wget -c ${MATHGL_URL}
          WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
          )
      else (HAVE_WGET)
	if (HAVE_CURL)
          add_custom_target (download_archive ALL
            COMMAND curl ${MATHGL_URL} -o ${MATHGL_TAR_ARCHIVE} -C -
            WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
            )
	else (HAVE_CURL)
          message (FATAL_ERROR "[MathGL] No tool found to download tar-archive!")
	endif (HAVE_CURL)
      endif (HAVE_WGET)
    else (MATHGL_DOWNLOAD_ARCHIVE)
      ## copy the tar-archive to the build directory ...
      add_custom_target (download_archive ALL
	COMMAND cp ${have_mathgl_tgz} .
	WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
	COMMENT "Copying tar-archive from USG_ROOT/external/mathgl ..."
	)
      ## ... and update the variable storing its name
      get_filename_component (MATHGL_TAR_ARCHIVE ${have_mathgl_tgz} NAME)
    endif (MATHGL_DOWNLOAD_ARCHIVE)
  endif (NOT have_mathgl_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive ALL
    COMMAND tar -xvzf ${MATHGL_TAR_ARCHIVE}
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Create symbolic links to required external packages

  if (GSL_INCLUDES)
    add_custom_target (symlink_gsl
      COMMAND rm -f gsl
      COMMAND ln -s ${GSL_INCLUDES}/gsl .
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/mathgl/include
      COMMENT "Creating symbolic link to location of GSL headers ..."
      )
    add_dependencies (symlink_gsl expand_archive)
  endif (GSL_INCLUDES)
  
  if (NUMPY_INCLUDES)
    add_custom_target (symlink_numpy
      COMMAND rm -f numpy
      COMMAND ln -s ${NUMPY_INCLUDES}/numpy .
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/mathgl/include
      COMMENT "Creating symbolic link to location of Numpy headers ..."
      )
    add_dependencies (symlink_numpy expand_archive)
  endif (NUMPY_INCLUDES)
  
  ##__________________________________________________________________
  ## Configure and build the project using CMake

  ## Create configuration files from templates

  configure_file (
    ${MATHGL_SOURCE_DIR}/mathgl.cmake.in
    ${MATHGL_BINARY_DIR}/mathgl.cmake
    )

  configure_file (
    ${MATHGL_SOURCE_DIR}/setup.py.in
    ${MATHGL_BINARY_DIR}/setup.py
    )

  ## have CMake configure the project
  
  add_custom_target (run_cmake ALL
    COMMAND cp ../setup.py .
    COMMAND cmake -C ${MATHGL_BINARY_DIR}/mathgl.cmake .
    COMMAND cmake .
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/mathgl
    COMMENT "Configuring project using CMake ..."
    )
  add_dependencies (run_cmake expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/mathgl
    COMMENT "Building MathGL ..."
    )
  if (MATHGL_ENABLE_PYTHON)
    add_dependencies (run_make run_cmake symlink_gsl symlink_numpy)
  else (MATHGL_ENABLE_PYTHON)
    add_dependencies (run_make run_cmake symlink_gsl)
  endif (MATHGL_ENABLE_PYTHON)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -f ${MATHGL_BINARY_DIR}/${MATHGL_TAR_ARCHIVE}
    WORKING_DIRECTORY ${MATHGL_BINARY_DIR}/mathgl
    COMMENT "Building MathGL ..."
    )
  add_dependencies (install run_make)

  ##__________________________________________________________________
  ## Configuration summary

  message ("-------------------------------------------------------------")
  message (STATUS "[MathGL] Summary of configuration status")
  message (STATUS "Forced rebuild from source  = ${MATHGL_FORCE_BUILD}")
  message (STATUS "Enable support for Python   = ${MATHGL_ENABLE_PYTHON}")
  message (STATUS "Enable support for Qt       = ${MATHGL_ENABLE_QT}")
  message (STATUS "GSL_INCLUDES                = ${GSL_INCLUDES}")
  message (STATUS "HDF5_INCLUDES               = ${HDF5_INCLUDES}")
  message (STATUS "NUMPY_INCLUDES              = ${NUMPY_INCLUDES}")
  message (STATUS "DESIRED_QT_VERSION          = ${DESIRED_QT_VERSION}")
  message (STATUS "QT3_INSTALLED               = ${QT3_INSTALLED}")
  message (STATUS "QT4_INSTALLED               = ${QT4_INSTALLED}")
  message ("-------------------------------------------------------------")

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  message (STATUS "Found system-wide installation of MathGL; skipping rebuild!")
endif (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
