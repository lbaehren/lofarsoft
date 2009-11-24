##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 2987 2009-09-04 12:43:48Z baehren                      $
##------------------------------------------------------------------------------

## Name of the project
PROJECT (presto C Fortran)

## Minimum required version of CMake to configure the project
#cmake_minimum_required (VERSION 2.6)

## =============================================================================
##
##  Project sub-directories and header locations
##
## =============================================================================

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/include)

add_subdirectory (src)
add_subdirectory (bin)
add_subdirectory (lib)
add_subdirectory (clig)
add_subdirectory (python)

add_custom_target (presto_includes
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/include
  COMMAND cp include/* ${CMAKE_INSTALL_PREFIX}/include/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/presto
  COMMENT "Install presto include files from ${PULSAR_SOURCE_DIR}/apps/presto/include into ${CMAKE_INSTALL_PREFIX}/include..."
  )

#configure_file (
#    ${PULSAR_SOURCE_DIR}/apps/presto/src/Makefile.in
#    ${PULSAR_SOURCE_DIR}/apps/presto/src/Makefile
#)

#add_custom_target (presto_noncmake
#  COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/presto/src/slalib ${PULSAR_BINARY_DIR}/apps/presto/src/fortran
#  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/src/*.c ${PULSAR_SOURCE_DIR}/apps/presto/src/*.f ${PULSAR_SOURCE_DIR}/apps/presto/src/Makefile ${PULSAR_BINARY_DIR}/apps/presto/src/
#    COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/src/fortran/* ${PULSAR_BINARY_DIR}/apps/presto/src/fortran/
#    COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/src/slalib/* ${PULSAR_BINARY_DIR}/apps/presto/src/slalib/
#  COMMAND make > ${PULSAR_BINARY_DIR}/apps/presto/src/make.log
#  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/src
#  COMMENT "Issueing command 'make' in ${PULSAR_BINARY_DIR}/apps/presto/src; log file ${PULSAR_BINARY_DIR}/apps/presto/src/make.log"
#  )
#add_dependencies (presto_noncmake presto_includes fftw_install)


if (PULSAR_BUILD_TESTS)
  add_subdirectory (tests)
endif (PULSAR_BUILD_TESTS)

