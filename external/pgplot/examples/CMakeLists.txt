## ------------------------------------------------------------------------------
## PGPLOT demo programs
##
##  When linking, use
##    -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L%p/lib -lpng -lz
##    -laquaterm -L$PGPLOT_DIR/lib -lpgplot

set (examples
  pgdemo1
#  pgdemo2
  pgdemo3
  pgdemo4
  pgdemo5
  pgdemo6
  pgdemo7
  pgdemo8
  pgdemo9
  pgdemo10
  pgdemo11
  )

foreach (src ${examples})
  add_executable (${src} ${src}.f)
  target_link_libraries (${src}
    pgplot
    XmPgplot
    tkpgplot
    ${libg2c}
    ${libgif}
    ${libtcl}
    ${libtk}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${X11_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )
  set_target_properties (${src}
    PROPERTIES LINKER_LANGUAGE Fortran)
endforeach (src)
