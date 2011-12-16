
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (DAL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

##  Package information
set (DAL_SOURCE_VERSION "0.2.0" )
set (DAL_SOURCE_ARCHIVE "dal-${DAL_SOURCE_VERSION}.tar.gz" )
set (DAL_URL            "${DAL_SOURCE_ARCHIVE}" )
set (DAL_GIT            "git://github.com/nextgen-astrodata/DAL.git" )

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (DAL_FORCE_BUILD        "Force build and installation of package?"    NO  )
option (DAL_FROM_RELEASE       "Use release version instead of trunk?"       YES )
option (DAL_ENABLE_TESTING     "Enable testing and experimental builds?"     NO  )
option (DOCUMENTATION_ONLY     "Configure for building documentation only?"  NO  )
option (DAL_WITH_DOCUMENTATION "Generate documentation for the DAL?"         YES )
option (DAL_SHARED_LIBRARY     "Build DAL as shared library?"                NO  )
option (DAL_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"   YES )
option (DAL_PYTHON_BINDINGS    "Create python bindings?"                     YES )
option (DAL_DEBUGGING_MESSAGES "Print debugging information?"                NO  )
option (DAL_VERBOSE_CONFIGURE  "Verbose output during configuration?"        NO  )
option (DAL_WITH_MYSQL         "Build with support for MySQL database?"      NO  )

## Handle options

if (DAL_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (DAL_ENABLE_TESTING)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )

  if (LUS_SOURCE_DIR)
    include (${LUS_SOURCE_DIR}/devel_common/cmake/CMakeSettings.cmake)
  else (LUS_SOURCE_DIR)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_SOURCE_DIR)
endif (NOT LUS_SOURCE_DIR)

## Check if DAL is installed already

set (DAL_FOUND FALSE)
include (FindDAL)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (${DAL_FOUND})
  if (${DAL_SOURCE_VERSION} VERSION_GREATER ${DAL_VERSION})
    set (DAL_FORCE_BUILD TRUE)
  endif (${DAL_SOURCE_VERSION} VERSION_GREATER ${DAL_VERSION})
endif (${DAL_FOUND})

if (NOT DAL_FOUND OR DAL_FORCE_BUILD)

  ## Generate settings for initial CMake cache

  configure_file (
    ${PROJECT_SOURCE_DIR}/dal.cmake.in
    ${PROJECT_BINARY_DIR}/dal.cmake
    )

  include (ExternalProject)

  ## Build package from source
<<<<<<< HEAD
  
  if (DAL_FROM_RELEASE)
    
    ExternalProject_Add (dal
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      GIT_REPOSITORY ${DAL_GIT}
      GIT_TAG release
      CMAKE_ARGS -C ${PROJECT_BINARY_DIR}/dal.cmake
      )
    
  else (DAL_FROM_RELEASE)
    
    ExternalProject_Add (dal
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      GIT_REPOSITORY ${DAL_GIT}
      CMAKE_ARGS -C ${PROJECT_BINARY_DIR}/dal.cmake
      )
    
  endif (DAL_FROM_RELEASE)
  
=======

  if (DAL_FROM_REPOS)

    if (DAL_FROM_RELEASE)

      ExternalProject_Add (dal
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	GIT_REPOSITORY ${DAL_GIT}
	GIT_TAG release
<<<<<<< HEAD
	CMAKE_ARGS -D DAL_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
=======
	CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
>>>>>>> Avoid using LUS_INSTALL_PREFIX; use CMAKE_INSTALL_PREFIX instead.
	INSTALL_COMMAND make install
	)

    else (DAL_FROM_RELEASE)

      ExternalProject_Add (dal
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	GIT_REPOSITORY ${DAL_GIT}
<<<<<<< HEAD
	CMAKE_ARGS -D DAL_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
=======
	CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
>>>>>>> Avoid using LUS_INSTALL_PREFIX; use CMAKE_INSTALL_PREFIX instead.
	INSTALL_COMMAND make install
	)

    endif (DAL_FROM_RELEASE)

  else (DAL_FROM_REPOS)

      ExternalProject_Add (dal
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	URL ${DAL_SOURCE}
<<<<<<< HEAD
	CMAKE_ARGS -D DAL_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
=======
	CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DOCUMENTATION_ONLY=${DOCUMENTATION_ONLY} -D DAL_WITH_DOCUMENTATION=${DAL_WITH_DOCUMENTATION} -D DAL_SHARED_LIBRARY=${DAL_SHARED_LIBRARY} -D DAL_ENABLE_TESTING=${DAL_ENABLE_TESTING} -D DAL_COMPILER_WARNINGS=${DAL_COMPILER_WARNINGS} -D DAL_PYTHON_BINDINGS=${DAL_PYTHON_BINDINGS} -D DAL_DEBUGGING_MESSAGES=${DAL_DEBUGGING_MESSAGES} -D DAL_VERBOSE_CONFIGURE=${DAL_VERBOSE_CONFIGURE} -D DAL_WITH_MYSQL=${DAL_WITH_MYSQL}
>>>>>>> Avoid using LUS_INSTALL_PREFIX; use CMAKE_INSTALL_PREFIX instead.
	INSTALL_COMMAND make install
	)

  endif (DAL_FROM_REPOS)

>>>>>>> Avoid using LUS_INSTALL_PREFIX; use CMAKE_INSTALL_PREFIX instead.
else (NOT DAL_FOUND OR DAL_FORCE_BUILD)
  
  add_custom_target (dal
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[DAL] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT DAL_FOUND OR DAL_FORCE_BUILD)

## ==============================================================================
##
##  Test programs to work with existing installation
##
## ==============================================================================

if (DAL_FOUND AND HDF5_FOUND)


  ## Assemble list of test programs
  file (GLOB dalTestPrograms *.cc)

  if (dalTestPrograms)

    include_directories (
      ${DAL_INCLUDES}
      ${HDF5_INCLUDES}
      )

    foreach (_dalTest ${dalTestPrograms})

      get_filename_component (_dalTestName ${_dalTest} NAME_WE)

      add_executable (${_dalTestName} ${_dalTest})

    endforeach (_dalTest)

  endif (dalTestPrograms)

endif (DAL_FOUND AND HDF5_FOUND)
