
## -----------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1475 2008-04-23 11:49:15Z baehren                      $
## -----------------------------------------------------------------------------

## Variables used through the configuration environment:
##
##  USG_ROOT              -- Root of the USG directory tree.
##  USG_CMAKE_CONFIG      -- Cache variable used to control running through the
##                           common set of instructions provided with this file.
##                           since this file will be included multiple times
##                           during the configuration process to a project, this
##                           variable serves as an include-guard, both protecting
##                           previously assigned variables as well as avoiding 
##                           unnecessary passes through the instructions.
##  USG_LIB_LOCATIONS     -- 
##  USG_INCLUDE_LOCATIONS -- 
##  USG_INSTALL_PREFIX    -- Prefix marking the location at which the finished
##                           software components will be installed
##  USG_VARIANTS_FILE     -- Variants file containing host-specific overrides
##                           to the common configuration settings/presets.
##

if (NOT USG_CMAKE_CONFIG)

  ## First pass: if USG_ROOT is still undefined we need to define it, since this
  ##             is the common starting point for all directory references below

  if (NOT USG_ROOT)
    message (STATUS "[USG CMake] USG_ROOT undefined; trying to locate it...")
    ## try to find the root directory based on the location of the release
    ## directory
    find_path (USG_INSTALL_PREFIX release/release_area.txt
      $ENV{LOFARSOFT}
      ..
      ../..
      NO_DEFAULT_PATH
      )
    ## convert the relative path to an absolute one
    get_filename_component (USG_ROOT ${USG_INSTALL_PREFIX} ABSOLUTE)
  endif (NOT USG_ROOT)

  ## Second pass: check once more if USG_ROOT is defined
  
  if (USG_ROOT)
    ## This addition to the module path needs to go into the cache,
    ## because otherwise it will be gone at the next time CMake is run
    set (CMAKE_MODULE_PATH ${USG_ROOT}/devel_common/cmake
      CACHE
      PATH 
      "USG cmake modules"
      FORCE)
    ## installation location
    set (USG_INSTALL_PREFIX ${USG_ROOT}/release
      CACHE
      PATH
      "USG default install area"
      FORCE
      )
    set (CMAKE_INSTALL_PREFIX ${USG_ROOT}/release
      CACHE
      PATH
      "CMake installation area"
      FORCE
      )
    ## header files
    include_directories (${USG_ROOT}/release/include
      CACHE
      PATH
      "USG include area"
      FORCE
      )
  else (USG_ROOT)
    message (SEND_ERROR "USG_ROOT is undefined!")
  endif (USG_ROOT)
  
  ## ---------------------------------------------------------------------------
  ## locations in which to look for applications/binaries
  
  set (bin_locations
    ${USG_INSTALL_PREFIX}/bin
    /usr/bin
    /usr/local/bin
    /sw/bin
    CACHE
    PATH
    "Extra directories to look for executable files"
    FORCE
    )
  
  ## ----------------------------------------------------------------------------
  ## locations in which to look for header files
  
  set (include_locations
    ${USG_INSTALL_PREFIX}/include
    /opt/include
    /opt/local/include
    /sw/include
    /usr/include
    /usr/local/include
    /usr/X11R6/include
    /opt/aips++/local/include
    /opt/casa/local/include    
    CACHE
    PATH
    "Directories to look for include files"
    FORCE
    )
  
  ## ----------------------------------------------------------------------------
  ## locations in which to look for libraries
  
  set (lib_locations
    ${USG_INSTALL_PREFIX}/lib
    /opt/lib
    /opt/local/lib
    /sw/lib
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /usr/X11R6/lib
    /opt/aips++/local/lib
    /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
    CACHE
    PATH
    "Directories to look for libraries"
    FORCE
    )

  ## ----------------------------------------------------------------------------
  ## Internal CMake variables

  if (CMAKE_MAJOR_VERSION GREATER 1 AND CMAKE_MINOR_VERSION GREATER 5)
    cmake_policy (VERSION 2.6)
  endif (CMAKE_MAJOR_VERSION GREATER 1 AND CMAKE_MINOR_VERSION GREATER 5)
  
  if (UNIX)
    set (CMAKE_FIND_LIBRARY_PREFIXES "lib" CACHE STRING
      "Library prefixes"
      FORCE
      )
    if (NOT APPLE AND NOT CMAKE_FIND_LIBRARY_SUFFIXES)
      set (CMAKE_FIND_LIBRARY_SUFFIXES "a;so" CACHE STRING
	"Library suffices"
	FORCE
	)
    endif (NOT APPLE AND NOT CMAKE_FIND_LIBRARY_SUFFIXES)
  endif (UNIX)
  
  set (USG_DOWNLOAD "http://usg.lofar.org/download" CACHE
    STRING
    "URL for the download area on the USG server"
    FORCE
    )
  
  ## ----------------------------------------------------------------------------
  ##  Host-specific overrides
  
  execute_process (COMMAND hostname -s
    OUTPUT_VARIABLE hostname
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set (USG_VARIANTS_FILE ${USG_ROOT}/devel_common/cmake/variants.${hostname})
  
  if (EXISTS ${USG_VARIANTS_FILE})
    message (STATUS "Loading settings variants " ${USG_VARIANTS_FILE})
    include (${USG_VARIANTS_FILE})
  endif (EXISTS ${USG_VARIANTS_FILE})
  
  ## ----------------------------------------------------------------------------
  ## Configuration flag
  
  set (USG_CMAKE_CONFIG TRUE CACHE BOOL "USG CMake configuration flag" FORCE)
  mark_as_advanced(USG_CMAKE_CONFIG)
  
  ## ----------------------------------------------------------------------------
  ## Feedback 

  message (STATUS "[USG CMake configuration]")
  message (STATUS "Hostname                    = ${hostname}")
  message (STATUS "USG_ROOT                    = ${USG_ROOT}")
  message (STATUS "USG_INSTALL_PREFIX          = ${USG_INSTALL_PREFIX}")
  message (STATUS "CMAKE_INSTALL_PREFIX        = ${CMAKE_INSTALL_PREFIX}")
  message (STATUS "CMAKE_FIND_LIBRARY_PREFIXES = ${CMAKE_FIND_LIBRARY_PREFIXES}")
  message (STATUS "CMAKE_FIND_LIBRARY_SUFFIXES = ${CMAKE_FIND_LIBRARY_SUFFIXES}")
  
endif (NOT USG_CMAKE_CONFIG)
