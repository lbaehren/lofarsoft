
## ==============================================================================
##
##  
##
## ==============================================================================

## Generate CMake master index matching installed version

execute_process (
  COMMAND ${CMAKE_COMMAND} --help-html
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  OUTPUT_FILE CMake.html
  )

## Generate CTest master index matching installed version

execute_process (
  COMMAND ${CMAKE_CTEST_COMMAND} --help-html
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  OUTPUT_FILE CTest.html
  )

## ==============================================================================
##
##  Documentation generation
##
## ==============================================================================

if (DOXYGEN_EXECUTABLE)
  if (DOXYGEN_GENERATE_LATEX)
    ## Locate required executables
    find_program (latex_executable      latex      PATHS ${bin_locations})
    find_program (makeindex_executable  makeindex  PATHS ${bin_locations})
    find_program (pdflatex_executable   pdflatex   PATHS ${bin_locations})
    ## Is LaTeX available?
    if (latex_executable AND makeindex_executable)
      set (DOXYGEN_GENERATE_LATEX YES)
    endif (latex_executable AND makeindex_executable)
    ## Is PDFLaTeX available?
    if (pdflatex_executable)
      set (DOXYGEN_USE_PDFLATEX YES)
    endif (pdflatex_executable)
  endif (DOXYGEN_GENERATE_LATEX)
  ## Creation configuration file for Doxygen
  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    )
  ## Create documentation
  add_custom_target (doc
    COMMAND ${DOXYGEN_EXECUTABLE}
    COMMAND rm -f *.tmp
    WORKING_DIRECTORY ${LUS_ROOT}/doc
    COMMENT "Creating browsable documentation in ${LUS_ROOT}/doc ..."
    )
  ## Source file dependency of build target
else (DOXYGEN_EXECUTABLE)
  add_custom_target (doc
    COMMAND ${CMAKE_COMMAND} --help-html CMake.html
    COMMAND ctest --help-html CTest.html
    WORKING_DIRECTORY ${LUS_ROOT}/doc
    COMMENT "Creating browsable documentation in ${LUS_ROOT}/doc ..."
    )
endif (DOXYGEN_EXECUTABLE)
