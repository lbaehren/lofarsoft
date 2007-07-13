PROJECT (LUS)

SUBDIRS (
  BDSM
  CR-Pipeline
  DAL
#  LOPES-Tools
  VLT
  )

## ------------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${CR_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for USG software packages.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)
