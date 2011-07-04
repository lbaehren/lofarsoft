
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (TMF)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

##  Package information
set (TMF_SOURCE_VERSION "2.0.7" )
set (TMF_SOURCE_ARCHIVE "tmf-${TMF_SOURCE_VERSION}.tar.gz" )
set (TMF_URL            "" )
set (TMF_GIT            "https://github.com/pschella/tmf.git" )

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (TMF_FORCE_BUILD    "Force build and installation of package?" NO  )
option (TMF_FROM_REPOS     "Install package from repository?"         YES )
option (TMF_FROM_RELEASE   "Use release version instead of trunk?"    YES )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT LUS_ROOT)
  find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )

  if (LUS_ROOT)
    include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (LUS_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_ROOT)
endif (NOT LUS_ROOT)

## Check if TMF is installed already

set (TMF_FOUND FALSE)
include (FindTMF)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (TMF_FROM_REPOS)
  message (STATUS "Forcing build")
  set (TMF_FORCE_BUILD TRUE)
endif (TMF_FROM_REPOS)

if (TMF_FOUND)
    if (TMF_VERSION)
        if (${TMF_SOURCE_VERSION} VERSION_GREATER ${TMF_VERSION})
            set (TMF_FORCE_BUILD TRUE)
        endif (${TMF_SOURCE_VERSION} VERSION_GREATER ${TMF_VERSION})
    else (TMF_VERSION)
        message (STATUS "TMF version cannot be determined, probably too old, build forced.")
        set (TMF_FORCE_BUILD TRUE)
    endif (TMF_VERSION)
endif (TMF_FOUND)

if (NOT TMF_FOUND OR TMF_FORCE_BUILD)

  ## Find Python
  if (NOT HAVE_PYTHON)
      include (FindPython)
  endif (NOT HAVE_PYTHON)

  include (ExternalProject)

  ## Locate source file to build from
  find_file (TMF_SOURCE ${TMF_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )

  ## If no source archive is provided, pull code from repository
  if (NOT TMF_SOURCE)
    set (TMF_FROM_REPOS TRUE)
  endif (NOT TMF_SOURCE)

  ## Build package from source
  if (TMF_FROM_REPOS)

    if (TMF_FROM_RELEASE)

      ExternalProject_Add (tmf
	GIT_REPOSITORY ${TMF_GIT}
	GIT_TAG release
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHONLIBS_FOUND=${HAVE_PYTHON} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE_PATH=${PYTHON_INCLUDES} -DPYTHON_SITE_PACKAGES=${CMAKE_INSTALL_PREFIX}/lib/python
	)

      add_dependencies (tmf git)

    else (TMF_FROM_RELEASE)

      ExternalProject_Add (tmf
	GIT_REPOSITORY ${TMF_GIT}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHONLIBS_FOUND=${HAVE_PYTHON} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE_PATH=${PYTHON_INCLUDES} -DPYTHON_SITE_PACKAGES=${CMAKE_INSTALL_PREFIX}/lib/python
	)

      add_dependencies (tmf git)

    endif (TMF_FROM_RELEASE)

  else (TMF_FROM_REPOS)

      ExternalProject_Add (tmf
	URL ${TMF_SOURCE}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHONLIBS_FOUND=${HAVE_PYTHON} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE_PATH=${PYTHON_INCLUDES} -DPYTHON_SITE_PACKAGES=${CMAKE_INSTALL_PREFIX}/lib/python
	)

  endif (TMF_FROM_REPOS)

else (NOT TMF_FOUND OR TMF_FORCE_BUILD)

  add_custom_target (tmf
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[TMF] Found system-wide installation; skipping rebuild!"
    )

endif (NOT TMF_FOUND OR TMF_FORCE_BUILD)

