
## -----------------------------------------------------------------------------
## $Id::                                                                       $
## -----------------------------------------------------------------------------

## Variables used through the configuration environment:
##
##  LUS_ROOT              -- Root of the USG directory tree.
##  USG_CMAKE_CONFIG      -- Cache variable used to control running through the
##                           common set of instructions provided with this file.
##                           since this file will be included multiple times
##                           during the configuration process to a project, this
##                           variable serves as an include-guard, both protecting
##                           previously assigned variables as well as avoiding 
##                           unnecessary passes through the instructions.
##  USG_LIB_LOCATIONS     -- 
##  USG_INCLUDE_LOCATIONS -- 
##  LUS_INSTALL_PREFIX    -- Prefix marking the location at which the finished
##                           software components will be installed
##  USG_VARIANTS_FILE     -- Variants file containing host-specific overrides
##                           to the common configuration settings/presets.
##
##  Th directory structure and the corresponding CMake variables are given below:
##
##  .                    LUS_ROOT(_DIR)      = ${LOFARSOFT}
##  |-- release          LUS_RELEASE_DIR     = LUS_INSTALL_PREFIX
##  |   |-- include      LUS_INCLUDE_DIR
##  |   `-- lib          LUS_LIBRARY_DIR
##  |-- build            LUS_BUILD_DIR
##  `-- external         LUS_EXTERNAL_DIR
## 

