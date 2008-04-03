
## -----------------------------------------------------------------------------
##
##  Standard locations where to look for required components
## 
## -----------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## locations in which to look for applications/binaries

if (NOT bin_locations)
  set (bin_locations
    ## system-wide installation
    /opt/bin
    /opt/local/bin
    /sw/bin
    /dp/bin
    /usr/bin
    /usr/local/bin
    /usr/X11R6/bin
    /sw/lib/gcc4.2/bin
    )

  foreach (location ${CMAKE_MODULE_PATH})
    list (APPEND bin_locations ${location}/../../release/bin)
  endforeach (location)
endif (NOT bin_locations)

## -----------------------------------------------------------------------------
## locations in which to look for header files

if (NOT include_locations)  
  set (include_locations
    ## system-wide installation
    /opt/include
    /opt/local/include
    /sw/include
    /dp/include
    /usr/include
    /usr/local/include
    /usr/X11R6/include
    /opt/aips++/local/include
    /opt/casa/local/include
    /sw/lib/gcc4.2/include
    /opt/gcc-4.2.1/include/c++/4.2.1
    )
  
  foreach (location ${CMAKE_MODULE_PATH})
    list (APPEND include_locations ${location}/../../release/include)
  endforeach (location)
endif (NOT include_locations)

## -----------------------------------------------------------------------------
## locations in which to look for libraries

if (NOT lib_locations)
  set (lib_locations
    ## system-wide installation
    /opt/lib
    /opt/local/lib
    /sw/lib
    /dp/lib
    /usr/lib
    /usr/local/lib
    /usr/X11R6/lib
    /opt/aips++/local/lib
    /sw/lib/gcc4.2/lib
    /opt/gcc-4.2.1/lib
    /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
    )

  foreach (location ${CMAKE_MODULE_PATH})
    list (APPEND lib_locations ${location}/../../release/lib)
  endforeach (location)
  
endif (NOT lib_locations)
