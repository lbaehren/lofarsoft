
## ==============================================================================
##
##  Check for the presence and value of the LD_LIBRARY_PATH variable
##
## ==============================================================================

set (PULSAR_LD_LIBRARY_PATH "")

##____________________________________________________________________
## Step 1: Is the LD_LIBRARY_PATH defined via an environment variable?

find_path (PULSAR_LD_LIBRARY_PATH_ENV
  NAMES libdal.so
  $ENV{LD_LIBRARY_PATH}
  )

if (PULSAR_LD_LIBRARY_PATH_ENV)
  list (APPEND PULSAR_LD_LIBRARY_PATH ${PULSAR_LD_LIBRARY_PATH_ENV})
endif (PULSAR_LD_LIBRARY_PATH_ENV)

##____________________________________________________________________
## Step 2: LD_LIBRARY_PATH extension for LUS Python modules

find_path (PULSAR_LD_LIBRARY_PATH_LUS
  NAMES
  PATHS
  ${CMAKE_INSTALL_PREFIX}
  ${CMAKE_INSTALL_PREFIX}/..
  ${CMAKE_INSTALL_PREFIX}/../..
  ${CMAKE_INSTALL_PREFIX}/../../lib
  PATH_SUFFIXES
  lib
  share/pulsar/lib
  )

if (NOT PULSAR_LD_LIBRARY_PATH_LUS)
  ## Reconstruct addition to LD path from LUS installation prefix
  if (LUS_INSTALL_PREFIX)
    set (PULSAR_LD_LIBRARY_PATH_LUS "")
    string(REPLACE "share/pulsar" "lib" PULSAR_LD_LIBRARY_PATH_LUS ${LUS_INSTALL_PREFIX})
  endif (LUS_INSTALL_PREFIX)
endif (NOT PULSAR_LD_LIBRARY_PATH_LUS)

if (PULSAR_LD_LIBRARY_PATH_LUS)
  list (APPEND PULSAR_LD_LIBRARY_PATH ${PULSAR_LD_LIBRARY_PATH_LUS})
endif (PULSAR_LD_LIBRARY_PATH_LUS)

##____________________________________________________________________
## Step 3: Add Pulsar-Tools specific extensions to LD_LIBRARY_PATH

list (APPEND PULSAR_LD_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
list (APPEND PULSAR_LD_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/../../lib)

##____________________________________________________________________
## Clean up the list: remove duplicate entries and export back to
## environment variable.

list (REMOVE_DUPLICATES PULSAR_LD_LIBRARY_PATH)

message (STATUS "A2    .. LD_LIBRARY_PATH           = ${PULSAR_LD_LIBRARY_PATH}"      )
message (STATUS "A2    .. ENV:LD_LIBRARY_PATH       = $ENV{LD_LIBRARY_PATH}"          )

set( ENV{LD_LIBRARY_PATH} ${PULSAR_LD_LIBRARY_PATH} )

message (STATUS "A2    .. LD_LIBRARY_PATH           = ${PULSAR_LD_LIBRARY_PATH}"      )
message (STATUS "A2    .. ENV:LD_LIBRARY_PATH       = $ENV{LD_LIBRARY_PATH}"          )

