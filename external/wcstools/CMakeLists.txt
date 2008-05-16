##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSTOOLS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

include (CTest)

## ------------------------------------------------------------------------------
## User selectable options

option (WCSTOOLS_BUILD_TOOLS "Build the applications?" ON)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${WCSTOOLS_SOURCE_DIR}
  ${WCSTOOLS_SOURCE_DIR}/..
  ${WCSTOOLS_SOURCE_DIR}/../..
  ${WCSTOOLS_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

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

find_library (libSystemStubs SystemStubs
  /usr/lib
  /usr/local/lib
  /opt/lib
  /sw/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  /Developer/SDKs/MacOSX10.3.9.sdk/usr/lib
  )

## ==============================================================================
##
##  Libraries
##
## ==============================================================================

add_subdirectory (libwcs)
add_subdirectory (libned)

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
    target_link_libraries (${app} wcs)
    ## installation instructions
    install (TARGETS ${app}
      RUNTIME DESTINATION bin
      )
  endforeach (app)

endif (WCSTOOLS_BUILD_TOOLS)