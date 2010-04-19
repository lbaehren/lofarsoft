##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 528 2007-08-09 17:53:34Z baehren                       $
##------------------------------------------------------------------------------

##_______________________________________________________________________________
##  Download and install test datasets

if (wget_executable)
  add_custom_target (test-data
    COMMAND ${wget_executable} -c ${USG_DOWNLOAD}/datasets/usg_data_test.tgz
    COMMAND tar -xvzf usg_data_test.tgz
    COMMAND rm -f usg_data_test.tgz
    WORKING_DIRECTORY ${USG_DATA}
    COMMENT "Retrieving test data-sets ..."
    )
else (wget_executable)
  if (curl_executable)
    add_custom_target (test-data
      COMMAND ${curl_executable} ${USG_DOWNLOAD}/datasets/usg_data_test.tgz -o usg_data_test.tgz -C -
      COMMAND tar -xvzf usg_data_test.tgz
      COMMAND rm -f usg_data_test.tgz
      WORKING_DIRECTORY ${USG_DATA}
      COMMENT "Retrieving test data-sets ..."
      )
  else (curl_executable)
    message (STATUS "WARNING: Unable to retrieve sample datasets - missing wget and curl!")
  endif (curl_executable)
endif (wget_executable)

