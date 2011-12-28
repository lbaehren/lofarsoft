
## Name of the project
PROJECT (presto C Fortran)

## =============================================================================
##
##  Project sub-directories and header locations
##
## =============================================================================

##____________________________________________________________________
## Header files

include_directories (
  ${presto_SOURCE_DIR}/include
  )

add_subdirectory (include)

##____________________________________________________________________
## Source directories to be processed

if (FFTW3_FFTW3F_LIBRARY)
  add_subdirectory (src)
else (FFTW3_FFTW3F_LIBRARY)
  message (STATUS "[presto] Missing fftw3f library - skip building library and executables!")
endif (FFTW3_FFTW3F_LIBRARY)

add_subdirectory (bin)
add_subdirectory (lib)
add_subdirectory (clig)
add_subdirectory (python)

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
#add_dependencies (presto_noncmake fftw)


if (PULSAR_BUILD_TESTS)
  add_subdirectory (tests)
endif (PULSAR_BUILD_TESTS)

