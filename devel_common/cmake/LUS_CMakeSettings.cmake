
if (NOT LUS_CMAKE_SETTINGS_CMAKE)
  
  set (LUS_CMAKE_SETTINGS_CMAKE TRUE)

  ## ============================================================================
  ## 
  ##                                                          Installation prefix
  ##
  ##  By default installation is performed into ${LUS_SOURCE_DIR}/release. 
  ##  However LUS_INSTALL_PREFIX can be used to over-ride the default and have
  ##  the software installed into a location of choice ; keep in mind though,
  ##  that special privileges might be required.
  ##
  ## ============================================================================
  
  if (LUS_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX ${LUS_INSTALL_PREFIX} CACHE PATH
      "CMake installation area" 
      FORCE
      )
  else (LUS_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR} CACHE PATH
      "CMake installation area" 
      FORCE
      )
    set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR} CACHE PATH
      "CMake installation area" 
      FORCE
      )
  endif (LUS_INSTALL_PREFIX)
  
  ## ============================================================================
  ##
  ##                                                             Search locations
  ##
  ## ============================================================================
  
  foreach (_pathBase
      /app/usg
      /opt/casa/local
      /usr/X11R6
      /usr/local
      /usr
      /sw
      /opt/local
      /opt
      /opt/cep
      ${CMAKE_INSTALL_PREFIX}
      )
    
    ## Locations to search for header files
    list (INSERT CMAKE_INCLUDE_PATH 0 ${_pathBase}/include )
    
    ## Locations to search for libraries
    list (INSERT CMAKE_LIBRARY_PATH 0 ${_pathBase}         )
    list (INSERT CMAKE_LIBRARY_PATH 0 ${_pathBase}/lib     )
    
    ## Locations to search for program executables
    list (INSERT CMAKE_PROGRAM_PATH 0 ${_pathBase}/bin     )
    
  endforeach (_pathBase)
  
  ## ============================================================================
  ##
  ##                                                Macro: Include search modules
  ##
  ##   The macro accepts a list of module names for which a "Find<module>.cmake"
  ##   file exists. Optionally a "verbose" parameter (boolean) can be passed along
  ##   as last argument; it the latter is set to "FALSE", the find script is
  ##   loaded with <module>_FIND_QUIETLY
  ##
  ## ============================================================================
  
  macro (lusFindModules varModules)
  
    ##________________________________________________________________
    ## Check for optional "verbose" parameter; as this can be passed
    ## along as last argument, we first invert the list of arguments
    ## to inspect the then first element in the list. If the first
    ## list element matches possibles value for a boolean variable, it
    ## is stored and the argument removed from the list.
    
    set (_args ${ARGV})
    list (REVERSE _args)
    list (GET _args 0 varFirstArgument)
    
    if (varFirstArgument MATCHES ON+|TRUE+|YES+)
      set (_verbose YES)
      list (REMOVE_AT _args 0)
    endif (varFirstArgument MATCHES ON+|TRUE+|YES+)
    
    if (varFirstArgument MATCHES OFF+|FALSE+|NO+)
      set (_verbose NO)
      list (REMOVE_AT _args 0)
    endif (varFirstArgument MATCHES OFF+|FALSE+|NO+)
    
    ##________________________________________________________________
    ## Once the optional argument has been removed, sort the list of
    ## inputs

    list (SORT _args)
    
  endmacro (lusFindModules)
  
endif (NOT LUS_CMAKE_SETTINGS_CMAKE)
