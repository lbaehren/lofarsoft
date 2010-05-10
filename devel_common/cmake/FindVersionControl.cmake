# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
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
#  - GIT_EXECUTABLE [out]
#       Path to the git executable.
#  - SVN_EXECUTABLE [out]
#       Path to the SVN executable.
#  - VERSION_CONTROL_GIT    [out]
#  - VERSION_CONTROL_SVN    [out]
#  - VERSION_CONTROL_SYSTEM [out]
#       Name of the version control system used for the local working copy.
#       Returned value will be "git" or "svn".
#

##_______________________________________________________________________________
##  Include common setting and definitions

include (CMakeSettings)

##_______________________________________________________________________________
##  Check for git

find_program (GIT_EXECUTABLE git PATHS ${bin_locations})

find_path (VERSION_CONTROL_DIR_GIT description hooks index
  PATH .git
  )

##_______________________________________________________________________________
##  Check for Subversion

find_program (SVN_EXECUTABLE svn PATHS ${bin_locations})

find_path (VERSION_CONTROL_DIR_SVN entries
  PATH .svn
  )

##_______________________________________________________________________________
##  Set up VERSION_CONTROL_SYSTEM variable

set (VERSION_CONTROL_SYSTEM "")

if (GIT_EXECUTABLE AND VERSION_CONTROL_DIR_GIT)
  set (VERSION_CONTROL_SYSTEM "git")
else (GIT_EXECUTABLE AND VERSION_CONTROL_DIR_GIT)
  if (SVN_EXECUTABLE AND VERSION_CONTROL_DIR_SVN)
    set (VERSION_CONTROL_SYSTEM "svn")
  endif (SVN_EXECUTABLE AND VERSION_CONTROL_DIR_SVN)
endif (GIT_EXECUTABLE AND VERSION_CONTROL_DIR_GIT)

##  Feedback

if (NOT VERSION_CONTROL_FIND_QUIETLY)
  message (STATUS "VERSION_CONTROL_SYSTEM  = ${VERSION_CONTROL_SYSTEM}")
  message (STATUS "GIT_EXECUTABLE          = ${GIT_EXECUTABLE}")
  message (STATUS "VERSION_CONTROL_DIR_GIT = ${VERSION_CONTROL_DIR_GIT}")
  message (STATUS "SVN_EXECUTABLE          = ${SVN_EXECUTABLE}")
  message (STATUS "VERSION_CONTROL_DIR_SVN = ${VERSION_CONTROL_DIR_SVN}")
endif (NOT VERSION_CONTROL_FIND_QUIETLY)