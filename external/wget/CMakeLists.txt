
## Name of the project
project (WGET)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (WGET_FORCE_BUILD "Force rebuild and local installation?"  YES )
option (WGET_FORCE_BUILD "Be verbose during configuration?"       NO  )
option (WGET_ENABLE_OPIE "Enable Opie support for FTP?"           NO  )

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
  
  ## ------------------------------------------------------------------------------
  ## System inspection

  include (CheckFunctionExists)
  include (CheckTypeSize)
  include (TestBigEndian)
  
  check_type_size ("int"           SIZEOF_INT       )
  check_type_size ("uint32_t"      SIZEOF_UINT32_T       )
  check_type_size ("int64_t"       SIZEOF_INT64_T   )
  check_type_size ("float"         SIZEOF_FLOAT     )
  check_type_size ("double"        SIZEOF_DOUBLE    )
  check_type_size ("long"          SIZEOF_LONG      )
  check_type_size ("long long"     SIZEOF_LONG_LONG )
  check_type_size ("short"         SIZEOF_SHORT     )
  check_type_size ("off_t"         SIZEOF_OFF_T     )
  check_type_size ("sig_atomic_t"  SIZEOF_SIG_ATOMIC_T )

  find_path (HAVE_ALLOCA_H     alloca.h      PATHS ${include_locations} )
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

  find_library (HAVE_LIBDL      dl       PATHS ${lib_locations} )
  find_library (HAVE_LIBNSL     nsl      PATHS ${lib_locations} )
  find_library (HAVE_LIBRT      rt       PATHS ${lib_locations} )
  find_library (HAVE_LIBGNUTLS  gnutls   PATHS ${lib_locations} )
  find_library (HAVE_LIBSOCKET  socket   PATHS ${lib_locations} )
  find_library (HAVE_LIBSSL     ssl      PATHS ${lib_locations} )

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
  check_function_exists (sigblock       HAVE_SIGBLOCK       )

  ## ------------------------------------------------------------------------------
  ## Set compiler flags

  if (CMAKE_SYSTEM_BIG_ENDIAN)
    add_definitions (-DWORDS_BIGENDIAN)
  endif (CMAKE_SYSTEM_BIG_ENDIAN)

  ## Size of types
  
  add_definitions (
    -DSIZEOF_INT=${SIZEOF_INT}
    -DSIZEOF_SHORT=${SIZEOF_SHORT}
    -DSIZEOF_FLOAT=${SIZEOF_FLOAT}
    -DSIZEOF_DOUBLE=${SIZEOF_DOUBLE}
    -DSIZEOF_LONG=${SIZEOF_LONG}
    -DSIZEOF_LONG_LONG=${SIZEOF_LONG_LONG}
    -DSIZEOF_OFF_T=${SIZEOF_OFF_T}
    )

  if (SIZEOF_UINT32_T)
    add_definitions (-DHAVE_UINT32_T)
  endif (SIZEOF_UINT32_T)

  if (SIZEOF_INT64_T)
    add_definitions (-DHAVE_INT64_T)
  endif (SIZEOF_INT64_T)

  if (SIZEOF_SIG_ATOMIC_T)
    add_definitions (-DHAVE_SIG_ATOMIC_T)
  endif (SIZEOF_SIG_ATOMIC_T)

  ## Header files

  if (HAVE_ALLOCA_H)
    add_definitions (-DHAVE_ALLOCA_H)
  endif (HAVE_ALLOCA_H)

  if (HAVE_INTTYPES_H)
    add_definitions (-DHAVE_INTTYPES_H)
  endif (HAVE_INTTYPES_H)

  if (HAVE_LIBINTL_H)
    add_definitions (-DHAVE_LIBINTL_H)
  endif (HAVE_LIBINTL_H)

  if (HAVE_MEMORY_H)
    add_definitions (-DHAVE_MEMORY_H)
  endif (HAVE_MEMORY_H)

  if (HAVE_PWD_H)
    add_definitions (-DHAVE_PWD_H)
  endif (HAVE_PWD_H)

  if (HAVE_STDINT_H)
    add_definitions (-DHAVE_STDINT_H)
  endif (HAVE_STDINT_H)

  if (HAVE_STDLIB_H)
    add_definitions (-DHAVE_STDLIB_H)
  endif (HAVE_STDLIB_H)

  if (HAVE_STRINGS_H)
    add_definitions (-DHAVE_STRINGS_H)
  endif (HAVE_STRINGS_H)

  if (HAVE_STRING_H)
    add_definitions (-DHAVE_STRING_H)
  endif (HAVE_STRING_H)

  if (HAVE_SYS_IOCTL_H)
    add_definitions (-DHAVE_SYS_IOCTL_H)
  endif (HAVE_SYS_IOCTL_H)

  if (HAVE_SYS_SELECT_H)
    add_definitions (-DHAVE_SYS_SELECT_H)
  endif (HAVE_SYS_SELECT_H)

  if (HAVE_SYS_STAT_H)
    add_definitions (-DHAVE_SYS_STAT_H)
  endif (HAVE_SYS_STAT_H)

  if (HAVE_SYS_TIME_H)
    add_definitions (-DHAVE_SYS_TIME_H)
  endif (HAVE_SYS_TIME_H)

  if (HAVE_SYS_TYPES_H)
    add_definitions (-DHAVE_SYS_TYPES_H)
  endif (HAVE_SYS_TYPES_H)

  if (HAVE_SYS_UTIME_H)
    add_definitions (-DHAVE_SYS_UTIME_H)
  endif (HAVE_SYS_UTIME_H)

  if (HAVE_TERMIOS_H)
    add_definitions (-DHAVE_TERMIOS_H)
  endif (HAVE_TERMIOS_H)

  if (HAVE_UNISTD_H)
    add_definitions (-DHAVE_UNISTD_H)
  endif (HAVE_UNISTD_H)

  if (HAVE_UTIME_H)
    add_definitions (-DHAVE_UTIME_H)
  endif (HAVE_UTIME_H)

  if (HAVE_WCHAR_H)
    add_definitions (-DHAVE_WCHAR_H)
  endif (HAVE_WCHAR_H)

  ## Libraries

  if (HAVE_LIBDL)
    add_definitions (-DHAVE_LIBDL)
    list (APPEND wget_link_libraries ${HAVE_LIBDL})
  endif (HAVE_LIBDL)

  if (HAVE_LIBNSL)
    add_definitions (-DHAVE_LIBNSL)
    list (APPEND wget_link_libraries ${HAVE_LIBNSL})
  endif (HAVE_LIBNSL)

  if (HAVE_LIBRT)
    add_definitions (-DHAVE_LIBRT)
    list (APPEND wget_link_libraries ${HAVE_LIBRT})
  endif (HAVE_LIBRT)

  if (HAVE_LIBGNUTLS)
    add_definitions (-DHAVE_LIBGNUTLS)
    list (APPEND wget_link_libraries ${HAVE_LIBGNUTLS})
  endif (HAVE_LIBGNUTLS)

  if (HAVE_LIBSOCKET)
    add_definitions (-DHAVE_LIBSOCKET)
    list (APPEND wget_link_libraries ${HAVE_LIBSOCKET})
  endif (HAVE_LIBSOCKET)

  if (HAVE_LIBSSL)
    add_definitions (-DHAVE_LIBSSL)
    list (APPEND wget_link_libraries ${HAVE_LIBSSL})
  endif (HAVE_LIBSSL)

  ## Functions

  if (HAVE_CLOCK_GETTIME)
    add_definitions (-DHAVE_CLOCK_GETTIME)
  endif (HAVE_CLOCK_GETTIME)  

  if (HAVE_FTELLO)
    add_definitions (-DHAVE_FTELLO)
  endif (HAVE_FTELLO)  

  if (HAVE_SYMLINK)
    add_definitions (-DHAVE_SYMLINK)
  endif (HAVE_SYMLINK)  

  if (HAVE_TIMEGM)
    add_definitions (-DHAVE_TIMEGM)
  endif (HAVE_TIMEGM)  

  if (HAVE_USLEEP)
    add_definitions (-DHAVE_USLEEP)
  endif (HAVE_USLEEP)  

  if (HAVE_STRNCASECMP)
    add_definitions (-DHAVE_STRNCASECMP)
  endif (HAVE_STRNCASECMP)  

  if (HAVE_STRPTIME)
    add_definitions (-DHAVE_STRPTIME)
  endif (HAVE_STRPTIME)  

  if (HAVE_STRTOLL)
    add_definitions (-DHAVE_STRTOLL)
  endif (HAVE_STRTOLL)  

  if (HAVE_VASPRINTF)
    add_definitions (-DHAVE_VASPRINTF)
  endif (HAVE_VASPRINTF)  

  if (HAVE_VSNPRINTF)
    add_definitions (-DHAVE_VSNPRINTF)
  endif (HAVE_VSNPRINTF)  

  if (HAVE_WCWIDTH)
    add_definitions (-DHAVE_WCWIDTH)
  endif (HAVE_WCWIDTH)  

  if (HAVE_GETADDRINFO)
    add_definitions (-DHAVE_GETADDRINFO)
  endif (HAVE_GETADDRINFO)  

  if (HAVE_GETHOSTBYNAME)
    add_definitions (-DHAVE_GETHOSTBYNAME)
  endif (HAVE_GETHOSTBYNAME)  

  if (HAVE_GETPAGESIZE)
    add_definitions (-DHAVE_GETPAGESIZE)
  endif (HAVE_GETPAGESIZE)  

  if (HAVE_MEMRCHR)
    add_definitions (-DHAVE_MEMRCHR)
  endif (HAVE_MEMRCHR)  

  if (HAVE_ISATTY)
    add_definitions (-DHAVE_ISATTY)
  endif (HAVE_ISATTY)  

  if (HAVE_GETTEXT)
    add_definitions (-DHAVE_GETTEXT)
  endif (HAVE_GETTEXT)  

  if (HAVE_INET_NTOA)
    add_definitions (-DHAVE_INET_NTOA)
  endif (HAVE_INET_NTOA)  

  if (HAVE_MBTOWC)
    add_definitions (-DHAVE_MBTOWC)
  endif (HAVE_MBTOWC)  

  if (HAVE_SNPRINTF)
    add_definitions (-DHAVE_SNPRINTF)
  endif (HAVE_SNPRINTF)  

  if (HAVE_STRCASECMP)
    add_definitions (-DHAVE_STRCASECMP)
  endif (HAVE_STRCASECMP)  

  if (HAVE_STRDUP)
    add_definitions (-DHAVE_STRDUP)
  endif (HAVE_STRDUP)  

  if (HAVE_SIGBLOCK)
    add_definitions (-DHAVE_SIGBLOCK)
  endif (HAVE_SIGBLOCK)  

  ## ------------------------------------------------------------------------------
  ## Create config.h

  configure_file (${USG_ROOT}/devel_common/cmake/config_wget.h.in
    ${WGET_BINARY_DIR}/config.h)

  ## ------------------------------------------------------------------------------
  ## Build executable

  include_directories (
    ${WGET_SOURCE_DIR}/src
    ${WGET_BINARY_DIR}
    )

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
#    $(SSL_OBJ)
    )

  add_executable (wget ${wget_sources})

  target_link_libraries (wget ${wget_link_libraries})
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_WGET OR WGET_FORCE_BUILD)
  message (STATUS "Found system-wide installation of WGET; skipping rebuild!")
endif (NOT HAVE_WGET OR WGET_FORCE_BUILD)
