## ------------------------------------------------------------------------------
## PGPLOT demo programs
##
##  When linking, use
##    -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L%p/lib -lpng -lz
##    -laquaterm -L$PGPLOT_DIR/lib -lpgplot

set (pgplot_examples
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
  pgdemo12
  pgdemo13
  pgdemo14
  pgdemo15
  pgdemo16
  pgdemo17
  )

foreach (src ${pgplot_examples})
  ## compile
  add_executable (${src} ${src}.f)
  ## link
  target_link_libraries (${src}
    pgplot
    XmPgplot
    tkpgplot
    ${G2C_LIBRARIES}
    ${GIF_LIBRARIES}
    ${TCL_LIBRARIES}
    ${TK_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${X11_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )
  set_target_properties (${src}
    PROPERTIES LINKER_LANGUAGE Fortran)
  ## testing
  add_test (${src} ${src})
endforeach (src)
