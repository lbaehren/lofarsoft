
## -----------------------------------------------------------------------------
##
##  Standard locations where to look for required components
## 
## -----------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## locations in which to look for applications/binaries

set (bin_locations "")

foreach (location ${CMAKE_MODULE_PATH})
  list (APPEND bin_locations ${location}/../../release/bin)
endforeach (location)

list (APPEND bin_locations /opt/bin)
list (APPEND bin_locations /opt/local/bin)
list (APPEND bin_locations /sw/bin)
list (APPEND bin_locations /dp/bin)
list (APPEND bin_locations /usr/bin)
list (APPEND bin_locations /usr/local/bin)
list (APPEND bin_locations /usr/X11R6/bin)
list (APPEND bin_locations /sw/lib/gcc4.2/bin)

## -----------------------------------------------------------------------------
## locations in which to look for header files

set (include_locations "")

foreach (location ${CMAKE_MODULE_PATH})
  list (APPEND include_locations ${location}/../../release/include)
endforeach (location)

list (APPEND include_locations /opt/include)
list (APPEND include_locations /opt/local/include)
list (APPEND include_locations /sw/include)
list (APPEND include_locations /dp/include)
list (APPEND include_locations /usr/include)
list (APPEND include_locations /usr/local/include)
list (APPEND include_locations /usr/X11R6/include)
list (APPEND include_locations /opt/aips++/local/include)
list (APPEND include_locations /opt/casa/local/include)
list (APPEND include_locations /sw/lib/gcc4.2/include)
list (APPEND include_locations /opt/gcc-4.2.1/include/c++/4.2.1)

## -----------------------------------------------------------------------------
## locations in which to look for libraries

set (lib_locations "")

foreach (location ${CMAKE_MODULE_PATH})
  list (APPEND lib_locations ${location}/../../release/lib)
endforeach (location)

list (APPEND lib_locations /opt/lib)
list (APPEND lib_locations /opt/local/lib)
list (APPEND lib_locations /sw/lib)
list (APPEND lib_locations /dp/lib)
list (APPEND lib_locations /usr/lib)
list (APPEND lib_locations /usr/local/lib)
list (APPEND lib_locations /usr/X11R6/lib)
list (APPEND lib_locations /opt/aips++/local/lib)
list (APPEND lib_locations /sw/lib/gcc4.2/lib)
list (APPEND lib_locations /opt/gcc-4.2.1/lib)
list (APPEND lib_locations /Developer/SDKs/MacOSX10.4u.sdk/usr/lib)
