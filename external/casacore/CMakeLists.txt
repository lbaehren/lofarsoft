
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (CASACORE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

##  Package information
set (CASACORE_VERSION        "1.2.0-r21024" )
set (CASACORE_SOURCE_ARCHIVE "casacore-${CASACORE_VERSION}.tar.bz2" )
set (CASACORE_URL            "http://casacore.googlecode.com/files/${CASACORE_SOURCE_ARCHIVE}" )
set (CASACORE_SVN            "http://casacore.googlecode.com/svn/trunk" )
set (CASACORE_SVN_REVISION   "-r21024")

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (CASACORE_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (CASACORE_FORCE_BUILD       "Force build and installation of package?" OFF )
option (CASACORE_FROM_REPOS        "Install package from repository?"         NO  )
option (CASACORE_FROM_REPOS_HEAD   "Install package from repository HEAD?"    YES )
option (CASACORE_WITH_FFTW3        "Enable FFTW3 library support?"            OFF )
option (CASACORE_WITH_HDF5         "Enable HDF5 library support?"             YES )
option (CASACORE_BUILD_TESTING     "Build test programs?"                     OFF )

##__________________________________________________________
## Handle option: Build type

if (CASACORE_FROM_REPOS_HEAD)
  set (CASACORE_FROM_REPOS TRUE)
endif (CASACORE_FROM_REPOS_HEAD)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

##__________________________________________________________
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_SOURCE_DIR)

##__________________________________________________________
## Find installation of required packages

if (LUS_SOURCE_DIR)

  set (CMAKE_MODULE_PATH "${LUS_SOURCE_DIR}/devel_common/cmake")
  set (HDF5_ROOT_DIR     "${CMAKE_INSTALL_PREFIX}/include")
  set (WCSLIB_ROOT_DIR   "${CMAKE_INSTALL_PREFIX}/include")
  
  include (LUS_CMakeSettings)
  
  foreach (casacoreExternalPackages
      CASACORE
      CFITSIO
      GFortran
      HDF5
      LAPACK
      MPI
      WCSLIB
      ZLIB
      )
    find_package (${casacoreExternalPackages})
  endforeach (casacoreExternalPackages)
  
  if (GFORTRAN_LIBRARY)
    list (APPEND LAPACK_LIBRARIES ${GFORTRAN_LIBRARY})
  endif (GFORTRAN_LIBRARY)

endif (LUS_SOURCE_DIR)
  
##__________________________________________________________
## Handle option: HDF5 library support

if (CASACORE_WITH_HDF5)

  list (INSERT HDF5_INCLUDES 0 ${CMAKE_INSTALL_PREFIX}/include )
  
  if (HDF5_FOUND)
    
    if (MPI_FOUND)
      list (APPEND HDF5_INCLUDES ${MPI_INCLUDE_PATH})
      list (APPEND HDF5_LIBRARIES ${MPI_LIBRARIES})
    endif (MPI_FOUND)
    
    if (ZLIB_LIBRARIES)
      list (APPEND HDF5_LIBRARIES ${ZLIB_LIBRARIES})
    endif (ZLIB_LIBRARIES)
    
  else (HDF5_FOUND)
    
    set (CASACORE_WITH_HDF5 FALSE)
    
  endif (HDF5_FOUND)
  
endif (CASACORE_WITH_HDF5)

##__________________________________________________________
## Handle option: FFTW3 library support

if (CASACORE_WITH_FFTW3)
  include (FindFFTW3)

  if (NOT FFTW3_FOUND)
    set (CASACORE_WITH_FFTW3 FALSE)
  endif (NOT FFTW3_FOUND)
endif (CASACORE_WITH_FFTW3)

##__________________________________________________________
## Create configure file for initial cache

configure_file (
  ${PROJECT_SOURCE_DIR}/casacore.cmake.in
  ${PROJECT_BINARY_DIR}/casacore.cmake)

find_file (CASACORE_CMAKE casacore.cmake
  PATHS ${PROJECT_BINARY_DIR}
  )

if (NOT CASACORE_CMAKE)
  message (SEND_ERROR "[casacore] Failed to locate initial cache values!")
endif (NOT CASACORE_CMAKE)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (CASACORE_FROM_REPOS)
  set (CASACORE_FORCE_BUILD TRUE)
endif (CASACORE_FROM_REPOS)

if (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  
  ## Locate source file to build from
  
  find_file (CASACORE_SOURCE ${CASACORE_SOURCE_ARCHIVE}
    PATHS ${PROJECT_SOURCE_DIR}
    )
  
  if (NOT CASACORE_SOURCE)
    set (CASACORE_SOURCE ${CASACORE_URL})
  endif (NOT CASACORE_SOURCE)
  
  ## Build package from source
  
  if (CASACORE_FROM_REPOS)
    
    if (CASACORE_FROM_REPOS_HEAD)
      ## Check out most recent version (SVN HEAD)
      ExternalProject_Add (casacore
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	SVN_REPOSITORY ${CASACORE_SVN}
	CMAKE_ARGS -C${CASACORE_CMAKE}
	PATCH_COMMAND cat ${PROJECT_SOURCE_DIR}/casacore.patch.cmake >> ${PROJECT_BINARY_DIR}/source/CMakeLists.txt
	BUILD_COMMAND ${CMAKE_BUILD_TOOL} -j2
	INSTALL_COMMAND make install
	)
    else (CASACORE_FROM_REPOS_HEAD)
      ## Check out specific revision
      ExternalProject_Add (casacore
	PREFIX ${PROJECT_BINARY_DIR}
	DOWNLOAD_DIR download
	SOURCE_DIR source
	SVN_REPOSITORY ${CASACORE_SVN}
	SVN_REVISION ${CASACORE_SVN_REVISION}
	CMAKE_ARGS -C${CASACORE_CMAKE}
	PATCH_COMMAND cat ${PROJECT_SOURCE_DIR}/casacore.patch.cmake >> ${PROJECT_BINARY_DIR}/source/CMakeLists.txt
	BUILD_COMMAND ${CMAKE_BUILD_TOOL} -j2
	INSTALL_COMMAND make install
	)
    endif (CASACORE_FROM_REPOS_HEAD)
    
    
  else (CASACORE_FROM_REPOS)
    
    ExternalProject_Add (casacore
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      URL ${CASACORE_SOURCE}
      CMAKE_ARGS -C${CASACORE_CMAKE}
      PATCH_COMMAND cat ${PROJECT_SOURCE_DIR}/casacore.patch.cmake >> ${PROJECT_BINARY_DIR}/source/CMakeLists.txt
      BUILD_COMMAND ${CMAKE_BUILD_TOOL} -j2
      INSTALL_COMMAND make install
      )
    
  endif (CASACORE_FROM_REPOS)

  ## ==============================================================================
  ##
  ##  Configuration summary
  ##
  ## ==============================================================================
  
  if (CASACORE_VERBOSE_CONFIGURE)
    
    message (STATUS "+------------------------------------------------------------+" )
    message (STATUS "| [casacore] Summary of configuration settings               |" )
    message (STATUS "+------------------------------------------------------------+" )
    message (STATUS " casacore package description:"                             )
    message (STATUS " .. Version                   = ${CASACORE_VERSION}"        )
    message (STATUS " .. Source archive            = ${CASACORE_SOURCE_ARCHIVE}" )
    message (STATUS " .. Build  from SVN HEAD      = ${CASACORE_FROM_REPOS_HEAD}")
    message (STATUS " .. SVN revision number       = ${CASACORE_SVN_REVISION}"   )
    message (STATUS " PROJECT_SOURCE_DIR           = ${PROJECT_SOURCE_DIR}"      )
    message (STATUS " CMAKE_INSTALL_PREFIX         = ${CMAKE_INSTALL_PREFIX}"    )
    message (STATUS " Enable FFTW3 library support = ${CASACORE_WITH_FFTW3}"     )
    message (STATUS " Enable HDF5 library support  = ${CASACORE_WITH_HDF5}"      )
    message (STATUS " Build test programs          = ${CASACORE_BUILD_TESTING}"  )
    message (STATUS " Found CFITSIO library        = ${CFITSIO_FOUND}"           )
    message (STATUS " Found FFTW3 library          = ${FFTW3_FOUND}"             )
    message (STATUS " Found HDF5 library           = ${HDF5_FOUND}"              )
    message (STATUS " Found LAPACK library         = ${LAPACK_FOUND}"            )
    message (STATUS " Found MPI library            = ${MPI_FOUND}"               )
    message (STATUS " Found WCSLIB library         = ${WCSLIB_FOUND}"            )
    message (STATUS "+------------------------------------------------------------+" )
    
  endif (CASACORE_VERBOSE_CONFIGURE)
  
else (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
  
  add_custom_target (casacore
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[CASACORE] Found system-wide installation; skipping rebuild!"
    )
  
endif (NOT HAVE_CASACORE OR CASACORE_FORCE_BUILD)
