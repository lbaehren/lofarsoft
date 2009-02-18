# +-----------------------------------------------------------------------------+
# | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                               $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

## ==============================================================================
##
## Macro definition: get_casacore_data
##
## -----------------
##
## Input parameters:
##
##  - none so far
##
## ==============================================================================

##____________________________________________________________________
## Connect to configuration framework

if (NOT USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (NOT USG_ROOT)  

find_program (HAVE_WGET  wget  ${bin_locations} )
find_program (HAVE_CURL  curl  ${bin_locations} )
find_program (HAVE_TAR   tar   ${bin_locations} )

set (MEASURES_DATA_TARFILE "casacore-data.tgz")

##____________________________________________________________________
## Retrieve tar-archive of the measures data from the USG server

if (HAVE_WGET)
  add_custom_target (measures_download
    COMMAND wget -c ${USG_DOWNLOAD}/software/${MEASURES_DATA_TARFILE}
    WORKING_DIRECTORY ${USG_ROOT}/data
    COMMENT "Retrieving tar-archive of the measures data from the USG server ..."
    )
else (HAVE_WGET)
  if (HAVE_CURL)
    add_custom_target (measures_download
      COMMAND curl ${USG_DOWNLOAD}/software/${MEASURES_DATA_TARFILE}
      WORKING_DIRECTORY ${USG_ROOT}/data
      COMMENT "Retrieving tar-archive of the measures data from the USG server ..."
      )
  else (HAVE_CURL)
    message (FATAL_ERROR "No tool found to download tar-archive!")
  endif (HAVE_CURL)
endif (HAVE_WGET)

##____________________________________________________________________
## If the download was successful: expand the tar archive within the
## data directory.

if (HAVE_TAR)
  add_custom_target (measures_unpack
    WORKING_DIRECTORY ${USG_ROOT}/data
    COMMAND tar -xvzf ${USG_ROOT}/data/${MEASURES_DATA_TARFILE}
    COMMENT "Expanding tar-archive of measures data ..."
    )
  add_dependencies (measures_unpack measures_download)
endif (HAVE_TAR)

##____________________________________________________________________
## Summary target, which also also does post-installation cleaning

add_custom_target (measures_install
  WORKING_DIRECTORY ${USG_ROOT}/data
  COMMAND rm -rf ${MEASURES_DATA_TARFILE}
  COMMENT "Post-installation cleanup of the data directory ..."
  )
add_dependencies (measures_install measures_unpack)
