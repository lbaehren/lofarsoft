
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

cmake_minimum_required (VERSION 2.8.3)
cmake_policy (VERSION 2.8)

project (PSRFITS)

##__________________________________________________________
##                                Set up initial CMake cache

configure_file (
  ${PSRFITS_SOURCE_DIR}/psrfits_utils.cmake.in
  ${PSRFITS_BINARY_DIR}/psrfits_utils.cmake
  )

##__________________________________________________________
##                                    Target to handle build

add_custom_target (psrfits_utils
  COMMAND ${CMAKE_COMMAND} -C ${PSRFITS_BINARY_DIR}/psrfits_utils.cmake ${CMAKE_CURRENT_SOURCE_DIR}/psrfits_utils-525dbed
  COMMAND ${CMAKE_BUILD_TOOL} install
  WORKING_DIRECTORY ${PSRFITS_BINARY_DIR}
  COMMENT "Triggering build of project psrfits_utils ..."
  SOURCES ${PROJECT_SOURCE_DIR}/psrfits_utils.cmake.in
  )
