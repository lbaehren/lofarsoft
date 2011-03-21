
##_______________________________________________________________________________
## Patch for scimath/CMakeLists.txt

if (APPLE)
  
  set_target_properties(casa_scimath
    PROPERTIES 
    COMPILE_FLAGS "-fPIC -O2 -fPIC -Wno-unused"
    LINK_FLAGS "-undefined dynamic_lookup"
    )

endif (APPLE)