if (NOT USG_CMAKE_CONFIG)

  ##__________________________________________________________________
  ## Check if LUS_ROOT is defined

  if (NOT LUS_ROOT)

    message (STATUS "[USG CMake] LUS_ROOT undefined; trying to locate it...")

    ## Try to locate the top-level directory based on a) the CMake configuration
    ## files expected to be there or b) the boostrap script

    find_path (LUS_ROOT CMakeOptions.cmake build/bootstrap
      ${CMAKE_CURRENT_SOURCE_DIR}/..
      ${CMAKE_CURRENT_SOURCE_DIR}/../..
      ${CMAKE_CURRENT_SOURCE_DIR}/../../..
      HINT
      $ENV{LOFARSOFT}
      )

  endif (NOT LUS_ROOT)
  
  ##__________________________________________________________________
  ## Define secondary LUS CMake variables
  
  if (LUS_ROOT)

    ## Augment CMake module path

    set (CMAKE_MODULE_PATH ${LUS_ROOT}/devel_common/cmake CACHE PATH
      "LUS CMake modules"
      FORCE)

    ## Sources for external packages

    set (LUS_EXTERNAL_DIR ${LUS_ROOT}/external CACHE PATH
      "Sources for external packages"
      FORCE
      )

    ## Build directory

    set (LUS_BUILD_DIR ${LUS_ROOT}/build CACHE PATH
      "Build directory"
      FORCE
      )

    ## Installation area

    set (LUS_RELEASE_DIR ${LUS_ROOT}/release CACHE PATH
      "LUS installation area"
      FORCE
      )

    set (LUS_INSTALL_PREFIX ${LUS_RELEASE_DIR} CACHE PATH
      "LUS installation area"
      FORCE
      )
    set (CMAKE_INSTALL_PREFIX ${LUS_RELEASE_DIR} CACHE PATH
      "CMake installation area" 
      FORCE
      )

    ## Location of installed header files

    set (LUS_INCLUDE_DIR ${LUS_ROOT}/release/include CACHE PATH
      "Location of installed header files"
      FORCE
      )

    include_directories (${LUS_INCLUDE_DIR} CACHE PATH
      "Location of installed header files"
      FORCE
      )

    ## Location of installed libraries

    set (LUS_LIBRARY_DIR ${LUS_ROOT}/release/lib CACHE PATH
      "Location of installed libraries"
      FORCE
      )

    ## (Test) data
    set (LUS_DATA_DIR ${LUS_ROOT}/data CACHE PATH
      "USG data area"
      FORCE
      )
    ## USG augmentation to PYTHONPATH
    set (USG_PYTHONPATH ${LUS_ROOT}/release/lib/python2.6;${LUS_ROOT}/release/lib/python2.5
      CACHE
      PATH
      "USG data area"
      FORCE
      )
    ## Directories inside the release directory
    if (LUS_INSTALL_PREFIX)
      execute_process (
	COMMAND mkdir -p lib
	COMMAND ln -s lib lib64
	WORKING_DIRECTORY ${LUS_INSTALL_PREFIX}
	)
    endif (LUS_INSTALL_PREFIX)
  else (LUS_ROOT)
    message (SEND_ERROR "LUS_ROOT is undefined!")
  endif (LUS_ROOT)
  
  ## ---------------------------------------------------------------------------
  ## generic search locations

  set (search_locations
    ${LUS_INSTALL_PREFIX}
    /opt
    /opt/local
    /sw
    /usr
    /usr/local
    /usr/X11R6
    /opt/casa/local
    /app/usg
    CACHE
    PATH
    "Directories to look for include files"
    FORCE
    )

  ## ---------------------------------------------------------------------------
  ## locations in which to look for applications/binaries
  
  set (bin_locations
    ${LUS_INSTALL_PREFIX}/bin
    /opt/local/bin
    /sw/bin
    /usr/bin
    /usr/local/bin
    /app/usg/release/bin
    CACHE
    PATH
    "Extra directories to look for executable files"
    FORCE
    )
  
  ## ----------------------------------------------------------------------------
  ## locations in which to look for header files
  
  set (include_locations
    ${LUS_INCLUDE_DIR}
    /opt/include
    /opt/local/include
    /sw/include
    /usr/include
    /usr/local/include
    /usr/X11R6/include
    /opt/casa/local/include    
    /app/usg/release/include
    /Developer/SDKs/MacOSX10.5.sdk/usr/include
    CACHE
    PATH
    "Directories to look for include files"
    FORCE
    )
  
  ## ----------------------------------------------------------------------------
  ## locations in which to look for libraries
  
  set (lib_locations
    ${LUS_LIBRARY_DIR}
    ${LUS_INSTALL_PREFIX}/lib64
    /opt/lib
    /opt/local/lib
    /sw/lib
    /usr/local/lib64
    /usr/local/lib
    /usr/lib64
    /usr/lib
    /usr/X11R6/lib
    /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
    /Developer/SDKs/MacOSX10.5.sdk/usr/lib
    /app/usg/release/lib
    CACHE
    PATH
    "Directories to look for libraries"
    FORCE
    )

  ## ============================================================================
  ##
  ##  Modules and Macro definitions
  ##
  ## ============================================================================

  include (ExternalProject)
  include (FindTestDatasets)
  include (MD5Checksum)
  include (AddPythonModule)
  include (OptimizeForArchitecture)

  ## ============================================================================
  ##
  ##  System inspection
  ##
  ## ============================================================================

  ##________________________________________________________
  ## Size of variable types
  
  include (CheckTypeSize)
  
  check_type_size ("short"          SIZEOF_SHORT         )
  check_type_size ("int"            SIZEOF_INT           )
  check_type_size ("float"          SIZEOF_FLOAT         )
  check_type_size ("double"         SIZEOF_DOUBLE        )
  check_type_size ("long"           SIZEOF_LONG          )
  check_type_size ("long int"       SIZEOF_LONG_INT     )
  check_type_size ("long long"      SIZEOF_LONG_LONG     )
  check_type_size ("long long int"  SIZEOF_LONG_LONG_INT )
  check_type_size ("uint"           SIZEOF_UINT          )
  check_type_size ("int8_t"         SIZEOF_INT8_T        )
  check_type_size ("int16_t"        SIZEOF_INT16_T       )
  check_type_size ("int32_t"        SIZEOF_INT32_T       )
  check_type_size ("int64_t"        SIZEOF_INT64_T       )
  check_type_size ("uint8_t"        SIZEOF_UINT8_T       )
  check_type_size ("uint16_t"       SIZEOF_UINT16_T      )
  check_type_size ("uint32_t"       SIZEOF_UINT32_T      )
  check_type_size ("uint64_t"       SIZEOF_UINT64_T      )
  
  if (CMAKE_SIZEOF_VOID_P)
    if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      add_definitions (-DWORDSIZE_IS_64)
    endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  else (CMAKE_SIZEOF_VOID_P)
    message (STATUS "Unable to check size of void*")
  endif (CMAKE_SIZEOF_VOID_P)
  
  if (UNIX)
    execute_process (
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMAND uname -m
      TIMEOUT 20
      OUTPUT_VARIABLE CMAKE_SYSTEM_KERNEL
      )
  endif (UNIX)
  
  ##__________________________________________________________
  ## System header files
  
  find_path (HAVE_LIBGEN_H      libgen.h      PATHS ${include_locations} )
  find_path (HAVE_LIMITS_H      limits.h      PATHS ${include_locations} )
  find_path (HAVE_MATH_H        math.h        PATHS ${include_locations} )
  find_path (HAVE_MEMORY_H      memory.h      PATHS ${include_locations} )
  find_path (HAVE_STDINT_H      stdint.h      PATHS ${include_locations} )
  find_path (HAVE_STDIO_H       stdio.h       PATHS ${include_locations} )
  find_path (HAVE_STDLIB_H      stdlib.h      PATHS ${include_locations} )
  find_path (HAVE_STRING_H      string.h      PATHS ${include_locations} )
  find_path (HAVE_STRINGS_H     strings.h     PATHS ${include_locations} )
  find_path (HAVE_TIME_H        time.h        PATHS ${include_locations} )
  find_path (HAVE_SYS_SOCKET_H  sys/socket.h  PATHS ${include_locations} )
  find_path (HAVE_SYS_STAT_H    sys/stat.h    PATHS ${include_locations} )
  find_path (HAVE_SYS_SYSCTL_H  sys/sysctl.h  PATHS ${include_locations} )
  find_path (HAVE_SYS_TIME_H    sys/time.h    PATHS ${include_locations} )
  find_path (HAVE_SYS_TYPES_H   sys/types.h   PATHS ${include_locations} )
  find_path (HAVE_SYS_UTIME_H   sys/utime.h   PATHS ${include_locations} )

  ##__________________________________________________________
  ## System Libraries
  
  find_library (HAVE_LIBM        m        PATHS ${lib_locations} )
  find_library (HAVE_LIBUTIL     util     PATHS ${lib_locations} )
  find_library (HAVE_LIBDL       dl       PATHS ${lib_locations} )
  find_library (HAVE_LIBGD       gd       PATHS ${lib_locations} )
  find_library (HAVE_LIBPTHREAD  pthread  PATHS ${lib_locations} )
  find_library (HAVE_LIBZ        z        PATHS ${lib_locations} )
  
  ## ============================================================================
  ##
  ##  Internal CMake variables
  ##
  ## ============================================================================
  
  if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
