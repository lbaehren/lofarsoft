
##_______________________________________________________________________________
##  Download and install test datasets

if (WGET_FOUND)

  add_custom_target (test-data
    COMMAND ${WGET_EXECUTABLE} -c ${USG_DOWNLOAD}/datasets/usg_data_test.tgz
    COMMAND tar -xvzf usg_data_test.tgz
    COMMAND rm -f usg_data_test.tgz
    WORKING_DIRECTORY ${LUS_DATA_DIR}
    COMMENT "Retrieving test data-sets ..."
    )

else (WGET_FOUND)

  if (curl_executable)
    add_custom_target (test-data
      COMMAND ${curl_executable} ${USG_DOWNLOAD}/datasets/usg_data_test.tgz -o usg_data_test.tgz -C -
      COMMAND tar -xvzf usg_data_test.tgz
      COMMAND rm -f usg_data_test.tgz
      WORKING_DIRECTORY ${LUS_DATA_DIR}
      COMMENT "Retrieving test data-sets ..."
      )
  else (curl_executable)
    message (STATUS "WARNING: Unable to retrieve sample datasets - missing wget and curl!")
  endif (curl_executable)

endif (WGET_FOUND)

