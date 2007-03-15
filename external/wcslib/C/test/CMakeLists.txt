
## ------------------------------------------------------------------------------
## The test programs for the WCSLIB

set (test_sources
  tcel1
  tcel2
  tfitshdr
  tlin
  tlog
  tofits
  tpih1
  tpih2
  tprj1
  tprj2
  tspc
  tsph
  tspx
  ttab1
  ttab2
  ttab3
  tunits
  twcs
  twcsfix
  twcshdr
  twcsmix
  twcssub
  twcstab
  )

## PGPLOTLIB := -lcpgplot -lpgplot -lpng -lz -lX11 -lg2c 

if (HAVE_PGPLOT)
  foreach (test_source ${test_sources})
    add_executable (${test_source} ${test_source}.c)
    target_link_libraries (${test_source}
      pgsbox
      wcs
      ${PGPLOT_LIBRARIES}
      ${PNG_LIBRARIES}
      ${libz}
      ${MOTIF_LIBRARIES}
      ${G2C_LIBRARIES}
      )
  endforeach (test_source)
else (HAVE_PGPLOT)
  message (STATUS "Unable to build WCSLIB test programs!")
  message (STATUS " wcslib    = ${wcs}")
  message (STATUS " libpgsbox = ${pgsplot}")
endif (HAVE_PGPLOT)
