
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

## ==============================================================================
##
##  External packages/libraries
##
## ==============================================================================

<<<<<<< HEAD
find_library (libsocket      socket      )
find_library (libnsl         nsl         )
find_library (libm           m           )
find_library (libSystemStubs SystemStubs )
=======
find_library (libsocket socket)
find_library (libnsl nsl)
find_library (libm m)
find_library (libSystemStubs SystemStubs)
>>>>>>> Cleaning up installation of DAL.

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
    scat
    sethead
    addpix
    getpix
    setpix
    sky2xy
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
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      )
  endforeach (app)
  
endif (WCSTOOLS_BUILD_TOOLS)
