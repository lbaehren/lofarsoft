
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
  
endif (NOT LUS_CMAKE_SETTINGS_CMAKE)
