
## -----------------------------------------------------------------------------
##
## GNU Wget is a free utility for non-interactive download of files from the
## Web. It supports HTTP, HTTPS, and FTP protocols, as well as retrieval
## through HTTP proxies.
##
## -----------------------------------------------------------------------------

## Name of the project
project (WGET C)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Todo

# - HAVE_STRUCT_SOCKADDR_IN6
# - HAVE_STRUCT_SOCKADDR_STORAGE
# - HAVE_STRUCT_UTIMBUF

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (WGET_FORCE_BUILD "Force rebuild and local installation?"   YES )
option (WGET_CMAKE_BUILD "Use CMake to configure and build WGET?"  NO )
option (WGET_VERBOSE_CONFIGURE "Be verbose during configuration?"  NO  )
option (WGET_VERBOSE_BUILD     "Be verbose during build process?"  NO  )
option (WGET_ENABLE_OPIE       "Enable Opie support for FTP?"      NO  )
option (WGET_ENABLE_NLS        "Enable NLS support?"               NO  )
option (WGET_ENABLE_IPV6       "Enable IPV6 support?"              YES )
option (WGET_ENABLE_DEBUG      "Enable debug output support?"      YES )
option (WGET_ENABLE_DIGEST     "Enable HTTP Digest Authorization?" YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${WGET_SOURCE_DIR}
  ${WGET_SOURCE_DIR}/..
  ${WGET_SOURCE_DIR}/../..
  ${WGET_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

find_program (HAVE_WGET wget ${bin_locations})


## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_WGET OR WGET_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (WGET_prefix release_area.txt
    PATHS
    ${WGET_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT WGET_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT WGET_prefix)
  
  ## <-- begin configure/build method ------------------------------------------>
  
  if (WGET_CMAKE_BUILD)
    
    ## --------------------------------------------------------------------------
    ## System inspection
    
    set (OS_TYPE      ${CMAKE_SYSTEM}       )
    set (PACKAGE_NAME ${CMAKE_PROJECT_NAME} )
    
    ## CMake modules
    
    include (CheckFunctionExists)
    include (CheckLibraryExists)
    include (CheckSymbolExists)
    include (CheckTypeSize)
    include (TestBigEndian)
    
    ## Custom CMake modules
    
    if (NOT WGET_VERBOSE_CONFIGURE)
      set (SSL_FIND_QUIETLY TRUE)
    endif (NOT WGET_VERBOSE_CONFIGURE)
    
    include (FindSSL)
    
    ## Check type sizes --------------------------------------
    
    check_type_size ("int"           SIZEOF_INT         )
    check_type_size ("uint32_t"      HAVE_UINT32_T      )
    check_type_size ("int64_t"       HAVE_INT64_T     )
    check_type_size ("float"         SIZEOF_FLOAT       )
    check_type_size ("double"        SIZEOF_DOUBLE      )
    check_type_size ("long"          SIZEOF_LONG        )
    check_type_size ("long long"     SIZEOF_LONG_LONG   )
    check_type_size ("short"         SIZEOF_SHORT       )
    check_type_size ("off_t"         SIZEOF_OFF_T       )
    check_type_size ("pid_t"         SIZEOF_PID_T       )
    check_type_size ("size_t"        SIZEOF_SIZE_T      )
    check_type_size ("socklen_t"     SIZEOF_SOCKLEN_T    )
    check_type_size ("sig_atomic_t"  SIZEOF_SIG_ATOMIC_T )
    check_type_size ("void *"        SIZEOF_VOID_P       )
    check_type_size ("uintptr_t"     SIZEOF_UINTPTR_T    )
    check_type_size ("_Bool"         HAVE__BOOL          )
    check_type_size ("intptr_t"      SIZEOF_INTPTR_T     )
    
    check_type_size ("struct sockaddr_in6"      SIZEOF_STRUCT_SOCKADDR_IN6     )
    check_type_size ("struct sockaddr_storage"  SIZEOF_STRUCT_SOCKADDR_STORAGE )
    check_type_size ("struct utimbuf"           SIZEOF_STRUCT_UTIMBUF          )
    
    ## Check for existence of header files -------------------
    
    find_path (HAVE_ALLOCA_H     alloca.h      PATHS ${include_locations} )
    find_path (HAVE_CTYPE_H      ctype.h       PATHS ${include_locations} )
    find_path (HAVE_INTTYPES_H   inttypes.h    PATHS ${include_locations} )
    find_path (HAVE_LIBINTL_H    libintl.h     PATHS ${include_locations} )
    find_path (HAVE_MEMORY_H     memory.h      PATHS ${include_locations} )
    find_path (HAVE_PWD_H        pwd.h         PATHS ${include_locations} )
    find_path (HAVE_STDINT_H     stdint.h      PATHS ${include_locations} )
    find_path (HAVE_STDLIB_H     stdlib.h      PATHS ${include_locations} )
    find_path (HAVE_STRINGS_H    strings.h     PATHS ${include_locations} )
    find_path (HAVE_STRING_H     string.h      PATHS ${include_locations} )
    find_path (HAVE_SYS_IOCTL_H  sys/ioctl.h   PATHS ${include_locations} )
    find_path (HAVE_SYS_SELECT_H sys/select.h  PATHS ${include_locations} )
    find_path (HAVE_SYS_STAT_H   sys/stat.h    PATHS ${include_locations} )
    find_path (HAVE_SYS_TIME_H   sys/time.h    PATHS ${include_locations} )
    find_path (HAVE_SYS_TYPES_H  sys/types.h   PATHS ${include_locations} )
    find_path (HAVE_SYS_UTIME_H  sys/utime.h   PATHS ${include_locations} )
    find_path (HAVE_TERMIOS_H    termios.h     PATHS ${include_locations} )
    find_path (HAVE_UNISTD_H     unistd.h      PATHS ${include_locations} )
    find_path (HAVE_UTIME_H      utime.h       PATHS ${include_locations} )
    find_path (HAVE_WCHAR_H      wchar.h       PATHS ${include_locations} )
    find_path (HAVE_FNMATCH_H    fnmatch.h     PATHS ${include_locations} )
    find_path (HAVE_STDBOOL_H    stdbool.h     PATHS ${include_locations} )
    
    find_path (HAVE_LOCALE br ca cs da de
      PATHS ${include_locations} /usr/local/share
      PATH_SUFFIXES locale
      NO_DEFAULT_PATH
      )
    
    ## Check for existence of libraries ----------------------
    
    find_library (HAVE_LIBCRYPTO   crypto   PATHS ${lib_locations} )
    find_library (HAVE_LIBDL       dl       PATHS ${lib_locations} )
    find_library (HAVE_LIBNSL      nsl      PATHS ${lib_locations} )
    find_library (HAVE_LIBRT       rt       PATHS ${lib_locations} )
    find_library (HAVE_LIBGNUTLS   gnutls   PATHS ${lib_locations} )
    find_library (HAVE_LIBSOCKET   socket   PATHS ${lib_locations} )
    find_library (HAVE_LIBINTL     intl     PATHS ${lib_locations} )
    
    find_library (HAVE_LIBNTLM ntlm
      PATHS ${lib_locations}
      PATH_SUFFIXES sasl2
      )
    
    ## Check for functions defined inside the libraries
    
    check_function_exists (clock_gettime  HAVE_CLOCK_GETTIME )
    check_function_exists (ftello         HAVE_FTELLO        )
    check_function_exists (getaddrinfo    HAVE_GETADDRINFO   )
    check_function_exists (gethostbyname  HAVE_GETHOSTBYNAME )
    check_function_exists (getpagesize    HAVE_GETPAGESIZE   )
    check_function_exists (isatty         HAVE_ISATTY        )
    check_function_exists (memrchr        HAVE_MEMRCHR       )
    check_function_exists (strncasecmp    HAVE_STRNCASECMP   )
    check_function_exists (strptime       HAVE_STRPTIME      )
    check_function_exists (strtoll        HAVE_STRTOLL       )
    check_function_exists (symlink        HAVE_SYMLINK       )
    check_function_exists (timegm         HAVE_TIMEGM        )
    check_function_exists (usleep         HAVE_USLEEP        )
    check_function_exists (vasprintf      HAVE_VASPRINTF     )
    check_function_exists (vsnprintf      HAVE_VSNPRINTF     )
    check_function_exists (wcwidth        HAVE_WCWIDTH       )
    check_function_exists (gettext        HAVE_GETTEXT       )
    check_function_exists (inet_ntoa      HAVE_INET_NTOA     )
    check_function_exists (mbtowc         HAVE_MBTOWC        )
    check_function_exists (snprintf       HAVE_SNPRINTF      )
    check_function_exists (strcasecmp     HAVE_STRCASECMP    )
    check_function_exists (strdup         HAVE_STRDUP        )
    check_function_exists (sigblock       HAVE_SIGBLOCK      )
    check_function_exists (sigsetjmp      HAVE_SIGSETJMP     )
    check_function_exists (nanosleep      HAVE_NANOSLEEP     )
    check_function_exists (fseeko         HAVE_FSEEKO        )
    check_function_exists (mmap           HAVE_MMAP          )
    check_function_exists (drand48        HAVE_DRAND48       )
    
    if (NOT HAVE_GETTEXT)
      if (HAVE_LIBINTL)
	check_library_exists (${HAVE_LIBINTL} "gettext" "" LIBINTL_HAS_GETTEXT)
      endif (HAVE_LIBINTL)
    endif (NOT HAVE_GETTEXT)
    
    if (LIBINTL_HAS_GETTEXT)
      set (HAVE_GETTEXT 1)
    endif (LIBINTL_HAS_GETTEXT)
    
    ## --------------------------------------------------------------------------
    ## Set compiler flags
    
    if (CMAKE_SYSTEM_BIG_ENDIAN)
      set (WORDS_BIGENDIAN 1)
    endif (CMAKE_SYSTEM_BIG_ENDIAN)
    
    if (HAVE_LOCALE)
      add_definitions (-DLOCALEDIR=\"${HAVE_LOCALE}\")
    endif (HAVE_LOCALE)
    
    ## Size of types -----------------------------------------
    
    if (SIZEOF_SIG_ATOMIC_T)
      set (HAVE_SIG_ATOMIC_T ${SIZEOF_SIG_ATOMIC_T})
    endif (SIZEOF_SIG_ATOMIC_T)
    
    #  Define to `int' if <sys/types.h> does not define.
    if (NOT SIZEOF_PID_T)
      set (pid_t "int")
    endif (NOT SIZEOF_PID_T)
    
    # Define to `unsigned int' if <sys/types.h> does not define.
    if (NOT SIZEOF_SIZE_T)
      set (size_t "unsigned int")
    endif (NOT SIZEOF_SIZE_T)
    
    # Define to int or size_t on systems without socklen_t.
    if (NOT SIZEOF_SOCKLEN_T)
      set (socklen_t "int")
    endif (NOT SIZEOF_SOCKLEN_T)
    
    # Define to 1 if the system has the type `intptr_t'.
    if (SIZEOF_INTPTR_T)
      set (HAVE_INTPTR_T 1)
    endif (SIZEOF_INTPTR_T)
    
    ## Header files ------------------------------------------
    
    include_directories (
      ${WGET_SOURCE_DIR}/src
      ${WGET_BINARY_DIR}
      )
    
    if (HAVE_STDBOOL_H)
      include_directories (${HAVE_STDBOOL_H})
    endif (HAVE_STDBOOL_H)
    
    if (HAVE_ALLOCA_H)
      add_definitions (-DHAVE_ALLOCA_H)
    endif (HAVE_ALLOCA_H)
    
    if (HAVE_FNMATCH_H)
      set (HAVE_WORKING_FNMATCH_H 1)
    endif (HAVE_FNMATCH_H)
    
    ## Libraries ---------------------------------------------
    
    if (HAVE_LIBNSL)
      list (APPEND wget_link_libraries ${HAVE_LIBNSL})
    endif (HAVE_LIBNSL)
    
    if (HAVE_LIBRT)
      list (APPEND wget_link_libraries ${HAVE_LIBRT})
    endif (HAVE_LIBRT)
    
    if (HAVE_LIBGNUTLS)
      list (APPEND wget_link_libraries ${HAVE_LIBGNUTLS})
    endif (HAVE_LIBGNUTLS)
    
    if (HAVE_LIBSOCKET)
      list (APPEND wget_link_libraries ${HAVE_LIBSOCKET})
    endif (HAVE_LIBSOCKET)
    
    if (LIBINTL_HAS_GETTEXT)
      list (APPEND wget_link_libraries ${HAVE_LIBINTL})
    endif (LIBINTL_HAS_GETTEXT)
    
    #  if (HAVE_LIBNTLM)
    #    list (APPEND wget_link_libraries ${HAVE_LIBNTLM})
    #  endif (HAVE_LIBNTLM)
    
    if (HAVE_LIBDL)
      list (APPEND wget_link_libraries ${HAVE_LIBDL})
    endif (HAVE_LIBDL)
    
    if (HAVE_SSL)
      set (HAVE_LIBSSL 1)
      include_directories (${SSL_INCLUDES})
      list (APPEND wget_link_libraries ${SSL_LIBRARIES})
    endif (HAVE_SSL)
    
    if (HAVE_LIBCRYPTO)
      list (APPEND wget_link_libraries ${HAVE_LIBCRYPTO})
    endif (HAVE_LIBCRYPTO)
    
    ## --------------------------------------------------------------------------
    ## Options
    
    if (WGET_ENABLE_DEBUG)
      set (ENABLE_DEBUG 1)
    endif (WGET_ENABLE_DEBUG)
    
    if (WGET_ENABLE_DIGEST)
      set (ENABLE_DIGEST 1)
    endif (WGET_ENABLE_DIGEST)
    
    if (WGET_ENABLE_IPV6)
      set (ENABLE_IPV6 1)
    endif (WGET_ENABLE_IPV6)
    
    if (WGET_ENABLE_OPIE)
      set (ENABLE_OPIE 1)
    endif (WGET_ENABLE_OPIE)
    
    if (WGET_VERBOSE_BUILD)
      set (CMAKE_VERBOSE_MAKEFILE YES)
    endif (WGET_VERBOSE_BUILD)
    
    ## --------------------------------------------------------------------------
    ## Create config.h
    
    configure_file (${USG_ROOT}/devel_common/cmake/config_wget.h.in
      ${WGET_BINARY_DIR}/config.h)
    
    ## --------------------------------------------------------------------------
    ## Build executable
    
    set (wget_sources
      src/cmpt.c
      src/connect.c
      src/convert.c
      src/cookies.c
      src/ftp.c
      src/ftp-basic.c
      src/ftp-ls.c
      src/hash.c
      src/host.c
      src/html-parse.c
      src/html-url.c
      src/http.c
      src/init.c
      src/log.c
      src/main.c
      src/netrc.c
      src/progress.c
      src/ptimer.c
      src/recur.c
      src/res.c
      src/retr.c
      src/safe-ctype.c
      src/snprintf.c
      src/spider.c
      src/url.c
      src/utils.c
      src/version.c
      src/xmalloc.c
      #    $(ALLOCA)
      #    $(OPIE_OBJ)
      #    $(GETOPT_OBJ)
      #    $(NTLM_OBJ)
      #    $(MD5_OBJ)
      )
    
    if (HAVE_SSL)
      if (HAVE_LIBGNUTLS)
	list (APPEND wget_sources src/gnutls.c)
	list (APPEND wget_sources src/openssl.c)
      endif (HAVE_LIBGNUTLS)
    endif (HAVE_SSL)
    
    add_executable (wget ${wget_sources})
    
    target_link_libraries (wget ${wget_link_libraries})
    
    set_target_properties (wget PROPERTIES
      COMPILE_FLAGS "-O2 -Wall -DHAVE_CONFIG_H"
      LINK_FLAGS "-fPIC"
      )
    
    ## --------------------------------------------------------------------------
    ## Configuration summary
    
    if (WGET_VERBOSE_CONFIGURE)
      message (STATUS "CMAKE_SYSTEM ............... = ${CMAKE_SYSTEM}")
      message (STATUS "CMAKE_SYSTEM_PROCESSOR ..... = ${CMAKE_SYSTEM_PROCESSOR}")
      message (STATUS "CMAKE_SYSTEM_64BIT ......... = ${CMAKE_SYSTEM_64BIT}")
      message (STATUS "CMAKE_SYSTEM_BIG_ENDIAN .... = ${CMAKE_SYSTEM_BIG_ENDIAN}")
      message (STATUS "CMAKE_REQUIRED_FLAGS ....... = ${CMAKE_REQUIRED_FLAGS}")
      message (STATUS "CMAKE_REQUIRED_DEFINITIONS . = ${CMAKE_REQUIRED_DEFINITIONS}")
      message (STATUS "CMAKE_REQUIRED_INCLUDES .... = ${CMAKE_REQUIRED_INCLUDES}")
      message (STATUS "CMAKE_REQUIRED_LIBRARIES ... = ${CMAKE_REQUIRED_LIBRARIES}")
      ## Size of variables ##
      message (STATUS "Size of int ................ = ${SIZEOF_INT}")
      message (STATUS "Size of float .............. = ${SIZEOF_FLOAT}")
      message (STATUS "Size of double ............. = ${SIZEOF_DOUBLE}")
      message (STATUS "Size of uint32_t ........... = ${SIZEOF_UINT32_T}")
      message (STATUS "Size of int64_t ............ = ${SIZEOF_INT64_T}")
      message (STATUS "Size of long ............... = ${SIZEOF_LONG}")
      message (STATUS "Size of short .............. = ${SIZEOF_SHORT}")
      message (STATUS "Size of off_t .............. = ${SIZEOF_OFF_T}")
      message (STATUS "Size of pid_t .............. = ${SIZEOF_PID_T}")
      message (STATUS "Size of size_t ............. = ${SIZEOF_SIZE_T}")
      message (STATUS "Size of socklen_t .......... = ${SIZEOF_SOCKLEN_T}")
      message (STATUS "Size of void * ............. = ${SIZEOF_VOID_P}")
      message (STATUS "Size of uintptr_t .......... = ${SIZEOF_UINTPTR_T}")
      message (STATUS "Size of _Bool .............. = ${SIZEOF__BOOL}")
      ## Libraries ##
      message (STATUS "libcrypto .................. = ${HAVE_LIBCRYPTO}")
      message (STATUS "libdl ...................... = ${HAVE_LIBDL}")
      message (STATUS "libgnutls .................. = ${HAVE_LIBGNUTLS}")
      message (STATUS "libnsl ..................... = ${HAVE_LIBNSL}")
      message (STATUS "librt ...................... = ${HAVE_LIBRT}")
      message (STATUS "libsocket .................. = ${HAVE_LIBSOCKET}")
      message (STATUS "libssl ..................... = ${HAVE_SSL}")
    endif (WGET_VERBOSE_CONFIGURE)
    
  else (WGET_CMAKE_BUILD)
    
    ## [1] configure
    
    execute_process (
      WORKING_DIRECTORY ${WGET_SOURCE_DIR}
      COMMAND ./configure --prefix=${WGET_prefix} --includedir=${WGET_prefix}/src
      TIMEOUT 300
      ERROR_VARIABLE error_configure
      ERROR_FILE error.log
      )
    
    if (error_configure)
      message (STATUS "There was an error running the WGET configure script!")
      message (SEND_ERROR "${error_configure}")
    endif (error_configure)
    
    ## [2] make
    
    execute_process (
      WORKING_DIRECTORY ${WGET_SOURCE_DIR}
      COMMAND make 
      TIMEOUT 3600
      ERROR_VARIABLE error_make
      ERROR_FILE error.log
      )
    
    ## [3] make install
    
    execute_process (
      WORKING_DIRECTORY ${WGET_SOURCE_DIR}
      COMMAND make install
      TIMEOUT 180
      ERROR_VARIABLE error_make_install
      ERROR_FILE error.log
      )
    
    ## <-- end configure/build method ------------------------------------------>
    
  endif (WGET_CMAKE_BUILD)
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_WGET OR WGET_FORCE_BUILD)
  message (STATUS "Found system-wide installation of WGET; skipping rebuild!")
endif (NOT HAVE_WGET OR WGET_FORCE_BUILD)
