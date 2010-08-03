
macro (md5sum _checksum _filename)

  ##__________________________________________________________________
  ## Check for the md5sum executable required for the computation of
  ## the checksum

  find_program (md5sum_executable md5sum ${bin_locations})
  
  if (md5sum_executable)

    ##________________________________________________________________
    ## Extract filename components for further processing

    get_filename_component (_filename_path ${_filename} PATH)
    get_filename_component (_filename_name ${_filename} NAME)

    if (_filename_path)
      
      ##______________________________________________________________
      ## Execute md5sum to compute the MD5 checksum for the file
      
      execute_process(COMMAND ${md5sum_executable} ${_filename_name}
	WORKING_DIRECTORY ${_filename_path}
	RESULT_VARIABLE _md5sum_result
	OUTPUT_VARIABLE _md5sum_output
	ERROR_VARIABLE _md5sum_error
	)

      message (STATUS "[MD5Checksum]")
      message (STATUS "md5sum result = ${_md5sum_result}")
      message (STATUS "md5sum output = ${_md5sum_output}")
      message (STATUS "md5sum error  = ${_md5sum_error}")
      
    endif (_filename_path)
    
  else (md5sum_executable)
    
    message (WARNING "[MD5Checksum] Unable to compute checksum - missing generator!")
    
  endif (md5sum_executable)
  
endmacro (md5sum)
