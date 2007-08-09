##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

project (CFITSIO)

set (CFITSIO_VERSION_MAJOR 3)
set (CFITSIO_VERSION_MINOR 4)
set (CFITSIO_VERSION "${CFITSIO_VERSION_MAJOR}.${CFITSIO_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## Optional build settings

option (CFITSIO_BUILD_TESTS "Build test programs for CFITSIO?" 1)
option (CFITSIO_BUILD_HERA "Build for HERA project (for LHEA use only)" 0)

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
)

set (include_locations
  /usr/include
  /usr/local/include
  /sw/include
  /sw/lib/gcc4.2/include/c++/4.2.1
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/c++/4.0.0
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
)

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
check_include_files (unistd.h HAVE_UNISTD_H)
check_include_files (inttypes.h HAVE_INTTYPES_H)

find_PATH (HAVE_IPC_H ipc.h PATHS ${include_locations} PATH_SUFFIXES sys)
find_PATH (HAVE_SEM_H sem.h PATHS ${include_locations} PATH_SUFFIXES sys)
find_PATH (HAVE_SHM_H shm.h PATHS ${include_locations} PATH_SUFFIXES sys)

## -----------------------------------------------------------------------------
## Check if functions are defined

include (CheckFunctionExists)

check_function_exists (ftruncate HAVE_FTRUNCATE)

## -----------------------------------------------------------------------------
## Check size of variables types

include (CheckTypeSize)

check_type_size ("long" HAVE_LONG)
check_type_size ("long long" HAVE_LONGLONG)

## -----------------------------------------------------------------------------
## Check for external libraries

find_library (libm m ${lib_locations})

if (NOT libm)
  message (SEND_ERROR "Unable to find libm!")
endif (NOT libm)

find_library (libnsl nsl ${lib_locations})

if (NOT libnsl)
  set (HAVE_NET_SERVICES 0)
endif (NOT libnsl)

## -----------------------------------------------------------------------------
## CFITSIO Lexical Parser

## find required programs

find_program (flex_bin flex ${bin_locations})
find_program (bison_bin bison ${bin_locations})
find_program (sed_bin sed ${bin_locations})

## eval.l  ->  eval_l.c1  ->  eval_l.c

if (flex_bin)
  message (STATUS "Flex: processing eval.l ...")
  execute_process (
    WORKING_DIRECTORY ${CFITSIO_SOURCE_DIR}
    COMMAND ${flex_bin} -t eval.l
    TIMEOUT 10
    OUTPUT_FILE eval_l.c1
    ERROR_VARIABLE flex_error
    OUTPUT_QUIET
  )
  if (flex_error)
    message (SEND_ERROR "[flex] ${flex_error}")
  endif (flex_error)
  ## replace symbols in the previously generated file
  if (sed_bin)
    message (STATUS "sed: processing eval_l.c1 ...")
    execute_process (
      WORKING_DIRECTORY ${CFITSIO_SOURCE_DIR}
      COMMAND ${sed_bin} -e "s/yy/ff/g" -e "s/YY/FF/g" eval_l.c1
      TIMEOUT 10
      OUTPUT_FILE eval_l.c
      ERROR_VARIABLE sed_error
      OUTPUT_QUIET
    )
	if (sed_error)
	  message (SEND_ERROR "[sed] ${sed_error}")
	endif (sed_error)
  endif (sed_bin)
  ## remove temporary file
  FILE (REMOVE "eval_l.c1")
endif (flex_bin)

## eval.y  ->  eval_y.c

if (bison_bin)
  message (STATUS "Bison: processing eval.y ...")
  execute_process (
    WORKING_DIRECTORY ${CFITSIO_SOURCE_DIR}
    COMMAND ${bison_bin} -d -v -y eval.y
    TIMEOUT 20
    ERROR_VARIABLE bison_error
    OUTPUT_QUIET
  )
  ## replace symbols in the previously generated file
  if (sed_bin)
    message (STATUS "sed: processing y.tab.c ...")
    execute_process (
      WORKING_DIRECTORY ${CFITSIO_SOURCE_DIR}
      COMMAND ${sed_bin} -e "s/yy/ff/g" -e "s/YY/FF/g" y.tab.c
      TIMEOUT 20
      OUTPUT_FILE eval_y.c
      ERROR_VARIABLE sed_error
      OUTPUT_QUIET
    )
	if (sed_error)
	  message (SEND_ERROR "[sed] ${sed_error}")
	endif (sed_error)
    message (STATUS "sed: processing y.tab.c ...")
    execute_process (
      WORKING_DIRECTORY ${CFITSIO_SOURCE_DIR}
      COMMAND ${sed_bin} -e "s/yy/ff/g" -e "s/YY/FF/g" y.tab.h
      TIMEOUT 20
      OUTPUT_FILE eval_tab.h
      ERROR_VARIABLE sed_error
      OUTPUT_QUIET
    )
	if (sed_error)
	  message (SEND_ERROR "[sed] ${sed_error}")
	endif (sed_error)
  endif (sed_bin)
