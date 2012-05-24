
## ==============================================================================
##
##  Check for the presence and value of the LD_LIBRARY_PATH variable
##
## ==============================================================================

set (PULSAR_LD_LIBRARY_PATH "")

##____________________________________________________________________
## Step 3: Add Pulsar-Tools specific extensions to LD_LIBRARY_PATH

list (APPEND PULSAR_LD_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
#list (APPEND PULSAR_LD_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/../../lib)

##____________________________________________________________________
## Clean up the list: remove duplicate entries and export back to
## environment variable.

list (REMOVE_DUPLICATES PULSAR_LD_LIBRARY_PATH)

message (STATUS "A2    .. LD_LIBRARY_PATH           = ${PULSAR_LD_LIBRARY_PATH}"      )
message (STATUS "A2    .. ENV:LD_LIBRARY_PATH       = $ENV{LD_LIBRARY_PATH}"          )

set( ENV{LD_LIBRARY_PATH} ${PULSAR_LD_LIBRARY_PATH} )

message (STATUS "A2    .. LD_LIBRARY_PATH           = ${PULSAR_LD_LIBRARY_PATH}"      )
message (STATUS "A2    .. ENV:LD_LIBRARY_PATH       = $ENV{LD_LIBRARY_PATH}"          )

