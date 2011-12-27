
## ==============================================================================
##
##  Check for the presence and value of the PYTHONPATH variable
##
## ==============================================================================

set (PULSAR_PYTHONPATH "")

##____________________________________________________________________
## Step 1: Is the PYTHONPATH defined via an environment variable?

find_path (PULSAR_PYTHONPATH_ENV
  NAMES pylab.py
  $ENV{PYTHONPATH}
  )

if (PULSAR_PYTHONPATH_ENV)
  list (APPEND PULSAR_PYTHONPATH ${PULSAR_PYTHONPATH_ENV})
endif (PULSAR_PYTHONPATH_ENV)

##____________________________________________________________________
## Step 2: PYTHONPATH extension for LUS Python modules

find_path (PULSAR_PYTHONPATH_LUS
  NAMES
  pylab.py
  numpu/matlib.py
  PATHS 
  ${CMAKE_INSTALL_PREFIX}
  ${CMAKE_INSTALL_PREFIX}/..
  ${CMAKE_INSTALL_PREFIX}/../..
  PATH_SUFFIXES
  lib
  lib/python
  share/pulsar/lib
  )

if (PULSAR_PYTHONPATH_LUS)
  list (APPEND PULSAR_PYTHONPATH ${PULSAR_PYTHONPATH_LUS})
endif (PULSAR_PYTHONPATH_LUS)

##____________________________________________________________________
## Step 3: Add Pulsar-Tools specific extensions to PYTHONPATH

list (APPEND PULSAR_PYTHONPATH ${CMAKE_INSTALL_PREFIX}/bin)
list (APPEND PULSAR_PYTHONPATH ${CMAKE_INSTALL_PREFIX}/lib/python)

##____________________________________________________________________
## Clean up the list: remove duplicate entries and export back to
## environment variable.

list (REMOVE_DUPLICATES PULSAR_PYTHONPATH)

set( ENV{PYTHONPATH} ${PULSAR_PYTHONPATH} )