endif (bison_bin)

## -----------------------------------------------------------------------------
## Create the library
##
## libcfitsio.a:	${OBJECTS}
## 		ar rv libcfitsio.a ${OBJECTS}; \
##  		${RANLIB} libcfitsio.a;
##
## [cfitsio 2.5.1]
## cc -c -O -O2 -Dg77Fortran -fPIC -fno-common
##    -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MATH_H=1 -DHAVE_LIMITS_H=1
##    -DHAVE_FTRUNCATE=1 -DHAVE_LONGLONG=1 -DHAVE_SHMEM_SERVICES=1
##    -DHAVE_UNION_SEMUN=1 -DHAVE_NET_SERVICES=1  pliocomp.c
## [cfitsio 3.0.4]
## cc -c -O -O2 -Dg77Fortran -fPIC -fno-common 
##    -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" 
##    -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\"
##    -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1
##    -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1
##    -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDLIB_H=1
##    -DHAVE_STRING_H=1 -DHAVE_MATH_H=1 -DHAVE_LIMITS_H=1 -D_LARGEFILE_SOURCE=1
##    -D_FILE_OFFSET_BITS=64 -DHAVE_FTRUNCATE=1 -DHAVE_LONGLONG=1
##    -DHAVE_SHMEM_SERVICES=1 -DHAVE_UNION_SEMUN=1 -DHAVE_NET_SERVICES=1  cfileio.c
##

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
  -DHAVE_INTTYPES_H=${HAVE_INTTYPES_H}
  -DHAVE_LIMITS_H=${HAVE_LIMITS_H}
  -DHAVE_MATH_H=${HAVE_MATH_H}
  -DHAVE_MEMORY_H=${HAVE_MEMORY_H}
  -DHAVE_STDLIB_H=${HAVE_STDLIB_H}
  -DHAVE_STRING_H=${HAVE_STRING_H}
  -DHAVE_STRINGS_H=${HAVE_STRINGS_H}
  -DHAVE_UNISTD_H=${HAVE_UNISTD_H}
  ## functions
  -DHAVE_FTRUNCATE=${HAVE_FTRUNCATE}
  ## variable types
  -DHAVE_LONGLONG=${HAVE_LONGLONG}
  ## libraries
  -DHAVE_NET_SERVICES=${HAVE_NET_SERVICES}
  ## build options
  -DBUILD_HERA=${CFITSIO_BUILD_HERA}
)

## additional compiler flags for large file support

if (UNIX)
  add_definitions (
    -D_FILE_OFFSET_BITS=64
	-D_LARGEFILE_SOURCE
  )
  if (APPLE)
    add_definitions (-D__APPLE__)
  endif (APPLE)
endif (UNIX)

add_library (cfitsio ${cfitsio_sources})
target_link_libraries (cfitsio ${libm})

## -----------------------------------------------------------------------------
## Test program(s)

if (CFITSIO_BUILD_TESTS)

  set (cfitsio_tests
    cookbook
    fitscopy
    imcopy
    listhead
    speed
    testprog
  )

  if (HAVE_NET_SERVICES)
    list (APPEND cfitsio_tests smem)
  endif (HAVE_NET_SERVICES)

  ## build the test programs

  foreach (test ${cfitsio_tests})
    add_executable (${test} ${test}.c)
    target_link_libraries (${test} cfitsio ${libm})
  endforeach (test)

endif (CFITSIO_BUILD_TESTS)

## =============================================================================
##
##  Installation
##
## =============================================================================

find_path (prefix release_area.txt
  PATHS
  ${CFITSIO_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package CFITSIO.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

## Installation of the library

install (TARGETS cfitsio ${cfitsio_tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## Installation of the executables

if (CFITSIO_BUILD_TESTS)
  install (TARGETS ${cfitsio_tests}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (CFITSIO_BUILD_TESTS)

## Installation of header files

install (FILES ${cfitsio_headers}
  DESTINATION include/cfitsio
  )
