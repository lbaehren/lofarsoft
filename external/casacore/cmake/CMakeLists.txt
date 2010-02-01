##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 2431 2009-03-10 09:55:26Z baehren                      $
##------------------------------------------------------------------------------

## =============================================================================
##
##  Basic project settings
##
## =============================================================================

## Name of the project
project (CASACORE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## =============================================================================
##
##  User defined build/install options
##
## =============================================================================

option (CASACORE_ENABLE_TESTING    "Enable running test programs?"        NO  )
option (CASACORE_ENABLE_SHARED     "Build libraries as shared libraries?" NO  )
option (CASACORE_ENABLE_HDF5       "Enable support for HDF5?"             YES )

## Handle option: testing mode on/off

if (CASACORE_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (CASACORE_ENABLE_TESTING)

## =============================================================================
##
##  Check for required external components
##
## =============================================================================

set (CMAKE_INCLUDE_PATH "/Users/lars/Code/lofar/usg/release/include")
set (CMAKE_LIBRARY_PATH "/Users/lars/Code/lofar/usg/release/lib")

##__________________________________________________________
## Load required CMake modules

include (CPack)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckTypeSize)

##__________________________________________________________
## Set search locations for header files and libraries

set (include_locations
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
  "Search locations for header files."
  FORCE
  )

##__________________________________________________________
## Locations to search for libraries

set (lib_locations
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
  "Search locations for libraries."
  FORCE
  )

##__________________________________________________________
##                                                   CFITSIO

FIND_PATH (CFITSIO_INCLUDES fitsio.h
  PATHS ${include_locations}
  PATH_SUFFIXES cfitsio include include/cfitsio
  )

FIND_LIBRARY (CFITSIO_LIBRARIES
  NAMES cfitsio
  PATHS ${lib_locations}
  PATH_SUFFIXES cfitsio
  )

if (CFITSIO_INCLUDES) 
  include_directories (${CFITSIO_INCLUDES})
endif (CFITSIO_INCLUDES)

##__________________________________________________________
##                                                  GFORTRAN

set (GFORTRAN_LIBRARIES "")

find_program (GFORTRAN_EXECUTABLE gfortran gfortran-4.3 gfortran-4.2  gfortran-4.1
  PATHS ${bin_locations}
  )

## libgfortran

find_library (GFORTRAN_LIBRARY gfortran
  PATHS ${lib_locations}
  /opt/local
  $ENV{LD_LIBRARY_PATH}
  PATH_SUFFIXES
  gfortran/lib  
  gfortran/lib/x86_64
  lib/gcc43
  )

if (GFORTRAN_LIBRARY)
  list (APPEND GFORTRAN_LIBRARIES ${GFORTRAN_LIBRARY})
endif (GFORTRAN_LIBRARY)

## libg2c

find_library (G2C_LIBRARY g2c
  PATHS ${lib_locations}
  )

if (G2C_LIBRARY)
  list (APPEND GFORTRAN_LIBRARIES ${G2C_LIBRARY})
endif (G2C_LIBRARY)

##__________________________________________________________
##                                                      HDF5

if (CASACORE_ENABLE_HDF5) 
  
  ## Header files
  
  find_path (HDF5_INCLUDES hdf5.h hdf5_hl.h
    PATHS ${include_locations}
    PATH_SUFFIXES hdf5 include include/hdf5
    )
  
  find_file (HAVE_H5PUBLIC_H H5public.h
    PATHS ${include_locations}
    PATH_SUFFIXES hdf5 include include/hdf5
    )
  
  ## Libraries
  
  find_library (HDF5_HDF5_LIBRARY
    NAMES hdf5
    PATHS ${lib_locations}
    PATH_SUFFIXES hdf5 include include/hdf5
    )
  
  if (HDF5_HDF5_LIBRARY)
    set (HDF5_LIBRARIES ${HDF5_HDF5_LIBRARY})
  endif (HDF5_HDF5_LIBRARY)
    
  FIND_LIBRARY (HDF5_HDF5_HL_LIBRARY
    NAMES hdf5_hl
    PATHS ${lib_locations}
    PATH_SUFFIXES hdf5
    )
  
  if (HDF5_HDF5_HL_LIBRARY)
    list (APPEND HDF5_LIBRARIES ${HDF5_HDF5_HL_LIBRARY})
  endif (HDF5_HDF5_HL_LIBRARY)

  ## Determine library version
  
  if (HAVE_H5PUBLIC_H)
    
    ## extract library major version
    file (STRINGS ${HAVE_H5PUBLIC_H} HDF5_MAJOR_VERSION
      REGEX "H5_VERS_MAJOR.*For major"
      )
    string (REGEX REPLACE
      "#define H5_VERS_MAJOR" "" HDF5_MAJOR_VERSION ${HDF5_MAJOR_VERSION})
    string (REGEX MATCH
      "[0-9]" HDF5_MAJOR_VERSION ${HDF5_MAJOR_VERSION})

    ## extract library minor version
    file (STRINGS ${HAVE_H5PUBLIC_H} HDF5_MINOR_VERSION
      REGEX "H5_VERS_MINOR.*For minor"
      )
    string (REGEX REPLACE
      "#define H5_VERS_MINOR" "" HDF5_MINOR_VERSION ${HDF5_MINOR_VERSION})
    string (REGEX MATCH
      "[0-9]" HDF5_MINOR_VERSION ${HDF5_MINOR_VERSION})
    
    ## extract library release version
    file (STRINGS ${HAVE_H5PUBLIC_H} HDF5_RELEASE_VERSION
      REGEX "H5_VERS_RELEASE.*.For tweaks"
      )
    string (REGEX REPLACE
      "#define H5_VERS_RELEASE" "" HDF5_RELEASE_VERSION ${HDF5_RELEASE_VERSION})
    string (REGEX MATCH
      "[0-9]" HDF5_RELEASE_VERSION ${HDF5_RELEASE_VERSION})
  
    ## if we have HDF5, is it the correct version?
    if (${HDF5_MAJOR_VERSION} STRLESS 1)
      message (STATUS "Major version of HDF5 too old for casacore!")
      set (HAVE_HDF5 FALSE)
    else (${HDF5_MAJOR_VERSION} STRLESS 1)
      if (${HDF5_MINOR_VERSION} STRLESS 8)
	message (STATUS "HDF5 library version is < 1.8")
	set (HAVE_HDF5 FALSE)
      else (${HDF5_MINOR_VERSION} STRLESS 8)
	message (STATUS "HDF5 library version is >= 1.8")
      endif (${HDF5_MINOR_VERSION} STRLESS 8)
    endif (${HDF5_MAJOR_VERSION} STRLESS 1)
    
  endif (HAVE_H5PUBLIC_H)
  
endif (CASACORE_ENABLE_HDF5) 

##__________________________________________________________
##                                                    LAPACK

set (LAPACK_LIBRARIES "")

foreach (LAPACK_LIBRARY lapack cblas blas atlas)
  ##
  string(TOUPPER ${LAPACK_LIBRARY} LAPACK_TMP)
  ## search for the library
  find_library (LAPACK_${LAPACK_TMP}_LIBRARY ${LAPACK_LIBRARY}
    PATHS ${lib_locations}
    PATH_SUFFIXES lapack
    )
  ## add library to the list of LAPACK libraries
  if (LAPACK_${LAPACK_TMP}_LIBRARY)
    list (APPEND LAPACK_LIBRARIES ${LAPACK_${LAPACK_TMP}_LIBRARY})
  endif (LAPACK_${LAPACK_TMP}_LIBRARY)
endforeach (LAPACK_LIBRARY)

##__________________________________________________________
##                                                    WCSLIB

find_path (WCSLIB_INCLUDES wcs/wcs.h wcslib/wcs.h
  PATHS ${include_locations}
  )

find_library (WCSLIB_LIBRARIES wcs
  PATHS ${lib_locations}
  PATH_SUFFIXES wcs wcslib darwin/lib linux_gnu/lib stable/linux_gnu/lib
  )

if (WCSLIB_INCLUDES) 
  include_directories (${WCSLIB_INCLUDES})
endif (WCSLIB_INCLUDES)

## -----------------------------------------------------------------------------
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

check_include_files (assert.h  HAVE_ASSERT_H  )
check_include_files (ctype.h   HAVE_CTYPE_H   )
check_include_files (fcntl.h   HAVE_FCNTL_H   )
check_include_files (stdlib.h  HAVE_STDLIB_H  )
check_include_files (stdio.h   HAVE_STDIO_H   )
check_include_files (string.h  HAVE_STRING_H  )
check_include_files (time.h    HAVE_TIME_H    )
check_include_files (typeinfo  HAVE_TYPEINFO  )
check_include_files (unistd.h  HAVE_UNISTD_H  )

check_type_size ("int"       SIZEOF_INT       )
check_type_size ("float"     SIZEOF_FLOAT     )
check_type_size ("double"    SIZEOF_DOUBLE    )
check_type_size ("short"     SIZEOF_SHORT     )
check_type_size ("long"      SIZEOF_LONG      )
check_type_size ("long long" SIZEOF_LONG_LONG )

find_library (libm  m  PATHS ${lib_locations})
find_library (libdl dl PATHS ${lib_locations})

find_program (bin_bison  bison  ${bin_locations} )
find_program (curl_bin   curl   ${bin_locations} )
find_program (flex_bin   flex   ${bin_locations} )
find_program (bin_svn    svn   ${bin_locations} )
find_program (uname_bin  uname  ${bin_locations} )
find_program (bin_wget   wget   ${bin_locations} )

## --- mtio.h -----------------------------------------------
#  Lives in an unconventional path on OS X "Snow Leopard"

if (APPLE)
  
  find_path (HAVE_SYS_MTIO_H    sys/mtio.h   PATHS ${include_locations})
  find_path (HAVE_SYS_TYPES_H   sys/types.h  PATHS ${include_locations})
  
endif (APPLE)
  
## =============================================================================
##
##  Macro definitions
##
## =============================================================================

##__________________________________________________________
## Process input files with Bison

macro (casacore_process_bison _infile _sources_list)

  ## Extract components of the full filename

  get_filename_component(_infile_path      ${_infile} PATH)
  get_filename_component(_infile_filename  ${_infile} NAME)
  get_filename_component(_infile_name      ${_infile} NAME_WE)
  get_filename_component(_infile_extension ${_infile} EXT)
  
  string (REPLACE ${CASACORE_SOURCE_DIR} ${CASACORE_BINARY_DIR}
    _outfile_path ${_infile_path})
  set (_outfile_filename "${_infile_name}.ycc")
  set (_outfile "${_outfile_path}/${_outfile_filename}")

  ## Check if the output file already exists

  find_file (have_outfile ${_outfile} ${_outfile_path})
  
  ## Run Bison to process input file

  if (have_outfile)
    message (STATUS "[bison] ${_infile_filename} : Skip processing file.")
  else (have_outfile)
    ## status message
    message (STATUS "[bison] ${_infile_filename} -> ${_outfile_filename}")
    ## run Bison
    file (MAKE_DIRECTORY ${_outfile_path})
    execute_process (
      WORKING_DIRECTORY ${_infile_path}
      COMMAND ${bin_bison} -p ${_infile_name} -o ${_outfile} ${_infile}
      TIMEOUT 20
      ERROR_VARIABLE bison_error
      OUTPUT_QUIET
      )
    ## add output file to list of source files
    if (_sources_list)
      list (APPEND _sources_list ${_outfile})
    endif (_sources_list)
  endif (have_outfile)
  
endmacro (casacore_process_bison _infile)

##__________________________________________________________
## Process input files with Flex

macro (casacore_process_flex _infile _sources_list)
  
  ## Extract components of the full filename

  get_filename_component(_infile_path      ${_infile} PATH)
  get_filename_component(_infile_filename  ${_infile} NAME)
  get_filename_component(_infile_name      ${_infile} NAME_WE)
  get_filename_component(_infile_extension ${_infile} EXT)
  
  string (REPLACE ${CASACORE_SOURCE_DIR} ${CASACORE_BINARY_DIR}
    _outfile_path ${_infile_path})
  set (_outfile_filename "${_infile_name}.lcc")
  set (_outfile "${_outfile_path}/${_outfile_filename}")

  ## Check if the output file already exists

  find_file (have_outfile ${_outfile} ${_outfile_path})
  
  ## Run Flex to process input file

  if (have_outfile)
    message (STATUS "[flex] ${_infile_filename} : Skip processing file.")
  else (have_outfile)
    ## status message
    message (STATUS "[flex] ${_infile_filename} -> ${_outfile_filename}")
    ## run flex
    execute_process (
      WORKING_DIRECTORY ${_infile_path}
      COMMAND ${flex_bin} -t -P${_infile_name} ${_infile_filename}
      TIMEOUT 20
      OUTPUT_FILE ${_outfile}
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    ## evaluate error
    if (flex_error)
      message (SEND_ERROR "[flex] Error processing ${_infile_filename}!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
    ## add output file to list of source files
    if (_sources_list)
      list (APPEND _sources_list ${_outfile})
    endif (_sources_list)
  endif (have_outfile)
  
endmacro (casacore_process_flex _infile)

##__________________________________________________________
## Build a module library

macro (casacore_build_module_library _module _submodules)

  ## convert module name to upper-case
  string(TOUPPER ${_module} _module_var)
  
  foreach (MODULE_${_module_var} ${CASACORE_MODULES_${_module_var}})
    ## Files processed by Bison
    file (GLOB bison_${MODULE_${_module_var}} ${MODULE_${_module_var}}/*.yy)
    list (APPEND MODULE_${_module_var}_BISON ${bison_${MODULE_${_module_var}}})
    ## Files processed by Flex
    file (GLOB flex_${MODULE_${_module_var}} ${MODULE_${_module_var}}/*.ll)
    list (APPEND MODULE_${_module_var}_FLEX ${flex_${MODULE_${_module_var}}})
    ## Source files
    file (GLOB msfits_${MODULE_${_module_var}} ${MODULE_${_module_var}}/*.cc ${MODULE_${_module_var}}/*.tcc)
    list (APPEND MODULE_${_module_var}_SOURCES ${msfits_${MODULE_${_module_var}}})
  endforeach (MODULE_${_module_var})

endmacro (casacore_build_module_library _module)

##__________________________________________________________
##                                Download Measures database

macro (casacore_measures_data _url)

  message (STATUS "Downloading Measures data from ${_url}")

endmacro (casacore_measures_data _url)

##__________________________________________________________
##                                       Update working copy

if (bin_svn)
  add_custom_target (casacore_update
    COMMAND ${bin_svn} update
    WORKING_DIRECTORY ${CASACORE_SOURCE_DIR}
    COMMENT "Updating working copy ..."
    )
endif (bin_svn)

##__________________________________________________________
##                               Archive CMake configuration

add_custom_target (casacore_cmake_config
  COMMAND rm -f ${CASACORE_BINARY_DIR}/cmake.tgz
  COMMAND tar -cvzf ${CASACORE_BINARY_DIR}/cmake.tgz `find . -name CMakeLists.txt`
  WORKING_DIRECTORY ${CASACORE_SOURCE_DIR}
  COMMENT "Creating archive of CMake configuration ..."
)

## =============================================================================
##
##  Compiler and Linker options
##
## =============================================================================

##__________________________________________________________
## Module dependencies

# casa
# scimath     casa
# tables      casa
# measures    tables scimath casa
# lattices    tables scimath casa
# fits        measures tables scimath casa cfitsio
# ms          measures tables scimath casa
# msfits      ms fits measures tables scimath casa cfitsio
# coordinates fits measures tables scimath casa wcslib cfitsio
# components  coordinates fits measures tables scimath casa wcslib cfitsio
# images      components coordinates fits measures tables scimath casa mirlib wcslib cfitsio

##__________________________________________________________
## List of casacore packages/modules

set (CASACORE_PACKAGES
  casa
  scimath_f
  scimath
  tables
  measures
  lattices
  fits
  ms
  msfits
  coordinates
  components
  mirlib
  images
  )

##__________________________________________________________
## Include directories

## casacore module directories

include_directories (
  ## source tree
  ${CASACORE_SOURCE_DIR}
  ${CASACORE_SOURCE_DIR}/mirlib
  ## build tree (dynamically generated sources)
  ${CASACORE_BINARY_DIR}/tables/Tables
  ${CASACORE_BINARY_DIR}/ms/MeasurementSets
  ${CASACORE_BINARY_DIR}/images/Images
  )

if (APPLE)
  message (STATUS "Host system is Mac OS X.")
  if (${CMAKE_HOST_SYSTEM_VERSION} GREATER 9.9.0)
    message (STATUS "Host version is greater than 9.9.0.")
    if (HAVE_SYS_MTIO_H)
      include_directories (${HAVE_SYS_MTIO_H})
    endif (HAVE_SYS_MTIO_H)
  endif (${CMAKE_HOST_SYSTEM_VERSION} GREATER 9.9.0)
endif (APPLE)

##____________________________________________________________________
## Compiler settings

IF (UNIX)

  ## --- Platform test Big/Little Endian -------------------
  
#  include (TestBigEndian)
#  test_big_endian (CMAKE_SYSTEM_BIG_ENDIAN)
  
  if (CMAKE_SYSTEM_BIG_ENDIAN)
    add_definitions (-DAIPS_BIG_ENDIAN)
  endif (CMAKE_SYSTEM_BIG_ENDIAN)
  
  ## --- Platform test 32/64 bit ---------------------------
  
  if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set (CMAKE_SYSTEM_64BIT 1)
    add_definitions (-DAIPS_64B)
  else (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set (CMAKE_SYSTEM_64BIT 0)
  endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)

  ## --- Kernel version ------------------------------------
  
  execute_process (
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND uname -m
    TIMEOUT 20
    OUTPUT_VARIABLE CMAKE_SYSTEM_KERNEL
    )

  ## --- Compiler flags ------------------------------------

  add_definitions (
    -fPIC
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    )

  IF (APPLE)
    add_definitions (-DAIPS_DARWIN)
  ELSE (APPLE)
    ADD_DEFINITIONS (-DAIPS_LINUX)
  ENDIF (APPLE)
  
  ## --- Linker flags --------------------------------------
  
  if (APPLE)
    if (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
      set (CMAKE_MODULE_LINKER_FLAGS "-Xlinker -rpath -Xlinker -m64")
    else (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
      if (CMAKE_SYSTEM_KERNEL MATCHES i386)
	set (CMAKE_MODULE_LINKER_FLAGS "-Xlinker -rpath -Xlinker -m32")
      endif (CMAKE_SYSTEM_KERNEL MATCHES i386)
    endif (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
  else (APPLE)
    set (CMAKE_MODULE_LINKER_FLAGS "-Xlinker -rpath -Xlinker")
  endif (APPLE)
  
ENDIF (UNIX)

## -----------------------------------------------------------------------------
## Sub-directories

foreach (CASACORE_package ${CASACORE_PACKAGES})
  message (STATUS "adding subdirectory ${CASACORE_package} ...")
  add_subdirectory (${CASACORE_package})
endforeach (CASACORE_package)

## =============================================================================
##
##  Summary of settings
##
## =============================================================================

message (STATUS "+------------------------------------------------------------+")
message (STATUS "| [casacore] Configuration summary                           |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS "CMAKE_SYSTEM ............... = ${CMAKE_SYSTEM}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR ..... = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_SYSTEM_KERNEL ........ = ${CMAKE_SYSTEM_KERNEL}")
message (STATUS "CMAKE_MODULE_LINKER_FLAGS .. = ${CMAKE_MODULE_LINKER_FLAGS}")
message (STATUS "CMAKE_INSTALL_PREFIX ....... = ${CMAKE_INSTALL_PREFIX}")
message (STATUS "+------------------------------------------------------------+")
message (STATUS "Measures data directory .... = ${casacore_measures_data}")
message (STATUS "Curl network downloader .... = ${curl_bin}")
message (STATUS "Wget network downloader .... = ${bin_wget}")
message (STATUS "Bison lexical parser ....... = ${bison_bin}")
message (STATUS "Flex lexical parser ........ = ${flex_bin}")
message (STATUS "CFITSIO_INCLUDES ........... = ${CFITSIO_INCLUDES}")
message (STATUS "CFITSIO_LIBRARIES .......... = ${CFITSIO_LIBRARIES}")
message (STATUS "GFORTRAN_LIBRARIES ......... = ${GFORTRAN_LIBRARIES}")
message (STATUS "HDF5_INCLUDES .............. = ${HDF5_INCLUDES}")
message (STATUS "HDF5_LIBRARIES ............. = ${HDF5_LIBRARIES}")
message (STATUS "HDF5 library version ....... = ${HDF5_MAJOR_VERSION}.${HDF5_MINOR_VERSION}")
message (STATUS "LAPACK library? ............ = ${LAPACK_LAPACK_LIBRARY}")
message (STATUS "WCSLIB_INCLUDES ............ = ${WCSLIB_INCLUDES}")
message (STATUS "WCSLIB_LIBRARIES ........... = ${WCSLIB_LIBRARIES}")
message (STATUS "+------------------------------------------------------------+")
