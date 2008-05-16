
## ==============================================================================
##
##  Library: libwcs
##
## ==============================================================================

set (libwcs_sources
  ## Core WCSLIB sources
  cel.c
  lin.c
  sph.c
  ## Extensions
  imsetwcs.c imgetwcs.c matchstar.c findstar.c daoread.c wcscon.c
  fitswcs.c wcsinit.c wcs.c ty2read.c webread.c tmcread.c
  gscread.c gsc2read.c ujcread.c uacread.c ubcread.c ucacread.c
  sdssread.c tabread.c binread.c ctgread.c actread.c catutil.c
  skybotread.c imrotate.c fitsfile.c imhfile.c
  hget.c hput.c imio.c dateutil.c imutil.c
  worldpos.c tnxpos.c dsspos.c platepos.c
  sortstar.c platefit.c iget.c fileutil.c
  wcslib.c proj.c wcstrig.c distort.c
  )

include_directories (${WCSTOOLS_SOURCE_DIR}/libwcs)

## ------------------------------------------------------------------------------
## Dynamic library (also to be usable as module from Python)

add_library (wcstools MODULE ${libwcs_sources})

if (APPLE)
  set_target_properties (wcstools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS "-DPYTHON"
    LINK_FLAGS "-fPIC -flat_namespace"
    )
else (APPLE)
  set_target_properties (wcstools
    PROPERTIES
    PREFIX ""
    SUFFIX .so
    COMPILE_FLAGS -DPYTHON
    LINK_FLAGS "-fPIC -shared"
    )
endif (APPLE)

install (TARGETS wcstools
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ------------------------------------------------------------------------------
## Additional library (static) required for building applications

if (WCSTOOLS_BUILD_TOOLS)
  ## compiler instructions
  add_library (wcs ${libwcs_sources})
  ## installation instructions
  install (TARGETS wcs
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (WCSTOOLS_BUILD_TOOLS)

