
## -----------------------------------------------------------------------------
##
##  Standard locations where to look for required components
## 
## -----------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## locations in which to look for applications/binaries

if (NOT bin_locations)
  set (bin_locations
    ## local installation
    ${CMAKE_MODULE_PATH}/../../release/bin
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
endif (NOT bin_locations)

## -----------------------------------------------------------------------------
## locations in which to look for header files

if (NOT include_locations)  
  set (include_locations
    ## local installation
    ${CMAKE_MODULE_PATH}/../../release/include
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
endif (NOT include_locations)

## -----------------------------------------------------------------------------
## locations in which to look for libraries

if (NOT lib_locations)
  set (lib_locations
    ## local installation
    ${CMAKE_MODULE_PATH}/../../release/lib
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
endif (NOT lib_locations)
