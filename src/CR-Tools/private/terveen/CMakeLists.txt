message (STATUS "[CR-Tools] private/terveen")
## -------------------------------------------------------------------
## Build instructions

if (HAVE_CASACORE AND HAVE_DAL AND DAL_LIBRARIES AND HDF5_LIBRARIES)
  
  foreach (src 
      TBBpartskymapper
      TBBskymapper
      TransientTriggerFromSkymap      
      imagededisperse
      MSDynSpec
      zenittest
      sandertest
      tDynSpec
    )
    
    ## compiler instructions
    add_executable (${src} ${src}.cc)
    
    ## linker instructions
    target_link_libraries (${src} 
      cr
      ${HAVE_LIBZ}
      ${DAL_LIBRARIES}
      ${CASA_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CFITSIO_LIBRARIES}
      )
    
    endforeach (src)
    
endif (HAVE_CASACORE AND HAVE_DAL AND DAL_LIBRARIES AND HDF5_LIBRARIES)
