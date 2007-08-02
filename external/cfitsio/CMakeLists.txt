##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

project (libcfitsio)

set (libcfitsio_VERSION_MAJOR 3)
set (libcfitsio_VERSION_MINOR 4)
set (libcfitsio_VERSION "${libcfitsio_VERSION_MAJOR}.${libcfitsio_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (include_locations
  /usr/include
  /usr/local/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/c++/4.0.0
  /sw/include
  /sw/lib/gcc4.2/include/c++/4.2.1
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
)

include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CPack)

## -----------------------------------------------------------------------------
## Check for ANSI header files

check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (math.h HAVE_MATH_H)
check_include_files (memory.h HAVE_MEMORY_H)
check_include_files (limits.h HAVE_LIMITS_H)
check_include_files (pwd.h HAVE_PWD_H)
check_include_files (strings.h HAVE_STRINGS_H)
check_include_files (string.h HAVE_STRING_H)

## -----------------------------------------------------------------------------
## Check if functions are defined

check_function_exists (ftruncate HAVE_FTRUNCATE)

## -----------------------------------------------------------------------------
## Check for external libraries

find_library (libm m ${lib_locations})

if (NOT libm)
  message (SEND_ERROR "Unable to find libm!")
endif (NOT libm)

## -----------------------------------------------------------------------------
## Build options

option (BUILD_HERA
  "Build for HERA project (for LHEA use only)"
  0
  )

## -----------------------------------------------------------------------------
## Create the library
##
## libcfitsio.a:	${OBJECTS}
## 		ar rv libcfitsio.a ${OBJECTS}; \
##  		${RANLIB} libcfitsio.a;

file (GLOB cfitsio_headers *.h)

set (cfitsio_sources
  buffers.c
  cfileio.c
  checksum.c
  compress.c
  drvrfile.c
  drvrmem.c
  drvrnet.c
  drvrsmem.c
  drvrgsiftp.c
  editcol.c
  edithdu.c
  eval_l.c
  eval_y.c
  eval_f.c
  fitscore.c
  getcol.c
  getcolb.c
  getcold.c
  getcole.c
  getcoli.c
  getcolj.c
  getcolk.c
  getcoll.c
  getcols.c
  getcolsb.c
  getcoluk.c
  getcolui.c
  getcoluj.c
  getkey.c
  group.c
  grparser.c
  histo.c
  iraffits.c
  modkey.c
  putcol.c
  putcolb.c
  putcold.c
  putcole.c
  putcoli.c
  putcolj.c
  putcolk.c
  putcoluk.c
  putcoll.c
  putcols.c
  putcolsb.c
  putcolu.c
  putcolui.c
  putcoluj.c
  putkey.c
  region.c
  scalnull.c
  swapproc.c
  wcssub.c
  wcsutil.c
  imcompress.c
  quantize.c
  ricecomp.c
  pliocomp.c
  fits_hcompress.c
  fits_hdecompress.c
)

set (cfitsio_f77
  f77_wrap1.c
  f77_wrap2.c
  f77_wrap3.c
  f77_wrap4.c
)

include_directories (
  .
  ..
  ${HAVE_STDLIB_H}
  ${HAVE_MATH_H}
)

add_definitions (
  -O
  -O2
  -fPIC
  -fno-common
  ## header files
  -DHAVE_STDLIB_H=${HAVE_STDLIB_H}
  -DHAVE_STRING_H=${HAVE_STRING_H}
  -DHAVE_MATH_H=${HAVE_MATH_H}
  -DHAVE_MEMORY_H=${HAVE_MEMORY_H}
  -DHAVE_LIMITS_H=${HAVE_LIMITS_H}
  -DHAVE_STRINGS_H=${HAVE_STRINGS_H}
  -DHAVE_STRING=${HAVE_STRING}
  ## functions
  -DHAVE_FTRUNCATE=${HAVE_FTRUNCATE}
  ## build options
  -DBUILD_HERA=${BUILD_HERA}
)

## additional compiler flags for large file support

if (UNIX)
  add_definitions (
    -D_FILE_OFFSET_BITS=64
	-D_LARGEFILE_SOURCE
  )
endif (UNIX)

add_library (cfitsio ${cfitsio_sources})
target_link_libraries (cfitsio ${libm})

## -----------------------------------------------------------------------------
## Test program(s)

add_executable (testprog testprog.c)
target_link_libraries (testprog cfitsio ${libm})

## -----------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${libcfitsio_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package libcfitsio.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## Installation of library and executables

install (TARGETS cfitsio testprog
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Installation of header files

install (FILES ${cfitsio_headers}
  DESTINATION include/cfitsio
  )
