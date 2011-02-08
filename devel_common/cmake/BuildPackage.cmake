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

  ## Locate common options

  find_file (LUS_CMAKE_OPTIONS CMakeOptions.cmake
    PATHS ${LUS_BUILD_DIR}
    )

  ## get components of the path to the package's source code
  get_filename_component (_packageLocation ${_packageSource} PATH)

  ## feedback
  if (${_packageLocation} STREQUAL "external")
    message (STATUS "Adding external package ${_packageName} ...")
    set (_packageBuildLocation ${LUS_BUILD_DIR}/external/${_packageName})
  else (${_packageLocation} STREQUAL "external")
    set (_packageBuildLocation ${LUS_BUILD_DIR}/${_packageName})
  endif (${_packageLocation} STREQUAL "external")

  ## create the directory within which the build is performed
  file (MAKE_DIRECTORY ${_packageBuildLocation})

  if (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_ROOT}/${_packageSource} -C${LUS_CMAKE_OPTIONS} -DCMAKE_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND make install
      WORKING_DIRECTORY ${_packageBuildLocation}
      COMMENT "Building package ${package_name} ..."
      )
  else (LUS_CMAKE_OPTIONS)
    add_custom_target (${_packageName}
      COMMAND ${CMAKE_COMMAND} ${LUS_ROOT}/${_packageSource} -DCMAKE_INSTALL_PREFIX=${LUS_INSTALL_PREFIX}
      COMMAND make install
      WORKING_DIRECTORY ${_packageBuildLocation}
      COMMENT "Building package ${package_name} ..."
      )
  endif (LUS_CMAKE_OPTIONS)
  
endmacro (build_package _packageName _packageSource)