#      set (CMAKE_OSX_ARCHITECTURES i386;x86_64)
      if (CMAKE_SIZEOF_VOID_P)
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	  set (CMAKE_SYSTEM_WORDSIZE 64)
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	  set (CMAKE_SYSTEM_WORDSIZE 32)
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      endif (CMAKE_SIZEOF_VOID_P)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
  endif (APPLE)
  
  if (UNIX)
    set (CMAKE_FIND_LIBRARY_PREFIXES "lib" CACHE STRING
      "Library prefixes"
      FORCE
      )
    if (NOT APPLE AND NOT CMAKE_FIND_LIBRARY_SUFFIXES)
      set (CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so" CACHE STRING
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
  
  ## ============================================================================
  ##
  ##  Host-specific overrides
  ##
  ## ============================================================================

  execute_process (COMMAND hostname -s
    OUTPUT_VARIABLE hostname
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set (USG_VARIANTS_FILE ${LUS_ROOT}/devel_common/cmake/variants.${hostname})
  
  if (EXISTS ${USG_VARIANTS_FILE})
    message (STATUS "Loading settings variants " ${USG_VARIANTS_FILE})
    include (${USG_VARIANTS_FILE})
  endif (EXISTS ${USG_VARIANTS_FILE})
  
  ## ----------------------------------------------------------------------------
  ## Configuration flag
  
  set (USG_CMAKE_CONFIG TRUE CACHE BOOL "USG CMake configuration flag" FORCE)
  mark_as_advanced(USG_CMAKE_CONFIG)
  
  ## ============================================================================
  ##
  ##  Configuration summary
  ##
  ## ============================================================================

  message (STATUS)
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS)

  message (STATUS "[USG CMake configuration]")
  message (STATUS " CMAKE_SYSTEM .............. : ${CMAKE_SYSTEM}"                )
  message (STATUS " CMAKE_SYSTEM_VERSION ...... : ${CMAKE_SYSTEM_VERSION}"        )
  message (STATUS " CMAKE_SYSTEM_PROCESSOR .... : ${CMAKE_SYSTEM_PROCESSOR}"      )
  message (STATUS " CMAKE_SYSTEM_KERNEL ....... : ${CMAKE_SYSTEM_KERNEL}"         )
  message (STATUS " LUS_ROOT .................. : ${LUS_ROOT}"                    )
  message (STATUS " CMAKE_INSTALL_PREFIX ...... : ${CMAKE_INSTALL_PREFIX}"        )
  message (STATUS " CMAKE_FIND_LIBRARY_PREFIXES : ${CMAKE_FIND_LIBRARY_PREFIXES}" )
  message (STATUS " CMAKE_FIND_LIBRARY_SUFFIXES : ${CMAKE_FIND_LIBRARY_SUFFIXES}" )
  message (STATUS " Size of void* ............. : ${CMAKE_SIZEOF_VOID_P}"         )
  
  message (STATUS)
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS)
  
endif (NOT USG_CMAKE_CONFIG)
