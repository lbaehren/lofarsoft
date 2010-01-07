
## -----------------------------------------------------------------------------
## $Id::                                                                       $
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
      ${CMAKE_CURRENT_SOURCE_DIR}/..
      ${CMAKE_CURRENT_SOURCE_DIR}/../..
      ${CMAKE_CURRENT_SOURCE_DIR}/../../..
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
    ## (Test) data
    set (USG_DATA ${USG_ROOT}/data
      CACHE
      PATH
      "USG data area"
      FORCE
      )
    ## USG augmentation to PYTHONPATH
    set (USG_PYTHONPATH ${USG_ROOT}/release/lib/python2.6;${USG_ROOT}/release/lib/python2.5
      CACHE
      PATH
      "USG data area"
      FORCE
      )
  else (USG_ROOT)
    message (SEND_ERROR "USG_ROOT is undefined!")
  endif (USG_ROOT)
  
  ## ---------------------------------------------------------------------------
  ## generic search locations

  set (search_locations
    ${USG_INSTALL_PREFIX}
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
    ${USG_INSTALL_PREFIX}/bin
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
    ${USG_INSTALL_PREFIX}/include
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
    ${USG_INSTALL_PREFIX}/lib
    ${USG_INSTALL_PREFIX}/lib64
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
  ##  Test datasets
  ##
  ## ============================================================================

  ## FITS
  
  find_file (dataset_fits WN65341H.fits
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    )
  
  ## Measurement Set

  find_path (dataset_ms table.dat table.info
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    PATH_SUFFIXES
    L2007_01810_SB18-20_OB20.MS
    lofar/cs1/L2007_01810_SB18-20_OB20.MS
    10602381_S0_T20.MS
    wsrt/lffe/10602381_S0_T20.MS
    )

  ## TBB time-series data
  
  find_file (dataset_tbb rw_20090417_181700.h5
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    )
  
  find_file (dataset_tbb_raw rw_20071024_090656_0101.dat rw_20071024_090656_0102.dat
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    )

  ## Beam-formed data

  find_file (dataset_bf_raw
    NAMES
    bf_20080604_121337.cor
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    )
  
  find_file (dataset_beamformed
    NAMES
    bf_20080604_121337.cor.h5 bf_20080604_121337.h5 TBB1.cor.h5
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    )
  
  find_file (dataset_lopes 2007.01.31.23:59:33.960.event
    PATHS
    ${USG_ROOT}/data
    ${USG_ROOT}/data/test
    PATH_SUFFIXES
    lopes
    )

  ## ============================================================================
  ##
  ##  System inspection
  ##
  ## ============================================================================

  ##________________________________________________________
  ## Size of variable types
  
  include (CheckTypeSize)
  
  check_type_size ("short"         SIZEOF_SHORT       )
  check_type_size ("int"           SIZEOF_INT         )
  check_type_size ("float"         SIZEOF_FLOAT       )
  check_type_size ("double"        SIZEOF_DOUBLE      )
  check_type_size ("long"          SIZEOF_LONG        )
  check_type_size ("long int"      SIZEOF_LONG_INT    )
  check_type_size ("long long"     SIZEOF_LONG_LONG   )
  check_type_size ("uint"          SIZEOF_UINT        )
  check_type_size ("int32_t"       SIZEOF_INT32_T     )
  check_type_size ("int64_t"       SIZEOF_INT64_T     )
  check_type_size ("uint32_t"      SIZEOF_UINT32_T    )
  check_type_size ("uint64_t"      SIZEOF_UINT64_T    )
  
  if (CMAKE_SIZEOF_VOID_P)
    if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      message (STATUS "Adding compiler flag -DWORDSIZE_IS_64")
      add_definitions (-DWORDSIZE_IS_64)
    endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  else (CMAKE_SIZEOF_VOID_P)
    message (STATUS "Unable to check size of void*")
  endif (CMAKE_SIZEOF_VOID_P)
  
  ##__________________________________________________________
  ## System header files
  
  find_path (HAVE_LIBGEN_H     libgen.h      PATHS ${include_locations} )
  find_path (HAVE_STDINT_H     stdint.h      PATHS ${include_locations} )
  find_path (HAVE_STDLIB_H     stdlib.h      PATHS ${include_locations} )
  find_path (HAVE_STRING_H     string.h      PATHS ${include_locations} )
  find_path (HAVE_STRINGS_H    strings.h     PATHS ${include_locations} )
  find_path (HAVE_SYS_STAT_H   sys/stat.h    PATHS ${include_locations} )
  find_path (HAVE_SYS_TIME_H   sys/time.h    PATHS ${include_locations} )
  find_path (HAVE_SYS_TYPES_H  sys/types.h   PATHS ${include_locations} )
  find_path (HAVE_SYS_UTIME_H  sys/utime.h   PATHS ${include_locations} )
  
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
  
  #set (CMAKE_OSX_ARCHITECTURES i386;ppc)
  
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

  message (STATUS)
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS)

  message (STATUS "[USG CMake configuration]")
  message (STATUS " CMAKE_SYSTEM .............. : ${CMAKE_SYSTEM}")
  message (STATUS " CMAKE_SYSTEM_VERSION ...... : ${CMAKE_SYSTEM_VERSION}")
  message (STATUS " CMAKE_SYSTEM_PROCESSOR .... : ${CMAKE_SYSTEM_PROCESSOR}")
  message (STATUS " CMAKE_INSTALL_PREFIX ...... : ${CMAKE_INSTALL_PREFIX}")
  message (STATUS " CMAKE_FIND_LIBRARY_PREFIXES : ${CMAKE_FIND_LIBRARY_PREFIXES}")
  message (STATUS " CMAKE_FIND_LIBRARY_SUFFIXES : ${CMAKE_FIND_LIBRARY_SUFFIXES}")
  message (STATUS " Size of void* ............. : ${CMAKE_SIZEOF_VOID_P}")
  message (STATUS " USG_ROOT .................. : ${USG_ROOT}")
  
  message (STATUS)
  message (STATUS "+------------------------------------------------------------+")
  message (STATUS)
  
endif (NOT USG_CMAKE_CONFIG)
