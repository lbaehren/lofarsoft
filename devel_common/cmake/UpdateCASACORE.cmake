##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## =============================================================================
## Update 
## =============================================================================

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  /opt
)

set (casacore_libs
  casa
  tables
  mirlib
  scimath
  measures
  fits
  coordinates
  components
  lattices
  ms
  images
  msfits
  msvis
)

set (casacore_url "http://casacore.googlecode.com/svn/trunk")

## =============================================================================
##  
##  CMake script to update the source code distribution of the "casacore"
##  packages, as provided as part of the LOFAR user software, with the latest
##  source code retrieved from the original code repository.
## 
## =============================================================================

## -----------------------------------------------------------------------------
## [1] Find the location of the casacore source code distribution, which is part
## of the LOFAR User software; this is what we are going to update.
 
find_path (AIPS_H aips_h aipsdef.h aipsenv.h
  PATHS
  ./casacore
  ../casacore
  ../../casacore
  ./external/casacore
  ../../external/casacore
  PATH_SUFFIXES casa/casa
)

if (AIPS_H)
  string (REGEX REPLACE casa/casa "" CASACORE_USG ${AIPS_H})
  message (STATUS "Found USG distribution of casacore: ${CASACORE_USG}")
elseif (AIPS_H)
  message (FATAL_ERROR "Unable to find USG distribution of casacore!")
endif (AIPS_H)

## -----------------------------------------------------------------------------
## [2] Check for the required system tools

find_program (SVN_EXECUTABLE svn PATHS ${bin_locations})
find_program (TAR_EXECUTABLE tar PATHS ${bin_locations})

## -----------------------------------------------------------------------------
## Collect CMake configuration files and Subversion tags into a tar-archive

# tar -cvzf casacore.tgz `find casacore -name .svn` `find casacore -name CMakeLists.txt`

if (TAR_EXECUTABLE)
elseif (TAR_EXECUTABLE)
  message (FATAL_ERROR "Unable to find tar executable on your system!")
endif (TAR_EXECUTABLE)

## -----------------------------------------------------------------------------
## [3] Check out the lastest version of the original code base

if (SVN_EXECUTABLE)
  execute_process (
    WORKING_DIRECTORY .
    COMMAND ${SVN_EXECUTABLE} co ${casacore_url} casacore
    TIMEOUT 300
    ERROR_VARIABLE svn_cerr
    OUTPUT_QUIET
  )
  if (svn_cerr)
    message (STATUS "[svn] Unable to check out the code from the repository!")
    message (STATUS "${svn_cerr}")
  endif (svn_cerr)
elseif (SVN_EXECUTABLE)
  message (FATAL_ERROR "Unable to out the code from the repository without SVN!")
endif (SVN_EXECUTABLE)
