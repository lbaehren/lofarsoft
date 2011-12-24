
## ==============================================================================
##
##  SUMMARY:
##  
##    This CMake module is intended to check the installation of the Pulsar
##    tools software collection for completeness; one of the reoccuring issues
##    with the setup of the configuration and build framework is, that not 
##    automatic tests exist which would help to verify the validity of the 
##    installation.
##
## ==============================================================================

## === [1] General environment ==================================================
##
##     Provide some basic feedback on the (CMake) environment of the Pulsar-Tools

message (STATUS "=============================================================="    )
message (STATUS " [Pulsar-Tools] Check of installation                         "    )
message (STATUS "+------------------------------------------------------------+"    )
message (STATUS " System configuration:"                                            )
message (STATUS " .. CMAKE_COMMAND                = ${CMAKE_COMMAND}"               )
message (STATUS " .. CMAKE_SYSTEM                 = ${CMAKE_SYSTEM}"                )
message (STATUS " .. CMAKE_SYSTEM_PROCESSOR       = ${CMAKE_SYSTEM_PROCESSOR}"      )
message (STATUS " Package configuration:"                                           )
message (STATUS " .. PULSAR_SOURCE_DIR            = ${PULSAR_SOURCE_DIR}"           )
message (STATUS " .. PULSAR_BINARY_DIR            = ${PULSAR_BINARY_DIR}"           )
message (STATUS " .. CMAKE_INSTALL_PREFIX         = ${CMAKE_INSTALL_PREFIX}"        )
message (STATUS " .. Enable test programs         = ${PULSAR_ENABLE_TESTING}"       )
message (STATUS " .. Enable compiler warnings     = ${PULSAR_COMPILER_WARNINGS}"    )
message (STATUS " .. Print debugging messages     = ${PULSAR_DEBUGGING_MESSAGES}"   )
message (STATUS " .. Enable verbose configure     = ${PULSAR_VERBOSE_CONFIGURE}"    )
message (STATUS " .. Enable Python bindings       = ${PULSAR_PYTHON_BINDINGS}"      )

## === [2] Check installation directories =======================================
##
##     Go through the list of directories expected to be present below the 
##     installation prefix.

foreach (_dir bin doc lib include data)

  ## Name of variable corresponding to directory being checked
  string (TOUPPER ${_dir} _varDir)
  
  if (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${_dir})
    set (HAVE_INSTALL_DIR_${_varDir} TRUE)
  else (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${_dir})
    set (HAVE_INSTALL_DIR_${_varDir} FALSE)
  endif (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${_dir})
  
endforeach (_dir)

message (STATUS " Installation directories:"                                        )
message (STATUS " .. <prefix>/bin                 = ${HAVE_INSTALL_DIR_BIN}"        )
message (STATUS " .. <prefix>/data                = ${HAVE_INSTALL_DIR_DATA}"       )
message (STATUS " .. <prefix>/doc                 = ${HAVE_INSTALL_DIR_DOC}"        )
message (STATUS " .. <prefix>/include             = ${HAVE_INSTALL_DIR_INCLUDE}"    )
message (STATUS " .. <prefix>/lib                 = ${HAVE_INSTALL_DIR_LIB}"        )

## === [3] Check presence of individual files ===================================
##
##     Besides the libraries and header files (which are being check in subsequent
##     steps), there is a number of configuration and settings files dropped into
##     installation directories - check if they can be found at their destination.

find_file (HAVE_INSTALL_FILE_WISDOM 
  NAMES fftw_wisdom.txt
  PATHS ${CMAKE_INSTALL_PREFIX}
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH
  )

message (STATUS " Installed configuration files:"                                   )
message (STATUS " .. <prefix>/lib/fftw_wisdom.txt = ${HAVE_INSTALL_FILE_WISDOM}"    )
