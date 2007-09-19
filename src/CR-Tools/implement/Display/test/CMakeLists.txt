##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

include_directories (
  .
  ..
  ../..
  )

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tSimplePlot
  )

if (HAVE_PGPLOT)
  list (APPEND tests tPGPlotterCR)
endif (HAVE_PGPLOT)

## -----------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  ## build executable
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} cr ${PGPLOT_LIBRARIES} ${X11_LIBRARIES})
  ## testing
  add_test (${src} ${src})
endforeach (src)

## -----------------------------------------------------------------------------
## Installation

install (
  TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
