##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tMasking
  tProgressBar
  tStringTools
  tTestsCommon
  )

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr ${cr_link_libraries})
  ## testing
  add_test (${src} ${src})
endforeach (src)

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

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
