
## ==============================================================================
##
##  Library: libned
##
## ==============================================================================

set (libned_sources 
  ned_cli.c
  ned_cif.c
  ned_err.c
  ned_sk.c
  )

include_directories (
  ${WCSTOOLS_SOURCE_DIR}
  ${WCSTOOLS_SOURCE_DIR}/libned
  ${WCSTOOLS_SOURCE_DIR}/libwcs
  )

add_library (ned ${libned_sources})

install (TARGETS ned
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Applications
##
## ==============================================================================

if (WCSTOOLS_BUILD_TOOLS)
  
  set (nedtools
    nedname
    #  nedpos
    name_resolver
    byname
    nearname
    nearposn
    iau
    ex_refcode
    ref
    )
  
  foreach (app ${nedtools})
    ## compiler instructions
    add_executable (${app} ${app}.c)
    ## linker instructions
    target_link_libraries (${app} wcs ned ${link_libraries})
    ## installation instructions
    install (TARGETS ${app}
      RUNTIME DESTINATION bin
      )
  endforeach (app)
  
endif (WCSTOOLS_BUILD_TOOLS)
