
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
  configure_file (${LUS_ROOT}/doc/Doxyfile.in ${LUS_ROOT}/doc/Doxyfile)
  ## Create documentation
  add_custom_target (doc
    COMMAND cmake --help-html CMake.html
    COMMAND ctest --help-html CTest.html
    COMMAND ${DOXYGEN_EXECUTABLE}
    COMMAND rm -f *.tmp
    WORKING_DIRECTORY ${LUS_ROOT}/doc
    COMMENT "Creating browsable documentation in ${LUS_ROOT}/doc ..."
    )
  ## Source file dependency of build target
else (DOXYGEN_EXECUTABLE)
  add_custom_target (doc
    COMMAND cmake --help-html CMake.html
    COMMAND ctest --help-html CTest.html
    WORKING_DIRECTORY ${LUS_ROOT}/doc
    COMMENT "Creating browsable documentation in ${LUS_ROOT}/doc ..."
    )
endif (DOXYGEN_EXECUTABLE)
