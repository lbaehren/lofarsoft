# +-----------------------------------------------------------------------------+
# | $Id:: template_FindXX.cmake 1643 2008-06-14 10:19:20Z baehren             $ |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2010                                                        |
# |   Lars B"ahren <bahren@astron.nl>                                           |
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
#    Macro to load a find module
#
# SYNOPSIS:
#    The macro accepts a list of module names for which a "Find<module>.cmake"
#    file exists. Optionally a "verbose" parameter (boolean) can be passed along
#    as last argument; it the latter is set to "FALSE", the find script is
#    loaded with <module>_FIND_QUIETLY
#

##_______________________________________________________________________________
##  Macro definition

macro (include_find_module _modules)

  ## Check for optional "verbose" parameter; as this can be passed along as last
  ## argument, we first invert the list of arguments to inspect the then first
  ## element in the list. If the first list element matches possibles value for
  ## a boolean variable, it is stored and the argument removed from the list.

  set (_args ${ARGV})
  list (REVERSE _args)
  list (GET _args 0 _firstArgument)

  message (STATUS "[1] Arguments = ${_args}")
  
  if (_firstArgument MATCHES ON+|TRUE+|YES+)
    set (_verbose YES)
    list (REMOVE_AT _args 0)
  endif (_firstArgument MATCHES ON+|TRUE+|YES+)

  if (_firstArgument MATCHES OFF+|FALSE+|NO+)
    set (_verbose NO)
    list (REMOVE_AT _args 0)
  endif (_firstArgument MATCHES OFF+|FALSE+|NO+)

  ## Once the optional argument has been removed, sort the list of inputs
  list (SORT _args)

  message (STATUS "[2] Arguments = ${_args}")

  foreach (_module ${_args})
    
    ## convert module name to upper case ...
    string (TOUPPER ${_module} _var)
    ## ... to initialize HAVE_<MODULE> variable
    set (HAVE_${_var} FALSE)
    
    ## Mute output of the find script
    if (_verbose)
      set (${_var}_FIND_QUIETLY NO )
    else (_verbose)
      set (${_var}_FIND_QUIETLY YES )
    endif (_verbose)
    
    set (_find Find${_module})
    message (STATUS "Loading CMake module ${_find}")
    include (${_find})
    
  endforeach (_module)
  
endmacro (include_find_module _modules)
