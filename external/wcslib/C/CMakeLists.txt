##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Create "config.h"
##
## When using the GNU Autotools this typically is created by configure from
## config.h.in - since we bypass this setup but the source code requires the
## settings, we need to create the file here.

## CMake modules -------------------------------------------

include (CheckFunctionExists)
include (CheckLibraryExists)
include (CheckSymbolExists)
include (CheckTypeSize)
include (TestBigEndian)

## Check include files -------------------------------------

check_include_files (errno.h      HAVE_ERRNO_H     )
check_include_files (inttypes.h   HAVE_INTTYPES_H  )
check_include_files (unistd.h     HAVE_UNISTD_H    )

## Check type sizes ----------------------------------------

check_type_size ("int *"          SIZEOF_INT_P          )
check_type_size ("size_t"         SIZEOF_SIZE_T         )

## Define to `unsigned' if <sys/types.h> does not define
if (NOT SIZEOF_SIZE_T)
  set (size_t "unsigned")
endif (NOT SIZEOF_SIZE_T)

check_function_exists (floor    HAVE_FLOOR   )
check_function_exists (malloc   HAVE_MALLOC  )
check_function_exists (memset   HAVE_MEMSET  )
check_function_exists (pow      HAVE_POW     )
check_function_exists (sqrt     HAVE_SQRT    )
check_function_exists (strchr   HAVE_STRCHR  )
check_function_exists (strstr   HAVE_STRSTR  )

## Create config.h -----------------------------------------

configure_file (${USG_ROOT}/devel_common/cmake/config_wcslib.h.in
  ${WCSLIB_BINARY_DIR}/config.h)

## -----------------------------------------------------------------------------
## Files processed by Flex
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

set (WCSLIB_flex_inputs
  fitshdr
  wcspih
  wcsulex
  wcsutrn
)

if (flex_executable)
  foreach (src ${WCSLIB_flex_inputs})
    message (STATUS "Flex processing ${src}...")
    execute_process (
      WORKING_DIRECTORY ${WCSLIB_SOURCE_DIR}/C
      COMMAND ${flex_executable} -o${WCSLIB_BINARY_DIR}/C/${src}.c -P${src} ${src}.l
      TIMEOUT 20
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    if (flex_error)
      message (SEND_ERROR "There was an error processing ${src}.l!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
  endforeach (src)
else (flex_executable)
  message (SEND_ERROR "Unable to process flex input files!")
endif (flex_executable)

## -----------------------------------------------------------------------------
## Sources and instructions to build the library

file (GLOB WCSLIB_headers *.h)

set (WCSLIB_sources 
  cel.c
  lin.c
  log.c
  prj.c
  spc.c
  sph.c
  spx.c
  tab.c
  wcs.c
  wcsfix.c
  wcshdr.c
  wcstrig.c
  wcsunits.c
  wcsutil.c
  ${WCSLIB_BINARY_DIR}/C/fitshdr.c
  ${WCSLIB_BINARY_DIR}/C/wcspih.c
  ${WCSLIB_BINARY_DIR}/C/wcsulex.c
  ${WCSLIB_BINARY_DIR}/C/wcsutrn.c
)

include_directories (
  ${WCSLIB_BINARY_DIR}
  ${WCSLIB_BINARY_DIR}/C
  ${WCSLIB_SOURCE_DIR}
  ${WCSLIB_SOURCE_DIR}/C
  ${WCSLIB_SOURCE_DIR}/C/test
  ${HAVE_LIMITS_H}
  ${HAVE_MATH_H}
  ${HAVE_STDLIB_H}
)

add_definitions (-fPIC)

add_library (wcs ${WCSLIB_sources})
target_link_libraries (wcs ${HAVE_LIBM})

## -----------------------------------------------------------------------------
## Installation

## Installation of the library

install (TARGETS wcs
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Installation of header files

install (FILES ${WCSLIB_headers} ${WCSLIB_BINARY_DIR}/config.h
  DESTINATION include/wcslib
  )

## -----------------------------------------------------------------------------
## Location of the test programs

if (WCSLIB_ENABLE_TESTING)
  add_subdirectory (test)
endif (WCSLIB_ENABLE_TESTING)

## -----------------------------------------------------------------------------
## Book-keeping of some of the temporary variables

mark_as_advanced (
  # header files
  HAVE_ERRNO_H
  HAVE_INTTYPES_H
  HAVE_LIMITS_H
  HAVE_MATH_H
  HAVE_MEMORY_H
  HAVE_STDINT_H
  HAVE_STDIO_H
  HAVE_STDLIB_H
  HAVE_STRINGS_H
  HAVE_STRING_H
  HAVE_SYS_STAT_H
  HAVE_SYS_TYPES_H
  HAVE_UNISTD_H
  # functions defined in header files
  HAVE_FLOOR
  HAVE_MEMSET
  HAVE_POW
  HAVE_SQRT
  HAVE_STRCHR
  HAVE_STRSTR
)
