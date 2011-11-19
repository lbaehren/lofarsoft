##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSTOOLS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

include (CTest)

## ------------------------------------------------------------------------------
## User selectable options

option (WCSTOOLS_BUILD_TOOLS  "Build the applications?"   YES )
option (WCSTOOLS_BUILD_LIBNED "Build the libned library?" NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_CMAKE_MODULES)
  find_path (LUS_CMAKE_MODULES template_FindXX.cmake LUS_CMakeSettings.cmake
    PATHS 
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    PATH_SUFFIXES
    cmake
    devel_common/cmake
    )
endif (NOT LUS_CMAKE_MODULES)

if (LUS_CMAKE_MODULES)
  ## Augment CMAKE_MODULE_PATH variable
  set (CMAKE_MODULE_PATH ${LUS_CMAKE_MODULES} CACHE PATH
    "CMake module path"
    FORCE)
  ## Load custom CMake settings
  include (LUS_CMakeSettings)
endif (LUS_CMAKE_MODULES)

## ==============================================================================
##
##  External packages/libraries
##
## ==============================================================================

find_library (libsocket socket
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

find_library (libnsl nsl
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

find_library (libm m
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  )

find_library (libSystemStubs SystemStubs
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  /Developer/SDKs/MacOSX10.3.9.sdk/usr/lib
  )

## set up chain of libraries to link against

set (link_libraries "")

if (libnsl)
  list (APPEND link_libraries ${libnsl})
endif (libnsl)

if (libm)
  list (APPEND link_libraries ${libm})
endif (libm)

## ==============================================================================
##
##  Libraries
##
## ==============================================================================

add_subdirectory (libwcs)

if (WCSTOOLS_BUILD_LIBNED)
  add_subdirectory (libned)
endif (WCSTOOLS_BUILD_LIBNED)

## ==============================================================================
##
##  Applications
##
## ==============================================================================

if (WCSTOOLS_BUILD_TOOLS)
  
  set (wcstools
    cphead
    delwcs
    delhead
    edhead
    fixpix
    gethead
    i2f
    imcat
    imhead
    immatch
    imrot
    imsize
    imstar
    imwcs
    scat sethead addpix getpix setpix sky2xy
    keyhead skycoor subpix xy2sky wcshead conpix gettab newfits getfits
    imstack imextract sumpix remap getcol getdate imfill imsmooth imresize
    fileroot filename filext char2sp sp2char crlf isnum isrange simpos bincat
    )
  
  foreach (app ${wcstools})
    ## compiler instructions
    add_executable (${app} ${app}.c)
    ## linker instructions
    target_link_libraries (${app} wcs ${link_libraries})
    ## installation instructions
    install (TARGETS ${app}
      RUNTIME DESTINATION bin
      )
  endforeach (app)

endif (WCSTOOLS_BUILD_TOOLS)
