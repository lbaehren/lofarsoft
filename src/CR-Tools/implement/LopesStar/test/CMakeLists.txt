##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 567 2007-08-16 21:09:23Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
#  root2csv
  timestamp_compare
  timestamp_parse
  chplot4root
  TreeConvert
  starTrace
  RunLoop
  OptWindowSize
  MC_timestamp
  )

if (HAVE_PostgreSQL)
  list (APPEND tests CalibrationD30)
endif (HAVE_PostgreSQL)

## -----------------------------------------------------------------------------
## Build the test programs

if (CR_PYTHON_SUPPORT)
  foreach (src ${tests})
    ## build executable
    add_executable (${src} ${src}.cc)
    target_link_libraries (${src}
      cr
      ${FFTW3_LIBRARIES}
      ${ROOT_LIBRARIES}
      ${PYTHON_LIBRARIES}
      )
    ## testing
    add_test (${src} ${src})
  endforeach (src)
endif (CR_PYTHON_SUPPORT)

## -----------------------------------------------------------------------------
## Installation

if (CR_PYTHON_SUPPORT)
  install (
    TARGETS ${tests}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
endif (CR_PYTHON_SUPPORT)
