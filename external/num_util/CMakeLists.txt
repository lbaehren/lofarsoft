
include (ExternalProject)

## ==============================================================================
##
##                                                   $LOFARSOFT/external/num_util
##
## ==============================================================================

if (NOT NUM_UTIL_FOUND)
  find_package (NumUtil)
endif (NOT NUM_UTIL_FOUND)

if (NOT NUM_UTIL_FOUND)

  find_package (NumPy)
  ## Check for Boost++
  if (NOT BOOST_FOUND)
    find_package (Boost)
  endif (NOT BOOST_FOUND)
  ## Check for Python
  if (NOT PYTHON_FOUND)
    find_package (Python)
  endif (NOT PYTHON_FOUND)

  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/num_util.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/num_util.cmake
    )
  
  if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    ExternalProject_Add (num_util
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
      URL ${CMAKE_CURRENT_SOURCE_DIR}/src
      CMAKE_ARGS -C${CMAKE_CURRENT_BINARY_DIR}/num_util.cmake
      )
  else (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    ## Check out code from the repository
    ExternalProject_Add (num_util
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src
      SVN_REPOSITORY http://usg.lofar.org/svn/code/trunk/external/num_util/src
      CMAKE_ARGS -C${CMAKE_CURRENT_BINARY_DIR}/num_util.cmake
      )
  endif (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
  
else (NOT NUM_UTIL_FOUND)

  add_custom_target (num_util
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[num_util] Found system-wide installation; skipping rebuild!"
    )

endif (NOT NUM_UTIL_FOUND)
