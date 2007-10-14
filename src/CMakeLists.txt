PROJECT (LUS)

SUBDIRS (
  DAL
#  BDSM
#  CR-Tools
#  DSP
  Timba
  VLT
  )

## ------------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${LUS_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for USG software packages.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)
