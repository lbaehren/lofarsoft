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


MACRO (get_casacore_data)

  ## -----------------------------------------------------------------
  ## Check environment

  if (NOT USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (NOT USG_ROOT)  

  ## -----------------------------------------------------------------
  ## Check for required tools

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  ## -----------------------------------------------------------------
  ## Retrieve the tar-archive from the server
  
  if (HAVE_WGET)
    message (STATUS "Retrieving tar-archive from USG server ...")
    execute_process (
      WORKING_DIRECTORY ${USG_ROOT}/data
      COMMAND wget -c ${USG_DOWNLOAD}/software/casacore-data.tgz
      TIMEOUT 600
      ERROR_VARIABLE error_wget
      )
  else (HAVE_WGET)
    if (HAVE_CURL)
      execute_process (
	WORKING_DIRECTORY ${USG_ROOT}/data
	COMMAND curl ${USG_DOWNLOAD}/software/casacore-data.tgz
	TIMEOUT 600
	ERROR_VARIABLE error_curl
	)
    else (HAVE_CURL)
      message (FATAL_ERROR "[ROOT] No tool found to download tar-archive!")
    endif (HAVE_CURL)
  endif (HAVE_WGET)
  
  ## -----------------------------------------------------------------
  ## Expand the retrieved archive into the data directory

  
  
ENDMACRO (get_casacore_data)
