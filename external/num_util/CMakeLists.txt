
include (ExternalProject)

## ==============================================================================
##
##                                                   $LOFARSOFT/external/num_util
##
## ==============================================================================

##__________________________________________________________
## System inspection

if (NOT NUM_UTIL_FOUND)
  find_package (NumUtil)
endif (NOT NUM_UTIL_FOUND)

##__________________________________________________________
## Build instructions

if (NOT NUM_UTIL_FOUND)
  
  if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    ExternalProject_Add (num_util
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
      URL ${CMAKE_CURRENT_SOURCE_DIR}/src
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DLUS_SOURCE_DIR=${LUS_SOURCE_DIR}
      )
  else (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    ## Check out code from the repository
    ExternalProject_Add (num_util
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
      SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/num_util/src
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DLUS_SOURCE_DIR=${LUS_SOURCE_DIR}
      )
  endif (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
  
else (NOT NUM_UTIL_FOUND)

  add_custom_target (num_util
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[num_util] Found system-wide installation; skipping rebuild!"
    )

endif (NOT NUM_UTIL_FOUND)
