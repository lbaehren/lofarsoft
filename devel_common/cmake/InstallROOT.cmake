##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 494 2007-08-02 09:39:15Z baehren                       $
##------------------------------------------------------------------------------

## =============================================================================
## CMake script to aid installation of the ROOT data analysis framework
## =============================================================================

## -----------------------------------------------------------------------------
## Settings 

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt
)

set (root_version_major 1)
set (root_version_minor 34)
set (root_version_patch 1)
set (root_version "${root_version_major}_${root_version_minor}_${root_version_patch}")

set (root_source_dir "root_${root_version}")
set (root_source_file "${root_source_dir}.tar.gz")

set (root_url http://surfnet.dl.sourceforge.net/sourceforge/boost)

## -----------------------------------------------------------------------------
## Locate required tools

find_program (CURL_EXECUTABLE curl PATHS ${bin_locations})
find_program (TAR_EXECUTABLE tar PATHS ${bin_locations})
find_program (WGET_EXECUTABLE wget PATHS ${bin_locations})

## -----------------------------------------------------------------------------
## Feedback

message (STATUS "ROOT source file ... : ${root_source_file}")
message (STATUS "curl ............... : ${CURL_EXECUTABLE}")
message (STATUS "tar ................ : ${TAR_EXECUTABLE}")
message (STATUS "wget ............... : ${WGET_EXECUTABLE}")

## =============================================================================
##  Processing 
## =============================================================================

## -----------------------------------------------------------------------------
## [1] Retrieve the source distribution; this will only be required if a source
## tar-ball isn't there already

## Is the source package there already?

find_file (HAVE_ROOT_SOURCE ${root_source_file}
  PATHS . .. /opt /tmp /Volumes/UNIX
)

## If the source package is not there yet, we try to fetch it

if (NOT HAVE_ROOT_SOURCE)
  message (STATUS "No source package found; try to fetch it now ...")
  if (WGET_EXECUTABLE)
    execute_process (
      WORKING_DIRECTORY .
      COMMAND ${WGET_EXECUTABLE} -c ${root_url}/${root_source_file}
      TIMEOUT 300
      ERROR_VARIABLE wget_cerr
      OUTPUT_QUIET
    )
	if (wget_cerr)
	  message (STATUS "[wget] Unable to download source package!")
	  message (STATUS "${wget_cerr}")
	endif (wget_cerr)
  elseif (WGET_EXECUTABLE)
    message (STATUS "Unable to use wget; trying to switch to curl.")
    if (CURL_EXECUTABLE)
    elseif (CURL_EXECUTABLE)
    endif (CURL_EXECUTABLE)
  endif (WGET_EXECUTABLE)
endif (NOT HAVE_ROOT_SOURCE)

## Once we have retrieved the source package, adjust the variable pointing to it

find_file (HAVE_ROOT_SOURCE ${root_source_file} PATHS . ..)

## -----------------------------------------------------------------------------
## [2] Unpack the source package

if (TAR_EXECUTABLE AND HAVE_ROOT_SOURCE)
  message (STATUS "Unpacking source package ${HAVE_ROOT_SOURCE} ...")
  execute_process (
    WORKING_DIRECTORY .
    COMMAND ${TAR_EXECUTABLE} -xvzf ${HAVE_ROOT_SOURCE}
    TIMEOUT 120
    ERROR_VARIABLE tar_cerr
    OUTPUT_QUIET
  )
  if (tar_cerr)
    message (STATUS "[tar] Error expanding the source package!")
	message (STATUS ${tar_cerr})
  endif (tar_cerr)
endif (TAR_EXECUTABLE AND HAVE_ROOT_SOURCE)

## -----------------------------------------------------------------------------
## [3] Configure the source package

## Determine the platform - depending on this we may modify the installation 
## path

if (UNIX)
  ## Default location for *NIX platforms
  set (root_prefix "/usr/local")
  if (APPLE)
    ## Check if there is a Fink installation on the system?
    find_file (HAVE_FINK VERSION PATHS /sw/fink /sw/fink/dists)
	if (HAVE_FINK)
	  set (root_prefix "/sw")
	endif (HAVE_FINK)
  endif (APPLE)
endif (UNIX)

## Adjust the path to the source package directory previously created

find_path (HAVE_ROOT_SOURCE_DIR project-root.jam PATHS . ./${root_source_dir})

if (HAVE_ROOT_SOURCE_DIR)
  message (STATUS "Configuring source package for installation at ${root_prefix} ...")
  execute_process (
    WORKING_DIRECTORY ${HAVE_ROOT_SOURCE_DIR}
    COMMAND ./configure --prefix=${root_prefix}
    TIMEOUT 120
    ERROR_VARIABLE configure_cerr
    OUTPUT_QUIET
  )
  if (configure_cerr)
    message (STATUS "[configure] Error while configuring source package!")
	message (STATUS ${configure_cerr})
  endif (configure_cerr)
endif (HAVE_ROOT_SOURCE_DIR)

## -----------------------------------------------------------------------------
## [4] Build the package from source

