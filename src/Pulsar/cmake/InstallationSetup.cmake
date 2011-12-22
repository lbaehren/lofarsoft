
## ==============================================================================
##
##                                                             Installation setup
##
##  We install into $LOFARSOFT/release/share/pulsar to keep all the components
##  in one place but not have them interfere with possible other installations
##  of the same packages (e.g. FFTW3)
##
## ==============================================================================

if (LUS_SOURCE_DIR)
 message (STATUS "[PULSAR] Installation area located for package PULSAR ${LUS_SOURCE_DIR}/release/share/pulsar.")
 set (CMAKE_INSTALL_PREFIX ${LUS_SOURCE_DIR}/release/share/pulsar)
else (LUS_SOURCE_DIR)
 message (STATUS "[PULSAR] WARNING: Installation area NOT located for package PULSAR!")
endif (LUS_SOURCE_DIR)

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

if (EXISTS "${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")
  message (STATUS "[PULSAR] Makewisdom has already been run; skipping!")
  set (PRESTO_MAKEWISDOM FALSE)
else (EXISTS "${CMAKE_INSTALL_PREFIX}/lib/fftw_wisdom.txt")
  message (STATUS "[PULSAR] Makewisdom not been run;  adding to make queue")
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
