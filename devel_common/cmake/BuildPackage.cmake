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
#    Macro to build a package within the LUS code tree.
#

##_______________________________________________________________________________
##  Macro definition

macro (build_package _packageName _packageSource)
  ## get components of the path to the package's source code
  get_filename_component (_packageLocation ${_packageSource} PATH)
  ## feedback
  if (${_packageLocation} STREQUAL "external")
    message (STATUS "Adding external package ${_packageName} ...")
    set (_packageBuildLocation ${USG_BUILD}/external/${_packageName})
  else (${_packageLocation} STREQUAL "external")
    set (_packageBuildLocation ${USG_BUILD}/${_packageName})
  endif (${_packageLocation} STREQUAL "external")
  ## create the directory within which the build is performed
  file (MAKE_DIRECTORY ${_packageBuildLocation})
  add_custom_target (${_packageName}
    COMMAND ${CMAKE_COMMAND} ${USG_ROOT}/${_packageSource}
    COMMAND make install
    WORKING_DIRECTORY ${_packageBuildLocation}
    COMMENT "Building package ${package_name} ..."
    )
endmacro (build_package _packageName _packageSource)

