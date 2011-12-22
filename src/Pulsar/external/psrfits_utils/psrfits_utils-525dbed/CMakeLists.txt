## ==============================================================================
##
##                                                         CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (psrfits)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##                                                              System inspection
##
## ==============================================================================

if (NOT CFITSIO_FOUND)
  find_package (CFITSIO)
endif (NOT CFITSIO_FOUND)

## ==============================================================================
##
##                                                             Build instructions
##
## ==============================================================================

if (CFITSIO_FOUND)
  
  ## Include directories
  include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CFITSIO_INCLUDES}
    )

  add_definitions (
    -DPSRFITS_TEMPLATE_DIR='${CMAKE_INSTALL_PREFIX}/share'
    )

  ##________________________________________________________
  ##                                           psrfits_utils
  
  add_library (psrfits_utils
    downsample.c
    fold.c
    misc_utils.c
    polyco.c
    read_psrfits.c
    write_psrfits.c
    )
  target_link_libraries (psrfits_utils
    ${CFITSIO_LIBRARIES}
    )
  
  ##________________________________________________________
  ##                                         psrfits_subband
  
  ## Compiler instructions
  add_executable (psrfits_subband
    psrfits_subband.c
    psrfits_subband_cmd.h
    psrfits_subband_cmd.c
    )
  ## Linker instructions
  target_link_libraries (psrfits_subband
    psrfits_utils
    )

  ##________________________________________________________
  ##                                            fold_psrfits
  
  ## Compiler instructions
  add_executable (fold_psrfits
    fold_psrfits.c)
  ## Linker instructions
  target_link_libraries (fold_psrfits
    psrfits_utils
    )
  
  ##________________________________________________________
  ##                                     psrfits_singlepulse
  
  ## Compiler instructions
  add_executable (psrfits_singlepulse
    psrfits_singlepulse.c)
  ## Linker instructions
  target_link_libraries (psrfits_singlepulse
    psrfits_utils
    )
  
  ##________________________________________________________
  ##                               Installation instructions

  install (TARGETS psrfits_singlepulse fold_psrfits psrfits_subband psrfits_utils
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

  install (
    FILES fold.h polyco.h polyco_struct.h psrfits.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/psrfits_utils
    )
  
  install (
    FILES guppi_PSRFITS_v3.4_fold_template.txt guppi_PSRFITS_v3.4_search_template.txt
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share
    )
  
endif (CFITSIO_FOUND)
