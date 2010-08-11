
## === Include guard (begin) ====================================================

if (NOT HAVE_AddPythonModule)
  
  set (HAVE_AddPythonModule TRUE)
  
  ## ============================================================================
  ##
  ##  Macro definition
  ##
  ## ============================================================================
  
  macro (add_python_module _moduleName _moduleSources)

    message (STATUS "[add_python_module]")
    message (STATUS "_moduleName    = ${_moduleName}")
    message (STATUS "_moduleSources = ${_moduleSources}")
    
    ## Compiler instructions
    
    add_library(${_moduleName} MODULE ${_moduleSources} )
    
    ## Additional target properties
    
    if (APPLE)
      set_target_properties (${_moduleName}
	PROPERTIES
	PREFIX ""
	SUFFIX .so
	COMPILE_FLAGS "-DPYTHON -fpermissive"
	LINK_FLAGS "-fPIC -flat_namespace"
	LINKER_LANGUAGE C
	)
    else (APPLE)
      set_target_properties (${_moduleName}
	PROPERTIES
	PREFIX ""
	SUFFIX .so
	COMPILE_FLAGS -DPYTHON
	LINK_FLAGS "-fPIC -shared"
	LINKER_LANGUAGE C
	)
    endif (APPLE)
    
  endmacro (add_python_module)
  
  ## === Include guard (end) ====================================================
  
endif (NOT HAVE_AddPythonModule)
