PROJECT (VLT)

SUBDIRS (implement)

## ------------------------------------------------------------------------------
## Installation

## Locate the installation area

find_path (prefix release_area.txt
  PATHS
  ${VLT_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package VLT.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)
