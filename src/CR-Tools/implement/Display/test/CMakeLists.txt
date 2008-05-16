##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## tSimplePlot

if (HAVE_PLPLOT)
  ## compiler instructions
  add_executable (tSimplePlot tSimplePlot.cc)
  ## linker instructions
  target_link_libraries (tSimplePlot cr ${PLPLOT_LIBRARIES} ${X11_LIBRARIES})
  ## testing
  add_test (tSimplePlot tSimplePlot)
  ## installation
  install (
    TARGETS tSimplePlot
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (HAVE_PLPLOT)

