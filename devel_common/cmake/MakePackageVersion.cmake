# +-----------------------------------------------------------------------------+
# | $Id::                                                                     $ |
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
## Macro definition: make_package_version
##
## -----------------
##
## Input parameters:
##
##  - MODULE_PATH       = Path to the module for which the package version
##                        information is to be processed.
##  - CREATE_VERSION_H  = Create file Package_Version.h
##  - CREATE_VERSION_CC = Create file Package_Version.cc
##
## -----------------
##
## Pending issues:
##
##  - The package version files should not be written into the source tree, but
##    rather into the location defined by PROJECT_BINARY_DIR
##
## ==============================================================================

MACRO (make_package_version MODULE_PATH CREATE_VERSION_H CREATE_VERSION_CC)

  ## -----------------------------------------------------------------
  ## Retrieve SVN information

  execute_process (
    WORKING_DIRECTORY ${MODULE_PATH}
    COMMAND svn info
    TIMEOUT 300
    RESULT_VARIABLE svn_info_result
    OUTPUT_VARIABLE svn_info_output
    ERROR_VARIABLE svn_info_error
    )

  ## -----------------------------------------------------------------
  ## Extract the name of the module

  string (REGEX REPLACE ".*/" "" MODULE_NAME ${MODULE_PATH})

  ## -----------------------------------------------------------------
  ## Parse the boolean parameters; since their values are handed over
  ## as strings and not as booleans we should make sure they are
  ## converted to the latter.
  
  if (${CREATE_VERSION_H} MATCHES "TRUE")
    set (CREATE_VERSION_H TRUE)
  else (${CREATE_VERSION_H} MATCHES "TRUE")
    set (CREATE_VERSION_H FALSE)
    if (${CREATE_VERSION_H} MATCHES "YES")
      set (CREATE_VERSION_H TRUE)
    else (${CREATE_VERSION_H} MATCHES "YES")
      set (CREATE_VERSION_H FALSE)
    endif (${CREATE_VERSION_H} MATCHES "YES")
  endif (${CREATE_VERSION_H} MATCHES "TRUE")
  
  if (${CREATE_VERSION_CC} MATCHES "TRUE")
    set (CREATE_VERSION_CC TRUE)
  else (${CREATE_VERSION_CC} MATCHES "TRUE")
    set (CREATE_VERSION_CC FALSE)
    if (${CREATE_VERSION_CC} MATCHES "YES")
      set (CREATE_VERSION_CC TRUE)
    else (${CREATE_VERSION_CC} MATCHES "YES")
      set (CREATE_VERSION_CC FALSE)
    endif (${CREATE_VERSION_CC} MATCHES "YES")
  endif (${CREATE_VERSION_CC} MATCHES "TRUE")
  
  ## -----------------------------------------------------------------
  ## Extract the revision number

  string (REGEX MATCH "Revision: .*Node" svn_info_revision ${svn_info_output})
  string (REGEX REPLACE "\nNode" "" svn_info_revision ${svn_info_revision})
  string (REGEX REPLACE "Revision: " "" svn_info_revision ${svn_info_revision})

  ## -----------------------------------------------------------------
  ## Extract author of last changes

  string (REGEX MATCH "Author: .*Last Changed Rev" svn_info_author ${svn_info_output})
  string (REGEX REPLACE "\nLast Changed Rev" "" svn_info_author ${svn_info_author})
  string (REGEX REPLACE "Author: " "" svn_info_author ${svn_info_author})

  ## -----------------------------------------------------------------
  ## Extract the revision of the last changes

  string (REGEX MATCH "Changed Rev: .*Last Changed Date" svn_info_rev ${svn_info_output})
  string (REGEX REPLACE "\nLast Changed Date" "" svn_info_rev ${svn_info_rev})
  string (REGEX REPLACE "Changed Rev: " "" svn_info_rev ${svn_info_rev})

  ## -----------------------------------------------------------------
  ## Create Package_Version.h

  if (CREATE_VERSION_H)
    # variables to be inserted into the configuration file
    set (VERSION_H_NAME "${MODULE_PATH}/Package__Version.h")
    string (TOUPPER "LOFAR_${MODULE_NAME}_PACKAGE__VERSION_H" VERSION_H_GUARD)
    # create file
    configure_file (
      ${CMAKE_MODULE_PATH}/Package__Version.h.in
      ${MODULE_PATH}/Package__Version.h
      )
  endif (CREATE_VERSION_H)
  
  ## -----------------------------------------------------------------
  ## Create Package_Version.cc
  
  if (CREATE_VERSION_CC)
    # create file
    configure_file (
      ${CMAKE_MODULE_PATH}/Package__Version.cc.in
      ${MODULE_PATH}/Package__Version.cc
      )
  endif (CREATE_VERSION_CC)
  
  ## -----------------------------------------------------------------
  ## Summary 

  message (STATUS "[make_package_version]")
  message (STATUS "MODULE_PATH         = ${MODULE_PATH}")
  message (STATUS "MODULE_NAME         = ${MODULE_NAME}")
  message (STATUS "CREATE_VERSION_H    = ${CREATE_VERSION_H}")
  message (STATUS "CREATE_VERSION_CC   = ${CREATE_VERSION_CC}")

  if (svn_info_result) 
    message (STATUS "Result              = ${svn_info_result}")
    message (STATUS "Output              = ${svn_info_output}")
    message (STATUS "Error               = ${svn_info_error}")
  else (svn_info_result) 
    message (STATUS "Revision            = ${svn_info_revision}")
    message (STATUS "Last Changed Author = ${svn_info_author}")
    message (STATUS "Last Changed Rev    = ${svn_info_rev}")
  endif (svn_info_result)
  
ENDMACRO (make_package_version)