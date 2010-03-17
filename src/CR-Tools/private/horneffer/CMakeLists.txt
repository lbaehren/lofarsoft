
## -------------------------------------------------------------------
## Build instructions

if (HAVE_CASACORE AND HAVE_DAL AND DAL_LIBRARIES AND HDF5_LIBRARIES)

  foreach (src ${cr_horneffer_sources})
    
    ## get the name of the file without the extension
    get_filename_component (extension ${src} EXT)
    get_filename_component (src ${src} NAME_WE)
    
    ## compiler instructions
    add_executable(${src} ${src}${extension})
    
    ## linker instructions
    target_link_libraries (${src} 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    
    ## installation instructions
    install (TARGETS ${src}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      )
    
  endforeach (src)
  
endif (HAVE_CASACORE AND HAVE_DAL AND DAL_LIBRARIES AND HDF5_LIBRARIES)

