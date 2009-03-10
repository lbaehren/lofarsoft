
## Name of the project
project (MATHGL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATHGL_FORCE_BUILD "Force rebuild and local installation?"     NO  )
option (MATHGL_CMAKE_BUILD "Use CMake to configure and build MATHGL?"  YES )
option (MATHGL_VERBOSE_CONFIGURE "Be verbose during configuration?"    NO  )
option (MATHGL_VERBOSE_BUILD     "Be verbose during build process?"    NO  )
option (MATHGL_ENABLE_PYTHON     "Enable support for Python?"          YES )
option (MATHGL_ENABLE_QT         "Enable support for Qt?"              NO  )

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
  ## GNU Scientific Library

  set (GSL_FIND_QUIETLY YES)
  include (FindGSL)

  if (NOT HAVE_GSL)
    message (SEND_ERROR "[MathGL] Unable to find GSL - please install!")
  endif (NOT HAVE_GSL)

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
  ## Locate OpenGL

  include (FindOpenGL)
  
  ##__________________________________________________________________
  ## Locate Qt4 libraries and header files

  find_file (QT_MOC_EXECUTABLE moc
    PATHS ${bin_locations} /sw/lib /opt/casa/local/bin
    PATH_SUFFIXES qt4-mac qt4-mac/bin qt4-x11 qt4-x11/bin
    )

  ## Libraries

  find_library (QT_QTCORE_LIBRARY QtCore
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/bin
    )
  if (QT_QTCORE_LIBRARY)
    list (APPEND QT_LIBRARIES ${QT_QTCORE_LIBRARY})
  endif (QT_QTCORE_LIBRARY)
  
  find_library (QT_QTGUI_LIBRARY QtGui
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
    )
  if (QT_QTGUI_LIBRARY)
    list (APPEND QT_LIBRARIES ${QT_QTGUI_LIBRARY})
  endif (QT_QTGUI_LIBRARY)

  find_library (QT_QTDESIGNER_LIBRARY QtDesigner
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
    )
  if (QT_QTDESIGNER_LIBRARY)
    list (APPEND QT_LIBRARIES ${QT_QTDESIGNER_LIBRARY})
  endif (QT_QTDESIGNER_LIBRARY)
  
  find_library (QT_QTOPENGL_LIBRARY QtOpenGL
    PATHS ${lib_locations}
    PATH_SUFFIXES qt4-mac qt4-mac/lib qt4-x11 qt4-x11/lib
    )
  if (QT_QTOPENGL_LIBRARY)
    list (APPEND QT_LIBRARIES ${QT_QTOPENGL_LIBRARY})
  endif (QT_QTOPENGL_LIBRARY)

  if (QT_LIBRARIES)
    list(REVERSE QT_LIBRARIES)
  endif (QT_LIBRARIES)
  
  ## locate the Qt4 header files

  find_path (QT_INCLUDE_DIR QtCore/QTimer QtGui/QApplication
    PATHS ${include_locations} /usr/lib /sw/lib /opt/aips++/local/include
    PATH_SUFFIXES qt4-mac/include qt4-x11/include
    )

  ## QtCore
  find_path (QT_QTCORE_INCLUDE_DIR QtCore/QTimer QtCore/qbasictimer.h
    PATHS ${include_locations} /usr/lib /sw/lib /opt/aips++/local/include
    PATH_SUFFIXES qt4-mac/include qt4-x11/include
    )
  if (QT_QTCORE_INCLUDE_DIR)
    list (APPEND QT_INCLUDE_DIR ${QT_QTCORE_INCLUDE_DIR}/QtCore)
  endif (QT_QTCORE_INCLUDE_DIR) 
  
  ## QtGui
  find_path (QT_QTGUI_INCLUDE_DIR QtGui/QApplication QtGui/QMouseEvent
    PATHS ${include_locations} /usr/lib /sw/lib /opt/aips++/local/include
    PATH_SUFFIXES qt4-mac/include qt4-x11/include
    )
  if (QT_QTGUI_INCLUDE_DIR)
    list (APPEND QT_INCLUDE_DIR ${QT_QTGUI_INCLUDE_DIR}/QtGui)
  endif (QT_QTGUI_INCLUDE_DIR)

  ## QtOpenGL
  find_path (QT_QTOPENGL_INCLUDE_DIR QtOpenGL/QtOpenGL QtOpenGL/QGLWidget
    PATHS ${include_locations} /usr/lib /sw/lib /opt/aips++/local/include
    PATH_SUFFIXES qt4-mac/include qt4-x11/include
    )
  if (QT_QTOPENGL_INCLUDE_DIR)
    list (APPEND QT_INCLUDE_DIR ${QT_QTOPENGL_INCLUDE_DIR}/QtOpenGL)
  endif (QT_QTOPENGL_INCLUDE_DIR)

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_mathgl_tgz ${MATHGL_TAR_ARCHIVE} ${MATHGL_SOURCE_DIR})
  
  if (have_mathgl_tgz)
    add_custom_target (download_archive ALL
      COMMAND cp ${have_mathgl_tgz} .
      WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
      COMMENT "Copying tar-archive from USG_ROOT/external/mathgl ..."
      )
  else (have_mathgl_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive ALL
	COMMAND wget -c ${MATHGL_URL}
	WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${MATHGL_URL} -o ${MATHGL_TAR_ARCHIVE} -C
	  WORKING_DIRECTORY ${MATHGL_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[MathGL] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_mathgl_tgz)
  
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
  message (STATUS "NUMPY_INCLUDES              = ${NUMPY_INCLUDES}")
  message ("-------------------------------------------------------------")

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
  message (STATUS "Found system-wide installation of MathGL; skipping rebuild!")
endif (NOT HAVE_MATHGL OR MATHGL_FORCE_BUILD)
