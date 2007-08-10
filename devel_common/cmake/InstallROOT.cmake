##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 494 2007-08-02 09:39:15Z baehren                       $
##------------------------------------------------------------------------------

## =============================================================================
## CMake script to aid installation of the ROOT data analysis framework
## =============================================================================

## -----------------------------------------------------------------------------
## Settings 

## Location of the CMake scripts

find_path (cmake_scripts FindROOT.cmake InstallROOT.cmake
  PATHS
  ..
  ../..
  PATH_SUFFIXES
  devel_common/cmake
)

if (cmake_scripts)
  message (STATUS "Found CMake script directory : ${cmake_scripts}")
elseif (cmake_scripts)
  message (FATAL_ERROR "Cannot find CMake script directory; unable to continue!")
endif (cmake_scripts)

## Locations in which to search

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt
)

## URL : ftp://root.cern.ch/root/root_v5.16.00.source.tar.gz

set (root_version_major 5)
set (root_version_minor 16)
set (root_version_patch 00)
set (root_version "${root_version_major}.${root_version_minor}.${root_version_patch}")

set (root_source_dir "root")
set (root_source_file "root_v${root_version}.source.tar.gz")

set (root_url "ftp://root.cern.ch/root/${root_source_file}")

## -----------------------------------------------------------------------------
## Locate required tools

find_program (CURL_EXECUTABLE curl PATHS ${bin_locations})
find_program (TAR_EXECUTABLE tar PATHS ${bin_locations})
find_program (WGET_EXECUTABLE wget PATHS ${bin_locations})

## -----------------------------------------------------------------------------
## Feedback

message (STATUS "ROOT source file ...... : ${root_source_file}")
message (STATUS "ROOT source package URL : ${root_source_file}")
message (STATUS "curl .................. : ${CURL_EXECUTABLE}")
message (STATUS "tar ................... : ${TAR_EXECUTABLE}")
message (STATUS "wget .................. : ${WGET_EXECUTABLE}")

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
      COMMAND ${WGET_EXECUTABLE} -c ${root_url}
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

## Determine installation

find_path (root_prefix release_area.txt
  PATHS
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (root_prefix)
  message (STATUS "Installation area located for package wcslib.")
  set (CMAKE_INSTALL_PREFIX ${root_prefix}) 
endif (root_prefix)

## Adjust the path to the source package directory previously created

find_path (HAVE_ROOT_SOURCE_DIR configure LICENSE
  PATHS . ./${root_source_dir} ../${root_source_dir}
  )

## Handle the various installation options for ROOT

# enable/disable options, prefix with either --enable- or --disable-
#   afs                AFS support, requires AFS libs and objects
#   alien              AliEn support, requires libgapiUI from ALICE
#   asimage            Image processing support, requires libAfterImage
#   astiff             Include tiff support in image processing
#   builtin-afterimage Built included libAfterImage, or use system libAfterImage
#   builtin-freetype   Built included libfreetype, or use system libfreetype
#   builtin-pcre       Built included libpcre, or use system libpcre
#   builtin-zlib       Built included libz, or use system libz
#   castor             CASTOR support, requires libshift from CASTOR >= 1.5.2
#   cern               CERNLIB usage, build h2root and g2root
#   chirp              Chirp support (Condor remote I/O), requires libchirp_client
#   cint7              Build the *experimental* new Cint library
#   cintex             Build the libCintex Reflex interface library
#   clarens            Clarens RPC support, optionally used by PROOF
#   dcache             dCache support, requires libdcap from DESY
#   exceptions         Turn on compiler exception handling capability
#   explicitlink       Explicitly link with all dependent libraries
#   fftw3              Fast Fourier Transform support, requires libfftw3
#   gdml               GDML writer and reader
#   gfal               GFAL support, requires libgfal
#   g4root             GEANT4 navigator based on TGeo, requires GEANT4 installed
#   globus             Globus authentication support, requires Globus toolkit
#   krb5               Kerberos5 support, requires Kerberos libs

set (root_options "")

include (${cmake_scripts}/FindFFTW3.cmake)

if (HAVE_FFTW3)
  set (root_options " -with-fftw3")
endif (HAVE_FFTW3)

if (HAVE_ROOT_SOURCE_DIR)
  message (STATUS "Configuring source package for installation at ${root_prefix} ...")
  execute_process (
    WORKING_DIRECTORY ${HAVE_ROOT_SOURCE_DIR}
    COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} ${root_options}
    TIMEOUT 180
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

