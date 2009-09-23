
## Name of the project
project (BOOST)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BOOST_FORCE_BUILD       "Force rebuild and local installation?"     NO  )
option (BOOST_CMAKE_CUSTOM      "Use custom CMake environment for build?"   YES )
option (BOOST_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (BOOST_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

option (BOOST_BUILD_DEBUG       "Build debugging libraries?"                NO )
option (BOOST_BUILD_SHARED      "Build shared libraries?"                   NO )
option (BOOST_BUILD_BCP         "Build shared libraries?"                   NO )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BOOST_SOURCE_DIR}
  ${BOOST_SOURCE_DIR}/..
  ${BOOST_SOURCE_DIR}/../..
  ${BOOST_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT BOOST_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY  YES )
endif (NOT BOOST_VERBOSE_CONFIGURE)

include (FindBoost)

find_file (boost_cmake_tgz boost_cmake.tgz ${BOOST_SOURCE_DIR})

## ------------------------------------------------------------------------------
## Check if the located Boost installation is good enough by compiling a small
## test program.

if (HAVE_BOOST)
  
  find_file (HAVE_TESTBOOST TestBoost.cc
    PATHS ${CMAKE_MODULE_PATH} ${USG_ROOT}
    PATH_SUFFIXES devel_common/cmake
    )
  
  if (HAVE_TESTBOOST) 
    ## Augment the list of required libraries
    list (APPEND CMAKE_REQUIRED_LIBRARIES ${BOOST_boost_program_options})
    list (APPEND CMAKE_REQUIRED_LIBRARIES ${BOOST_LIBRARIES})
    ## Try to compile the test program
    try_compile(TESTBOOST_RESULT
      ${CMAKE_BINARY_DIR}
      ${HAVE_TESTBOOST}
      CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}
      COMPILE_DEFINITIONS -I${BOOST_INCLUDES} -I${BOOST_INCLUDES}/boost
      OUTPUT_VARIABLE TESTBOOST_OUTPUT
      )
    ## some basic feedback
    if (NOT TESTBOOST_RESULT)
      message (STATUS "Found Boost installation seems incomplete - rebuilding!")
      set (BOOST_FORCE_BUILD TRUE)
    endif (NOT TESTBOOST_RESULT)
  else (HAVE_TESTBOOST)
    message (STATUS "[Boost] Unable to further test existing Boost installation.")
  endif (HAVE_TESTBOOST)
  
endif (HAVE_BOOST)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## 

  set (BOOST_VERSION "1_39_0")
  set (BOOST_DIRECTORY "boost_${BOOST_VERSION}")
  set (BOOST_TAR_ARCHIVE "${BOOST_DIRECTORY}.tar.gz")
  
  find_file (have_boost_tgz ${BOOST_TAR_ARCHIVE} ${BOOST_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})

  if (have_boost_tgz AND tar_executable)
    
    ## expand the tar-archive
    add_custom_target (boost_expand
      COMMAND ${tar_executable} -xvzf ${have_boost_tgz} --exclude *.html
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMENT "[Boost] Expanding tar-archive ..."
      )
    
    if (BOOST_CMAKE_CUSTOM)
      
      ## expand the tar-archive with the CMake files
      add_custom_target (boost_expand_cmake
	COMMAND ${tar_executable} -xvzf ${boost_cmake_tgz}
	WORKING_DIRECTORY ${BOOST_BINARY_DIR}/${BOOST_DIRECTORY}
	COMMENT "[Boost] Expanding tar-archive with CMake files ..."
	)
      add_dependencies (boost_expand_cmake boost_expand)
      
      ## configure the package
      add_custom_target (boost_configure
	COMMAND cmake .
	WORKING_DIRECTORY ${BOOST_BINARY_DIR}/${BOOST_DIRECTORY}
	COMMENT "[Boost] Configuring project ..."
	)
      add_dependencies (boost_configure boost_expand_cmake)
      
    else (BOOST_CMAKE_CUSTOM)
      
      ## configure the package
      add_custom_target (boost_configure
	COMMAND cmake . -DCMAKE_IS_EXPERIMENTAL=YES_I_KNOW -DBUILD_DEBUG:BOOL=${BOOST_BUILD_DEBUG} -DBUILD_SHARED:BOOL=${BOOST_BUILD_SHARED} -DBUILD_BCP:BOOL=${BOOST_BUILD_BCP} -DCMAKE_INSTALL_PREFIX:PATH=${USG_INSTALL_PREFIX}
	WORKING_DIRECTORY ${BOOST_BINARY_DIR}/${BOOST_DIRECTORY}
	COMMENT "[Boost] Configuring project ..."
	)
      add_dependencies (boost_configure boost_expand)
      
    endif (BOOST_CMAKE_CUSTOM)
    
    ## build the package
    add_custom_target (boost_build
      COMMAND make
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}/${BOOST_DIRECTORY}
      COMMENT "[BOOST] Starting build ..."
      )
    add_dependencies (boost_build boost_configure)
    
    ## build and install the package
    add_custom_target (boost_install
      COMMAND make install
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}/${BOOST_DIRECTORY}
      COMMENT "[BOOST] Installing package ..."
      )
    add_dependencies (boost_install boost_build)
    
    ## post-install clean-up
    add_custom_target (install
      COMMAND rm -rf *
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMENT "[BOOST] Cleaning up build directory ..."
      )
    add_dependencies (install boost_install)
    
  else (have_boost_tgz AND tar_executable)
    message (STATUS "[BOOST] Unable to install - missing tar-archive!")
  endif (have_boost_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)
  message (STATUS "Found system-wide installation of BOOST; skipping rebuild!")
endif (NOT HAVE_BOOST OR BOOST_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Additional targets

add_custom_target (update_cmake
  COMMAND rm ../boost_cmake.tgz
  WORKING_DIRECTORY ${BOOST_SOURCE_DIR}/${BOOST_DIRECTORY}
  COMMENT "[BOOST] Cleaning up build directory ..."
  )
