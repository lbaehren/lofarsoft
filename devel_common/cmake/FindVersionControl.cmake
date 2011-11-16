# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
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

# DESCRIPTION:
#   Inspect the current working copy to detect which version control system 
#   (e.g. Subversion, git, etc.) is used.
#
# PARAMETERS:
#  - VERSION_CONTROL_FIND_QUIETLY [in]
#       Be quiet during search; of set "TRUE" search will be run in verbose mode.
#  - VERSION_CONTROL_SYSTEM [out]
#       Name of the version control system used for the local working copy.
#       Returned value will be "bzr", "git" or "svn".
#  - BZR_EXECUTABLE [out]
#       Path to the bzr executable.
#  - GIT_EXECUTABLE [out]
#       Path to the git executable.
#  - SVN_EXECUTABLE [out]
#       Path to the SVN executable.
#  - BZR_CONFIG_DIR [out]
#  - GIT_CONFIG_DIR [out]
#  - SVN_CONFIG_DIR [out]
#

##_______________________________________________________________________________
##  Include common setting and definitions

include (CMakeSettings)

##_______________________________________________________________________________
##  Generic search to determine version control system 

foreach (_name bzr git svn)

  string (TOUPPER ${_name} _nameUpper)

  message (STATUS "Checking for version control system ${_name}")

  ## Check for executable
  find_program (${_nameUpper}_EXECUTABLE ${_name} PATHS ${bin_locations})

  ## Check for configuration directory
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.${_name})
    set (${_nameUpper}_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.${_name}) 
  endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.${_name})
  
  ## Feedback
  if (${_nameUpper}_EXECUTABLE AND ${_nameUpper}_CONFIG_DIR)
    message (STATUS "Checking for version control system ${_name} - found.")
  else (${_nameUpper}_EXECUTABLE AND ${_nameUpper}_CONFIG_DIR)
    message (STATUS "Checking for version control system ${_name} - not found.")
  endif (${_nameUpper}_EXECUTABLE AND ${_nameUpper}_CONFIG_DIR)
  
endforeach (_name)

##_______________________________________________________________________________
##  Set up VERSION_CONTROL_SYSTEM variable

set (VERSION_CONTROL_SYSTEM "")

if (BZR_EXECUTABLE AND BZR_CONFIG_DIR) 
  set (VERSION_CONTROL_SYSTEM "bzr")
endif (BZR_EXECUTABLE AND BZR_CONFIG_DIR) 

if (GIT_EXECUTABLE AND GIT_CONFIG_DIR)
  set (VERSION_CONTROL_SYSTEM "git")
else (GIT_EXECUTABLE AND GIT_CONFIG_DIR)
  if (SVN_EXECUTABLE AND SVN_CONFIG_DIR)
    set (VERSION_CONTROL_SYSTEM "svn")
  endif (SVN_EXECUTABLE AND SVN_CONFIG_DIR)
endif (GIT_EXECUTABLE AND GIT_CONFIG_DIR)

##_______________________________________________________________________________
##  Feedback

if (NOT VERSION_CONTROL_FIND_QUIETLY)
  message (STATUS "VERSION_CONTROL_SYSTEM  = ${VERSION_CONTROL_SYSTEM}")
  message (STATUS "GIT_EXECUTABLE          = ${GIT_EXECUTABLE}")
  message (STATUS "GIT_CONFIG_DIR          = ${GIT_CONFIG_DIR}")
  message (STATUS "SVN_EXECUTABLE          = ${SVN_EXECUTABLE}")
  message (STATUS "SVN_CONFIG_DIR          = ${SVN_CONFIG_DIR}")
endif (NOT VERSION_CONTROL_FIND_QUIETLY)
