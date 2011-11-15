
##____________________________________________________________________
##                                                    Search locations

foreach (_pathBase
    /app/usg
    /opt/casa/local
    /usr/X11R6
    /usr/local
    /usr
    /sw
    /opt/local
    /opt
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
