
## ==============================================================================
##
##                                                             Installation setup
##
##  We install into $LOFARSOFT/release/share/pulsar to keep all the components
##  in one place but not have them interfere with possible other installations
##  of the same packages (e.g. FFTW3)
##
## ==============================================================================

##____________________________________________________________________
## Set up the base directory for the installation

if (LUS_SOURCE_DIR)
  ## Set installation prefix for Pulsar software collection
  set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR}/release/share/pulsar
    CACHE PATH
    "CMake installation area" 
    FORCE
    )
  set (LUS_INSTALL_PREFIX ${LUS_SOURCE_DIR}/release/share/pulsar
    CACHE PATH
    "CMake installation area" 
    FORCE
    )
  ## Include guard against overwriting settings
  set (USG_CMAKE_CONFIG TRUE CACHE BOOL "USG configuration set?" FORCE)
else (LUS_SOURCE_DIR)
 message (STATUS "[PULSAR] WARNING: Installation area NOT located for package PULSAR!")
endif (LUS_SOURCE_DIR)

##____________________________________________________________________
##                                          Create directory structure

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
  message (STATUS "[PULSAR] Installation location ${CMAKE_INSTALL_PREFIX} exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
  message (STATUS "[PULSAR] Installation location ${CMAKE_INSTALL_PREFIX} does not exist!")
  message (STATUS "[PULSAR] Creating installation location ${CMAKE_INSTALL_PREFIX}")
  EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX})
  EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin)
  EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib)
  EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/doc)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc")
  message (STATUS "[PULSAR] Documentation Installation location ${CMAKE_INSTALL_PREFIX}/doc exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc")
  message (STATUS "[PULSAR] Documentaion Installation location ${CMAKE_INSTALL_PREFIX}/bin does not exist!")
  message (STATUS "[PULSAR] Creating documentation installation location ${CMAKE_INSTALL_PREFIX}/doc")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/doc)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data")
  message (STATUS "[PULSAR] Documentation Installation location ${CMAKE_INSTALL_PREFIX}/data exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data")
  message (STATUS "[PULSAR] Documentaion Installation location ${CMAKE_INSTALL_PREFIX}/data does not exist!")
  message (STATUS "[PULSAR] Creating documentation installation location ${CMAKE_INSTALL_PREFIX}/data")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/data)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")
  message (STATUS "[PULSAR] Bin Installation location ${CMAKE_INSTALL_PREFIX}/bin exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")
  message (STATUS "[PULSAR] Bin Installation location ${CMAKE_INSTALL_PREFIX}/bin does not exist!")
  message (STATUS "[PULSAR] Creating bin installation location ${CMAKE_INSTALL_PREFIX}/bin")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/bin)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/include")
  message (STATUS "[PULSAR] Include Installation location ${CMAKE_INSTALL_PREFIX}/include exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/include")
  message (STATUS "[PULSAR] Include Installation location ${CMAKE_INSTALL_PREFIX}/include does not exist!")
  message (STATUS "[PULSAR] Creating include installation location ${CMAKE_INSTALL_PREFIX}/include")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/include)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/include")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
  message (STATUS "[PULSAR] Library Installation location ${CMAKE_INSTALL_PREFIX}/lib exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
  message (STATUS "[PULSAR] Library Installation location ${CMAKE_INSTALL_PREFIX}/lib does not exist!")
  message (STATUS "[PULSAR] Creating library installation location ${CMAKE_INSTALL_PREFIX}/lib")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")

if (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/python")
  message (STATUS "[PULSAR] Library Installation python location ${CMAKE_INSTALL_PREFIX}/lib/python exists!")
else (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/python")
  message (STATUS "[PULSAR] Library Installation python location ${CMAKE_INSTALL_PREFIX}/lib/python does not exist!")
  message (STATUS "[PULSAR] Creating library installation python location ${CMAKE_INSTALL_PREFIX}/lib/python")
  EXECUTE_PROCESS( COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/python)
endif (IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/python")

if (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/presto/src")
  message (STATUS "[PULSAR] Build location ${PULSAR_BINARY_DIR}/apps/presto/src exists!")
else (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/presto/src")
  message (STATUS "[PULSAR] Creating location ${PULSAR_BINARY_DIR}/apps/presto/src")
  EXECUTE_PROCESS( COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/presto/src)
endif (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/presto/src")

if (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc")
  message (STATUS "[PULSAR] Build location ${PULSAR_BINARY_DIR}/apps/sigproc exists!")
else (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc")
  message (STATUS "[PULSAR] Creating location ${PULSAR_BINARY_DIR}/apps/sigproc")
  EXECUTE_PROCESS( COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/sigproc)
endif (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc")

IF(NOT SYSTEM_UNAME_TEST)
   EXEC_PROGRAM("uname -n"
                OUTPUT_VARIABLE SYSTEM_UNAME_TEST)
   SET(SYSTEM_UNAME_TEST "${SYSTEM_UNAME_TEST}" CACHE STRING "Name of build machine for Wisdom Flags")
ENDIF(NOT SYSTEM_UNAME_TEST)
SET(SYSTEM_UNAME_TEST "${SYSTEM_UNAME_TEST}")

if (SYSTEM_UNAME_TEST STREQUAL "lhn001")
   message (STATUS "[PULSAR] Working/building on lhn001 - fftw_wisdom.txt will be copied")
   EXECUTE_PROCESS( COMMAND cp ${PULSAR_SOURCE_DIR}/apps/presto/LUS/LUS_lhn001_fftw_wisdom.txt ${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt)
   message (STATUS "[PULSAR] cp ${PULSAR_SOURCE_DIR}/apps/presto/LUS/LUS_lhn001_fftw_wisdom.txt ${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")
else (SYSTEM_UNAME_TEST STREQUAL lhn001)
   message (STATUS "[PULSAR] Not working/build on lhn001 - fftw_widom.txt will be created if it does not exist")
endif (SYSTEM_UNAME_TEST STREQUAL "lhn001")

if (EXISTS "${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")
  message (STATUS "[PULSAR] Makewisdom file is present; skipping build target!")
  set (PRESTO_MAKEWISDOM FALSE)
else (EXISTS "${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")
  message (STATUS "[PULSAR] Makewisdom not been run;  adding to build make queue")
  set (PRESTO_MAKEWISDOM TRUE)
endif (EXISTS "${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")

# skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH YES)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH YES)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH YES)
