
project (boost)

## -----------------------------------------------------------------------------
## Settings 

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt
)

set (boost_version_major 1)
set (boost_version_minor 34)
set (boost_version_patch 1)
set (boost_version "${boost_version_major}_${boost_version_minor}_${boost_version_patch}")

set (boost_source_dir "boost_${boost_version}")
set (boost_source_file "${boost_source_dir}.tar.gz")

set (boost_url http://surfnet.dl.sourceforge.net/sourceforge/boost)

## -----------------------------------------------------------------------------
## Locate required tools

find_program (CURL_EXECUTABLE curl PATHS ${bin_locations})
find_program (TAR_EXECUTABLE tar PATHS ${bin_locations})
find_program (WGET_EXECUTABLE wget PATHS ${bin_locations})

## -----------------------------------------------------------------------------
## Feedback

message (STATUS "boost source file .. : ${boost_source_file}")
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

find_file (HAVE_BOOST_SOURCE ${boost_source_file} PATHS . .. /Volumes/UNIX)

## If the source package is not there yet, we try to fetch it

if (NOT HAVE_BOOST_SOURCE)
  message (STATUS "No source package found; try to fetch it now ...")
  if (WGET_EXECUTABLE)
    execute_process (
      WORKING_DIRECTORY .
      COMMAND ${WGET_EXECUTABLE} -c ${boost_url}/${boost_source_file}
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
endif (NOT HAVE_BOOST_SOURCE)

## Once we have retrieved the source package, adjust the variable pointing to it

find_file (HAVE_BOOST_SOURCE ${boost_source_file} PATHS . ..)

## -----------------------------------------------------------------------------
## [2] Unpack the source package

if (TAR_EXECUTABLE AND HAVE_BOOST_SOURCE)
  message (STATUS "Unpacking source package ${HAVE_BOOST_SOURCE} ...")
  execute_process (
    WORKING_DIRECTORY .
    COMMAND ${TAR_EXECUTABLE} -xvzf ${HAVE_BOOST_SOURCE}
    TIMEOUT 120
    ERROR_VARIABLE tar_cerr
    OUTPUT_QUIET
  )
  if (tar_cerr)
    message (STATUS "[tar] Error expanding the source package!")
	message (STATUS ${tar_cerr})
  endif (tar_cerr)
endif (TAR_EXECUTABLE AND HAVE_BOOST_SOURCE)

## -----------------------------------------------------------------------------
## [3] Configure the source package

## Determine the platform - depending on this we may modify the installation 
## path

if (UNIX)
  ## Default location for *NIX platforms
  set (boost_prefix "/usr/local")
  if (APPLE)
    ## Check if there is a Fink installation on the system?
    find_file (HAVE_FINK VERSION PATHS /sw/fink /sw/fink/dists)
	if (HAVE_FINK)
	  set (boost_prefix "/sw")
	endif (HAVE_FINK)
  endif (APPLE)
endif (UNIX)

## Adjust the path to the source package directory previously created

find_path (HAVE_BOOST_SOURCE_DIR project-root.jam PATHS . ./${boost_source_dir})

if (HAVE_BOOST_SOURCE_DIR)
  message (STATUS "Configuring source package for installation at ${boost_prefix} ...")
  execute_process (
    WORKING_DIRECTORY ${HAVE_BOOST_SOURCE_DIR}
    COMMAND ./configure --prefix=${boost_prefix}
    TIMEOUT 120
    ERROR_VARIABLE configure_cerr
    OUTPUT_QUIET
  )
  if (configure_cerr)
    message (STATUS "[configure] Error while configuring source package!")
	message (STATUS ${configure_cerr})
  endif (configure_cerr)
endif (HAVE_BOOST_SOURCE_DIR)

## -----------------------------------------------------------------------------
## [4] Build the package from source

