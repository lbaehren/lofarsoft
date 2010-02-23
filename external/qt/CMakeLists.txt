
## Name of the project
project (QT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (QT_FORCE_BUILD       "Force rebuild and local installation?"   NO  )
option (QT_CMAKE_BUILD       "Use CMake to configure and build QT?"    YES )
option (QT_VERBOSE_CONFIGURE "Be verbose during configuration?"        NO  )
option (QT_VERBOSE_BUILD     "Be verbose during build process?"        NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${QT_SOURCE_DIR}
  ${QT_SOURCE_DIR}/..
  ${QT_SOURCE_DIR}/../..
  ${QT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindQt4)

if (QT4_FOUND)
  ## set internal variable
  set (HAVE_QT YES)
  ## Summary about the version of Qt found on the system
  message (STATUS "Have Qt                       = ${QT_FOUND}")
  message (STATUS "Have Qt4                      = ${QT4_FOUND}")
  message (STATUS "The major version of Qt found = ${QT_VERSION_MAJOR}")
  message (STATUS "The minor version of Qt found = ${QT_VERSION_MINOR}")
  message (STATUS "The patch version of Qt found = ${QT_VERSION_PATCH}")
  message (STATUS "The edition of Qt             = ${QT_EDITION}")
endif (QT4_FOUND)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_QT OR QT_FORCE_BUILD)
  
  set (QT_VERSION "4.6.2")
  set (QT_TAR_ARCHIVE "qt-everywhere-opensource-src-${QT_VERSION}.tar.gz")
  set (QT_URL "http://get.qt.nokia.com/qt/source/${QT_TAR_ARCHIVE}")
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_qt_tgz ${QT_TAR_ARCHIVE} ${QT_SOURCE_DIR})
  
  if (have_qt_tgz)
    ExternalProject_Add (qt
      PREFIX ${QT_BINARY_DIR}
      DOWNLOAD_DIR ${QT_BINARY_DIR}
      SOURCE_DIR ${QT_BINARY_DIR}
      URL ${have_qt_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${QT_BINARY_DIR}/configure --prefix=${USG_INSTALL_DIR}
      )
  else (have_qt_tgz)
    ExternalProject_Add (qt
      PREFIX ${QT_BINARY_DIR}
      DOWNLOAD_DIR ${QT_BINARY_DIR}
      SOURCE_DIR ${QT_BINARY_DIR}
      URL ${QT_URL}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${QT_BINARY_DIR}/configure --prefix=${USG_INSTALL_DIR}
      )
  endif (have_qt_tgz)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_QT OR QT_FORCE_BUILD)
  add_custom_target (qt
    WORKING_DIRECTORY ${QT_BINARY_DIR}
    COMMENT "[Qt] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Qt] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_QT OR QT_FORCE_BUILD)
